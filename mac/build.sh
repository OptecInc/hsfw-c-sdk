gcc -c ../hidapi/mac/hid.c ../src/libhsfw.c ../src/hsfw-app.c -I../hidapi/hidapi -I../include
gcc -o hsfw-app libhsfw.o hsfw-app.o hid.o -framework IOKit -framework CoreFoundation