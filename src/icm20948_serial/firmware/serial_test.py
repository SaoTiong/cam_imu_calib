import serial
import time

# --- CONFIGURATION ---
PORT = '/dev/ttyUSB0'   
BAUD = 115200           # Must match Arduino!
PACKET_SIZE = 26        # 1 Header + 24 Payload + 1 Footer
# ---------------------

try:
    ser = serial.Serial(PORT, BAUD, timeout=1)
    print(f"Connected to {PORT} at {BAUD} baud.")
    print("Measuring Binary Packet Rate... (Press Ctrl+C to stop)")
    
    ser.reset_input_buffer()
    
    count = 0
    start_time = time.time()
    
    while True:
        # Check if enough data is waiting for a full packet
        if ser.in_waiting >= PACKET_SIZE:
            
            # Read exactly one packet size (26 bytes)
            # We don't care about parsing it right now, just counting speed.
            ser.read(PACKET_SIZE)
            
            count += 1
            
            # FPS Calculation
            current_time = time.time()
            if (current_time - start_time) >= 1.0:
                fps = count / (current_time - start_time)
                print(f"Rate: {fps:.2f} Hz")
                
                count = 0
                start_time = current_time

except serial.SerialException as e:
    print(f"Error: {e}")
except KeyboardInterrupt:
    print("\nStopped.")
