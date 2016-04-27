CFILES := cc3200reset.c

ifneq ($(OS),Windows_NT)
	OS := $(shell uname -s)
endif

#CFLAGS = -fno-stack-protector -Os -o cc3200reset
CFLAGS = -Os -o cc3200reset -mmacosx-version-min=10.5
GCC = gcc

ifeq ($(OS),Darwin)
LDFLAGS = -lobjc -framework CoreFoundation -framework IOKit
CFLAGS += -Imacos/inc
LIBS = macos/libs/libftdi1.a macos/libs/libusb-1.0.a
endif

ifeq ($(OS),Linux)
LDFLAGS = -lpthread
LBITS := $(shell getconf LONG_BIT)
ifeq ($(LBITS),64)
LIBS = linux/libs64/libftdi.a linux/libs64/libusb.a
else
LIBS = linux/libs/libftdi.a linux/libs/libusb.a
endif
endif

ifeq ($(OS),Windows_NT)
LDFLAGS =
CFLAGS += -Iwin/inc
LIBS = win/libs/libftd2xx.a -lpthread
endif

all:
	$(GCC) $(CFILES) $(LIBS) $(CFLAGS) $(LDFLAGS)

clean:
	rm -f cc3200reset *.o
ifeq ($(OS),Darwin)
	rm -rf serial.dSYM
endif
