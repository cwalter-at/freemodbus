cmake_minimum_required(VERSION 3.16)

# FreeModbus Library CMake configuration
# This builds only the core FreeModbus library without platform-specific code

# Define the FreeModbus library target
add_library(freemodbus STATIC)

# Define include directories for FreeModbus
set(FREEMODBUS_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/modbus/include
    ${CMAKE_CURRENT_SOURCE_DIR}/modbus/rtu
    ${CMAKE_CURRENT_SOURCE_DIR}/modbus/ascii
    ${CMAKE_CURRENT_SOURCE_DIR}/modbus/tcp
    ${CMAKE_CURRENT_SOURCE_DIR}/demo/STM32_CMAKE/port
    ${CMAKE_CURRENT_SOURCE_DIR}/../Drivers/CMSIS/Include
)

# Use FREEMODBUS_INCLUDE_DIRS in target_include_directories
target_include_directories(freemodbus PUBLIC
    ${FREEMODBUS_INCLUDE_DIRS}
)

# Core FreeModbus library source files
set(FREEMODBUS_CORE_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/modbus/mb.c
    # Functions
    ${CMAKE_CURRENT_SOURCE_DIR}/modbus/functions/mbfunccoils.c
    #${CMAKE_CURRENT_SOURCE_DIR}/modbus/functions/mbfuncdiag.c
    ${CMAKE_CURRENT_SOURCE_DIR}/modbus/functions/mbfuncdisc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/modbus/functions/mbfuncholding.c
    ${CMAKE_CURRENT_SOURCE_DIR}/modbus/functions/mbfuncinput.c
    ${CMAKE_CURRENT_SOURCE_DIR}/modbus/functions/mbfuncother.c
    ${CMAKE_CURRENT_SOURCE_DIR}/modbus/functions/mbutils.c
    # RTU mode
    ${CMAKE_CURRENT_SOURCE_DIR}/modbus/rtu/mbcrc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/modbus/rtu/mbrtu.c
    # ASCII mode (include for completeness)
    ${CMAKE_CURRENT_SOURCE_DIR}/modbus/ascii/mbascii.c
)

# Add all sources to the library target
target_sources(freemodbus PRIVATE
    ${FREEMODBUS_CORE_SOURCES}
)

# Define platform-dependent sources for the application
set(FREEMODBUS_APP_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/demo/STM32_CMAKE/port/portserial.c
    ${CMAKE_CURRENT_SOURCE_DIR}/demo/STM32_CMAKE/port/porttimer.c
    ${CMAKE_CURRENT_SOURCE_DIR}/demo/STM32_CMAKE/port/portevent.c
    PARENT_SCOPE
)

# Enable position-independent code (if needed)
set_property(TARGET freemodbus PROPERTY POSITION_INDEPENDENT_CODE ON)

# Set compiler options
target_compile_options(freemodbus PRIVATE
    -DSTM32_CMAKE
)

