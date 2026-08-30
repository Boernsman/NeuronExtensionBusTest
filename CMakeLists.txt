cmake_minimum_required(VERSION 3.20)

# Set project name and version
project(neuron_extension_bus_test VERSION 1.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# Find the required package for Modbus
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBMODBUS REQUIRED IMPORTED_TARGET libmodbus)

# Find Boost
find_package(Boost REQUIRED COMPONENTS program_options)

# The command line utility
add_executable(${PROJECT_NAME} src/main.cpp)
target_include_directories(${PROJECT_NAME} PRIVATE include)
target_link_libraries(${PROJECT_NAME}
  PRIVATE
    PkgConfig::LIBMODBUS
    Boost::program_options
)

# Unit tests
find_package(GTest REQUIRED)

add_executable(${PROJECT_NAME}_tests tests/modbus_rtu_gtests.cpp)
target_include_directories(${PROJECT_NAME}_tests PRIVATE include)
target_link_libraries(${PROJECT_NAME}_tests
  PRIVATE
    PkgConfig::LIBMODBUS
    GTest::gtest
)

# Enable testing
enable_testing()
add_test(NAME ModbusRTUTests COMMAND ${PROJECT_NAME}_tests)
