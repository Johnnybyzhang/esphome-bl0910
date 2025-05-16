from esphome import automation
from esphome.automation import maybe_simple_id
import esphome.codegen as cg
from esphome.components import sensor, uart, spi
import esphome.config_validation as cv
from esphome.const import (
    CONF_CHANNEL, CONF_CURRENT, CONF_ENERGY, CONF_FREQUENCY, CONF_ID, CONF_NAME, CONF_POWER, CONF_TEMPERATURE, CONF_TOTAL_POWER, CONF_VOLTAGE, CONF_POWER_FACTOR, DEVICE_CLASS_CURRENT, DEVICE_CLASS_ENERGY, DEVICE_CLASS_FREQUENCY, DEVICE_CLASS_POWER, DEVICE_CLASS_TEMPERATURE, DEVICE_CLASS_VOLTAGE, DEVICE_CLASS_POWER_FACTOR, ICON_CURRENT_AC, ICON_THERMOMETER, STATE_CLASS_MEASUREMENT, STATE_CLASS_TOTAL_INCREASING, UNIT_AMPERE, UNIT_CELSIUS, UNIT_HERTZ, UNIT_KILOWATT_HOURS, UNIT_VOLT, UNIT_WATT, CONF_CS_PIN, CONF_MODE,
)

# Custom icons
ICON_POWER = "mdi:gauge"
ICON_ENERGY = "mdi:lightning-bolt"
ICON_FREQUENCY = "mdi:metronome"
ICON_VOLTAGE = "mdi:sine-wave"
ICON_POWER_FACTOR = "mdi:angle-acute"

# Depends on UART or SPI components based on the mode
MULTI_CONF = True
DEPENDENCIES = ["sensor"]
AUTO_LOAD = []

# Custom constants
CONF_TOTAL_ENERGY = "total_energy"
CONF_COMMUNICATION_MODE = "communication_mode"
CONF_MODE_UART = "uart"
CONF_MODE_SPI = "spi"

# Define namespace and classes
bl0910_ns = cg.esphome_ns.namespace("bl0910")
BL0910 = bl0910_ns.class_("BL0910", cg.PollingComponent)
BL0910UART = bl0910_ns.class_("BL0910UART", BL0910, uart.UARTDevice)
BL0910SPI = bl0910_ns.class_("BL0910SPI", BL0910, spi.SPIDevice)
ResetEnergyAction = bl0910_ns.class_("ResetEnergyAction", automation.Action)

# Sensor schema creation helper
def create_sensor_schema(icon, accuracy_decimals, device_class, unit, state_class):
    return sensor.sensor_schema(
        icon=icon,
        accuracy_decimals=accuracy_decimals,
        device_class=device_class,
        unit_of_measurement=unit,
        state_class=state_class,
    )

# Base configuration schema without communication specific options
BASE_CONFIG_SCHEMA = cv.Schema(
    {
        cv.Optional(CONF_FREQUENCY): create_sensor_schema(ICON_FREQUENCY, 2, DEVICE_CLASS_FREQUENCY, UNIT_HERTZ, STATE_CLASS_MEASUREMENT),
        cv.Optional(CONF_TEMPERATURE): create_sensor_schema(ICON_THERMOMETER, 2, DEVICE_CLASS_TEMPERATURE, UNIT_CELSIUS, STATE_CLASS_MEASUREMENT),
        cv.Optional(CONF_VOLTAGE): create_sensor_schema(ICON_VOLTAGE, 1, DEVICE_CLASS_VOLTAGE, UNIT_VOLT, STATE_CLASS_MEASUREMENT),
        cv.Optional(CONF_TOTAL_POWER): create_sensor_schema(ICON_POWER, 3, DEVICE_CLASS_POWER, UNIT_WATT, STATE_CLASS_MEASUREMENT),
        cv.Optional(CONF_TOTAL_ENERGY): create_sensor_schema(ICON_ENERGY, 3, DEVICE_CLASS_ENERGY, UNIT_KILOWATT_HOURS, STATE_CLASS_TOTAL_INCREASING),
    }
).extend(
    cv.Schema(
        {
            cv.Optional(f"{CONF_CHANNEL}_{i + 1}"): cv.Schema(
                {
                    cv.Optional(CONF_CURRENT): cv.maybe_simple_value(
                        create_sensor_schema(ICON_CURRENT_AC, 3, DEVICE_CLASS_CURRENT, UNIT_AMPERE, STATE_CLASS_MEASUREMENT),
                        key=CONF_NAME,
                    ),
                    cv.Optional(CONF_POWER): cv.maybe_simple_value(
                        create_sensor_schema(ICON_POWER, 3, DEVICE_CLASS_POWER, UNIT_WATT, STATE_CLASS_MEASUREMENT),
                        key=CONF_NAME,
                    ),
                    cv.Optional(CONF_ENERGY): cv.maybe_simple_value(
                        create_sensor_schema(ICON_ENERGY, 3, DEVICE_CLASS_ENERGY, UNIT_KILOWATT_HOURS, STATE_CLASS_TOTAL_INCREASING),
                        key=CONF_NAME,
                    ),
                    cv.Optional(CONF_POWER_FACTOR): cv.maybe_simple_value(
                        create_sensor_schema(ICON_POWER_FACTOR, 3, DEVICE_CLASS_POWER_FACTOR, "", STATE_CLASS_MEASUREMENT),
                        key=CONF_NAME,
                    ),
                }
            )
            for i in range(10) # Create 10 channel configurations
        }
    )
).extend(cv.polling_component_schema("10s"))

# Communication mode configuration
COMM_MODE_SCHEMA = cv.Schema(
    {
        cv.Required(CONF_MODE): cv.enum({CONF_MODE_UART: 0, CONF_MODE_SPI: 1}, lower=True),
    }
)

# UART mode configuration
UART_CONFIG_SCHEMA = BASE_CONFIG_SCHEMA.extend(uart.UART_DEVICE_SCHEMA).extend(
    {
        cv.GenerateID(): cv.declare_id(BL0910UART),
    }
)

# SPI mode configuration
SPI_CONFIG_SCHEMA = BASE_CONFIG_SCHEMA.extend(spi.spi_device_schema()).extend(
    {
        cv.GenerateID(): cv.declare_id(BL0910SPI),
    }
)

# Combined configuration schema
CONFIG_SCHEMA = cv.typed_schema(
    {
        CONF_MODE_UART: UART_CONFIG_SCHEMA,
        CONF_MODE_SPI: SPI_CONFIG_SCHEMA,
    },
    key=CONF_MODE,
    default_type=CONF_MODE_UART,
)

# Device validation differs based on communication mode
def final_validate(config):
    if config[CONF_MODE] == CONF_MODE_UART:
        return uart.final_validate_device_schema(
            "bl0910", baud_rate=19200, require_tx=True, require_rx=True
        )(config)
    return config

FINAL_VALIDATE_SCHEMA = final_validate

# Register "reset energy" action
@automation.register_action(
    "bl0910.reset_energy",
    ResetEnergyAction,
    maybe_simple_id(
        {
            cv.Required(CONF_ID): cv.use_id(BL0910),
        }
    ),
)
async def reset_energy_to_code(config, action_id, template_arg, args):
    var = cg.new_Pvariable(action_id, template_arg)
    await cg.register_parented(var, config[CONF_ID])
    return var

# Helper function to create and register sensors
async def register_sensor(var, config, sensor_name, sensor_fn):
    if sensor_config := config.get(sensor_name):
        sens = await sensor.new_sensor(sensor_config)
        cg.add(sensor_fn(sens))

# Main function: generate code based on configuration
async def to_code(config):
    mode = config[CONF_MODE]
    
    if mode == CONF_MODE_UART:
        var = cg.new_Pvariable(config[CONF_ID])
        await cg.register_component(var, config)
        await uart.register_uart_device(var, config)
        # Set UART communication mode
        cg.add(var.set_comm_mode(cg.RawExpression("esphome::bl0910::CommunicationMode::UART")))
    
    elif mode == CONF_MODE_SPI:
        var = cg.new_Pvariable(config[CONF_ID])
        await cg.register_component(var, config)
        await spi.register_spi_device(var, config)
        # Set SPI communication mode
        cg.add(var.set_comm_mode(cg.RawExpression("esphome::bl0910::CommunicationMode::SPI")))

    # Register sensors: frequency, temperature, voltage, total power, total energy
    await register_sensor(var, config, CONF_FREQUENCY, var.set_frequency_sensor)
    await register_sensor(var, config, CONF_TEMPERATURE, var.set_temperature_sensor)
    await register_sensor(var, config, CONF_VOLTAGE, var.set_voltage_sensor)
    await register_sensor(var, config, CONF_TOTAL_POWER, var.set_total_power_sensor)
    await register_sensor(var, config, CONF_TOTAL_ENERGY, var.set_total_energy_sensor)

    # Loop through 10 channels, register current, power, energy and power factor sensors for each
    for i in range(10):
        if channel_config := config.get(f"{CONF_CHANNEL}_{i + 1}"):
            await register_sensor(var, channel_config, CONF_CURRENT, getattr(var, f"set_current_{i + 1}_sensor"))
            await register_sensor(var, channel_config, CONF_POWER, getattr(var, f"set_power_{i + 1}_sensor"))
            await register_sensor(var, channel_config, CONF_ENERGY, getattr(var, f"set_energy_{i + 1}_sensor"))
            await register_sensor(var, channel_config, CONF_POWER_FACTOR, getattr(var, f"set_power_factor_{i + 1}_sensor")) 