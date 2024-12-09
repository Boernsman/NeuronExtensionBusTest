#include <boost/program_options.hpp>
#include <chrono>
#include <filesystem>
#include <iostream>
#include <modbus.h>
#include <string>
#include <termios.h>
#include <vector>

#include "modbus_rtu.h"

namespace po = boost::program_options;

void print_available_ports() {
  std::cout << "Available serial ports:" << std::endl;
  for (const auto &entry : std::filesystem::directory_iterator("/dev")) {
    const std::string port = entry.path();
    if (port.find("ttyS") != std::string::npos ||
        port.find("ttyNS") != std::string::npos ||
        port.find("ttyAMA") != std::string::npos ||
        port.find("ttyUSB") != std::string::npos ||
        port.find("ttyACM") != std::string::npos) {
      std::cout << "  " << port << std::endl;
    }
  }
}

std::string select_serial_port() {
  while (true) {
    print_available_ports();
    std::string port;
    std::cout << "Select the serial port:" << std::endl;
    std::cin >> port;
    return port;
  }
}

int main(int argc, char *argv[]) {

  std::string port_name;
  int baudrate = 19200;
  std::string parity = "none";
  int address = 1;
  int request_count = 1000;

  try {
    po::options_description desc(
        "Usage: neuron_extension_bus_test [options]\nOptions");
    desc.add_options()("help,h", "Show help message")(
        "serial,s", po::value<std::string>(&port_name), "Set the serial port")(
        "baud,b", po::value<int>(&baudrate)->default_value(19200),
        "Set the baudrate")(
        "parity,p", po::value<std::string>(&parity)->default_value("none"),
        "Set parity (even|none|odd)")(
        "address,a", po::value<int>(&address)->default_value(1),
        "Set slave address")(
        "count,c", po::value<int>(&request_count)->default_value(1000),
        "Set request count");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
      std::cout << desc << std::endl;
      return 0;
    }

  } catch (const std::exception &e) {
    std::cerr << "Error parsing command line arguments: " << e.what()
              << std::endl;
    return -1;
  }

  if (port_name.empty()) {
    port_name = select_serial_port();
  }

  try {
    char parity_char = 'N';
    if (parity == "even") {
      parity_char = 'E';
    } else if (parity == "odd") {
      parity_char = 'O';
    } else if (parity != "none") {
      throw std::invalid_argument(
          "Invalid parity value. Must be 'even', 'none', or 'odd'.");
    }
    ModbusRTU modbus(port_name, baudrate, parity_char);
    modbus.connect();
    modbus.set_slave(address);

    auto failure_count = 0;
    const auto register_count = 5;
    std::vector<uint16_t> data(register_count);

    const auto start_time = std::chrono::steady_clock::now();

    for (int i = 0; i < request_count; ++i) {
      if (!modbus.read_registers(0, register_count, data)) {
        failure_count++;
        std::cerr << "Error: " << modbus.error() << std::endl;
      }
    }

    const std::chrono::duration<double> elapsed_seconds =
        std::chrono::steady_clock::now() - start_time;

    std::cout << "Elapsed time for " << request_count
              << " requests: " << elapsed_seconds.count() << "s with "
              << failure_count << " errors." << std::endl;
  } catch (const std::exception &e) {
    std::cerr << "Exception: " << e.what() << std::endl;
    return -1;
  }

  return 0;
}
