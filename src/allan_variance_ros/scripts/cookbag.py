#!/usr/bin/env python3
"""Rewrite a ROS bag so message timestamps come from message headers."""

import argparse
import sys
import time

import rosbag


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Reorder a ROS bag using message header timestamps."
    )
    parser.add_argument("--input", required=True, help="Path to the source bag.")
    parser.add_argument("--output", required=True, help="Destination bag path.")
    parser.add_argument(
        "--compression",
        default="lz4",
        choices=["none", "bz2", "lz4"],
        help="Compression to use for the output bag.",
    )
    parser.add_argument(
        "--report-interval",
        type=float,
        default=5.0,
        help="Seconds between progress messages (default: 5).",
    )
    return parser.parse_args()


def format_duration(seconds: float) -> str:
    minutes, sec = divmod(int(seconds), 60)
    hours, minutes = divmod(minutes, 60)
    return f"{hours:d}h{minutes:02d}m{sec:02d}s"


def print_progress(count: int, total: int, rate: float, remaining: float) -> None:
    percent = (count / total * 100.0) if total else 0.0
    sys.stdout.write(
        f"\rProcessed {count}/{total if total else '?'} msgs "
        f"({percent:5.1f}%) | {rate:6.1f} msg/s | ETA {format_duration(remaining)}"
    )
    sys.stdout.flush()


def main() -> None:
    args = parse_args()

    with rosbag.Bag(args.input) as inbag, rosbag.Bag(
        args.output, "w", compression=args.compression
    ) as outbag:
        total_msgs = inbag.get_message_count()
        start_time = last_report = time.time()

        for count, (topic, msg, t) in enumerate(inbag.read_messages(), start=1):
            if topic == "/tf" and getattr(msg, "transforms", []):
                outbag.write(topic, msg, msg.transforms[0].header.stamp)
            else:
                outbag.write(topic, msg, msg.header.stamp if msg._has_header else t)

            now = time.time()
            if now - last_report >= args.report_interval:
                elapsed = now - start_time
                rate = count / elapsed if elapsed > 0 else 0.0
                remaining = (total_msgs - count) / rate if rate > 0 else 0.0
                print_progress(count, total_msgs, rate, remaining)
                last_report = now

        # final report
        elapsed = time.time() - start_time
        rate = total_msgs / elapsed if elapsed > 0 else 0.0
        print_progress(total_msgs, total_msgs, rate, 0.0)
        sys.stdout.write(
            f"\nFinished rewriting bag in {format_duration(elapsed)} "
            f"({total_msgs} messages).\n"
        )


if __name__ == "__main__":
    main()
