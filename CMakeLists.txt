cmake_minimum_required(VERSION 3.14)

# Set project name and version
project(neuron_extension_bus_test VERSION 1.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Find the required package for Modbus
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBMODBUS REQUIRED libmodbus)

# Find Boost
find_package(Boost REQUIRED COMPONENTS program_options)

# Find GoogleTest
find_package(GTest REQUIRED)

include_directories(include)

# Include directories and link libraries
include_directories(${LIBMODBUS_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS})
link_libraries(${LIBMODBUS_LIBRARIES} ${Boost_LIBRARIES})

# Add compiler definitions and flags
add_compile_definitions(${LIBMODBUS_CFLAGS_OTHER})
link_directories(${LIBMODBUS_LIBRARY_DIRS})

add_executable(${PROJECT_NAME} src/main.cpp)
add_executable(${PROJECT_NAME}_tests tests/modbus_rtu_gtests.cpp)

# Link GoogleTest libraries
target_link_libraries(${PROJECT_NAME}_tests GTest::GTest GTest::Main)

# Enable testing
enable_testing()
add_test(NAME ModbusRTUTests COMMAND ${PROJECT_NAME}_tests)
