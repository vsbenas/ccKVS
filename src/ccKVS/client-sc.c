
#include "util.h"






// see util.h for ClientContext



#include "inline_util.h"




void req_cache(erpc::ReqHandle *req_handle, void *_context) {
    //auto &req = req_handle->pre_req_msgbuf;

    auto *c = static_cast<ClientContext *>(_context);

    uint16_t local_client_id = c->clientid;

    erpc::MsgBuffer &resp = req_handle->pre_resp_msgbuf;

    const erpc::MsgBuffer *req = req_handle->get_req_msgbuf();

    size_t size = req->get_data_size();

    int updates = size / HERD_PUT_REQ_SIZE;

    //printf("Received cache ops %i %s!\n",size, (char *) req->buf);

    //struct cache_op *update_ops = reinterpret_cast<struct cache_op*>(req->buf);

    //struct mica_resp update_resp[BCAST_TO_CACHE_BATCH];

    memcpy(c->update_ops,req->buf,size); // needs to be aligned

    cache_batch_op_sc_with_cache_op(updates, local_client_id, &(c->update_ops), c->update_resp);

    c->crpc->resize_msg_buffer(&resp, 3);



    memcpy((void *) resp.buf, (void *) "OK\0", 3);

    //req_handle->prealloc_used = true;

    //printf("%s .\n",(char *) resp.buf);

    c->crpc->enqueue_response(req_handle,&resp);

    c_stats[local_client_id].received_updates_per_client+=updates;

    //printf(".\n");

}


void cache_response(void *_context, void* tag) {

    auto *c = static_cast<ClientContext *>(_context);

    uint16_t local_client_id = c->clientid;

    int rm_id = *(static_cast<int*>(tag));

    c->crpc->free_msg_buffer(c->creq[rm_id]);
    c->crpc->free_msg_buffer(c->cresp[rm_id]);
    c->cache_bufferused[rm_id] = 0;
    c->reqs_in_transit--;


    free(tag);

}










inline void broadcast_cache_ops(ClientContext* c, int* cache_sessions) {

    int clientid = c->clientid;

    if(c->cidx == 0) // no cache operations found
        return;
    // construct packet
    int total_length = c->cidx * c->creq_length;
    uint8_t *buffer = (uint8_t*) malloc(total_length);
    int offset = 0;

    for(int msg = 0; msg < c->cidx; msg++) {

        memcpy(buffer + offset, c->cbatch[msg], c->creq_length);

        offset += c->creq_length;

    }

    // send the packet to all machines
    for (int rm_id = 0; rm_id < MACHINE_NUM; rm_id++) {

        if (rm_id != machine_id) {

            assert(c->cache_bufferused[rm_id] == 0);

            int session = cache_sessions[rm_id];

            c->creq[rm_id] = c->crpc->alloc_msg_buffer_or_die(total_length);

            c->cresp[rm_id] = c->crpc->alloc_msg_buffer_or_die(total_length);

            memcpy(c->creq[rm_id].buf, buffer, total_length);

            c->cache_bufferused[rm_id] = 1;

            void *rmid_tag = malloc(sizeof(int));
            memcpy(rmid_tag, (void *) &rm_id, sizeof(int));

            c->crpc->enqueue_request(session, kReqCache, &(c->creq[rm_id]), &(c->cresp[rm_id]), cache_response, rmid_tag);

            c->reqs_in_transit++;
        }

    }
    c->cidx=0;

}




void receive_response(void *_context, void *tag) {

    auto *c = static_cast<ClientContext *>(_context);

    uint16_t local_client_id = c->clientid;

    int rm_id = *(static_cast<int*>(tag));

    // cyan_printf("Client %d received: %s (tag %d)\n",local_client_id, eresp[tag].buf, tag);
    c->crpc->free_msg_buffer(c->ereq[rm_id]);
    c->crpc->free_msg_buffer(c->eresp[rm_id]);
    c->bufferused[rm_id]=0;
    c->reqs_in_transit--;


    if ((MEASURE_LATENCY == 1) && c->glatency_info.measured_req_flag == REMOTE_REQ && local_client_id == 0 && machine_id == 0)
    {
        struct timespec end;
        clock_gettime(CLOCK_MONOTONIC, &end);
        int useconds = ((end.tv_sec - c->gstart.tv_sec) * 1000000) +
                       ((end.tv_nsec - c->gstart.tv_nsec) / 1000);  //(end.tv_nsec - start->tv_nsec) / 1000;
        if (ENABLE_ASSERTIONS) assert(useconds > 0);
        //		printf("Latency of a Remote read %u us\n", useconds);
        bookkeep_latency(useconds, REMOTE_REQ);
        c->glatency_info.measured_req_flag = NO_REQ;
    }


    free(tag);

}

void sm_handler(int, erpc::SmEventType, erpc::SmErrType, void *) {}





inline void send_requests(ClientContext* c, int *sessions) {

    int clientid = c->clientid;
    for(int rm_id=0;rm_id<MACHINE_NUM;rm_id++) {

        if( rm_id != machine_id && c->idx[rm_id] > 0) {

            int session = sessions[rm_id];
            // fill ereq
            assert(c->bufferused[rm_id] == 0);

            int total_length = 0;

            for(int msg = 0; msg < c->idx[rm_id]; msg++) {

                total_length += c->req_length[rm_id][msg];

            }

            c->ereq[rm_id] = c->crpc->alloc_msg_buffer_or_die(total_length);

            int resp_length = c->idx[rm_id] * sizeof(mica_resp);

            c->eresp[rm_id] = c->crpc->alloc_msg_buffer_or_die(resp_length);

            int offset = 0;

            //printf("CLIENT: Sending request to session %d rm_id %d length %d values:\n",session, rm_id,total_length);

            for(int msg = 0; msg < c->idx[rm_id]; msg++) {

                memcpy(c->ereq[rm_id].buf + offset , c->batch[rm_id][msg], c->req_length[rm_id][msg]);

                offset += c->req_length[rm_id][msg];
                //mica_print_op((struct mica_op *) batch[rm_id][msg]);
            }

            c->bufferused[rm_id]=1;

            c->idx[rm_id]=0;

            void *rmid_tag = malloc(sizeof(int));
            memcpy(rmid_tag, (void *) &rm_id, sizeof(int));

            c->crpc->enqueue_request(session, kReqData, &(c->ereq[rm_id]), &(c->eresp[rm_id]), receive_response, rmid_tag);
            c->reqs_in_transit++;
        }
    }


}



void *run_client(void *arg)
{



    cyan_printf("CLIENT: Size of worker req: %d, extra bytes: %d, ud req size: %d minimum worker req size %d, actual size of req_size %d  \n",
                WORKER_REQ_SIZE, EXTRA_WORKER_REQ_BYTES, UD_REQ_SIZE, MINIMUM_WORKER_REQ_SIZE, sizeof(struct wrkr_ud_req));


    int i, j;
    struct thread_params params = *(struct thread_params *) arg;
    int clt_gid = (machine_id * CLIENTS_PER_MACHINE) + params.id;	/* Global ID of this client thread */
    uint16_t local_client_id = (uint16_t) (clt_gid % CLIENTS_PER_MACHINE);
    uint16_t worker_qp_i = (uint16_t) ((local_client_id + machine_id) % WORKER_NUM_UD_QPS);
    uint16_t local_worker_id = (uint16_t) ((clt_gid % LOCAL_WORKERS) + ACTIVE_WORKERS_PER_MACHINE); // relevant only if local workers are enabled
    //if (local_client_id == 0 && machine_id == 0)



    int sessions[MACHINE_NUM]; // for data transfer
    int cache_sessions[MACHINE_NUM]; // cache coherence transfer


    int protocol = SEQUENTIAL_CONSISTENCY;
    int *recv_q_depths, *send_q_depths;
    uint16_t remote_buf_size =  ENABLE_WORKER_COALESCING == 1 ?
                                (GRH_SIZE + sizeof(struct wrkr_coalesce_mica_op)) : UD_REQ_SIZE ;
    printf("Remote clients buffer size %d\n", remote_buf_size);
    //setup_queue_depths(&recv_q_depths, &send_q_depths, protocol);
    // UNUSED
    struct hrd_ctrl_blk *cb; /* = hrd_ctrl_blk_init(clt_gid,	/* local_hid
                                                0, -1, /* port_index, numa_node_id
                                                0, 0,	/* #conn qps, uc
                                                NULL, 0, 0,	/* prealloc conn buf, buf size, key
                                                CLIENT_UD_QPS, remote_buf_size + SC_CLT_BUF_SIZE,	/* num_dgram_qps, dgram_buf_size
                                                MASTER_SHM_KEY + WORKERS_PER_MACHINE + local_client_id, /* key
                                                recv_q_depths, send_q_depths); /* Depth of the dgram RECV Q*/

    int push_ptr = 0, pull_ptr = -1;
    // UNUSED
    struct ud_req *incoming_reqs; // = (struct ud_req *)(cb->dgram_buf + remote_buf_size);
//    printf("dgram pointer %llu, incoming req ptr %llu \n", cb->dgram_buf, incoming_reqs);


    //}

    struct mcast_info *mcast_data;
    struct mcast_essentials *mcast;




    /* -----------------------------------------------------
    --------------DECLARATIONS------------------------------
    ---------------------------------------------------------*/
    int key_i, ret;
    struct ibv_send_wr rem_send_wr[WINDOW_SIZE], coh_send_wr[MESSAGES_IN_BCAST_BATCH],
            credit_send_wr[SC_MAX_CREDIT_WRS], *bad_send_wr;
    struct ibv_sge rem_send_sgl[WINDOW_SIZE], coh_send_sgl[MAX_BCAST_BATCH], credit_send_sgl;
    struct ibv_wc wc[MAX_REMOTE_RECV_WCS], signal_send_wc, credit_wc[SC_MAX_CREDIT_WRS], coh_wc[SC_MAX_COH_RECEIVES];
    struct ibv_recv_wr rem_recv_wr[WINDOW_SIZE], coh_recv_wr[SC_MAX_COH_RECEIVES],
            credit_recv_wr[SC_MAX_CREDIT_RECVS], *bad_recv_wr;
    struct ibv_sge rem_recv_sgl, coh_recv_sgl[SC_MAX_COH_RECEIVES], credit_recv_sgl;
    struct coalesce_inf coalesce_struct[MACHINE_NUM] = {0};

    long long remote_for_each_worker[WORKER_NUM] = {0}, rolling_iter = 0, trace_iter = 0,
            credit_tx = 0, br_tx = 0, remote_tot_tx = 0;
    uint8_t rm_id = 0, per_worker_outstanding[WORKER_NUM] = {0}, credits[SC_VIRTUAL_CHANNELS][MACHINE_NUM],
            broadcasts_seen[MACHINE_NUM] = {0};
    uint16_t wn = 0, wr_i = 0, br_i = 0, cb_i = 0, coh_i = 0, coh_buf_i = 0, rem_req_i = 0, prev_rem_req_i,
            credit_wr_i = 0, op_i = 0, next_op_i = 0, last_measured_wr_i = 0,
            ws[WORKERS_PER_MACHINE] = {0}; /* Window slot to use for LOCAL workers*/
    uint16_t previous_wr_i, worker_id = 0, print_count = 0, credit_rec_counter = 0;
    uint32_t cmd_count = 0, credit_debug_cnt = 0, outstanding_rem_reqs = 0;
    double empty_req_percentage;
    struct local_latency local_measure = {
            .measured_local_region = -1,
            .local_latency_start_polling = 0,
            .flag_to_poll = NULL,
    };


    struct cache_op *update_ops;
    struct key_home *key_homes, *next_key_homes, *third_key_homes;
    struct mica_resp *resp, *next_resp, *third_resp;
    struct mica_op *coh_buf;
    struct mica_resp update_resp[BCAST_TO_CACHE_BATCH] = {0}; // not sure why this is on stack
    struct ibv_mr *ops_mr, *coh_mr;
    struct extended_cache_op *ops, *next_ops, *third_ops;

    setup_ops(&ops, &next_ops, &third_ops, &resp, &next_resp, &third_resp,
              &key_homes, &next_key_homes, &third_key_homes);



    setup_coh_ops(&update_ops, NULL, NULL, NULL, update_resp, NULL, &coh_buf, protocol);
    setup_mrs(&ops_mr, &coh_mr, ops, coh_buf, cb);
    struct ibv_cq *coh_recv_cq = ENABLE_MULTICAST == 1 ? mcast->recv_cq : cb->dgram_recv_cq[BROADCAST_UD_QP_ID];
    struct ibv_qp *coh_recv_qp = ENABLE_MULTICAST == 1 ? mcast->recv_qp : cb->dgram_qp[BROADCAST_UD_QP_ID];
    uint16_t hottest_keys_pointers[HOTTEST_KEYS_TO_TRACK] = {0};

    /* ---------------------------------------------------------------------------
    ------------------------------INITIALIZE STATIC STRUCTUREs--------------------
        ---------------------------------------------------------------------------*/

    // TRACE
    struct trace_command *trace;
    trace_init(&trace, clt_gid);


    memset(per_worker_outstanding, 0, WORKER_NUM);
    struct timespec end;
    uint32_t dbg_per_worker[3]= {0};


    ClientContext context;

    context.clientid = local_client_id;

    context.update_ops = (struct cache_op*) memalign(4096,CACHE_BATCH_SIZE * sizeof(cache_op));
    //context.update_resp = {0};


    ClientContext *c = &context;


    /* -----------------------------------------------------
    --------------CONNECT WITH WORKERS AND CLIENTS-----------------------
    --------------------------------------------------------- */


    printf("Creating rpc for client %d using rpcid %d\n", local_client_id, WORKERS_PER_MACHINE + local_client_id);
    c->crpc = new erpc::Rpc<erpc::CTransport>(nexus, (void *) c, WORKERS_PER_MACHINE + local_client_id, sm_handler);
    c->crpc->retry_connect_on_invalid_rpc_id = true;
    printf("Connecting to remote machines...\n");

    assert(ip_vector.size() >= MACHINE_NUM);

    for (int i = 0; i < MACHINE_NUM; i++) {

        if(i != machine_id) {

            string remote_ip(ip_vector[i]);

            printf("Client %d connecting to worker %d at machine (%s:%i)\n", local_client_id, local_client_id,
                   ip_vector[i], worker_port);


            std::string server_uri = remote_ip + ":" + std::to_string(worker_port);
            int session_num = c->crpc->create_session(server_uri, local_client_id);

            sessions[i] = session_num;

            cyan_printf("Trying to connect...");
            while (!c->crpc->is_connected(session_num))
                c->crpc->run_event_loop_once();
            cyan_printf("Connected data transfer! Session id: %d\n",session_num);

            c->bufferused[i] = 0;

            printf("Client %d connecting to client %d at machine (%s:%i)\n", local_client_id, local_client_id,
                   ip_vector[i], worker_port);


            server_uri = remote_ip + ":" + std::to_string(worker_port);

            session_num = c->crpc->create_session(server_uri, WORKERS_PER_MACHINE + local_client_id);

            cache_sessions[i] = session_num;

            cyan_printf("Trying to connect...");
            while (!c->crpc->is_connected(session_num))
                c->crpc->run_event_loop_once();
            cyan_printf("Connected cache transfer! Session id: %d\n",session_num);

            c->cache_bufferused[i] = 0;


        }

    }

    c->reqs_in_transit = 0;


    if (local_client_id == 0) {
        if (spawn_stats_thread() != 0)
            red_printf("Stats thread was not successfully spawned \n");
        clt_needed_ah_ready=1;
    }
    else {
        while (clt_needed_ah_ready == 0);  usleep(200000);
    }
    assert(clt_needed_ah_ready == 1);



    /* ---------------------------------------------------------------------------
    ------------------------------START LOOP--------------------------------
    ---------------------------------------------------------------------------*/
    while (1) {

        /* Prepare next batch while waiting for responses! */


        if(previous_wr_i > 0) {
            outstanding_rem_reqs -= previous_wr_i;
        }

        // Swap the op buffers to facilitate correct ordering
        swap_ops(&ops, &next_ops, &third_ops,
                 &resp, &next_resp, &third_resp,
                 &key_homes, &next_key_homes, &third_key_homes);

        /* ---------------------------------------------------------------------------
        ------------------------------PROBE THE CACHE--------------------------------------
        ---------------------------------------------------------------------------*/


        trace_iter = batch_from_trace_to_cache(trace_iter, local_client_id, trace, ops, resp,
                                               key_homes, 0, next_op_i, &(c->glatency_info), &(c->gstart),
                                               hottest_keys_pointers);


        /* ---------------------------------------------------------------------------
        ------------------------------BROADCASTS--------------------------------------
        ---------------------------------------------------------------------------*/
        if (WRITE_RATIO > 0 && DISABLE_CACHE == 0) {
            perform_broadcasts_SC(ops, credits, cb, credit_wc,
                                  &credit_debug_cnt, coh_send_sgl, coh_send_wr,
                                  coh_buf, &coh_buf_i, &br_tx, credit_recv_wr,
                                  local_client_id, protocol, c);
        }

        /* ---------------------------------------------------------------------------
        ------------------------------ REMOTE & LOCAL REQUESTS------------------------
        ---------------------------------------------------------------------------*/
        previous_wr_i = wr_i;
        wr_i = 0;
        prev_rem_req_i = rem_req_i;
        rem_req_i = 0; next_op_i = 0;

        empty_req_percentage = c_stats[local_client_id].empty_reqs_per_trace;


        if (ENABLE_MULTI_BATCHES == 1) { // deprecated
            find_responses_to_enable_multi_batching (empty_req_percentage, cb, wc,
                                                     per_worker_outstanding, &outstanding_rem_reqs, local_client_id);
        }
        else memset(per_worker_outstanding, 0, WORKER_NUM);

        // Create the Remote Requests and handle locals
        wr_i = handle_cold_requests(ops, next_ops, resp,
                                    next_resp, key_homes, next_key_homes,
                                    &rem_req_i, &next_op_i, cb, rem_send_wr,
                                    rem_send_sgl, wc, remote_tot_tx, worker_qp_i,
                                    per_worker_outstanding, &outstanding_rem_reqs, remote_for_each_worker,
                                    ws, clt_gid, local_client_id, NULL, local_worker_id, protocol,
                                    &(c->glatency_info), &(c->gstart), &local_measure, hottest_keys_pointers, c); // IMPORTANT




        c_stats[local_client_id].remotes_per_client += rem_req_i;
        if (ENABLE_WORKER_COALESCING == 1) {
            rem_req_i = wr_i;
        }


        if(wr_i > 0) { // from poll_and_send_remotes

            c_stats[local_client_id].remote_messages_per_client += wr_i;

            c_stats[local_client_id].batches_per_client++;

        }
        else if (ENABLE_STAT_COUNTING == 1) {
            c_stats[local_client_id].wasted_loops++;
        }

        // wait for responses from old batches, before sending new ones
        if(previous_wr_i > 0) {

            uint32_t debug_cnt = 0;
            while(c->reqs_in_transit > 0) {

                c->crpc->run_event_loop_once(); // wait for responses (and answer new requests)

                debug_cnt ++;
                if (debug_cnt > M_256) {
                    printf("Client %d is stuck waiting for %d completions \n",local_client_id, c->reqs_in_transit );
                    debug_cnt = 0;
                }

            }
            c_stats[local_client_id].stalled_time_per_client += debug_cnt;
        }


        broadcast_cache_ops(c,cache_sessions); // cache ops
        send_requests(c,sessions); // data ops

        c->crpc->run_event_loop_once(); // burst TX (and answer new requests)

    }
    return NULL;
}
