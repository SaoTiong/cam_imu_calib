import sys
from pathlib import Path

import rosbag
import matplotlib

matplotlib.use("Agg")  # For Docker/headless setups
import matplotlib.pyplot as plt
import numpy as np


# Usage: python3 sample_plot.py <your_bag_file.bag> [topic] [duration_s]
if len(sys.argv) < 2:
    print("Usage: python3 sample_plot.py <your_bag_file.bag> [topic] [duration_s]")
    sys.exit(1)

bag_file = sys.argv[1]
topic = sys.argv[2] if len(sys.argv) > 2 else "/icm20948/imu"
duration_limit = float(sys.argv[3]) if len(sys.argv) > 3 else 80.0

bag_path = Path(bag_file)
print(f"Reading first {duration_limit:.1f} seconds of {bag_file} (topic: {topic})...")
timestamps = []
start_time = None

# 1. Read Data
try:
    bag = rosbag.Bag(bag_file)
    for _, msg, t in bag.read_messages(topics=[topic]):
        if getattr(msg, "_has_header", False):
            t_sec = msg.header.stamp.to_sec()
        else:
            t_sec = t.to_sec()
        if start_time is None:
            start_time = t_sec
        if (t_sec - start_time) > duration_limit:
            break
        timestamps.append(t_sec)
    bag.close()
except Exception as e:
    print(f"Error: {e}")
    sys.exit(1)

if len(timestamps) < 2:
    print("Not enough data.")
    sys.exit(1)

# 2. Calculate dt
timestamps = np.array(timestamps)
dts = np.diff(timestamps)
times = timestamps[1:] - timestamps[0]

# Statistics for the console
avg_dt = np.mean(dts)
std_dt = np.std(dts)
avg_ms = avg_dt * 1000.0
std_ms = std_dt * 1000.0

print(f"Avg dt: {avg_dt:.6f} s | Rate: {1.0/avg_dt:.2f} Hz | Std: {std_dt:.6f} s")

# 3. Generate Scatter Plot
print("Generating scatter plot...")
fig, ax = plt.subplots(figsize=(10, 6))

rate_ms = dts * 1000.0
ax.scatter(times, rate_ms, s=5, color="blue", alpha=0.7)

ax.axhline(y=avg_ms, color="navy", linewidth=1.2)

topic_label = topic.strip("/").replace("/", "_") or "imu0"
ax.set_title(f"{topic_label}: sample inertial rate")
ax.set_xlabel("time (s)")
ax.set_ylabel("sample rate (ms)")
ax.set_ylim(0, 5)
ax.grid(True, alpha=0.4)

text = f"avg dt (ms) = {avg_ms:.2f} +- {std_ms:.4f}"
if len(times):
    x_text = 0.05 * times[-1]
else:
    x_text = 0.0
ymin, ymax = ax.get_ylim()
y_text = ymin + 0.15 * (ymax - ymin)
ax.text(x_text, y_text, text, fontsize=12, color="black")

plt.tight_layout()

# Save figure near bag
output_file = bag_path.with_name(f"{bag_path.stem}_dt_scatter.png")
plt.savefig(output_file, dpi=150)
print(f"Scatter plot saved to: {output_file}")
