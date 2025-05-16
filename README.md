# BL0910 Energy Monitoring Component for ESPHome

This component provides support for the BL0910 energy monitoring chip in ESPHome, allowing you to monitor voltage, current, power, energy, and other electrical parameters for up to 10 channels.

## Features

- Supports both UART and SPI communication modes
- UART mode: suitable for single-chip setups
- SPI mode: suitable for both single-chip and multi-chip setups
- Monitors voltage, frequency, and temperature
- Monitors current, power, energy, and power factor for up to 10 channels
- Tracks total power and energy consumption
- Support for resetting energy counters via automations

## Installation

1.  **Copy Component Files**: 
    Create a directory named `bl0910` inside your ESPHome configuration's `custom_components` directory (e.g., `<esphome_config_dir>/custom_components/bl0910/`). If `custom_components` doesn't exist, create it.
    Copy the following files into the `custom_components/bl0910/` directory:
    - `__init__.py`
    - `bl0910.h`
    - `bl0910.cpp`
    - `constants.h`
    - `sensor.py`

2.  **Configure ESPHome**: 
    In your main ESPHome YAML configuration file (e.g., `your_device.yaml`), add the `external_components` block to tell ESPHome where to find your custom component:

    ```yaml
    esphome:
      # ... other esphome settings
      external_components:
        - source: ./custom_components # Or the full path to your custom_components/bl0910 directory
          components: [ bl0910 ] 
    ```
    If you placed `custom_components` directly in the same directory as your device's YAML file, the relative path `./custom_components` should work. Adjust the path if your structure is different.

## Configuration

Once the external component is declared, you can configure the `bl0910` component as follows:

### UART Mode (Single Chip)

```yaml
uart:
  tx_pin: GPIO1
  rx_pin: GPIO3
  baud_rate: 19200
  id: uart_bus

bl0910:
  mode: uart
  id: my_energy_monitor_uart
  update_interval: 10s
  frequency:
    name: "Mains Frequency UART"
  voltage:
    name: "Mains Voltage UART"
  temperature:
    name: "BL0910 Temperature UART"
  total_power:
    name: "Total Power UART"
  total_energy:
    name: "Total Energy UART"
  channel_1:
    current:
      name: "Channel 1 Current UART"
    power:
      name: "Channel 1 Power UART"
    energy:
      name: "Channel 1 Energy UART"
    power_factor:
      name: "Channel 1 Power Factor UART"
```

### SPI Mode (Single or Multi-Chip)

```yaml
spi:
  clk_pin: GPIO14
  mosi_pin: GPIO13
  miso_pin: GPIO12
  id: spi_bus

bl0910:
  mode: spi
  id: my_energy_monitor_spi
  cs_pin: GPIO15  # Chip select pin for SPI communication
  update_interval: 10s
  frequency:
    name: "Mains Frequency SPI"
  voltage:
    name: "Mains Voltage SPI"
  # Add other sensors as shown in the UART example
```

### Multi-Chip Setup with SPI

For a multi-chip setup, define multiple `bl0910` components with different CS pins:

```yaml
bl0910:
  - mode: spi
    id: energy_monitor_1
    cs_pin: GPIO15
    # Add sensors...
  - mode: spi
    id: energy_monitor_2
    cs_pin: GPIO4
    # Add sensors...
```

## Automations

### Reset Energy Counters

Make sure to use the correct `id` of your `bl0910` component instance.

```yaml
time:
  - platform: homeassistant
    id: midnight
    on_time:
      - hours: 0
        minutes: 0
        seconds: 0
        then:
          - bl0910.reset_energy: my_energy_monitor_spi # Or my_energy_monitor_uart
```

## Available Sensors

Each `bl0910` component can include the following sensors:

- **Global Sensors**:
  - `frequency`: Mains frequency in Hz
  - `voltage`: Mains voltage in Volts
  - `temperature`: Chip temperature in °C
  - `total_power`: Total power consumption in Watts
  - `total_energy`: Total energy consumption in kWh

- **Per-Channel Sensors** (for each `channel_1` through `channel_10`):
  - `current`: Current in Amperes
  - `power`: Power in Watts
  - `energy`: Energy in kWh
  - `power_factor`: Power factor (dimensionless)

## Technical Details

- The BL0910 chip supports up to 10 channels of current/power/energy measurement
- UART mode communicates at 19200 baud rate
- SPI mode uses the following configuration:
  - Bit order: MSB first
  - Clock polarity: Low
  - Clock phase: Leading
  - Data rate: 1 MHz

## References

- BL0910 datasheet: [Link to datasheet if available] 