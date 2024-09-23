################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
source/%.obj: ../source/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: MSP430 Compiler'
	"/Applications/ti/ccs1280/ccs/tools/compiler/ti-cgt-msp430_21.6.1.LTS/bin/cl430" -vmspx --data_model=restricted -Ooff --opt_for_speed=0 --use_hw_mpy=F5 --include_path="/Applications/ti/ccs1280/ccs/ccs_base/msp430/include" --include_path="/Users/samuelkosik/Documents/GitHub/MPC_SPS_2024/SlotCar_code" --include_path="/Applications/ti/ccs1280/ccs/tools/compiler/ti-cgt-msp430_21.6.1.LTS/include" --advice:power="all" --define=__MSP430F5437A__ -g --printf_support=minimal --diag_warning=225 --diag_wrap=off --display_error_number --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU23 --silicon_errata=CPU40 --preproc_with_compile --preproc_dependency="source/$(basename $(<F)).d_raw" --obj_directory="source" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


