################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL := cmd.exe
RM := rm -rf

USER_OBJS :=

LIBS := 
PROJ := 

O_SRCS := 
C_SRCS := 
S_SRCS := 
S_UPPER_SRCS := 
OBJ_SRCS := 
ASM_SRCS := 
PREPROCESSING_SRCS := 
OBJS := 
OBJS_AS_ARGS := 
C_DEPS := 
C_DEPS_AS_ARGS := 
EXECUTABLES := 
OUTPUT_FILE_PATH :=
OUTPUT_FILE_PATH_AS_ARGS :=
AVR_APP_PATH :=$$$AVR_APP_PATH$$$
QUOTE := "
ADDITIONAL_DEPENDENCIES:=
OUTPUT_FILE_DEP:=

# Every subdirectory with source files must be described here
SUBDIRS := 


# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS +=  \
../accu.c \
../adc.c \
../button_sv3.c \
../calib.c \
../controller.c \
../controller_debug.c \
../eeprom.c \
../pwm.c \
../pwm_table.c \
../rgbled.c \
../states_sv3.c \
../tbl_transitions.c \
../timer.c \
../uart.c


PREPROCESSING_SRCS += 


ASM_SRCS += 


OBJS +=  \
accu.o \
adc.o \
button_sv3.o \
calib.o \
controller.o \
controller_debug.o \
eeprom.o \
pwm.o \
pwm_table.o \
rgbled.o \
states_sv3.o \
tbl_transitions.o \
timer.o \
uart.o


OBJS_AS_ARGS +=  \
accu.o \
adc.o \
button_sv3.o \
calib.o \
controller.o \
controller_debug.o \
eeprom.o \
pwm.o \
pwm_table.o \
rgbled.o \
states_sv3.o \
tbl_transitions.o \
timer.o \
uart.o


C_DEPS +=  \
accu.d \
adc.d \
button_sv3.d \
calib.d \
controller.d \
controller_debug.d \
eeprom.d \
pwm.d \
pwm_table.d \
rgbled.d \
states_sv3.d \
tbl_transitions.d \
timer.d \
uart.d


C_DEPS_AS_ARGS +=  \
accu.d \
adc.d \
button_sv3.d \
calib.d \
controller.d \
controller_debug.d \
eeprom.d \
pwm.d \
pwm_table.d \
rgbled.d \
states_sv3.d \
tbl_transitions.d \
timer.d \
uart.d


OUTPUT_FILE_PATH +=v3_Single_T84.elf

OUTPUT_FILE_PATH_AS_ARGS +=v3_Single_T84.elf

ADDITIONAL_DEPENDENCIES:=

OUTPUT_FILE_DEP:= ./makedep.mk

# AVR32/GNU C Compiler





























./%.o: .././%.c
	@echo Building file: $<
	@echo Invoking: AVR/GNU C Compiler
	$(QUOTE)C:\MeineProgramme\HwCoDesign\AvrStudio51\extensions\Atmel\AVRGCC\3.3.1.27\AVRToolchain\bin\avr-gcc.exe$(QUOTE)  -funsigned-char -funsigned-bitfields -DF_CPU=8000000UL  -Os -fpack-struct -fshort-enums -Wall -c -std=gnu99 -MD -MP -MF "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)"  -mmcu=attiny84  -o"$@" "$<" 
	@echo Finished building: $<



# AVR32/GNU Preprocessing Assembler



# AVR32/GNU Assembler




ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: $(OUTPUT_FILE_PATH) $(ADDITIONAL_DEPENDENCIES)

$(OUTPUT_FILE_PATH): $(OBJS) $(USER_OBJS) $(OUTPUT_FILE_DEP)
	@echo Building target: $@
	@echo Invoking: AVR/GNU C Linker
	$(QUOTE)C:\MeineProgramme\HwCoDesign\AvrStudio51\extensions\Atmel\AVRGCC\3.3.1.27\AVRToolchain\bin\avr-gcc.exe$(QUOTE) -o$(OUTPUT_FILE_PATH_AS_ARGS) $(OBJS_AS_ARGS) $(USER_OBJS) $(LIBS) -Wl,-Map="v3_Single_T84.map" -Wl,-lm   -mmcu=attiny84  
	@echo Finished building target: $@
	"C:\MeineProgramme\HwCoDesign\AvrStudio51\extensions\Atmel\AVRGCC\3.3.1.27\AVRToolchain\bin\avr-objcopy.exe" -O ihex -R .eeprom -R .fuse -R .lock -R .signature  "v3_Single_T84.elf" "v3_Single_T84.hex"
	"C:\MeineProgramme\HwCoDesign\AvrStudio51\extensions\Atmel\AVRGCC\3.3.1.27\AVRToolchain\bin\avr-objcopy.exe" -j .eeprom  --set-section-flags=.eeprom=alloc,load --change-section-lma .eeprom=0  --no-change-warnings -O ihex "v3_Single_T84.elf" "v3_Single_T84.eep" || exit 0
	"C:\MeineProgramme\HwCoDesign\AvrStudio51\extensions\Atmel\AVRGCC\3.3.1.27\AVRToolchain\bin\avr-objdump.exe" -h -S "v3_Single_T84.elf" > "v3_Single_T84.lss"
	"C:\MeineProgramme\HwCoDesign\AvrStudio51\extensions\Atmel\AVRGCC\3.3.1.27\AVRToolchain\bin\avr-size.exe" -C --mcu=attiny84  "v3_Single_T84.elf"
	




# Other Targets
clean:
	-$(RM) $(OBJS_AS_ARGS)$(C_DEPS_AS_ARGS) $(EXECUTABLES) 
	rm -rf "v3_Single_T84.hex" "v3_Single_T84.lss" "v3_Single_T84.eep" "v3_Single_T84.map"
	