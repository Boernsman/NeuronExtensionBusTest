#include "modbus_rtu.h" // Replace with the path to your main Modbus RTU code
#include <gtest/gtest.h>

// Mocking the Modbus library
class MockModbus {
public:
  MockModbus() : connected_(false), slave_address_(-1) {}

  int connect() {
    connected_ = true;
    return 0;
  }

  int set_slave(int address) {
    if (!connected_) {
      return -1;
    }
    slave_address_ = address;
    return 0;
  }

  int read_registers(int addr, int nb, uint16_t *dest) {
    if (!connected_ || slave_address_ == -1) {
      return -1;
    }
    for (int i = 0; i < nb; ++i) {
      dest[i] = static_cast<uint16_t>(addr + i);
    }
    return nb;
  }

  bool connected() const { return connected_; }

  int slave_address() const { return slave_address_; }

private:
  bool connected_;
  int slave_address_;
};

class ModbusRTUTest : public ::testing::Test {
protected:
  void SetUp() override {
    // Initialize a mock Modbus object or any necessary setup
    modbus = new MockModbus();
  }

  void TearDown() override {
    // Cleanup
    delete modbus;
  }

  MockModbus *modbus;
};

TEST_F(ModbusRTUTest, ConnectTest) {
  ASSERT_EQ(modbus->connect(), 0);
  ASSERT_TRUE(modbus->connected());
}

TEST_F(ModbusRTUTest, SetSlaveTest) {
  modbus->connect();
  ASSERT_EQ(modbus->set_slave(1), 0);
  ASSERT_EQ(modbus->slave_address(), 1);
}

TEST_F(ModbusRTUTest, ReadRegistersTest) {
  modbus->connect();
  modbus->set_slave(1);

  uint16_t data[5];
  ASSERT_EQ(modbus->read_registers(0, 5, data), 5);

  for (int i = 0; i < 5; ++i) {
    ASSERT_EQ(data[i], static_cast<uint16_t>(i));
  }
}

TEST_F(ModbusRTUTest, ReadRegistersWithoutConnectionTest) {
  uint16_t data[5];
  ASSERT_EQ(modbus->read_registers(0, 5, data), -1);
}

TEST_F(ModbusRTUTest, SetSlaveWithoutConnectionTest) {
  ASSERT_EQ(modbus->set_slave(1), -1);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
