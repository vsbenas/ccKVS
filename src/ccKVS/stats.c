#include "util.h"
#include "inline_util.h"

void *print_stats(void* no_arg)
{
    int j;
    uint16_t i, print_count = 0;
    long long all_clients_cache_hits = 0, all_workers_remotes = 0, all_workers_locals = 0;
    double total_throughput = 0, all_clients_throughput = 0, all_workers_throughput = 0;
    double worker_throughput[WORKERS_PER_MACHINE];
    int sleep_time = 20;
    struct client_stats curr_c_stats[CLIENTS_PER_MACHINE], prev_c_stats[CLIENTS_PER_MACHINE];
    struct worker_stats curr_w_stats[WORKERS_PER_MACHINE], prev_w_stats[WORKERS_PER_MACHINE];
    struct stats all_stats;
    sleep(4);
    memcpy(prev_c_stats, (void*) c_stats, CLIENTS_PER_MACHINE * (sizeof(struct client_stats)));
    memcpy(prev_w_stats, (void*) w_stats, WORKERS_PER_MACHINE * (sizeof(struct worker_stats)));
    struct timespec start, end;
    clock_gettime(CLOCK_REALTIME, &start);
    while(true) {
        sleep(sleep_time);
        clock_gettime(CLOCK_REALTIME, &end);
        double seconds = (end.tv_sec - start.tv_sec) + (double) (end.tv_nsec - start.tv_nsec) / 1000000001;
        start = end;
        memcpy(curr_c_stats, (void*) c_stats, CLIENTS_PER_MACHINE * (sizeof(struct client_stats)));
        memcpy(curr_w_stats, (void*) w_stats, WORKERS_PER_MACHINE * (sizeof(struct worker_stats)));
        all_clients_cache_hits = 0; all_workers_remotes = 0; all_workers_locals = 0;
        print_count++;
        if (EXIT_ON_PRINT == 1 && print_count == PRINT_NUM) {
            if (MEASURE_LATENCY && machine_id == 0) print_latency_stats();
            printf("---------------------------------------\n");
            printf("------------RUN TERMINATED-------------\n");
            printf("---------------------------------------\n");
            exit(0);
        }
        seconds *= MILLION; // compute only MIOPS
        for (i = 0; i < CLIENTS_PER_MACHINE; i++) {
            all_clients_cache_hits += curr_c_stats[i].cache_hits_per_client - prev_c_stats[i].cache_hits_per_client;
            all_stats.cache_hits_per_client[i] = (curr_c_stats[i].cache_hits_per_client - prev_c_stats[i].cache_hits_per_client) / seconds;
            all_stats.remotes_per_client[i] = (curr_c_stats[i].remotes_per_client - prev_c_stats[i].remotes_per_client) / seconds;
            all_stats.locals_per_client[i] = (curr_c_stats[i].locals_per_client - prev_c_stats[i].locals_per_client) / seconds;
            all_stats.updates_per_client[i] = (curr_c_stats[i].updates_per_client - prev_c_stats[i].updates_per_client) / seconds;
            all_stats.invs_per_client[i] = (curr_c_stats[i].invs_per_client - prev_c_stats[i].invs_per_client) / seconds;
            all_stats.acks_per_client[i] = (curr_c_stats[i].acks_per_client - prev_c_stats[i].acks_per_client) / seconds;
            all_stats.received_updates_per_client[i] = (curr_c_stats[i].received_updates_per_client - prev_c_stats[i].received_updates_per_client) / seconds;
            all_stats.received_invs_per_client[i] = (curr_c_stats[i].received_invs_per_client - prev_c_stats[i].received_invs_per_client) / seconds;
            all_stats.received_acks_per_client[i] = (curr_c_stats[i].received_acks_per_client - prev_c_stats[i].received_acks_per_client) / seconds;
            if (curr_c_stats[i].remote_messages_per_client - prev_c_stats[i].remote_messages_per_client > 0) {
                all_stats.average_coalescing_per_client[i] =  ((curr_c_stats[i].remotes_per_client - prev_c_stats[i].remotes_per_client)
                                                               /(double) (curr_c_stats[i].remote_messages_per_client - prev_c_stats[i].remote_messages_per_client));
            }
            if (curr_c_stats[i].batches_per_client - prev_c_stats[i].batches_per_client > 0) {
                all_stats.batch_size_per_client[i] = (curr_c_stats[i].remotes_per_client - prev_c_stats[i].remotes_per_client) /
                                                     (double) (curr_c_stats[i].batches_per_client - prev_c_stats[i].batches_per_client);
                all_stats.stalled_time_per_client[i] = (curr_c_stats[i].stalled_time_per_client - prev_c_stats[i].stalled_time_per_client) /
                                                       (double)(curr_c_stats[i].batches_per_client - prev_c_stats[i].batches_per_client);
            }
            else {
                all_stats.batch_size_per_client[i] = 0;
                all_stats.stalled_time_per_client[i] = 0;
            }

            uint32_t total_loops = curr_c_stats[i].batches_per_client - prev_c_stats[i].batches_per_client +
                                   curr_c_stats[i].wasted_loops - prev_c_stats[i].wasted_loops;
            if (total_loops > 0)
                all_stats.empty_reqs_per_client[i] =  (curr_c_stats[i].tot_empty_reqs_per_trace - prev_c_stats[i].tot_empty_reqs_per_trace) / total_loops ;
        }

        // PER WORKER STATS
        for (i = 0; i < WORKERS_PER_MACHINE; i++) {
            all_workers_remotes += curr_w_stats[i].remotes_per_worker - prev_w_stats[i].remotes_per_worker;
            all_workers_locals += curr_w_stats[i].locals_per_worker - prev_w_stats[i].locals_per_worker;
            worker_throughput[i] = (curr_w_stats[i].remotes_per_worker - prev_w_stats[i].remotes_per_worker +
                                    curr_w_stats[i].locals_per_worker - prev_w_stats[i].locals_per_worker) / seconds;

            all_stats.remotes_per_worker[i] = (curr_w_stats[i].remotes_per_worker - prev_w_stats[i].remotes_per_worker) / seconds;
            all_stats.locals_per_worker[i] = (curr_w_stats[i].locals_per_worker - prev_w_stats[i].locals_per_worker) / seconds;
            if (curr_w_stats[i].batches_per_worker - prev_w_stats[i].batches_per_worker > 0) {
                all_stats.batch_size_per_worker[i] = (curr_w_stats[i].remotes_per_worker - prev_w_stats[i].remotes_per_worker) /
                                                     (double) (curr_w_stats[i].batches_per_worker - prev_w_stats[i].batches_per_worker);
                all_stats.aver_reqs_polled_per_worker[i] = (curr_w_stats[i].remotes_per_worker - prev_w_stats[i].remotes_per_worker) /
                                                           (double) (curr_w_stats[i].empty_polls_per_worker - prev_w_stats[i].empty_polls_per_worker +
                                                                     curr_w_stats[i].batches_per_worker - prev_w_stats[i].batches_per_worker);
            }
            else {
                all_stats.batch_size_per_worker[i] = 0;
                all_stats.aver_reqs_polled_per_worker[i] = 0;
            }
        }

        memcpy(prev_c_stats, curr_c_stats, CLIENTS_PER_MACHINE * (sizeof(struct client_stats)));
        memcpy(prev_w_stats, curr_w_stats, WORKERS_PER_MACHINE * (sizeof(struct worker_stats)));
        total_throughput = (all_clients_cache_hits + all_workers_remotes + all_workers_locals) / seconds;
        all_clients_throughput = all_clients_cache_hits / seconds;
        all_workers_throughput = (all_workers_remotes + all_workers_locals) / seconds;
        printf("---------------PRINT %d time elapsed %.5f---------------\n", print_count, seconds / MILLION);
        green_printf("SYSTEM MIOPS: %.5f Cache MIOPS: %.5f WORKER: MIOPS: %.5f \n",
                     total_throughput, all_clients_throughput, all_workers_throughput);
        for (i = 0; i < CLIENTS_PER_MACHINE; i++) {
            double cacheHitRate;
            double trace_ratio;
            long long total_reqs = curr_c_stats[i].cache_hits_per_client + curr_c_stats[i].remotes_per_client + curr_c_stats[i].locals_per_client;
            if (total_reqs > 0)
                trace_ratio =  curr_c_stats[i].cache_hits_per_client / (double)total_reqs;
            if (all_stats.remotes_per_client[i] > 0)
                cacheHitRate = all_stats.cache_hits_per_client[i] / (all_stats.cache_hits_per_client[i] + all_stats.remotes_per_client[i] + all_stats.locals_per_client[i]);
            yellow_printf("C%d: %.5f MIOPS-Batch %.5f(%.5f) -H %.5f -W %llu -E %.5f -AC %.5f  ", i, all_stats.cache_hits_per_client[i], all_stats.batch_size_per_client[i],
                          all_stats.stalled_time_per_client[i], trace_ratio, curr_c_stats[i].wasted_loops, all_stats.empty_reqs_per_client[i],
                          all_stats.average_coalescing_per_client[i]);
            if  (i > 0 && i % 2 == 0) printf("\n");
        }
        printf("\n");
        for (i = 0; i < WORKERS_PER_MACHINE; i++) {
            cyan_printf("WORKER %d: TOTAL: %.5f MIOPS, REMOTES: %.5f MIOPS, LOCALS: %.5f MIOPS, Batch %.5f(%.5f) \n",
                        i, worker_throughput[i], all_stats.remotes_per_worker[i], all_stats.locals_per_worker[i], all_stats.batch_size_per_worker[i],
                        all_stats.aver_reqs_polled_per_worker[i]);
        }
        printf("---------------------------------------\n");
        if(ENABLE_CACHE_STATS == 1)
            print_cache_stats(start, machine_id);
        // // Write to a file all_clients_throughput, per_worker_remote_throughput[], per_worker_local_throughput[]
        if(DUMP_STATS_2_FILE == 1)
            dump_stats_2_file(&all_stats);
        green_printf("SYSTEM MIOPS: %.5f Cache MIOPS: %.5f WORKER: MIOPS: %.5f \n",
                     total_throughput, all_clients_throughput, all_workers_throughput);

    }
}

#define FIRST_N_HOT_IN_WINDOW 10
void window_stats(struct extended_cache_op *op, struct mica_resp *resp) {
    int i = 0, j = 0;
    //struct cache_meta_stats meta;
    //cache_meta_reset(&meta);
    int window_hot_reads = 0;
    int window_nor_reads = 0;
    int window_nor_writes = 0;
    int window_hot_writes = 0;
    int hot_reads[FIRST_N_HOT_IN_WINDOW] = { 0 } ;
    int hot_writes[FIRST_N_HOT_IN_WINDOW] = { 0 } ;
    struct cache_key keys[FIRST_N_HOT_IN_WINDOW];
    uint32_t tmp;
    for(i = 0; i < FIRST_N_HOT_IN_WINDOW; i ++){
        tmp = (uint32_t) i;
        *(uint128 *) &keys[i] = CityHash128((char *) &(i), 4);
    }

    for(i = 0; i < CACHE_BATCH_SIZE; ++i) {
        for (j = 0; j < FIRST_N_HOT_IN_WINDOW; j++)
            if (keys_are_equal(&op[i].key, &keys[j]) == 1) {
                if (op[i].opcode == CACHE_OP_PUT)
                    hot_writes[j]++;
                else
                    hot_reads[j]++;
            }

        switch (resp[i].type) {
            case CACHE_GET_SUCCESS:
            case CACHE_GET_STALL:
                window_hot_reads++;
                break;
            case CACHE_PUT_SUCCESS:
            case CACHE_PUT_STALL:
                window_hot_writes++;
                break;
            case CACHE_MISS:
                if (op[i].opcode == CACHE_OP_GET)
                    window_nor_reads++;
                else if (op[i].opcode == CACHE_OP_PUT)
                    window_nor_writes++;
                else
                    assert(0);
                break;
        }
    }

    printf("Hot Reads in window: %d \n",window_hot_reads);
    for(i = 0; i < FIRST_N_HOT_IN_WINDOW; i++)
        printf("%d : %d,\t",i,hot_reads[i]);
    printf("\n");
    printf("Hot Writes in window: %d \n", window_hot_writes);
    for(i = 0; i < FIRST_N_HOT_IN_WINDOW; i++)
        printf("%d : %d,\t",i,hot_writes[i]);
    printf("\n");
    printf("Normal Reads %d Writes %d \n", window_nor_reads, window_nor_writes);
}

//assuming microsecond latency
void print_latency_stats(void){
    FILE *latency_stats_fd;
    int i = 0;
    char filename[128];
    char* path = "../../results/latency";
    const char * exectype[] = {
            "BS", //baseline
            "SC", //Sequential Consistency
            "LIN", //Linearizability (non stalling)
            "SS" //Strong Consistency (stalling)
    };

    sprintf(filename, "%s/latency_stats_%s_%s_%s_s_%d_a_%d_v_%d_m_%d_c_%d_w_%d_r_%d%s_C_%d.csv", path,
            DISABLE_CACHE == 1 ? "BS" : exectype[protocol],
            LOAD_BALANCE == 1 ? "UNIF" : "SKEW",
            EMULATING_CREW == 1 ? "CREW" : "EREW",
            DISABLE_CACHE == 0 && protocol == 2 && ENABLE_MULTIPLE_SESSIONS != 0 && SESSIONS_PER_CLIENT != 0 ? SESSIONS_PER_CLIENT: 0,
            SKEW_EXPONENT_A,
            USE_BIG_OBJECTS == 1 ? ((EXTRA_CACHE_LINES * 64) + BASE_VALUE_SIZE): BASE_VALUE_SIZE,
            MACHINE_NUM, CLIENTS_PER_MACHINE,
            WORKERS_PER_MACHINE, WRITE_RATIO,
            BALANCE_HOT_WRITES == 1  ? "_lbw" : "",
            CACHE_BATCH_SIZE);

    latency_stats_fd = fopen(filename, "w");
    fprintf(latency_stats_fd, "#---------------- Remote Reqs --------------\n");
    for(i = 0; i < LATENCY_BUCKETS; ++i)
        fprintf(latency_stats_fd, "rr: %d, %d\n",i * (MAX_LATENCY / LATENCY_BUCKETS), latency_count.remote_reqs[i]);
    fprintf(latency_stats_fd, "rr: -1, %d\n",latency_count.remote_reqs[LATENCY_BUCKETS]); //print outliers

    fprintf(latency_stats_fd, "#---------------- Local Reqs ---------------\n");
    for(i = 0; i < LATENCY_BUCKETS; ++i)
        fprintf(latency_stats_fd, "lr: %d, %d\n",i * (MAX_LATENCY / LATENCY_BUCKETS), latency_count.local_reqs[i]);
    fprintf(latency_stats_fd, "lr: -1, %d\n",latency_count.local_reqs[LATENCY_BUCKETS]); //print outliers

    fprintf(latency_stats_fd, "#---------------- Hot Reads ----------------\n");
    for(i = 0; i < LATENCY_BUCKETS; ++i)
        fprintf(latency_stats_fd, "hr: %d, %d\n",i * (MAX_LATENCY / LATENCY_BUCKETS), latency_count.hot_reads[i]);
    fprintf(latency_stats_fd, "hr: -1, %d\n",latency_count.hot_reads[LATENCY_BUCKETS]); //print outliers

    fprintf(latency_stats_fd, "#---------------- Hot Writes ---------------\n");
    for(i = 0; i < LATENCY_BUCKETS; ++i)
        fprintf(latency_stats_fd, "hw: %d, %d\n",i * (MAX_LATENCY / LATENCY_BUCKETS), latency_count.hot_writes[i]);
    fprintf(latency_stats_fd, "hw: -1, %d\n",latency_count.hot_writes[LATENCY_BUCKETS]); //print outliers

    fclose(latency_stats_fd);

    printf("Latency stats saved at %s\n", filename);
}
