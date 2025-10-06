#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PROCESSES 1000
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
    int turnaround_time;
    int waiting_time;
    int response_time;
    int first_run;
} Process;

typedef struct {
    int process_idx[MAX_PROCESSES];
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
    q->rear = (q->rear + 1) % MAX_PROCESSES;
    q->process_idx[q->rear] = idx;
    q->size++;
}

int dequeue(Queue *q) {
    if (q->size == 0) return -1;
    int idx = q->process_idx[q->front];
    q->front = (q->front + 1) % MAX_PROCESSES;
    q->size--;
    return idx;
}

int is_empty(Queue *q) {
    return q->size == 0;
}

int parse_line(char *line, Process *p) {
    char *token;
    int field = 0;
    
    token = strtok(line, ",");
    while (token != NULL && field < 4) {
        switch(field) {
            case 0: p->pid = atoi(token); break;
            case 1: p->arrival_time = atoi(token); break;
            case 2: p->time_until_first_response = atoi(token); break;
            case 3: p->burst_length = atoi(token); break;
        }
        token = strtok(NULL, ",");
        field++;
    }
    return field == 4;
}

void simulate_rr(Process processes[], int n, int quantum) {
    Queue ready_queue;
    init_queue(&ready_queue);
    
    int current_time = 0;
    int completed = 0;
    int next_arrival_idx = 0;
    
    // Initialize processes
    for (int i = 0; i < n; i++) {
        processes[i].remaining_time = processes[i].burst_length;
        processes[i].first_run = 1;
        processes[i].start_time = -1;
    }
    
    // Add first process(es) that arrive at time 0
    while (next_arrival_idx < n && processes[next_arrival_idx].arrival_time <= current_time) {
        enqueue(&ready_queue, next_arrival_idx);
        next_arrival_idx++;
    }
    
    while (completed < n) {
        if (is_empty(&ready_queue)) {
            // CPU idle, jump to next arrival
            if (next_arrival_idx < n) {
                current_time = processes[next_arrival_idx].arrival_time;
                while (next_arrival_idx < n && processes[next_arrival_idx].arrival_time <= current_time) {
                    enqueue(&ready_queue, next_arrival_idx);
                    next_arrival_idx++;
                }
            }
            continue;
        }
        
        // Add dispatcher latency
        current_time += LATENCY;
        
        // Get next process from queue
        int idx = dequeue(&ready_queue);
        
        // Record start time if first run
        if (processes[idx].first_run) {
            processes[idx].start_time = current_time;
            processes[idx].first_run = 0;
        }
        
        // Execute for quantum or remaining time, whichever is smaller
        int exec_time = (processes[idx].remaining_time < quantum) ? 
                        processes[idx].remaining_time : quantum;
        
        processes[idx].remaining_time -= exec_time;
        current_time += exec_time;
        
        // Check for new arrivals during execution
        while (next_arrival_idx < n && processes[next_arrival_idx].arrival_time <= current_time) {
            enqueue(&ready_queue, next_arrival_idx);
            next_arrival_idx++;
        }
        
        // Check if process completed
        if (processes[idx].remaining_time == 0) {
            processes[idx].finish_time = current_time;
            processes[idx].turnaround_time = processes[idx].finish_time - processes[idx].arrival_time;
            processes[idx].waiting_time = processes[idx].turnaround_time - processes[idx].burst_length;
            processes[idx].response_time = processes[idx].time_until_first_response;
            completed++;
        } else {
            // Process not finished, add back to queue
            enqueue(&ready_queue, idx);
        }
    }
}

void write_detail_results(FILE *fp, int quantum, Process processes[], int n) {
    for (int i = 0; i < n; i++) {
        fprintf(fp, "%d,%d,%d,%d,%d,%d,%d,%d\n",
                quantum,
                processes[i].pid,
                processes[i].arrival_time,
                processes[i].start_time,
                processes[i].finish_time,
                processes[i].turnaround_time,
                processes[i].waiting_time,
                processes[i].response_time);
    }
}

int main() {
    Process processes[MAX_PROCESSES];
    int n = 0;
    char line[MAX_LINE];
    
    // Read header
    if (fgets(line, MAX_LINE, stdin) == NULL) {
        fprintf(stderr, "Error reading header\n");
        return 1;
    }
    
    // Read all processes
    while (fgets(line, MAX_LINE, stdin) != NULL && n < MAX_PROCESSES) {
        if (parse_line(line, &processes[n])) {
            n++;
        }
    }
    
    if (n == 0) {
        fprintf(stderr, "No processes read\n");
        return 1;
    }
    
    printf("Read %d processes\n", n);
    
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
        // Create a copy of processes for this simulation
        Process sim_processes[MAX_PROCESSES];
        memcpy(sim_processes, processes, n * sizeof(Process));
        
        // Run simulation
        simulate_rr(sim_processes, n, quantum);
        
        // Write detailed results
        write_detail_results(detail_fp, quantum, sim_processes, n);
        
        // Calculate average metrics
        double total_waiting = 0, total_turnaround = 0, total_response = 0;
        int max_finish_time = 0;
        
        for (int i = 0; i < n; i++) {
            total_waiting += sim_processes[i].waiting_time;
            total_turnaround += sim_processes[i].turnaround_time;
            total_response += sim_processes[i].response_time;
            if (sim_processes[i].finish_time > max_finish_time) {
                max_finish_time = sim_processes[i].finish_time;
            }
        }
        
        double avg_waiting = total_waiting / n;
        double avg_turnaround = total_turnaround / n;
        double avg_response = total_response / n;
        double throughput = (double)n / max_finish_time;
        
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
    printf("Average results saved to rr_results_detail.csv\n");
    
    return 0;
}
