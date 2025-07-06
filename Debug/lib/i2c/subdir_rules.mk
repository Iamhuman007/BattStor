################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
lib/i2c/%.obj: ../lib/i2c/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: MSP430 Compiler'
	"C:/ti/ccs2001/ccs/tools/compiler/ti-cgt-msp430_21.6.1.LTS/bin/cl430" -vmsp --use_hw_mpy=none --include_path="C:/ti/ccs2001/ccs/ccs_base/msp430/include" --include_path="C:/Users/Prathamesh/workspace_ccstheia/BIG_FONT" --include_path="C:/ti/msp430ware_3_80_14_01/iqmathlib/include" --include_path="C:/ti/ccs2001/ccs/tools/compiler/ti-cgt-msp430_21.6.1.LTS/include" --advice:power="none" --define=__MSP430G2553__ -g --printf_support=minimal --diag_warning=225 --diag_wrap=off --display_error_number --preproc_with_compile --preproc_dependency="lib/i2c/$(basename $(<F)).d_raw" --obj_directory="lib/i2c" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


