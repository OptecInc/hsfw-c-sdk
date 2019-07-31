gcc -c ../hidapi/libusb/hid.c ../src/libhsfw.c ../src/hsfw-app.c -I../hidapi/hidapi -I../include -I/usr/include/libusb-1.0
gcc -o hsfw-app libhsfw.o hsfw-app.o hid.o -lusb-1.0 -pthread