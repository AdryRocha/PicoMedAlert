set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(CMAKE_C_COMPILER "C:/Users/User/.pico-sdk/toolchain/13_3_Rel1/bin/arm-none-eabi-gcc.exe")
set(CMAKE_CXX_COMPILER "C:/Users/User/.pico-sdk/toolchain/13_3_Rel1/bin/arm-none-eabi-g++.exe")
set(CMAKE_ASM_COMPILER "C:/Users/User/.pico-sdk/toolchain/13_3_Rel1/bin/arm-none-eabi-gcc.exe")

set(CMAKE_C_FLAGS "-mcpu=cortex-m0plus -mthumb" CACHE STRING "C compiler flags")
set(CMAKE_CXX_FLAGS "-mcpu=cortex-m0plus -mthumb" CACHE STRING "C++ compiler flags")
set(CMAKE_ASM_FLAGS "-mcpu=cortex-m0plus -mthumb" CACHE STRING "ASM compiler flags")