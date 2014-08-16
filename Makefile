
MIPS_DNAME = /home/amv/trash/mips/
BASE_DNAME = $(MIPS_DNAME)/CodeSourcery/Sourcery_CodeBench_Lite_for_MIPS_GNU_Linux/
BIN_BASE = $(BASE_DNAME)/bin
SDK_BASE = $(MIPS_DNAME)/work/
BROOT = $(SDK_BASE)/build_root

CC = $(BIN_BASE)/mips-linux-gnu-gcc
CXXC = $(BIN_BASE)/mips-linux-gnu-g++
LD = $(BIN_BASE)/mips-linux-gnu-gcc
AR = $(BIN_BASE)/mips-linux-gnu-ar

LIBS = -lm
CFLAGS = -Wall -march=xlp -O3 -mplt -std=gnu++11
LDFLAGS = -L$(BROOT)/usr/lib/ -lm -lamv -lsxf -liconv
INCLUDE_DIRS = \
	-Wno-poison-system-directories \
	-I. -I$(BROOT)/usr/include/ -I$(BROOT)/usr/include/sxf/ -I$(BROOT)/usr/include/libdrm/
SRC = \
	card/main.cpp card/main_loop.cpp card/map.cpp card/socket.cpp card/matrix.cpp \
	base/tree.cpp base/protocol.cpp base/socket.cpp \
	pci/pci.cpp

PREFIX = /home/amv/trash/nfs
BUILD_DIR = build/
OUTPUT_FNAME = card

release: clean

	mkdir -p $(BUILD_DIR)

	$(CXXC) $(CFLAGS) $(INCLUDE_DIRS) $(SRC) $(LIBS) $(LDFLAGS) -o $(BUILD_DIR)/$(OUTPUT_FNAME)

install: release

	cp $(BUILD_DIR)/$(OUTPUT_FNAME) $(PREFIX)/

clean:

	rm -f $(BUILD_DIR)/$(OUTPUT_FNAME)

