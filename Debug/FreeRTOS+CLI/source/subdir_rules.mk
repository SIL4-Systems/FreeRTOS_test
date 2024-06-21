################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
FreeRTOS+CLI/source/CLITask.obj: ../FreeRTOS+CLI/source/CLITask.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.2/bin/armcl" -mv7R4 --code_state=32 --float_support=VFPv3D16 --abi=eabi -me --include_path="C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.2/include" --include_path="C:/Users/Ray/workspace_v6_0/HelloWorld_RM46/include" --include_path="C:/Users/Ray/workspace_v6_0/HelloWorld_RM46/CustomApp/include" --include_path="C:/Users/Ray/workspace_v6_0/HelloWorld_RM46/FreeRTOS+UDP/include" --include_path="C:/Users/Ray/workspace_v6_0/HelloWorld_RM46/FreeRTOS+CLI/include" -g --relaxed_ansi --display_error_number --diag_warning=225 --diag_wrap=off --enum_type=packed --preproc_with_compile --preproc_dependency="FreeRTOS+CLI/source/CLITask.pp" --obj_directory="FreeRTOS+CLI/source" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

FreeRTOS+CLI/source/FreeRTOS_CLI.obj: ../FreeRTOS+CLI/source/FreeRTOS_CLI.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.2/bin/armcl" -mv7R4 --code_state=32 --float_support=VFPv3D16 --abi=eabi -me --include_path="C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.2/include" --include_path="C:/Users/Ray/workspace_v6_0/HelloWorld_RM46/include" --include_path="C:/Users/Ray/workspace_v6_0/HelloWorld_RM46/CustomApp/include" --include_path="C:/Users/Ray/workspace_v6_0/HelloWorld_RM46/FreeRTOS+UDP/include" --include_path="C:/Users/Ray/workspace_v6_0/HelloWorld_RM46/FreeRTOS+CLI/include" -g --relaxed_ansi --display_error_number --diag_warning=225 --diag_wrap=off --enum_type=packed --preproc_with_compile --preproc_dependency="FreeRTOS+CLI/source/FreeRTOS_CLI.pp" --obj_directory="FreeRTOS+CLI/source" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


