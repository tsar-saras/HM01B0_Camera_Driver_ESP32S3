import cv2
import numpy as np
import serial

WIDTH = 324
HEIGHT = 244
EXPECTED_BYTES = 79056 
PORT = '/dev/cu.usbserial-11110' # UPDATE THIS
BAUD = 2000000
SCALE = 2

ser = serial.Serial(PORT, BAUD, timeout=2)
print("Loading Output...")

# Set up the professional contrast filter
clahe = cv2.createCLAHE(clipLimit=2.0, tileGridSize=(8,8))

cv2.imshow("HM01B0 Feed", np.zeros((HEIGHT * SCALE, WIDTH * SCALE), np.uint8))

while True:
    ser.reset_input_buffer()
    ser.write(b'c')
    
    if ser.read(5) == b'*RDY*':
        raw_data = ser.read(EXPECTED_BYTES)
        
        if len(raw_data) == EXPECTED_BYTES:
            frame = np.frombuffer(raw_data, dtype=np.uint8)
            try:
                # 1. Shape the grid
                img = frame.reshape((HEIGHT, WIDTH))
                
                # 2. THE FIX: Clear the PCLK bit (Bit 0) and do NOT shift. 
                # This stops the bright lights from overflowing to black.
                img = img & 0xFE 

                # 3. A lil processing: Apply the shadow-lifting contrast filter
                img = clahe.apply(img)

                # 4. Scale it up for the big screen
                img_bigger = cv2.resize(img, (WIDTH * SCALE, HEIGHT * SCALE), interpolation=cv2.INTER_NEAREST)

                cv2.imshow("HM01B0 Feed", img_bigger)
            except Exception as e:
                pass

    key = cv2.waitKey(1) & 0xFF
    if key == ord('q'):
        break

ser.close()
cv2.destroyAllWindows()