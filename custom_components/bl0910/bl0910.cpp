#include "bl0910.h"
#include "constants.h"
#include <queue>
#include "esphome/core/log.h"

namespace esphome
{
  namespace bl0910
  {
    // Define log tag as "bl0910"
    static const char *const TAG = "bl0910";
    // Convert ube24_t type data to uint32_t type
    constexpr uint32_t to_uint32_t(ube24_t input) { return input.h << 16 | input.m << 8 | input.l; }
    // Convert sbe24_t type data to int32_t type
    constexpr int32_t to_int32_t(sbe24_t input) { return input.h << 16 | input.m << 8 | input.l; }
    // Checksum calculation function, calculate the checksum of address and data bytes
    constexpr uint8_t bl0910_checksum(const uint8_t address, const DataPacket *data)
    {
      return (address + data->l + data->m + data->h) ^ 0xFF;
    }

    // Main loop to read and process data from different channels
    void BL0910::loop()
    {
      // If current_channel_ is UINT8_MAX, return directly
      if (this->current_channel_ == UINT8_MAX)
      {
        return;
      }

      this->flush(); // Clear the buffer to avoid interference from residual data

      // Read different sensor data according to the current channel
      switch (this->current_channel_)
      {
      case 0:
        this->read_data_(BL0910_TEMPERATURE, BL0910_TREF, this->temperature_sensor_); // Temperature
        break;
      case 1:
        this->read_data_(BL0910_I_1_RMS, BL0910_IREF, this->current_1_sensor_);
        this->read_data_(BL0910_WATT_1, BL0910_PREF, this->power_1_sensor_);
        this->read_data_(BL0910_CF_1_CNT, BL0910_EREF, this->energy_1_sensor_);
        this->calculate_power_factor_(this->current_1_sensor_, this->voltage_sensor_, this->power_1_sensor_, this->power_factor_1_sensor_);
        break;
      case 2:
        this->read_data_(BL0910_I_2_RMS, BL0910_IREF, this->current_2_sensor_);
        this->read_data_(BL0910_WATT_2, BL0910_PREF, this->power_2_sensor_);
        this->read_data_(BL0910_CF_2_CNT, BL0910_EREF, this->energy_2_sensor_);
        this->calculate_power_factor_(this->current_2_sensor_, this->voltage_sensor_, this->power_2_sensor_, this->power_factor_2_sensor_);
        break;
      case 3:
        this->read_data_(BL0910_I_3_RMS, BL0910_IREF, this->current_3_sensor_);
        this->read_data_(BL0910_WATT_3, BL0910_PREF, this->power_3_sensor_);
        this->read_data_(BL0910_CF_3_CNT, BL0910_EREF, this->energy_3_sensor_);
        this->calculate_power_factor_(this->current_3_sensor_, this->voltage_sensor_, this->power_3_sensor_, this->power_factor_3_sensor_);
        break;
      case 4:
        this->read_data_(BL0910_I_4_RMS, BL0910_IREF, this->current_4_sensor_);
        this->read_data_(BL0910_WATT_4, BL0910_PREF, this->power_4_sensor_);
        this->read_data_(BL0910_CF_4_CNT, BL0910_EREF, this->energy_4_sensor_);
        this->calculate_power_factor_(this->current_4_sensor_, this->voltage_sensor_, this->power_4_sensor_, this->power_factor_4_sensor_);
        break;
      case 5:
        this->read_data_(BL0910_I_5_RMS, BL0910_IREF, this->current_5_sensor_);
        this->read_data_(BL0910_WATT_5, BL0910_PREF, this->power_5_sensor_);
        this->read_data_(BL0910_CF_5_CNT, BL0910_EREF, this->energy_5_sensor_);
        this->calculate_power_factor_(this->current_5_sensor_, this->voltage_sensor_, this->power_5_sensor_, this->power_factor_5_sensor_);
        break;
      case 6:
        this->read_data_(BL0910_I_6_RMS, BL0910_IREF, this->current_6_sensor_);
        this->read_data_(BL0910_WATT_6, BL0910_PREF, this->power_6_sensor_);
        this->read_data_(BL0910_CF_6_CNT, BL0910_EREF, this->energy_6_sensor_);
        this->calculate_power_factor_(this->current_6_sensor_, this->voltage_sensor_, this->power_6_sensor_, this->power_factor_6_sensor_);
        break;
      case 7:
        this->read_data_(BL0910_I_7_RMS, BL0910_IREF, this->current_7_sensor_);
        this->read_data_(BL0910_WATT_7, BL0910_PREF, this->power_7_sensor_);
        this->read_data_(BL0910_CF_7_CNT, BL0910_EREF, this->energy_7_sensor_);
        this->calculate_power_factor_(this->current_7_sensor_, this->voltage_sensor_, this->power_7_sensor_, this->power_factor_7_sensor_);
        break;
      case 8:
        this->read_data_(BL0910_I_8_RMS, BL0910_IREF, this->current_8_sensor_);
        this->read_data_(BL0910_WATT_8, BL0910_PREF, this->power_8_sensor_);
        this->read_data_(BL0910_CF_8_CNT, BL0910_EREF, this->energy_8_sensor_);
        this->calculate_power_factor_(this->current_8_sensor_, this->voltage_sensor_, this->power_8_sensor_, this->power_factor_8_sensor_);
        break;
      case 9:
        this->read_data_(BL0910_I_9_RMS, BL0910_IREF, this->current_9_sensor_);
        this->read_data_(BL0910_WATT_9, BL0910_PREF, this->power_9_sensor_);
        this->read_data_(BL0910_CF_9_CNT, BL0910_EREF, this->energy_9_sensor_);
        this->calculate_power_factor_(this->current_9_sensor_, this->voltage_sensor_, this->power_9_sensor_, this->power_factor_9_sensor_);
        break;
      case 10:
        this->read_data_(BL0910_I_10_RMS, BL0910_IREF, this->current_10_sensor_);
        this->read_data_(BL0910_WATT_10, BL0910_PREF, this->power_10_sensor_);
        this->read_data_(BL0910_CF_10_CNT, BL0910_EREF, this->energy_10_sensor_);
        this->calculate_power_factor_(this->current_10_sensor_, this->voltage_sensor_, this->power_10_sensor_, this->power_factor_10_sensor_);
        break;
      case (UINT8_MAX - 2):
        this->read_data_(BL0910_FREQUENCY, BL0910_FREF, this->frequency_sensor_); // Frequency
        this->read_data_(BL0910_V_RMS, BL0910_UREF, this->voltage_sensor_);       // Voltage
        break;
      case (UINT8_MAX - 1):
        this->read_data_(BL0910_WATT_SUM, BL0910_WATT, this->total_power_sensor_);  // Total power
        this->read_data_(BL0910_CF_SUM_CNT, BL0910_CF, this->total_energy_sensor_); // Total Energy
        break;
      default:
        this->current_channel_ = UINT8_MAX - 2; // Go to frequency and voltage
        return;
      }
      // Increment channel and process subsequent operations
      this->current_channel_++;
      this->handle_actions_();
    }

    // Initialization setup function
    void BL0910::setup()
    {
      ESP_LOGCONFIG(TAG, "Setting up BL0910...");
      // Removed CS pin setup for SPI mode, SPIDevice::spi_setup() handles it.
    }

    // Reset the current channel count to trigger the next data reading cycle
    void BL0910::update()
    {
      this->current_channel_ = 0;
    }

    std::queue<ActionCallbackFuncPtr> enqueue_action_;
    // Add action to queue
    size_t BL0910::enqueue_action_(ActionCallbackFuncPtr function)
    {
      this->action_queue_.push_back(function);
      return this->action_queue_.size();
    }

    // Process all operations in the action queue
    void BL0910::handle_actions_()
    {
      if (this->action_queue_.empty())
      {
        return;
      }
      ActionCallbackFuncPtr ptr_func = nullptr;
      // Traverse all actions and execute
      for (int i = 0; i < this->action_queue_.size(); i++)
      {
        ptr_func = this->action_queue_[i];
        if (ptr_func)
        {
          ESP_LOGI(TAG, "HandleActionCallback[%d]...", i);
          (this->*ptr_func)();
        }
      }
      // Read the remaining data and clear the queue
      while (this->available())
      {
        this->read_byte();
      }

      this->action_queue_.clear();

      // Clear buffer after handling action
      this->flush();
    }

    // Reset energy
    void BL0910::reset_energy_()
    {
      // Write initialization command and clear buffer
      this->write_array(BL0910_INIT[0], 6);
      delay(1);
      this->flush();
      ESP_LOGW(TAG, "Device reset with init command.");
    }

    // Read data
    void BL0910::read_data_(const uint8_t address, const float reference, sensor::Sensor *sensor)
    {
      if (sensor == nullptr)
      {
        return;
      }

      DataPacket buffer;
      ube24_t data_u24;
      sbe24_t data_s24;
      float value = 0;

      // Determine if the data type is signed
      bool signed_result = reference == BL0910_TREF || reference == BL0910_WATT || reference == BL0910_PREF;
      this->flush();
      this->write_byte(BL0910_READ_COMMAND);
      this->write_byte(address);

      // Read data
      if (this->read_array((uint8_t *)&buffer, sizeof(buffer) - 1))
      {
        if (bl0910_checksum(address, &buffer) == buffer.checksum)
        {
          // Handle different data formats based on whether they are signed
          if (signed_result)
          {
            data_s24.l = buffer.l;
            data_s24.m = buffer.m;
            data_s24.h = buffer.h;
          }
          else
          {
            data_u24.l = buffer.l;
            data_u24.m = buffer.m;
            data_u24.h = buffer.h;
          }
        }
        else
        {
          ESP_LOGW(TAG, "Checksum failed. Discarding message."); // If checksum error, discard data
          return;
        }
      }
      // Process data according to different reference values
      if (reference == BL0910_PREF || reference == BL0910_WATT)
      {
        value = (float)to_int32_t(data_s24) * reference;
      }
      if (reference == BL0910_UREF || reference == BL0910_IREF || reference == BL0910_EREF || reference == BL0910_CF)
      {
        value = (float)to_uint32_t(data_u24) * reference;
      }
      if (reference == BL0910_FREF)
      {
        value = reference / (float)to_uint32_t(data_u24);
      }
      if (reference == BL0910_TREF)
      {
        value = (float)to_int32_t(data_s24);
        value = (value - 64) * 12.5 / 59 - 40;
      }
      sensor->publish_state(value);
    }

    // Calculate power factor
    void BL0910::calculate_power_factor_(sensor::Sensor *current_sensor, sensor::Sensor *voltage_sensor, sensor::Sensor *power_sensor, sensor::Sensor *power_factor_sensor)
    {
      if (current_sensor != nullptr && voltage_sensor != nullptr && power_sensor != nullptr && power_factor_sensor != nullptr)
      {
        float power_factor = (current_sensor->state * voltage_sensor->state) / power_sensor->state;
        power_factor_sensor->publish_state(power_factor);
      }
    }

    // Bias calibration function
    void BL0910::bias_correction_(uint8_t address, float measurements, float correction)
    {
      DataPacket data;
      float i_rms0 = measurements * BL0910_KI;
      float i_rms = correction * BL0910_KI;
      int32_t value = (i_rms * i_rms - i_rms0 * i_rms0) / 256; // Calculate calibration value
      // Write calculated value to data packet
      data.l = (value >> 0) & 0xFF;
      data.m = (value >> 8) & 0xFF;
      data.h = (value >> 16) & 0xFF;
      data.checksum = bl0910_checksum(address, &data);
      this->flush();
      this->write_byte(BL0910_WRITE_COMMAND);
      this->write_byte(address);
      this->write_array((uint8_t *)&data, sizeof(data));
    }

    // Gain calibration function
    void BL0910::gain_correction_(uint8_t address, float measurements, float correction)
    {
      DataPacket data;
      float i_rms0 = measurements * BL0910_KI;
      float i_rms = correction * BL0910_KI;
      int32_t value = int((i_rms / i_rms0 - 1) * 65536); // Calculate calibration value
      // Write calculated value to data packet
      data.l = (value >> 0) & 0xFF;
      data.m = (value >> 8) & 0xFF;
      data.h = (value >> 16) & 0xFF;
      data.checksum = bl0910_checksum(address, &data);
      this->flush();
      this->write_byte(BL0910_WRITE_COMMAND);
      this->write_byte(address);
      this->write_array((uint8_t *)&data, sizeof(data));
    }

    void BL0910::dump_config()
    {
      ESP_LOGCONFIG(TAG, "BL0910:");
      ESP_LOGCONFIG(TAG, "  Communication Mode: %s", this->get_comm_mode() == CommunicationMode::UART ? "UART" : "SPI");
      
      LOG_SENSOR("  ", "Voltage", this->voltage_sensor_);

      LOG_SENSOR("  ", "Current1", this->current_1_sensor_);
      LOG_SENSOR("  ", "Current2", this->current_2_sensor_);
      LOG_SENSOR("  ", "Current3", this->current_3_sensor_);
      LOG_SENSOR("  ", "Current4", this->current_4_sensor_);
      LOG_SENSOR("  ", "Current5", this->current_5_sensor_);
      LOG_SENSOR("  ", "Current6", this->current_6_sensor_);
      LOG_SENSOR("  ", "Current7", this->current_7_sensor_);
      LOG_SENSOR("  ", "Current8", this->current_8_sensor_);
      LOG_SENSOR("  ", "Current9", this->current_9_sensor_);
      LOG_SENSOR("  ", "Current10", this->current_10_sensor_);

      LOG_SENSOR("  ", "Power1", this->power_1_sensor_);
      LOG_SENSOR("  ", "Power2", this->power_2_sensor_);
      LOG_SENSOR("  ", "Power3", this->power_3_sensor_);
      LOG_SENSOR("  ", "Power4", this->power_4_sensor_);
      LOG_SENSOR("  ", "Power5", this->power_5_sensor_);
      LOG_SENSOR("  ", "Power6", this->power_6_sensor_);
      LOG_SENSOR("  ", "Power7", this->power_7_sensor_);
      LOG_SENSOR("  ", "Power8", this->power_8_sensor_);
      LOG_SENSOR("  ", "Power9", this->power_9_sensor_);
      LOG_SENSOR("  ", "Power10", this->power_10_sensor_);

      LOG_SENSOR("  ", "Power factor 1", this->power_factor_1_sensor_);
      LOG_SENSOR("  ", "Power factor 2", this->power_factor_2_sensor_);
      LOG_SENSOR("  ", "Power factor 3", this->power_factor_3_sensor_);
      LOG_SENSOR("  ", "Power factor 4", this->power_factor_4_sensor_);
      LOG_SENSOR("  ", "Power factor 5", this->power_factor_5_sensor_);
      LOG_SENSOR("  ", "Power factor 6", this->power_factor_6_sensor_);
      LOG_SENSOR("  ", "Power factor 7", this->power_factor_7_sensor_);
      LOG_SENSOR("  ", "Power factor 8", this->power_factor_8_sensor_);
      LOG_SENSOR("  ", "Power factor 9", this->power_factor_9_sensor_);
      LOG_SENSOR("  ", "Power factor 10", this->power_factor_10_sensor_);

      LOG_SENSOR("  ", "Energy1", this->energy_1_sensor_);
      LOG_SENSOR("  ", "Energy2", this->energy_2_sensor_);
      LOG_SENSOR("  ", "Energy3", this->energy_3_sensor_);
      LOG_SENSOR("  ", "Energy4", this->energy_4_sensor_);
      LOG_SENSOR("  ", "Energy5", this->energy_5_sensor_);
      LOG_SENSOR("  ", "Energy6", this->energy_6_sensor_);
      LOG_SENSOR("  ", "Energy7", this->energy_7_sensor_);
      LOG_SENSOR("  ", "Energy8", this->energy_8_sensor_);
      LOG_SENSOR("  ", "Energy9", this->energy_9_sensor_);
      LOG_SENSOR("  ", "Energy10", this->energy_10_sensor_);

      LOG_SENSOR("  ", "Total Power", this->total_power_sensor_);
      LOG_SENSOR("  ", "Total Energy", this->total_energy_sensor_);
      LOG_SENSOR("  ", "Frequency", this->frequency_sensor_);
      LOG_SENSOR("  ", "Temperature", this->temperature_sensor_);
    }

    // SPI Implementation
    void BL0910SPI::write_byte(uint8_t data) {
      this->transfer_byte(data); // SPIDevice::transfer_byte handles CS
    }

    uint8_t BL0910SPI::read_byte() {
      return this->transfer_byte(0x00); // SPIDevice::transfer_byte handles CS, send dummy byte for read
    }

    bool BL0910SPI::read_array(uint8_t *data, size_t len) {
      // Fill data with dummy bytes (0x00) to send for reading
      // SPIDevice::transfer_array will overwrite data with received bytes and handles CS
      memset(data, 0x00, len);
      this->transfer_array(data, len);
      return true;
    }

    void BL0910SPI::write_array(const uint8_t *data, size_t len) {
      // Send each byte via SPI transfer
      for (size_t i = 0; i < len; i++) {
        this->transfer_byte(data[i]);
      }
    }

    void BL0910SPI::flush() 
    {
      // No buffer to flush in SPI mode
    }

    bool BL0910SPI::available() 
    {
      // SPI is always available for transaction
      return true;
    }

  } // namespace bl0910
} // namespace esphome
