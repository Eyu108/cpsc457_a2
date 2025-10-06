import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

def plot_fcfs_results():
    """Plot FCFS results with latency vs metrics"""
    # Read the results
    df = pd.read_csv('fcfs_results.csv')
    
    # Create figure with 4 subplots
    fig, axes = plt.subplots(2, 2, figsize=(14, 10))
    fig.suptitle('FCFS Scheduling: Impact of Scheduler/Dispatcher Latency', fontsize=16, fontweight='bold')
    
    # Plot 1: Throughput
    axes[0, 0].plot(df['Scheduler_Latency'], df['Throughput'], 'b-', linewidth=2, label='Throughput')
    axes[0, 0].set_xlabel('Scheduler/Dispatcher Latency (time units)', fontsize=11)
    axes[0, 0].set_ylabel('Throughput (processes/time unit)', fontsize=11)
    axes[0, 0].set_title('Throughput vs Latency', fontsize=12, fontweight='bold')
    axes[0, 0].grid(True, alpha=0.3)
    axes[0, 0].legend()
    
    # Plot 2: Average Waiting Time
    axes[0, 1].plot(df['Scheduler_Latency'], df['Avg_Waiting_Time'], 'r-', linewidth=2, label='Avg Waiting Time')
    axes[0, 1].set_xlabel('Scheduler/Dispatcher Latency (time units)', fontsize=11)
    axes[0, 1].set_ylabel('Average Waiting Time (time units)', fontsize=11)
    axes[0, 1].set_title('Average Waiting Time vs Latency', fontsize=12, fontweight='bold')
    axes[0, 1].grid(True, alpha=0.3)
    axes[0, 1].legend()
    
    # Plot 3: Average Turnaround Time
    axes[1, 0].plot(df['Scheduler_Latency'], df['Avg_Turnaround_Time'], 'g-', linewidth=2, label='Avg Turnaround Time')
    axes[1, 0].set_xlabel('Scheduler/Dispatcher Latency (time units)', fontsize=11)
    axes[1, 0].set_ylabel('Average Turnaround Time (time units)', fontsize=11)
    axes[1, 0].set_title('Average Turnaround Time vs Latency', fontsize=12, fontweight='bold')
    axes[1, 0].grid(True, alpha=0.3)
    axes[1, 0].legend()
    
    # Plot 4: Average Response Time
    axes[1, 1].plot(df['Scheduler_Latency'], df['Avg_Response_Time'], 'm-', linewidth=2, label='Avg Response Time')
    axes[1, 1].set_xlabel('Scheduler/Dispatcher Latency (time units)', fontsize=11)
    axes[1, 1].set_ylabel('Average Response Time (time units)', fontsize=11)
    axes[1, 1].set_title('Average Response Time vs Latency', fontsize=12, fontweight='bold')
    axes[1, 1].grid(True, alpha=0.3)
    axes[1, 1].legend()
    
    plt.tight_layout()
    plt.savefig('fcfs_plot.png', dpi=300, bbox_inches='tight')
    print("FCFS plot saved as fcfs_plot.png")
    plt.show()

def plot_rr_results():
    """Plot Round Robin results with quantum size vs metrics"""
    # Read the results
    df = pd.read_csv('rr_results.csv')
    
    # Create figure with 4 subplots
    fig, axes = plt.subplots(2, 2, figsize=(14, 10))
    fig.suptitle('Round Robin Scheduling: Impact of Quantum Size', fontsize=16, fontweight='bold')
    
    # Plot 1: Throughput
    axes[0, 0].plot(df['Quantum_Size'], df['Throughput'], 'b-', linewidth=2, label='Throughput')
    axes[0, 0].set_xlabel('Quantum Size (time units)', fontsize=11)
    axes[0, 0].set_ylabel('Throughput (processes/time unit)', fontsize=11)
    axes[0, 0].set_title('Throughput vs Quantum Size', fontsize=12, fontweight='bold')
    axes[0, 0].grid(True, alpha=0.3)
    axes[0, 0].legend()
    
    # Plot 2: Average Waiting Time
    axes[0, 1].plot(df['Quantum_Size'], df['Avg_Waiting_Time'], 'r-', linewidth=2, label='Avg Waiting Time')
    axes[0, 1].set_xlabel('Quantum Size (time units)', fontsize=11)
    axes[0, 1].set_ylabel('Average Waiting Time (time units)', fontsize=11)
    axes[0, 1].set_title('Average Waiting Time vs Quantum Size', fontsize=12, fontweight='bold')
    axes[0, 1].grid(True, alpha=0.3)
    axes[0, 1].legend()
    
    # Plot 3: Average Turnaround Time
    axes[1, 0].plot(df['Quantum_Size'], df['Avg_Turnaround_Time'], 'g-', linewidth=2, label='Avg Turnaround Time')
    axes[1, 0].set_xlabel('Quantum Size (time units)', fontsize=11)
    axes[1, 0].set_ylabel('Average Turnaround Time (time units)', fontsize=11)
    axes[1, 0].set_title('Average Turnaround Time vs Quantum Size', fontsize=12, fontweight='bold')
    axes[1, 0].grid(True, alpha=0.3)
    axes[1, 0].legend()
    
    # Plot 4: Average Response Time
    axes[1, 1].plot(df['Quantum_Size'], df['Avg_Response_Time'], 'm-', linewidth=2, label='Avg Response Time')
    axes[1, 1].set_xlabel('Quantum Size (time units)', fontsize=11)
    axes[1, 1].set_ylabel('Average Response Time (time units)', fontsize=11)
    axes[1, 1].set_title('Average Response Time vs Quantum Size', fontsize=12, fontweight='bold')
    axes[1, 1].grid(True, alpha=0.3)
    axes[1, 1].legend()
    
    plt.tight_layout()
    plt.savefig('rr_plot.png', dpi=300, bbox_inches='tight')
    print("Round Robin plot saved as rr_plot.png")
    plt.show()

def plot_combined_metrics():
    """Create combined plot showing all metrics on same graph for FCFS"""
    df = pd.read_csv('fcfs_results.csv')
    
    fig, ax = plt.subplots(figsize=(12, 7))
    
    # Normalize throughput for better visualization (scale it up)
    throughput_scaled = df['Throughput'] * 10000
    
    ax.plot(df['Scheduler_Latency'], throughput_scaled, 'b-', linewidth=2, label='Throughput (×10000)', marker='o', markersize=3, markevery=20)
    ax.plot(df['Scheduler_Latency'], df['Avg_Waiting_Time'], 'r-', linewidth=2, label='Avg Waiting Time', marker='s', markersize=3, markevery=20)
    ax.plot(df['Scheduler_Latency'], df['Avg_Turnaround_Time'], 'g-', linewidth=2, label='Avg Turnaround Time', marker='^', markersize=3, markevery=20)
    ax.plot(df['Scheduler_Latency'], df['Avg_Response_Time'], 'm-', linewidth=2, label='Avg Response Time', marker='d', markersize=3, markevery=20)
    
    ax.set_xlabel('Scheduler/Dispatcher Latency (time units)', fontsize=12)
    ax.set_ylabel('Metric Value', fontsize=12)
    ax.set_title('FCFS Scheduling: All Metrics vs Scheduler/Dispatcher Latency', fontsize=14, fontweight='bold')
    ax.grid(True, alpha=0.3)
    ax.legend(loc='best', fontsize=10)
    
    plt.tight_layout()
    plt.savefig('fcfs_combined_plot.png', dpi=300, bbox_inches='tight')
    print("FCFS combined plot saved as fcfs_combined_plot.png")
    plt.show()

def plot_rr_combined_metrics():
    """Create combined plot showing all metrics on same graph for RR"""
    df = pd.read_csv('rr_results.csv')
    
    fig, ax = plt.subplots(figsize=(12, 7))
    
    # Normalize throughput for better visualization
    throughput_scaled = df['Throughput'] * 10000
    
    ax.plot(df['Quantum_Size'], throughput_scaled, 'b-', linewidth=2, label='Throughput (×10000)', marker='o', markersize=3, markevery=20)
    ax.plot(df['Quantum_Size'], df['Avg_Waiting_Time'], 'r-', linewidth=2, label='Avg Waiting Time', marker='s', markersize=3, markevery=20)
    ax.plot(df['Quantum_Size'], df['Avg_Turnaround_Time'], 'g-', linewidth=2, label='Avg Turnaround Time', marker='^', markersize=3, markevery=20)
    ax.plot(df['Quantum_Size'], df['Avg_Response_Time'], 'm-', linewidth=2, label='Avg Response Time', marker='d', markersize=3, markevery=20)
    
    ax.set_xlabel('Quantum Size (time units)', fontsize=12)
    ax.set_ylabel('Metric Value', fontsize=12)
    ax.set_title('Round Robin Scheduling: All Metrics vs Quantum Size', fontsize=14, fontweight='bold')
    ax.grid(True, alpha=0.3)
    ax.legend(loc='best', fontsize=10)
    
    plt.tight_layout()
    plt.savefig('rr_combined_plot.png', dpi=300, bbox_inches='tight')
    print("RR combined plot saved as rr_combined_plot.png")
    plt.show()

if __name__ == "__main__":
    print("Generating plots...")
    print("\n1. FCFS Plots")
    plot_fcfs_results()
    plot_combined_metrics()
    
    print("\n2. Round Robin Plots")
    plot_rr_results()
    plot_rr_combined_metrics()
    
    print("\nAll plots generated successfully!")