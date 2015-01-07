## Hi this is my MakeFile.avr

PROJ = avrLibraries

## buid type
BUILD = "Release" 

## Give MCU name
MCU = atmega8

## F_CPU
F_CPU = 16000000UL

## Output hex and elf
TARGET = main

## Source files
SRC = src/test.cpp

## include directories if any
EXTRAINCDIRS = include/ /home/vinay/projects/avr-projects/avrLibraries/include/

#---------------- Debugging Options ----------------
# For simulavr only - target MCU frequency.
DEBUG_MFREQ = $(F_CPU)
DEBUG_UI = gdb
DEBUG_BACKEND = simulavr
GDBINIT_FILE = __avr_gdbinit
DEBUG_PORT = 1212
DEBUG_HOST = localhost


## format (can be srec, ihex, binary)
FORMAT = ihex

##
#mkdir -p bin

## CFLAGS
CFLAGS = -Os
#CFLAG +=-save-temps -g -gdwarf-3 -gstrict-dwarf # enable this if debug symbols are needed

## compiler options
CFLAGS += -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums -ffunction-sections
CFLAGS += -Wall -Wstrict-prototypes -pedantic
#CFLAGS += -Werror -pedantic-errors ## uncomment if want to remove all errors
## inline functions
CFLAGS += -finline-limit=3 
CFLAGS += -fno-tree-scev-cprop -fwhole-program -fno-tree-scev-cprop
CFLAGS += -ffunction-sections -fdata-sections -fno-exceptions
CFLAGS += -D__PROG_TYPES_COMPAT__
CFLAGS += $(patsubst %,-I%,$(EXTRAINCDIRS))

## library options
#PRINTF_LIB = -Wl,-u,vfprintf -lprintf_min -lprintf_flt
#SCANF_LIB = -Wl,-u,vfscanf -lscanf_min -lscanf_flt
MATH_LIB = -lm

## Linker options
LDFLAGS = -Wl,-Map=build/$(TARGET).map,--cref
#LDFLAGS += -Wl, --gc-sections
LDFLAGS += $(EXTMEMOPTS)
LDFLAGS += $(PRINTF_LIB) $(SCANF_LIB) $(MATH_LIB) $(EXTERNAL_LIB)


## ============================================ ###
# Define programs and commands.                                                                                  
SHELL = sh
CC = avr-gcc
OBJCOPY = avr-objcopy
OBJDUMP = avr-objdump
SIZE = avr-size
NM = avr-nm
AVRDUDE = avrdude
REMOVE = rm -f
COPY = cp
WINSHELL = xfce4-terminal

# Define Messages                                                                                                
# English                                                                                                        
MSG_ERRORS_NONE = Errors: none
MSG_BEGIN = -------- begin --------
MSG_END = --------  end  --------
MSG_SIZE_BEFORE = Size before:
MSG_SIZE_AFTER = Size after:
MSG_COFF = Converting to AVR COFF:
MSG_EXTENDED_COFF = Converting to AVR Extended COFF:
MSG_FLASH = Creating load file for Flash:
MSG_EEPROM = Creating load file for EEPROM:
MSG_EXTENDED_LISTING = Creating Extended Listing:
MSG_SYMBOL_TABLE = Creating Symbol Table:
MSG_LINKING = Linking:
MSG_COMPILING = Compiling:
MSG_ASSEMBLING = Assembling:
MSG_CLEANING = Cleaning project:

# Define all object files.                                                                                       
#OBJ = $(SRC:src/%.cpp=build/%.o)                                                                             
#OBJ = $(SRC:+/%.cpp=build/%.o)
OBJ = $(patsubst %.cpp,build/%.o, $(notdir $(SRC)))

# Define all listing files.                                                                                      
LST = $(SRC:src/%.cpp=build/%.lst)                                                                          

# Compiler flags to generate dependency files.                                                                   
GENDEPFLAGS = -MD -MP -MF .dep/$(@F).d                                                                           

# Combine all necessary flags and optional flags.                                                                
# Add target processor to flags.                                                                                 
ALL_CFLAGS = -mmcu=$(MCU) -I. $(CFLAGS) $(GENDEPFLAGS) -Wl,--gc-sections
ALL_ASFLAGS = -mmcu=$(MCU) -I. -x assembler-with-cpp $(ASFLAGS)                                                  


# Default target.                                                                                             
all: begin gccversion sizebefore build sizeafter end

build: begin elf hex eep lss sym end

elf: bin/elf/$(TARGET).elf                                                                                            
hex: bin/hex/$(TARGET).hex                                                                                            
eep: bin/eep/$(TARGET).eep                                                                                            
#lss: build/$(TARGET).lss                                                                                            
#sym: build/$(TARGET).sym  


## begin
begin:
	@echo ======================================================================================================
	@echo $(MSG_BEGIN)
	mkdir -p bin bin/eep/ bin/elf/ bin/hex/ build/

end:
	@echo $(MSG_END)
	@echo =====================================================================================================

# Display size of file.                                                                                       
HEXSIZE = $(SIZE) --target=$(FORMAT) --mcu=${MCU} bin/hex/$(TARGET).hex                                               
ELFSIZE = $(SIZE) --format=avr --mcu=${MCU} bin/elf/$(TARGET).elf  

sizebefore:
	@if test -f bin/elf/$(TARGET).elf; then echo; echo $(MSG_SIZE_BEFORE); $(ELFSIZE); 2>/dev/null;echo;fi

sizeafter:
	@if test -f bin/elf/$(TARGET).elf; then echo; echo $(MSG_SIZE_AFTER); $(ELFSIZE); 2>/dev/null;echo;fi

# Display compiler version information.                                                                       
gccversion :                                                                                                  
	@$(CC) --version


gdb-config:
	@$(REMOVE) $(GDBINIT_FILE)
	@echo define reset >> $(GDBINIT_FILE)
	@echo SIGNAL SIGHUP >> $(GDBINIT_FILE)
	@echo end >> $(GDBINIT_FILE)
	@echo file bin/hex/$(TARGET).hex >> $(GDBINIT_FILE)
	@echo target remote $(DEBUG_HOST):$(DEBUG_PORT) >> $(GDBINIT_FILE)
ifeq ($(DEBUG_BACKEND),simulavr)
	@echo load >> $(GDBINIT_FILE)
endif	
	@echo break main >> $(GDBINIT_FILE)

simavr-config:
	@$(REMOVE) $(SIMAVR_FILE)
	@echo "--gdbserver" >> $(SIMAVR_FILE)
	@echo "--device" $(MCU) >> $(SIMAVR_FILE)
	@echo "--clock-freq" $(DEBUG_MFREQ) >> $(SIMAVR_FILE)
	@echo "--port" $(DEBUG_PORT) >> $(SIMAVR_FILE)

debug: gdb-config bin/elf/$(TARGET).elf
ifeq ($(DEBUG_BACKEND), avarice)
	@echo Starting AVaRICE - Press enter when "waiting to connect" message displays.
	@$(WINSHELL) /c start avarice --jtag $(JTAG_DEV) --erase --program --file \
		bin/elf/$(TARGET).elf $(DEBUG_HOST):$(DEBUG_PORT)
	@$(WINSHELL) /c pause
else
	@$(WINSHELL) -H --command='simulavr -P simulavr-disp -g -d$(MCU) -c $(DEBUG_MFREQ) --port $(DEBUG_PORT) -F bin < bin/hex/$(TARGET).hex '
endif
	@$(WINSHELL) -H --command='avr-$(DEBUG_UI) --command=$(GDBINIT_FILE)'



# Convert ELF to COFF for use in debugging / simulating in AVR Studio or VMLAB.                               
COFFCONVERT=$(OBJCOPY) --debugging \
--change-section-address .data-0x800000 \
--change-section-address .bss-0x800000 \
--change-section-address .noinit-0x800000 \
--change-section-address .eeprom-0x810000

coff: bin/elf/$(TARGET).elf                                                                                           
	@echo =====================================================
	@echo $(MSG_COFF) build/$(TARGET).cof                                                                             
	$(COFFCONVERT) -O coff-avr $< build/$(TARGET).cof                                                                 

extcoff: bin/elf/$(TARGET).elf                                                                                        
	@echo =====================================================
	@echo $(MSG_EXTENDED_COFF) build/$(TARGET).cof                                                                    
	$(COFFCONVERT) -O coff-ext-avr $< build/$(TARGET).cof

# create hex file
bin/hex/%.hex: bin/elf/%.elf
	@echo ======================================================
	@echo $(MSG_FLASH) $@
	$(OBJCOPY) -O $(FORMAT) -R .eeprom $< $@

bin/eep/%.eep: bin/elf/%.elf                                                                                                  
	@echo ======================================================
	@echo $(MSG_EEPROM) $@                                                                                      
	$(OBJCOPY) -j .eeprom --set-section-flags .eeprom=alloc,load \
	--change-section-lma .eeprom=0 -O $(FORMAT) $< $@

# Create extended listing file from ELF output file.                                                          
build/%.lss: bin/elf/%.elf                                                                                                  
	@echo ========================================================
	@echo $(MSG_EXTENDED_LISTING) $@                                                                            
	$(OBJDUMP) -h -S $< > $@                                                                                    

# Create a symbol table from ELF output file.                                                                 
build/%.sym: bin/elf/%.elf                                                                                                  
	@echo ========================================================
	@echo $(MSG_SYMBOL_TABLE) $@                                                                                
	$(NM) -n $< > $@                                                                                            

# Link: create ELF output file from object files.                                                             
.SECONDARY : bin/elf/$(TARGET).elf                                                                                    
.PRECIOUS : $(OBJ)                                                                                            
bin/elf/$(TARGET).elf: $(OBJ)                                                                                                 
	@echo =========================================================
	@echo $(MSG_LINKING) $@                                                                                     
	$(CC) $(ALL_CFLAGS) $^ --output $@ $(LDFLAGS)                                                               

# Compile: create object files from C source files.                                                           
build/%.o : src/%.cpp                                                                                                   
	@echo =========================================================
	@echo $(MSG_COMPILING) $<                                                                                   
	$(CC) -c $(ALL_CFLAGS) $< -o $@                                                                             

# Compile: create assembler files from C source files.                                                        
build/%.s : src/%.cpp                                                                                                   
	$(CC) -S $(ALL_CFLAGS) $< -o $@                                                                             

# Assemble: create object files from assembler source files.                                                  
build/%.o : %.S                                                                                                     
	@echo =========================================================
	@echo $(MSG_ASSEMBLING) $<                                                                                  
	$(CC) -c $(ALL_ASFLAGS) $< -o $@                                                                           

# Create preprocessed source for use in sending a bug report.                                                 
build/%.i : src/%.cpp                                                                                                     
	$(CC) -E -mmcu=$(MCU) -I. $(CFLAGS) $< -o $@

## clean project
clean: begin clean_binary clean_objects end

clean_objects:
	@echo ==========================================================
	$(REMOVE) ${OBJ}
	$(REMOVE) build/$(SRC:src/%.cpp=%.o)
	$(REMOVE) build/$(SRC:src/%.cpp=%.s)
	$(REMOVE) build/$(SRC:src/%.cpp=%.i)

clean_binary:
	@echo =========================================================
	$(REMOVE) bin/hex/$(TARGET).hex
	$(REMOVE) bin/eep/$(TARGET).eep
	$(REMOVE) bin/elf/$(TARGET).elf
	$(REMOVE) build/$(TARGET).cof
	$(REMOVE) build/$(TARGET).map
	$(REMOVE) build/$(TARGET).sym
	$(REMOVE) build/$(TARGET).lss
	$(REMOVE) .dep/*

# Include the dependency files.
-include $(shell mkdir .dep 2>/dev/null) $(wildcard .dep/*)

.PHONY : all begin finish end sizebefore sizeafter gccversion \
	build elf hex eep lss sym coff extcoff \
	clean clean_list program debug gdb-config simavr-config
