################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
accelerometer.obj: ../accelerometer.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv5/tools/compiler/tms470_4.9.5/bin/cl470" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -O2 -g --include_path="C:/ti/ccsv5/tools/compiler/tms470_4.9.5/include" --include_path="C:/Users/Chris/workspace_v5_2/the_finglonger" --include_path="C:/ti/StellarisWare/boards/ek-lm4f120xl" --include_path="C:/ti/StellarisWare" --include_path="C:/ti/StellarisWare/third_party" --include_path="C:/ti/StellarisWare/third_party/FreeRTOS/Source/include" --include_path="C:/ti/StellarisWare/third_party/FreeRTOS" --include_path="C:/ti/StellarisWare/third_party/FreeRTOS/Source/portable/CCS/ARM_CM4F" --gcc --define=ccs="ccs" --define=PART_LM4F120H5QR --define=TARGET_IS_BLIZZARD_RA1 --diag_warning=225 --display_error_number --gen_func_subsections=on --ual --preproc_with_compile --preproc_dependency="accelerometer.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

led_task.obj: ../led_task.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv5/tools/compiler/tms470_4.9.5/bin/cl470" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -O2 -g --include_path="C:/ti/ccsv5/tools/compiler/tms470_4.9.5/include" --include_path="C:/Users/Chris/workspace_v5_2/the_finglonger" --include_path="C:/ti/StellarisWare/boards/ek-lm4f120xl" --include_path="C:/ti/StellarisWare" --include_path="C:/ti/StellarisWare/third_party" --include_path="C:/ti/StellarisWare/third_party/FreeRTOS/Source/include" --include_path="C:/ti/StellarisWare/third_party/FreeRTOS" --include_path="C:/ti/StellarisWare/third_party/FreeRTOS/Source/portable/CCS/ARM_CM4F" --gcc --define=ccs="ccs" --define=PART_LM4F120H5QR --define=TARGET_IS_BLIZZARD_RA1 --diag_warning=225 --display_error_number --gen_func_subsections=on --ual --preproc_with_compile --preproc_dependency="led_task.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

linear_controller_task.obj: ../linear_controller_task.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv5/tools/compiler/tms470_4.9.5/bin/cl470" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -O2 -g --include_path="C:/ti/ccsv5/tools/compiler/tms470_4.9.5/include" --include_path="C:/Users/Chris/workspace_v5_2/the_finglonger" --include_path="C:/ti/StellarisWare/boards/ek-lm4f120xl" --include_path="C:/ti/StellarisWare" --include_path="C:/ti/StellarisWare/third_party" --include_path="C:/ti/StellarisWare/third_party/FreeRTOS/Source/include" --include_path="C:/ti/StellarisWare/third_party/FreeRTOS" --include_path="C:/ti/StellarisWare/third_party/FreeRTOS/Source/portable/CCS/ARM_CM4F" --gcc --define=ccs="ccs" --define=PART_LM4F120H5QR --define=TARGET_IS_BLIZZARD_RA1 --diag_warning=225 --display_error_number --gen_func_subsections=on --ual --preproc_with_compile --preproc_dependency="linear_controller_task.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

startup_ccs.obj: ../startup_ccs.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv5/tools/compiler/tms470_4.9.5/bin/cl470" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -O2 -g --include_path="C:/ti/ccsv5/tools/compiler/tms470_4.9.5/include" --include_path="C:/Users/Chris/workspace_v5_2/the_finglonger" --include_path="C:/ti/StellarisWare/boards/ek-lm4f120xl" --include_path="C:/ti/StellarisWare" --include_path="C:/ti/StellarisWare/third_party" --include_path="C:/ti/StellarisWare/third_party/FreeRTOS/Source/include" --include_path="C:/ti/StellarisWare/third_party/FreeRTOS" --include_path="C:/ti/StellarisWare/third_party/FreeRTOS/Source/portable/CCS/ARM_CM4F" --gcc --define=ccs="ccs" --define=PART_LM4F120H5QR --define=TARGET_IS_BLIZZARD_RA1 --diag_warning=225 --display_error_number --gen_func_subsections=on --ual --preproc_with_compile --preproc_dependency="startup_ccs.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

switch_task.obj: ../switch_task.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv5/tools/compiler/tms470_4.9.5/bin/cl470" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -O2 -g --include_path="C:/ti/ccsv5/tools/compiler/tms470_4.9.5/include" --include_path="C:/Users/Chris/workspace_v5_2/the_finglonger" --include_path="C:/ti/StellarisWare/boards/ek-lm4f120xl" --include_path="C:/ti/StellarisWare" --include_path="C:/ti/StellarisWare/third_party" --include_path="C:/ti/StellarisWare/third_party/FreeRTOS/Source/include" --include_path="C:/ti/StellarisWare/third_party/FreeRTOS" --include_path="C:/ti/StellarisWare/third_party/FreeRTOS/Source/portable/CCS/ARM_CM4F" --gcc --define=ccs="ccs" --define=PART_LM4F120H5QR --define=TARGET_IS_BLIZZARD_RA1 --diag_warning=225 --display_error_number --gen_func_subsections=on --ual --preproc_with_compile --preproc_dependency="switch_task.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

the_finglonger.obj: ../the_finglonger.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv5/tools/compiler/tms470_4.9.5/bin/cl470" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -O2 -g --include_path="C:/ti/ccsv5/tools/compiler/tms470_4.9.5/include" --include_path="C:/Users/Chris/workspace_v5_2/the_finglonger" --include_path="C:/ti/StellarisWare/boards/ek-lm4f120xl" --include_path="C:/ti/StellarisWare" --include_path="C:/ti/StellarisWare/third_party" --include_path="C:/ti/StellarisWare/third_party/FreeRTOS/Source/include" --include_path="C:/ti/StellarisWare/third_party/FreeRTOS" --include_path="C:/ti/StellarisWare/third_party/FreeRTOS/Source/portable/CCS/ARM_CM4F" --gcc --define=ccs="ccs" --define=PART_LM4F120H5QR --define=TARGET_IS_BLIZZARD_RA1 --diag_warning=225 --display_error_number --gen_func_subsections=on --ual --preproc_with_compile --preproc_dependency="the_finglonger.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


