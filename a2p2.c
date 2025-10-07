#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_THREADS 1000
#define MAX_PROCESSES 50
#define MAX_LINE 256
#define LATENCY 20

typedef struct {
    int pid;
    int arrival_time;
    int time_until_first_response;
    int burst_length;
    int remaining_time;
    int start_time;
    int finish_time;
    int first_response_time;
    int first_run;
    int response_happened;
} Thread;

typedef struct {
    int pid;
    int earliest_arrival;
    int latest_finish;
    int first_start;
    int total_burst;
    int turnaround_time;
    int waiting_time;
    int response_time;
    int has_response;
} Process;

typedef struct {
    int thread_idx[MAX_THREADS];
    int front;
    int rear;
    int size;
} Queue;

void init_queue(Queue *q) {
    q->front = 0;
    q->rear = -1;
    q->size = 0;
}

void enqueue(Queue *q, int idx) {
    q->rear = (q->rear + 1) % MAX_THREADS;
    q->thread_idx[q->rear] = idx;
    q->size++;
}

int dequeue(Queue *q) {
    if (q->size == 0) return -1;
    int idx = q->thread_idx[q->front];
    q->front = (q->front + 1) % MAX_THREADS;
    q->size--;
    return idx;
}

int is_empty(Queue *q) {
    return q->size == 0;
}

int parse_line(char *line, Thread *t) {
    char *token;
    int field = 0;
    
    token = strtok(line, ",");
    while (token != NULL && field < 4) {
        switch(field) {
            case 0: t->pid = atoi(token); break;
            case 1: t->arrival_time = atoi(token); break;
            case 2: t->time_until_first_response = atoi(token); break;
            case 3: t->burst_length = atoi(token); break;
        }
        token = strtok(NULL, ",");
        field++;
    }
    return field == 4;
}

void simulate_rr(Thread threads[], int n, int quantum) {
    Queue ready_queue;
    init_queue(&ready_queue);
    
    int current_time = 0;
    int completed = 0;
    int next_arrival_idx = 0;
    
    // Initialize threads
    for (int i = 0; i < n; i++) {
        threads[i].remaining_time = threads[i].burst_length;
        threads[i].first_run = 1;
        threads[i].start_time = -1;
        threads[i].response_happened = 0;
        threads[i].first_response_time = -1;
    }
    
    // Add threads that arrive at time 0
    while (next_arrival_idx < n && threads[next_arrival_idx].arrival_time <= current_time) {
        enqueue(&ready_queue, next_arrival_idx);
        next_arrival_idx++;
    }
    
    while (completed < n) {
        if (is_empty(&ready_queue)) {
            // CPU idle, jump to next arrival
            if (next_arrival_idx < n) {
                current_time = threads[next_arrival_idx].arrival_time;
                while (next_arrival_idx < n && threads[next_arrival_idx].arrival_time <= current_time) {
                    enqueue(&ready_queue, next_arrival_idx);
                    next_arrival_idx++;
                }
            }
            continue;
        }
        
        // Add dispatcher latency
        current_time += LATENCY;
        
        // Get next thread from queue
        int idx = dequeue(&ready_queue);
        
        // Record start time if first run
        if (threads[idx].first_run) {
            threads[idx].start_time = current_time;
            threads[idx].first_run = 0;
        }
        
        // Execute for quantum or remaining time, whichever is smaller
        int exec_time = (threads[idx].remaining_time < quantum) ? 
                        threads[idx].remaining_time : quantum;
        
        // Check if response happens during this execution
        if (!threads[idx].response_happened && 
            threads[idx].time_until_first_response < exec_time) {
            threads[idx].first_response_time = current_time + threads[idx].time_until_first_response;
            threads[idx].response_happened = 1;
        }
        
        threads[idx].remaining_time -= exec_time;
        current_time += exec_time;
        
        // Check for new arrivals during execution
        while (next_arrival_idx < n && threads[next_arrival_idx].arrival_time <= current_time) {
            enqueue(&ready_queue, next_arrival_idx);
            next_arrival_idx++;
        }
        
        // Check if thread completed
        if (threads[idx].remaining_time == 0) {
            threads[idx].finish_time = current_time;
            // If response never happened, set it to finish time
            if (!threads[idx].response_happened) {
                threads[idx].first_response_time = current_time;
            }
            completed++;
        } else {
            // Thread not finished, add back to queue
            enqueue(&ready_queue, idx);
        }
    }
}

void aggregate_by_pid(Thread threads[], int n, Process processes[], int *num_processes) {
    *num_processes = 0;
    
    // Initialize all processes as not found
    for (int i = 0; i < MAX_PROCESSES; i++) {
        processes[i].pid = -1;
        processes[i].has_response = 0;
        processes[i].first_start = -1;
    }
    
    // Aggregate threads by PID
    for (int i = 0; i < n; i++) {
        int pid = threads[i].pid;
        
        // Find or create process entry
        int proc_idx = -1;
        for (int j = 0; j < *num_processes; j++) {
            if (processes[j].pid == pid) {
                proc_idx = j;
                break;
            }
        }
        
        if (proc_idx == -1) {
            // New process
            proc_idx = *num_processes;
            processes[proc_idx].pid = pid;
            processes[proc_idx].earliest_arrival = threads[i].arrival_time;
            processes[proc_idx].latest_finish = threads[i].finish_time;
            processes[proc_idx].first_start = threads[i].start_time;
            processes[proc_idx].total_burst = threads[i].burst_length;
            processes[proc_idx].response_time = threads[i].first_response_time - threads[i].arrival_time;
            processes[proc_idx].has_response = 1;
            (*num_processes)++;
        } else {
            // Update existing process
            if (threads[i].arrival_time < processes[proc_idx].earliest_arrival) {
                processes[proc_idx].earliest_arrival = threads[i].arrival_time;
            }
            if (threads[i].finish_time > processes[proc_idx].latest_finish) {
                processes[proc_idx].latest_finish = threads[i].finish_time;
            }
            if (processes[proc_idx].first_start == -1 || threads[i].start_time < processes[proc_idx].first_start) {
                processes[proc_idx].first_start = threads[i].start_time;
            }
            processes[proc_idx].total_burst += threads[i].burst_length;
            
            // Update response time if this thread has earlier first response
            int thread_response = threads[i].first_response_time - processes[proc_idx].earliest_arrival;
            if (!processes[proc_idx].has_response || thread_response < processes[proc_idx].response_time) {
                processes[proc_idx].response_time = thread_response;
                processes[proc_idx].has_response = 1;
            }
        }
    }
    
    // Calculate turnaround and waiting for each process
    for (int i = 0; i < *num_processes; i++) {
        processes[i].turnaround_time = processes[i].latest_finish - processes[i].earliest_arrival;
        processes[i].waiting_time = processes[i].turnaround_time - processes[i].total_burst;
    }
}

void write_detail_results(FILE *fp, int quantum, Process processes[], int num_processes) {
    for (int i = 0; i < num_processes; i++) {
        fprintf(fp, "%d,%d,%d,%d,%d,%d,%d,%d\n",
                quantum,
                processes[i].pid,
                processes[i].earliest_arrival,
                processes[i].first_start,
                processes[i].latest_finish,
                processes[i].turnaround_time,
                processes[i].waiting_time,
                processes[i].response_time);
    }
}

int main() {
    Thread threads[MAX_THREADS];
    int n = 0;
    char line[MAX_LINE];
    
    // Read header
    if (fgets(line, MAX_LINE, stdin) == NULL) {
        fprintf(stderr, "Error reading header\n");
        return 1;
    }
    
    // Read all threads
    while (fgets(line, MAX_LINE, stdin) != NULL && n < MAX_THREADS) {
        if (parse_line(line, &threads[n])) {
            n++;
        }
    }
    
    if (n == 0) {
        fprintf(stderr, "No threads read\n");
        return 1;
    }
    
    printf("Read %d threads\n", n);
    
    // Open output files
    FILE *detail_fp = fopen("rr_results_details.csv", "w");
    FILE *summary_fp = fopen("rr_results.csv", "w");
    
    if (!detail_fp || !summary_fp) {
        fprintf(stderr, "Error opening output files\n");
        return 1;
    }
    
    // Write headers
    fprintf(detail_fp, "Quantum_Size,Pid,Arrival_Time,Start_Time,Finish_Time,Turnaround_Time,Waiting_Time,Response_Time\n");
    fprintf(summary_fp, "Quantum_Size,Throughput,Avg_Waiting_Time,Avg_Turnaround_Time,Avg_Response_Time\n");
    
    // Run simulations for quantum 1 to 200
    for (int quantum = 1; quantum <= 200; quantum++) {
        // Create a copy of threads for this simulation
        Thread sim_threads[MAX_THREADS];
        memcpy(sim_threads, threads, n * sizeof(Thread));
        
        // Run simulation
        simulate_rr(sim_threads, n, quantum);
        
        // Aggregate by PID
        Process processes[MAX_PROCESSES];
        int num_processes = 0;
        aggregate_by_pid(sim_threads, n, processes, &num_processes);
        
        // Write detailed results
        write_detail_results(detail_fp, quantum, processes, num_processes);
        
        // Calculate average metrics over PROCESSES (not threads)
        double total_waiting = 0, total_turnaround = 0, total_response = 0;
        int max_finish_time = 0;
        
        for (int i = 0; i < num_processes; i++) {
            total_waiting += processes[i].waiting_time;
            total_turnaround += processes[i].turnaround_time;
            total_response += processes[i].response_time;
            if (processes[i].latest_finish > max_finish_time) {
                max_finish_time = processes[i].latest_finish;
            }
        }
        
        double avg_waiting = total_waiting / num_processes;
        double avg_turnaround = total_turnaround / num_processes;
        double avg_response = total_response / num_processes;
        double throughput = (double)num_processes / max_finish_time;
        
        // Write summary results
        fprintf(summary_fp, "%d,%.6f,%.2f,%.2f,%.2f\n",
                quantum, throughput, avg_waiting, avg_turnaround, avg_response);
        
        // Print progress
        if (quantum % 50 == 0 || quantum == 1) {
            printf("Completed quantum %d: Throughput=%.6f, Avg_Wait=%.2f, Avg_TAT=%.2f, Avg_RT=%.2f\n",
                   quantum, throughput, avg_waiting, avg_turnaround, avg_response);
        }
    }
    
    fclose(detail_fp);
    fclose(summary_fp);
    
    printf("\nRR simulation completed! Results saved to rr_results.csv\n");
    printf("Average results saved to rr_results_details.csv\n");
    
    return 0;
}