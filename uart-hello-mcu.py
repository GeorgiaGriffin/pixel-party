import serial

try:
	ser = serial.Serial("/dev/ttyAMA0", 115200, timeout=1)
	print("Opened UART Successfully\n")

	ser.write(b'H')
except Exception as e:
	print("ERROR: ", e);
	exit()

while True:
	data = ser.read(64)
	if data:
		print(data.decode(errors="replace"), end="")
		ser.write(b"Hello MCU!\r\n");
