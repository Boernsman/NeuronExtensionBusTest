#include <algorithm>
#include <boost/program_options.hpp>
#include <chrono>
#include <filesystem>
#include <iostream>
#include <modbus.h>
#include <stdexcept>
#include <string>
#include <system_error>
#include <termios.h>
#include <vector>

#include "modbus_rtu.h"

namespace po = boost::program_options;

std::vector<std::string> available_ports() {
  std::vector<std::string> ports;
  // The error_code overload reports a missing or unreadable /dev by returning
  // the end iterator instead of throwing.
  std::error_code ec;
  for (const auto &entry : std::filesystem::directory_iterator("/dev", ec)) {
    const std::string port = entry.path();
    if (port.find("ttyS") != std::string::npos ||
        port.find("ttyNS") != std::string::npos ||
        port.find("ttyAMA") != std::string::npos ||
        port.find("ttyUSB") != std::string::npos ||
        port.find("ttyACM") != std::string::npos) {
      ports.push_back(port);
    }
  }
  std::sort(ports.begin(), ports.end());
  return ports;
}

std::string select_serial_port() {
  const std::vector<std::string> ports = available_ports();
  if (ports.empty()) {
    throw std::runtime_error("No serial ports found in /dev.");
  }

  while (true) {
    std::cout << "Available serial ports:" << std::endl;
    for (const auto &port : ports) {
      std::cout << "  " << port << std::endl;
    }

    std::cout << "Select the serial port:" << std::endl;
    std::string port;
    if (!(std::cin >> port)) {
      throw std::runtime_error("No serial port selected.");
    }

    // Accept anything that exists, so devices the scan above does not know
    // about (or a /dev/serial/by-id symlink) can still be used.
    std::error_code ec;
    if (std::find(ports.begin(), ports.end(), port) != ports.end() ||
        std::filesystem::exists(port, ec)) {
      return port;
    }

    std::cerr << "'" << port << "' is not an available serial port."
              << std::endl;
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

  try {
    if (port_name.empty()) {
      port_name = select_serial_port();
    }

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
