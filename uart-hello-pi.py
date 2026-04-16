import serial

try:
	ser = serial.Serial("/dev/ttyAMA0", 115200, timeout=1)
	print("Opened UART Successfully\n")
except Exception as e:
	print("ERROR: ", e);
	exit()

while True:
	data = ser.read(64)
	if data:
		print(data.decode(errors="replace"), end="")
