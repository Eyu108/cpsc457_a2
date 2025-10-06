#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PROCESSES 1000
#define MAX_LINE 256

typedef struct {
    int pid;
    int arrival_time;
    int time_until_first_response;
    int burst_length;
    int start_time;
    int finish_time;
    int turnaround_time;
    int waiting_time;
    int response_time;
} Process;

// Function to parse CSV line
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

// FCFS simulation
void simulate_fcfs(Process processes[], int n, int latency) {
    int current_time = 0;
    
    for (int i = 0; i < n; i++) {
        // Wait for process to arrive if CPU is idle
        if (current_time < processes[i].arrival_time) {
            current_time = processes[i].arrival_time;
        }
        
        // Add dispatcher latency
        current_time += latency;
        
        // Start time is when process actually begins execution
        processes[i].start_time = current_time;
        
        // Execute the process
        current_time += processes[i].burst_length;
        
        // Finish time
        processes[i].finish_time = current_time;
        
        // Calculate metrics
        processes[i].turnaround_time = processes[i].finish_time - processes[i].arrival_time;
        processes[i].waiting_time = processes[i].start_time - processes[i].arrival_time;
        processes[i].response_time = processes[i].time_until_first_response;
    }
}

// Calculate and write results for specific latency
void write_detail_results(FILE *fp, int latency, Process processes[], int n) {
    for (int i = 0; i < n; i++) {
        fprintf(fp, "%d,%d,%d,%d,%d,%d,%d,%d\n",
                latency,
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
    FILE *detail_fp = fopen("fcfs_results_details.csv", "w");
    FILE *summary_fp = fopen("fcfs_results.csv", "w");
    
    if (!detail_fp || !summary_fp) {
        fprintf(stderr, "Error opening output files\n");
        return 1;
    }
    
    // Write headers
    fprintf(detail_fp, "Scheduler_Latency,Pid,Arrival_Time,Start_Time,Finish_Time,Turnaround_Time,Waiting_Time,Response_Time\n");
    fprintf(summary_fp, "Scheduler_Latency,Throughput,Avg_Waiting_Time,Avg_Turnaround_Time,Avg_Response_Time\n");
    
    // Run simulations for latency 1 to 200
    for (int latency = 1; latency <= 200; latency++) {
        // Create a copy of processes for this simulation
        Process sim_processes[MAX_PROCESSES];
        memcpy(sim_processes, processes, n * sizeof(Process));
        
        // Run simulation
        simulate_fcfs(sim_processes, n, latency);
        
        // Write detailed results for this latency
        write_detail_results(detail_fp, latency, sim_processes, n);
        
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
                latency, throughput, avg_waiting, avg_turnaround, avg_response);
        
        // Print progress
        if (latency % 50 == 0 || latency == 1) {
            printf("Completed latency %d: Throughput=%.6f, Avg_Wait=%.2f, Avg_TAT=%.2f, Avg_RT=%.2f\n",
                   latency, throughput, avg_waiting, avg_turnaround, avg_response);
        }
    }
    
    fclose(detail_fp);
    fclose(summary_fp);
    
    printf("\nSimulation completed! Process table results saved to fcfs_results_detail.csv\n");
    printf("Average results saved to fcfs_results.csv\n");
    
    return 0;
}