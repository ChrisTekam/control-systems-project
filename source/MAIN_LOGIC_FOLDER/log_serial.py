"""
log_serial.py

Reads CSV lines printed by the Arduino (Logger.h: "millis,setpoint,
temperature,duty,pwm") over serial and saves them to a timestamped
.csv file, while echoing them to the terminal so you can watch the run live.

Usage:
    python log_serial.py                  # auto-detects the port
    python log_serial.py COM5             # force a port (Windows)
    python log_serial.py /dev/ttyACM0     # force a port (Linux/Mac)

Stop with Ctrl+C - the file is flushed after every row.
Close the Arduino Serial Monitor first (only one program can use the port).

Requires: pip install pyserial
"""

import sys
import csv
import time
import serial
import serial.tools.list_ports
from datetime import datetime

BAUD_RATE = 9600          # must match Logger(baud) in Logger.h
OUTPUT_DIR = "."          # where to save the .csv files
HEADER = ["millis", "setpoint", "temperature", "duty", "pwm"]


def find_port():
    """Pick the first port that looks like an Arduino, else the first port."""
    ports = list(serial.tools.list_ports.comports())
    if not ports:
        return None
    for p in ports:
        desc = f"{p.description} {p.manufacturer}".lower()
        if "arduino" in desc or "ch340" in desc or "usb serial" in desc:
            return p.device
    return ports[0].device


def parse_row(raw):
    """Return a list of 5 fields if raw is a valid data row, else None."""
    parts = raw.split(",")
    if len(parts) != len(HEADER):
        return None
    try:
        int(parts[0])                      # millis must be an integer
        for p in parts[1:]:
            float(p)                       # accepts 'nan' too
    except ValueError:
        return None
    return parts


def main():
    port = sys.argv[1] if len(sys.argv) > 1 else find_port()
    if port is None:
        print("No serial port found. Plug in the Arduino, or pass the "
              "port explicitly, e.g.: python log_serial.py COM5")
        sys.exit(1)

    filename = f"{OUTPUT_DIR}/run_{datetime.now():%Y%m%d_%H%M%S}.csv"

    print(f"Opening {port} at {BAUD_RATE} baud...")
    try:
        ser = serial.Serial(port, BAUD_RATE, timeout=1)
    except serial.SerialException as e:
        print(f"Could not open {port}: {e}")
        sys.exit(1)

    # Opening the port resets the Uno; wait for it to boot.
    time.sleep(2)
    ser.reset_input_buffer()

    print(f"Logging to {filename}  (Ctrl+C to stop)\n")

    with open(filename, "w", newline="") as f:
        writer = csv.writer(f)
        writer.writerow(HEADER)            # always write our own header
        f.flush()
        row_count = 0

        try:
            while True:
                raw = ser.readline().decode("utf-8", errors="replace").strip()
                if not raw:
                    continue               # timeout, no data

                row = parse_row(raw)
                if row is None:
                    # Header echo, FATAL message, or a partial line.
                    print(f"[skipped] {raw}")
                    continue

                print(raw)
                writer.writerow(row)
                f.flush()
                row_count += 1

        except KeyboardInterrupt:
            print(f"\nStopped. {row_count} rows saved to {filename}")
        except serial.SerialException as e:
            print(f"\nSerial connection lost: {e}\n"
                  f"{row_count} rows saved to {filename}")
        finally:
            ser.close()


if __name__ == "__main__":
    main()
