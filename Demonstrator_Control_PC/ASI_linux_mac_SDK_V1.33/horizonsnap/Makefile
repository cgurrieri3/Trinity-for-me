platform = armv8

CC = g++
OPENCV = -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_imgcodecs



LIBSPATH = -L../lib/$(platform) -I../include



DEFS = -D_LIN -D_DEBUG -D_LIN -D_ARM
CFLAGS = -g -I $(INCLIB) -L $(LDLIB) $(DEFS) $(COMMON) $(LIBSPATH) -lpthread -DGLIBC_20
CC = aarch64-linux-gnu-g++
AR= aarch64-linux-gnu-ar

horizonsnap:horizonsnap.cpp
	$(CC) horizonsnap.cpp -o horizonsnap $(CFLAGS) $(OPENCV) -lASICamera2

clean:
	rm -f horizonsnap
