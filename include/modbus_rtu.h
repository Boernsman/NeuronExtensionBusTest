#include <modbus.h>
#include <stdexcept>
#include <string>
#include <vector>

class ModbusRTU {
public:
  ModbusRTU(const std::string &port_name, int baudrate, char parity,
            int data_bits = 8, int stop_bits = 1)
      : mb_(modbus_new_rtu(port_name.c_str(), baudrate, parity, data_bits,
                           stop_bits)) {
    if (!mb_) {
      throw std::runtime_error("Could not create Modbus RTU: " +
                               std::string(modbus_strerror(errno)));
    }
  }

  ~ModbusRTU() {
    if (mb_) {
      modbus_close(mb_);
      modbus_free(mb_);
    }
  }

  void connect() {
    if (modbus_connect(mb_) == -1) {
      throw std::runtime_error("Could not connect: " +
                               std::string(modbus_strerror(errno)));
    }
  }

  void set_slave(int address) {
    if (modbus_set_slave(mb_, address) == -1) {
      throw std::runtime_error("Could not set slave address: " +
                               std::string(modbus_strerror(errno)));
    }
  }

  bool read_registers(int addr, int nb, std::vector<uint16_t> &dest) {
    dest.resize(nb);
    return modbus_read_registers(mb_, addr, nb, dest.data()) == nb;
  }
  std::string error() { return modbus_strerror(errno); }

private:
  modbus_t *mb_;
};
