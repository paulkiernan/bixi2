# vim: noexpandtab filetype=make
#
#  ___.   .__       .__   _______________  ________
#  \_ |__ |__|__  __|__| /   __   \   _  \/   __   \
#   | __ \|  \  \/  /  | \____    /  /_\  \____    /
#   | \_\ \  |>    <|  |    /    /\  \_/   \ /    /
#   |___  /__/__/\_ \__|   /____/  \_____  //____/
#       \/         \/                    \/
#
# make all = Make software.
# make clean = Clean out built project files.
# make upload = Upload the hex file to the device

TARGET = bixi
TEENSY_MCU = mk66fx1m0  # 3.6
TEENSY_CORE_SPEED = 180000000
COREPATH = teensy3
LIBRARYPATH = libraries
TOOLSPATH = $(CURDIR)/tools
COMPILERPATH = $(TOOLSPATH)/arm/bin
BUILDDIR = $(abspath $(CURDIR)/build)

OPTIONS = -DUSB_SERIAL -DLAYOUT_US_ENGLISH

MSG_FLASH = Creating load file for Flash:


# CPPFLAGS = compiler options for C and C++
CPPFLAGS = \
	-Wall \
	-g \
	-Os \
	-mthumb \
	-ffunction-sections \
	-fdata-sections \
	-nostdlib \
	-MMD \
	$(OPTIONS) \
	-DTEENSYDUINO=124 \
	-DF_CPU=$(TEENSY_CORE_SPEED) \
	-Isrc \
	-I$(COREPATH) \
	-D__MK66FX1M0__\
	-mcpu=cortex-m4 \
	-mfloat-abi=hard \
	-mfpu=fpv4-sp-d16

# compiler options for C++ only
CXXFLAGS = \
	-std=gnu++0x \
	-felide-constructors \
	-fno-exceptions \
	-fno-rtti

# compiler options for C only
CFLAGS =

# linker options
LDSCRIPT = $(COREPATH)/mk66fx1m0.ld
LDFLAGS = \
	-Os \
	-Wl,--gc-sections \
	-mthumb \
	-mcpu=cortex-m4 \
	-mfloat-abi=hard \
	-mfpu=fpv4-sp-d16 \
	-T$(LDSCRIPT)

# additional libraries to link
LIBS = -lm -larm_cortexM4lf_math

# names for the compiler programs
CC = $(abspath $(COMPILERPATH))/arm-none-eabi-gcc
CXX = $(abspath $(COMPILERPATH))/arm-none-eabi-g++
OBJCOPY = $(abspath $(COMPILERPATH))/arm-none-eabi-objcopy
SIZE = $(abspath $(COMPILERPATH))/arm-none-eabi-size

# automatically create lists of the sources and objects
LC_FILES := $(wildcard $(LIBRARYPATH)/*/*.c)
LCPP_FILES := $(wildcard $(LIBRARYPATH)/*/*.cpp)
TC_FILES := $(wildcard $(COREPATH)/*.c)
TCPP_FILES := $(wildcard $(COREPATH)/*.cpp)
C_FILES := $(wildcard src/*.c)
CPP_FILES := $(wildcard src/*.cpp)
INO_FILES := $(wildcard src/*.ino)

# include paths for libraries
L_INC := $(foreach lib,$(filter %/, $(wildcard $(LIBRARYPATH)/*/)), -I$(lib))

SOURCES := $(C_FILES:.c=.o) $(CPP_FILES:.cpp=.o) $(INO_FILES:.ino=.o) $(TC_FILES:.c=.o) $(TCPP_FILES:.cpp=.o) $(LC_FILES:.c=.o) $(LCPP_FILES:.cpp=.o)
OBJS := $(foreach src,$(SOURCES), $(BUILDDIR)/$(src))

begin:
	@echo
	@echo " ___.   .__       .__   _______________  ________  "
	@echo " \_ |__ |__|__  __|__| /   __   \   _  \/   __   \\"
	@echo "  | __ \|  \  \/  /  | \____    /  /_\  \____    / "
	@echo "  | \_\ \  |>    <|  |    /    /\  \_/   \ /    /  "
	@echo "  |___  /__/__/\_ \__|   /____/  \_____  //____/   "
	@echo "      \/         \/                    \/          "
	@echo ============================================================================
	@echo ============= Compile starting .. $(shell date) ==============

end:
	@echo ============= Compile complete .. $(shell date) ==============
	@echo

all: begin hex end

build: $(TARGET).elf

hex: $(TARGET).hex

post_compile: $(TARGET).hex
	@echo "Informing the Teensy Loader of freshly compiled code..."
	@$(abspath $(TOOLSPATH))/teensy_post_compile -file="$(basename $<)" \
		-path=$(CURDIR) \
		-tools="$(abspath $(TOOLSPATH))"

reboot:
	@echo "Rebooting Teensy..."
	@-$(abspath $(TOOLSPATH))/teensy_reboot

upload: begin post_compile reboot end

$(BUILDDIR)/%.o: %.c
	@echo -e "[CC]\t$<"
	@mkdir -p "$(dir $@)"
	@$(CC) $(CPPFLAGS) $(CFLAGS) $(L_INC) -o "$@" -c "$<"

$(BUILDDIR)/%.o: %.cpp
	@echo -e "[CXX]\t$<"
	@mkdir -p "$(dir $@)"
	@$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(L_INC) -o "$@" -c "$<"

$(BUILDDIR)/%.o: %.ino
	@echo -e "[CXX]\t$<"
	@mkdir -p "$(dir $@)"
	@$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(L_INC) -o "$@" -x c++ -include Arduino.h -c "$<"

$(TARGET).elf: $(OBJS) $(LDSCRIPT)
	@echo -e "[LD]\t$@"
	@$(CC) $(LDFLAGS) -o "$@" $(OBJS) $(LIBS)

%.hex: %.elf
	@echo -e "[HEX]\t$@"
	@$(SIZE) "$<"
	@$(OBJCOPY) -O ihex -R .eeprom "$<" "$@"

# compiler generated dependency info
-include $(OBJS:.o=.d)

clean:
	@echo "Deleting files:"
	@rm -rfv "$(BUILDDIR)"
	@rm -fv "$(TARGET).elf" "$(TARGET).hex"
