#include "util.h"
#include "inline_util.h"


/*
 * Each thread has its own context, this speeds things up compared to global variables
 */
class WorkerContext {
public:
    int workerid;

    erpc::Rpc<erpc::CTransport> *rpc;

    int reqs_per_loop;
    struct mica_resp mica_resp_arr[WORKER_MAX_BATCH];
    struct mica_op *op_ptr_arr[WORKER_MAX_BATCH];

    int total_ops;
    erpc::ReqHandle *handle[MACHINE_NUM];
    int ops_in_req[MACHINE_NUM];

};

void req_handler(erpc::ReqHandle *req_handle, void *_context) {

    auto *c = static_cast<WorkerContext *>(_context);
    int workerid = c->workerid;
    int req_id = c->reqs_per_loop;
    c->handle[req_id] = req_handle; // this is used for responses

    const erpc::MsgBuffer *req = req_handle->get_req_msgbuf();

    size_t size = req->get_data_size();

    struct mica_op *ops = reinterpret_cast<struct mica_op*>(req->buf);

    c->ops_in_req[req_id] = 0;

    int offset = 0;

    while(offset < size) {

        ops = reinterpret_cast<struct mica_op*>(req->buf + offset);

        int size_of_op = ops->opcode == CACHE_OP_PUT ? HERD_PUT_REQ_SIZE : HERD_GET_REQ_SIZE;


		//mica_print_op(ops);
        //op_ptr_arr[workerid][wr_i] = ops;
        c->op_ptr_arr[c->total_ops] = ops;

        offset += size_of_op;

        c->total_ops++;

        c->ops_in_req[req_id] ++;

    }
    c->reqs_per_loop++;

#if ENABLE_KVS_BATCHING == 1
	assert(c->total_ops <= WORKER_MAX_BATCH);
#else
	KVS_BATCH_OP(&kv, c->total_ops, c->op_ptr_arr, c->mica_resp_arr);

	erpc::MsgBuffer &resp = req_handle->pre_resp_msgbuf;

	size_t size = c->total_ops * sizeof(mica_resp);

	c->rpc->resize_msg_buffer(&resp, size);

	memcpy((void *) resp.buf, (char *) c->mica_resp_arr, size);

	c->rpc->enqueue_response(req_handle,&resp); // send the response immediately
	w_stats[workerid].batches_per_worker++;
	w_stats[workerid].remotes_per_worker += c->total_ops;

	c->total_ops=0;

#endif

}
inline void drain_batch(WorkerContext *c)
{
    int workerid = c->workerid;

    KVS_BATCH_OP(&kv, c->total_ops, c->op_ptr_arr, c->mica_resp_arr);

    int offset = 0;
    for(int i=0;i < c->reqs_per_loop; i++) {

        erpc::MsgBuffer &resp = c->handle[i]->pre_resp_msgbuf;
        int num_ops = c->ops_in_req[i];

        size_t size = num_ops * sizeof(mica_resp);

        c->rpc->resize_msg_buffer(&resp, size);

        memcpy((void *) resp.buf, ((char *) c->mica_resp_arr) + offset, size);

        c->rpc->enqueue_response(c->handle[i],&resp);

        offset += size;

    }

    w_stats[workerid].batches_per_worker++;
    w_stats[workerid].remotes_per_worker += c->total_ops;

}


int connections = 0;
void sm_handlerc(int, erpc::SmEventType sm_event_type, erpc::SmErrType, void *)
{
	if (sm_event_type == erpc::SmEventType::kConnected)
		connections++;
	else
		connections--;

}




void *run_worker(void *arg) {




	int i, j, ret;
	uint16_t qp_i;
	struct thread_params params = *(struct thread_params *) arg;
    uint16_t wrkr_lid = params.id;    /* Local ID of this worker thread*/
	int num_server_ports = MAX_SERVER_PORTS, base_port_index = 0;


	cyan_printf("Wrkr %d is_roce %d\n", wrkr_lid, is_roce);



	cyan_printf("Setting up eRPC server for worker ID %d\n",wrkr_lid);

	WorkerContext context;
	context.workerid = wrkr_lid;

	/*
	void *wrkrid = malloc(sizeof(int));
	memcpy(wrkrid, (void *) &wrkr_lid, sizeof(int));*/


	context.rpc = new erpc::Rpc<erpc::CTransport>(nexus, (void *) &context, wrkr_lid, sm_handlerc);

	uint8_t worker_sl = 0;
	int remote_client_num = CLIENT_NUM - CLIENTS_PER_MACHINE;
	assert(MICA_MAX_BATCH_SIZE >= WORKER_MAX_BATCH);
	assert(HERD_VALUE_SIZE <= MICA_MAX_VALUE);
	assert(WORKER_SS_BATCH > WORKER_MAX_BATCH);    /* WORKER_MAX_BATCH check */

	/* ---------------------------------------------------------------------------
	------------Set up the KVS partition-----------------------------------------
	---------------------------------------------------------------------------*/
#if ENABLE_WORKERS_CRCW == 0
    struct mica_kv kv;

    mica_init(&kv, (int) wrkr_lid, 0, HERD_NUM_BKTS, HERD_LOG_CAP); //0 refers to numa node
	mica_populate_fixed_len(&kv, HERD_NUM_KEYS, HERD_VALUE_SIZE);
#endif

	/* ---------------------------------------------------------------------------
	------------Set up the control block-----------------------------------------
	---------------------------------------------------------------------------*/
	uint16_t worker_req_size = sizeof(struct wrkr_ud_req);
	assert(num_server_ports <= MAX_SERVER_PORTS);    /* Avoid dynamic alloc */
	struct hrd_ctrl_blk *cb[MAX_SERVER_PORTS];
	uint32_t wrkr_buf_size = worker_req_size * CLIENTS_PER_MACHINE * (MACHINE_NUM - 1) * WS_PER_WORKER;

	int *wrkr_recv_q_depth = (int*)malloc(WORKER_NUM_UD_QPS * sizeof(int));
	int *wrkr_send_q_depth = (int*)malloc(WORKER_NUM_UD_QPS * sizeof(int));
	for (qp_i = 0; qp_i < WORKER_NUM_UD_QPS; qp_i++) {
		wrkr_recv_q_depth[qp_i] = WORKER_RECV_Q_DEPTH; // TODO fix this as a performance opt(it should be at the qp granularity)
		wrkr_send_q_depth[qp_i] = WORKER_SEND_Q_DEPTH; // TODO fix this as a performance opt
	}


	/* ---------------------------------------------------------------------------
	------------Set up the buffer space for multiple UD QPs----------------------
	---------------------------------------------------------------------------*/


	uint16_t clts_per_qp[WORKER_NUM_UD_QPS];
	uint32_t per_qp_buf_slots[WORKER_NUM_UD_QPS], qp_buf_base[WORKER_NUM_UD_QPS];
	int pull_ptr[WORKER_NUM_UD_QPS] = {0}, push_ptr[WORKER_NUM_UD_QPS] = {0}; // it is useful to keep these signed
	setup_the_buffer_space(clts_per_qp, per_qp_buf_slots, qp_buf_base);
	uint32_t max_reqs = (uint32_t) wrkr_buf_size / worker_req_size;

	struct wrkr_ud_req *req[WORKER_NUM_UD_QPS]; // break up the buffer to ease the push/pull ptr handling


	if (wrkr_lid == 0) {
		//create_AHs_for_worker(wrkr_lid, cb[0]);
		//printf("checking ah = 0 for worker %i\n",wrkr_lid);
		assert(wrkr_needed_ah_ready == 0);
		wrkr_needed_ah_ready = 1;
	} else {
        //printf("sleep worker %i\n",wrkr_lid);
	    while (wrkr_needed_ah_ready == 0) usleep(200000);

    }
	assert(wrkr_needed_ah_ready == 1);
	//printf("WORKER %d has all the needed ahs\n", wrkr_lid );

	struct mica_op *op_ptr_arr[WORKER_MAX_BATCH];//, *local_op_ptr_arr;
	struct mica_resp mica_resp_arr[WORKER_MAX_BATCH], local_responses[CLIENTS_PER_MACHINE *
																	  LOCAL_WINDOW], mica_batch_resp[WORKER_MAX_BATCH]; // this is needed because we batch to MICA all reqs between 2 writes
	struct ibv_send_wr wr[WORKER_MAX_BATCH], *bad_send_wr = NULL;
	struct ibv_sge sgl[WORKER_MAX_BATCH];

	struct ibv_wc wc[WS_PER_WORKER * (CLIENT_NUM - CLIENTS_PER_MACHINE)], send_wc;
	struct ibv_recv_wr recv_wr[WORKER_MAX_BATCH], *bad_recv_wr;
	struct ibv_sge recv_sgl[WORKER_MAX_BATCH];
	long long rolling_iter = 0, local_nb_tx = 0, local_tot_tx = 0;
	long long nb_tx_tot[WORKER_NUM_UD_QPS] = {0};
	int ws[CLIENTS_PER_MACHINE] = {0}; // WINDOW SLOT (Push pointer) of local clients
	int clt_i = -1;
	uint16_t last_measured_wr_i = 0, resp_buf_i = 0, received_messages,
			wr_i = 0, per_qp_received_messages[WORKER_NUM_UD_QPS] = {0};
	struct mica_op *dbg_buffer = (struct mica_op *)malloc(HERD_PUT_REQ_SIZE);
	assert(CLIENTS_PER_MACHINE % num_server_ports == 0);

	struct mica_op *local_op_ptr_arr[CLIENTS_PER_MACHINE * LOCAL_WINDOW];
	for (i = 0; i < CLIENTS_PER_MACHINE * LOCAL_WINDOW; i++)
		local_op_ptr_arr[i] = (struct mica_op *) (local_req_region +
												  ((wrkr_lid * CLIENTS_PER_MACHINE * LOCAL_WINDOW) + i));
	struct wrkr_coalesce_mica_op *response_buffer; // only used when inlining is not possible
	struct ibv_mr *resp_mr;
	//setup_worker_WRs(&response_buffer, resp_mr, cb[0], recv_sgl, recv_wr, wr, sgl, wrkr_lid);

	qp_i = 0;
	uint16_t send_qp_i = 0, per_recv_qp_wr_i[WORKER_NUM_UD_QPS] = {0};
	uint32_t dbg_counter = 0;
	uint8_t requests_per_message[WORKER_MAX_BATCH] = {0};
	uint16_t send_wr_i;
	yellow_printf("wrkr %d reached the loop \n", wrkr_lid);
	// start the big loop
	//rpc->run_event_loop(-1);
	while (1) {
		/* Do a pass over requests from all clients */
        context.reqs_per_loop = 0;
        context.total_ops = 0;
        //context.mica_resp_arr = {}; this is overwritten
        // collect all the requests
        int oldreqs;
        do {
            oldreqs = context.reqs_per_loop;
            context.rpc->run_event_loop_once();
        }
#if ENABLE_KVS_BATCHING == 1
        while(oldreqs != context.reqs_per_loop); // collect all requests

        if (context.reqs_per_loop == 0) {
            w_stats[wrkr_lid].empty_polls_per_worker++;
            continue; // no request was found, start over
        }
        else {
			drain_batch(&context); // KVS BATCH
        }
#else
		while(false); // execute only once

		if (context.reqs_per_loop == 0) {
			w_stats[wrkr_lid].empty_polls_per_worker++;
			continue; // no request was found, start over
		}
#endif

		wr_i = 0;

		/* ---------------------------------------------------------------------------
		------------------------------ LOCAL REQUESTS--------------------------------
		---------------------------------------------------------------------------*/
		// Before polling for remote reqs, poll for all the local, such that you give time for the remote reqs to gather up
		if (DISABLE_LOCALS != 1) { // DEPRECATED
			if (ENABLE_LOCAL_WORKERS && wrkr_lid >= ACTIVE_WORKERS_PER_MACHINE || !ENABLE_LOCAL_WORKERS) {
				serve_local_reqs(wrkr_lid, &kv, local_op_ptr_arr, local_responses);
				if (ENABLE_LOCAL_WORKERS) continue;
			}
		}

	}
	delete context.rpc;
	return NULL;
}
