#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PROCESSES 1000
#define MAX_LINE 256
#define LATENCY 20
#define QUANTUM_Q1 40
#define QUANTUM_Q2 80

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
    int current_queue;  // 0=Q1, 1=Q2, 2=Q3
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

void simulate_mlfq(Process processes[], int n) {
    Queue q1, q2, q3;
    init_queue(&q1);
    init_queue(&q2);
    init_queue(&q3);
    
    int current_time = 0;
    int completed = 0;
    int next_arrival_idx = 0;
    
    // Initialize processes
    for (int i = 0; i < n; i++) {
        processes[i].remaining_time = processes[i].burst_length;
        processes[i].first_run = 1;
        processes[i].start_time = -1;
        processes[i].current_queue = 0;  // All start in Q1
    }
    
    // Add first process(es) that arrive at time 0
    while (next_arrival_idx < n && processes[next_arrival_idx].arrival_time <= current_time) {
        enqueue(&q1, next_arrival_idx);
        next_arrival_idx++;
    }
    
    while (completed < n) {
        int idx = -1;
        int quantum = 0;
        Queue *current_q = NULL;
        
        // Priority: Q1 > Q2 > Q3
        if (!is_empty(&q1)) {
            current_q = &q1;
            idx = dequeue(&q1);
            quantum = QUANTUM_Q1;
        } else if (!is_empty(&q2)) {
            current_q = &q2;
            idx = dequeue(&q2);
            quantum = QUANTUM_Q2;
        } else if (!is_empty(&q3)) {
            current_q = &q3;
            idx = dequeue(&q3);
            quantum = processes[idx].remaining_time;  // FCFS in Q3
        } else {
            // CPU idle, jump to next arrival
            if (next_arrival_idx < n) {
                current_time = processes[next_arrival_idx].arrival_time;
                while (next_arrival_idx < n && processes[next_arrival_idx].arrival_time <= current_time) {
                    enqueue(&q1, next_arrival_idx);
                    next_arrival_idx++;
                }
            }
            continue;
        }
        
        // Add dispatcher latency
        current_time += LATENCY;
        
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
            enqueue(&q1, next_arrival_idx);
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
            // Process not finished, demote to next queue
            if (processes[idx].current_queue == 0) {
                // Used full quantum in Q1, move to Q2
                processes[idx].current_queue = 1;
                enqueue(&q2, idx);
            } else if (processes[idx].current_queue == 1) {
                // Used full quantum in Q2, move to Q3
                processes[idx].current_queue = 2;
                enqueue(&q3, idx);
            } else {
                // Already in Q3, stay in Q3
                enqueue(&q3, idx);
            }
        }
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
    
    // Run simulation
    simulate_mlfq(processes, n);
    
    // Calculate average metrics
    double total_waiting = 0, total_turnaround = 0, total_response = 0;
    int max_finish_time = 0;
    
    for (int i = 0; i < n; i++) {
        total_waiting += processes[i].waiting_time;
        total_turnaround += processes[i].turnaround_time;
        total_response += processes[i].response_time;
        if (processes[i].finish_time > max_finish_time) {
            max_finish_time = processes[i].finish_time;
        }
    }
    
    double avg_waiting = total_waiting / n;
    double avg_turnaround = total_turnaround / n;
    double avg_response = total_response / n;
    double throughput = (double)n / max_finish_time;
    
    // Print results to terminal
    printf("\nThroughput,Avg_Waiting_Time,Avg_Turnaround_Time,Avg_Response_Time\n");
    printf("%.6f,%.2f,%.2f,%.2f\n", throughput, avg_waiting, avg_turnaround, avg_response);
    
    return 0;
}