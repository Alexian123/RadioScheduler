import matplotlib.pyplot as plt
import pandas as pd
import sys

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python script_name.py <csv_file_path>")
        sys.exit(1)

    csv_file_path = sys.argv[1]
    
    try:
        df = pd.read_csv(csv_file_path)
    except FileNotFoundError:
        print(f"Error: CSV file not found at '{csv_file_path}'")
        sys.exit(1)

    # Extract filename without extension for prefix
    output_prefix = csv_file_path.split('.')[0] 

    # --- Plot Network Load and Delay vs. Time ---
    fig, ax1 = plt.subplots()

    color = 'tab:red'
    ax1.set_xlabel('Time')
    ax1.set_ylabel('Network Load', color=color)
    ax1.plot(df['Time'], df['NetworkLoad'], color=color)
    ax1.tick_params(axis='y', labelcolor=color)

    ax2 = ax1.twinx()
    color = 'tab:blue'
    ax2.set_ylabel('Delay', color=color)
    ax2.plot(df['Time'], df['Delay'], color=color)
    ax2.tick_params(axis='y', labelcolor=color)

    plt.title('Network Load and Delay vs. Time')
    plt.savefig(f'{output_prefix}_graph.png')