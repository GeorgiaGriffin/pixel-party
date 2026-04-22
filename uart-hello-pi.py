import serial

# this print stuff that comes frm the usb uart converter that is plugged into USART6 on the PCB

try:
	ser = serial.Serial("/dev/ttyUSB0", 9600, timeout=1)
	print("Opened UART Successfully\n")
except Exception as e:
	print("ERROR: ", e);
	exit()

while True:
	data = ser.read(64)
	if data:
		print(data.decode(errors="replace"), end="")
