################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
source/%.obj: ../source/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"D:/SIL4/ti/ccs1120/ccs/tools/compiler/ti-cgt-arm_20.2.5.LTS/bin/armcl" -mv7R4 --code_state=32 --float_support=VFPv3D16 -me --include_path="D:/SIL4/ti/ccs1120/ccs/tools/compiler/ti-cgt-arm_20.2.5.LTS/include" --include_path="D:/SIL4/repos/TI/HelloWorld_RM46/include" --include_path="D:/SIL4/repos/TI/HelloWorld_RM46/CustomApp/include" --include_path="D:/SIL4/repos/TI/HelloWorld_RM46/FreeRTOS+UDP/include" -g --relaxed_ansi --diag_wrap=off --display_error_number --enum_type=packed --abi=eabi --preproc_with_compile --preproc_dependency="source/$(basename $(<F)).d_raw" --obj_directory="source" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

source/%.obj: ../source/%.asm $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"D:/SIL4/ti/ccs1120/ccs/tools/compiler/ti-cgt-arm_20.2.5.LTS/bin/armcl" -mv7R4 --code_state=32 --float_support=VFPv3D16 -me --include_path="D:/SIL4/ti/ccs1120/ccs/tools/compiler/ti-cgt-arm_20.2.5.LTS/include" --include_path="D:/SIL4/repos/TI/HelloWorld_RM46/include" --include_path="D:/SIL4/repos/TI/HelloWorld_RM46/CustomApp/include" --include_path="D:/SIL4/repos/TI/HelloWorld_RM46/FreeRTOS+UDP/include" -g --relaxed_ansi --diag_wrap=off --display_error_number --enum_type=packed --abi=eabi --preproc_with_compile --preproc_dependency="source/$(basename $(<F)).d_raw" --obj_directory="source" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

source/nandflash.obj: ../source/nandflash.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"D:/SIL4/ti/ccs1120/ccs/tools/compiler/ti-cgt-arm_20.2.5.LTS/bin/armcl" -mv7R4 --code_state=32 --float_support=VFPv3D16 -me --include_path="D:/SIL4/ti/ccs1120/ccs/tools/compiler/ti-cgt-arm_20.2.5.LTS/include" --include_path="D:/SIL4/repos/TI/HelloWorld_RM46/include" --include_path="D:/SIL4/repos/TI/HelloWorld_RM46/CustomApp/include" --include_path="D:/SIL4/repos/TI/HelloWorld_RM46/FreeRTOS+UDP/include" -g --relaxed_ansi --diag_wrap=off --display_error_number --enum_type=packed --abi=eabi --preproc_with_compile --preproc_dependency="source/$(basename $(<F)).d_raw" --obj_directory="source" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


