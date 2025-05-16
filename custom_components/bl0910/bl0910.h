#pragma once

#include "esphome/components/sensor/sensor.h"
#include "esphome/components/uart/uart.h"
#include "esphome/components/spi/spi.h"
#include "esphome/core/automation.h"
#include "esphome/core/component.h"
#include "esphome/core/datatypes.h"

namespace esphome
{
  namespace bl0910
  {

    struct DataPacket
    { // NOLINT(altera-struct-pack-align)
      uint8_t l{0};
      uint8_t m{0};
      uint8_t h{0};
      uint8_t checksum; // checksum
      uint8_t address;
    } __attribute__((packed));

    struct ube24_t
    { // NOLINT(readability-identifier-naming,altera-struct-pack-align)
      uint8_t l{0};
      uint8_t m{0};
      uint8_t h{0};
    } __attribute__((packed));

    struct sbe24_t
    { // NOLINT(readability-identifier-naming,altera-struct-pack-align)
      uint8_t l{0};
      uint8_t m{0};
      int8_t h{0};
    } __attribute__((packed));

    enum class CommunicationMode {
      UART,
      SPI
    };

    // Forward declarations
    template <typename... Ts>
    class ResetEnergyAction;
    class BL0910;
    using ActionCallbackFuncPtr = void (BL0910::*)();

    // Base class that will handle the common functionality
    class BL0910 : public PollingComponent
    {
      SUB_SENSOR(voltage)
      SUB_SENSOR(current_1)
      SUB_SENSOR(current_2)
      SUB_SENSOR(current_3)
      SUB_SENSOR(current_4)
      SUB_SENSOR(current_5)
      SUB_SENSOR(current_6)
      SUB_SENSOR(current_7)
      SUB_SENSOR(current_8)
      SUB_SENSOR(current_9)
      SUB_SENSOR(current_10)
      SUB_SENSOR(power_1)
      SUB_SENSOR(power_2)
      SUB_SENSOR(power_3)
      SUB_SENSOR(power_4)
      SUB_SENSOR(power_5)
      SUB_SENSOR(power_6)
      SUB_SENSOR(power_7)
      SUB_SENSOR(power_8)
      SUB_SENSOR(power_9)
      SUB_SENSOR(power_10)
      SUB_SENSOR(total_power)
      SUB_SENSOR(power_factor_1)
      SUB_SENSOR(power_factor_2)
      SUB_SENSOR(power_factor_3)
      SUB_SENSOR(power_factor_4)
      SUB_SENSOR(power_factor_5)
      SUB_SENSOR(power_factor_6)
      SUB_SENSOR(power_factor_7)
      SUB_SENSOR(power_factor_8)
      SUB_SENSOR(power_factor_9)
      SUB_SENSOR(power_factor_10)
      SUB_SENSOR(energy_1)
      SUB_SENSOR(energy_2)
      SUB_SENSOR(energy_3)
      SUB_SENSOR(energy_4)
      SUB_SENSOR(energy_5)
      SUB_SENSOR(energy_6)
      SUB_SENSOR(energy_7)
      SUB_SENSOR(energy_8)
      SUB_SENSOR(energy_9)
      SUB_SENSOR(energy_10)
      SUB_SENSOR(total_energy)
      SUB_SENSOR(frequency)
      SUB_SENSOR(temperature)

    public:
      void update() override;
      void dump_config() override;
      
      // Set communication mode
      void set_comm_mode(CommunicationMode mode) { this->comm_mode_ = mode; }
      
      // Get communication mode
      CommunicationMode get_comm_mode() const { return this->comm_mode_; }

    protected:
      template <typename... Ts>
      friend class ResetEnergyAction;
      
      // Pure virtual methods for communication that derived classes must implement
      virtual void write_byte(uint8_t data) = 0;
      virtual uint8_t read_byte() = 0;
      virtual bool read_array(uint8_t *data, size_t len) = 0;
      virtual void write_array(const uint8_t *data, size_t len) = 0;
      virtual void flush() = 0;
      virtual bool available() = 0;
      
      // Common methods used by both communication types
      void loop() override;
      void setup() override;
      void reset_energy_();
      void read_data_(uint8_t address, float reference, sensor::Sensor *sensor);
      void calculate_power_factor_(sensor::Sensor *current_sensor, sensor::Sensor *voltage_sensor, sensor::Sensor *power_sensor, sensor::Sensor *power_factor_sensor);
      void bias_correction_(uint8_t address, float measurements, float correction);
      void gain_correction_(uint8_t address, float measurements, float correction);
      size_t enqueue_action_(ActionCallbackFuncPtr function);
      void handle_actions_();

      CommunicationMode comm_mode_{CommunicationMode::UART};

    private:
      std::vector<ActionCallbackFuncPtr> action_queue_{};
      uint8_t current_channel_{0};
      uint8_t read_buffer_[64];
    };

    // UART specific implementation
    class BL0910UART : public BL0910, public uart::UARTDevice {
    public:
      void write_byte(uint8_t data) override { UARTDevice::write_byte(data); }
      uint8_t read_byte() override { uint8_t data; UARTDevice::read_byte(&data); return data; }
      bool read_array(uint8_t *data, size_t len) override { return UARTDevice::read_array(data, len); }
      void write_array(const uint8_t *data, size_t len) override { UARTDevice::write_array(data, len); }
      void flush() override { UARTDevice::flush(); }
      bool available() override { return UARTDevice::available(); }
    };

    // SPI specific implementation
    class BL0910SPI : public BL0910, public spi::SPIDevice<spi::BIT_ORDER_MSB_FIRST, 
                                                          spi::CLOCK_POLARITY_LOW,
                                                          spi::CLOCK_PHASE_LEADING, 
                                                          spi::DATA_RATE_1MHZ> {
    public:
      void write_byte(uint8_t data) override;
      uint8_t read_byte() override;
      bool read_array(uint8_t *data, size_t len) override;
      void write_array(const uint8_t *data, size_t len) override;
      void flush() override;
      bool available() override;
      
    protected:
      // Buffer for SPI reading/writing
      std::vector<uint8_t> rx_buffer_;
    };

    template <typename... Ts>
    class ResetEnergyAction : public Action<Ts...>, public Parented<BL0910>
    {
    public:
      void play(Ts... x) override { this->parent_->enqueue_action_(&BL0910::reset_energy_); }
    };

  } // namespace bl0910
} // namespace esphome 