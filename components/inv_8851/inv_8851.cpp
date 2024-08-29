#include "inv_8851.h"
#include "esphome/core/log.h"
#include "esphome/components/uart/uart_component.h"
#include "esphome/components/uart/uart_debugger.h"
#include <cinttypes>

namespace esphome {
namespace inv_8851 {

const uint8_t protocol_size = 2;
const uint8_t command_size = 2;
const uint8_t address_size = 2;
const uint8_t data_size_size = 2;
const uint8_t header_size = protocol_size + command_size + address_size + data_size_size;
const uint8_t crc_size = 2;
const uint8_t inv8851_protocol[protocol_size] = {0x88, 0x51};
const uint8_t read_command[command_size] = {0x00, 0x03};
const uint8_t write_command[command_size] = {0x00, 0x10};
const uint8_t state_address[address_size] = {0x00, 0x00};
const uint8_t config_address[address_size] = {0x02, 0x00};
const char *battery_type_options[] = { [AGM] = "AGM", [FLOODED] = "Flooded", [USER] = "User-defined", [LIB] = "Library" };
const char *charge_energy_priority_options[] = { [CSO] = "PV & Grid", [SNU] = "PV > Grid", [OSO] = "PV only" };
const char *frequency_options[] = { [FIFTY] = "50Hz", [SIXTY] = "60Hz" };
const char *grid_voltage_range_options[] = { [UPS] = "UPS", [APL] = "APL" };
const char *off_on_options[] = { [OFF] = "Off", [ON] = "On" };
const char *output_energy_priority_options[] = { [SUB] = "PV > Grid > Battery", [SBU] = "PV > Battery > Grid" };
const char *phase_options[] = { [A] = "A", [B] = "B", [C] = "C" };
const char *run_mode_options[] = { [standby_mode] = "standby", [fault_mode] = "fault", [shutdown_mode] = "shutdown", [normal_mode] = "normal", [no_battery_mode] = "no_battery", [discharge_mode] = "discharge", [parallel_discharge] = "parallel_disc", [bypass_mode] = "bypass", [charge_mode] = "charge", [grid_discharge_mode] = "grid_discharge", [micro_grid_discharge_mode] = "micro_grid_discharge" };

void Inv8851::clear_buffer_() {
  while (this->available()) this->read();
  this->last_read_ = millis();
}

uint16_t convert_le_(const uint8_t* data) {
  return data[1] << 8 | data[0];
}

std::string byte_array_to_string(const uint8_t *data, const uint16_t len) {
  std::string hex_string;
  for (size_t i = 0; i < len; ++i) {
    char buf[5];
    snprintf(buf, sizeof(buf), "%02X ", data[i]);
    hex_string += buf;
  }
  return hex_string;
}

Protocol Inv8851::read_protocol_() {
  uint8_t data[protocol_size];
  if (!this->read_array(data, protocol_size)) {
    ESP_LOGW(TAG, "Can't read protocol from buffer");
    return UNKNOWN_PROTOCOL;
  }
  ESP_LOGV(TAG, "Protocol: %02X %02X", data[0], data[1]);
  if (memcmp(data, inv8851_protocol, protocol_size) == 0) {
    ESP_LOGVV(TAG, "Protocol matches expected %02X %02X", inv8851_protocol[0], inv8851_protocol[1]);
    return INV8851_PROTOCOL;
  }
  ESP_LOGW(TAG, "Protocol %02X %02X doesn't match expected %02X %02X", data[0], data[1], inv8851_protocol[0], inv8851_protocol[1]);
  return UNKNOWN_PROTOCOL;
}

Command Inv8851::read_command_() {
  uint8_t data[protocol_size];
  if (!this->read_array(data, command_size)) {
    ESP_LOGW(TAG, "Can't read command from buffer");
    return UNKNOWN_COMMAND;
  }
  ESP_LOGV(TAG, "Command: %02X %02X", data[0], data[1]);
  if (memcmp(data, read_command, command_size) == 0) {
    ESP_LOGV(TAG, "This is read command");
    return READ_COMMAND;
  } else if (memcmp(data, write_command, command_size) == 0) {
    ESP_LOGV(TAG, "This is write command");
    return WRITE_COMMAND;
  }
  ESP_LOGW(TAG, "%02X %02X is neither read nor write command", data[0], data[1]);
  return UNKNOWN_COMMAND;
}

Address Inv8851::read_address_() {
  uint8_t data[address_size];
  if (!this->read_array(data, address_size)) {
    ESP_LOGW(TAG, "Can't read address from buffer");
    return UNKNOWN_ADDRESS;
  }
  ESP_LOGV(TAG, "Address: %02X %02X", data[0], data[1]);
  if (memcmp(data, state_address, address_size) == 0) {
    ESP_LOGV(TAG, "This is state address");
    return STATE_ADDRESS;
  } else if (memcmp(data, config_address, address_size) == 0) {
    ESP_LOGV(TAG, "This is config address");
    return CONFIG_ADDRESS;
  }
  ESP_LOGW(TAG, "%02X %02X is neither state or config address", data[0], data[1]);
  return UNKNOWN_ADDRESS;
}

esphome::optional<uint16_t> Inv8851::read_data_size_() {
  uint8_t data[data_size_size];
  if (!this->read_array(data, data_size_size)) {
    ESP_LOGW(TAG, "Can't read data size from buffer");
    return {};
  }
  auto data_size = convert_le_(data);
  ESP_LOGV(TAG, "Data size: %d", data_size);
  return data_size;
}

esphome::optional<std::vector<uint8_t>> Inv8851::read_data_(const uint16_t size) {
  while (this->available() < size + crc_size && millis() - this->last_read_ < 1000) {
    ESP_LOGVV(TAG, "Available %d < data size %d, waiting for data to arrive", this->available(), size);
    delay(50);
  }
  if (this->available() < size + crc_size) {
    ESP_LOGW(TAG, "Can't read data block from buffer");
    return {};
  }
  std::vector<uint8_t> data(size);
  if (!this->read_array(data.data(), size)) {
    ESP_LOGW(TAG, "Can't read data block from buffer");
    return {};
  }
  ESP_LOGV(TAG, "Data: %s", byte_array_to_string(data.data(), size).c_str());
  return data;
}

bool Inv8851::read_crc16_(const uint8_t *data, const uint8_t len) {
  uint8_t crc16_data[crc_size];
  if (!this->read_array(crc16_data, crc_size)) {
    ESP_LOGW(TAG, "Can't read CRC16 from buffer");
    return false;
  }
  auto actual_crc16 = convert_le_(crc16_data);
  ESP_LOGV(TAG, "CRC16: 0x%04X (%d)", actual_crc16, actual_crc16);
  auto expected_crc16 = crc16(data, len);
  if (actual_crc16 == expected_crc16) {
    ESP_LOGVV(TAG, "Actual CRC16 0x%04X (%d) matches expected CRC16 0x%04X (%d)", actual_crc16, actual_crc16, expected_crc16, expected_crc16);
    return true;
  }
  ESP_LOGW(TAG, "Actual CRC16 0x%04X (%d) doesn't match expected CRC16 0x%04X (%d)", actual_crc16, actual_crc16, expected_crc16, expected_crc16);
  ESP_LOGW(TAG, "Input for CRC16: %s", byte_array_to_string(data, len).c_str());
  return false;
}

void Inv8851::write_config_() {
  inv8851_config_s config;
  uint8_t *buff = (uint8_t *) &config;
  memcpy(&config, &this->config_, inv8851_config_pkt_len);
  config.command = INV8851_CONFIG_CMD_WRITE;
  config.data_size = inv8851_config_pkt_len - header_size - crc_size;
  config.crc = crc16(buff, inv8851_config_pkt_len - crc_size);
  this->write_array(buff, inv8851_config_pkt_len);
  publish_config_((uint8_t *) &config);
}

void Inv8851::setup() {
  this->clear_buffer_();
  this->update();
}

void Inv8851::loop() {
  if (millis() - this->last_read_ < 200 || this->available() < header_size) return;
  this->last_read_ = millis();
  auto protocol = this->read_protocol_();
  if (protocol == UNKNOWN_PROTOCOL) return this->clear_buffer_();
  auto command = this->read_command_();
  if (command == UNKNOWN_COMMAND || command == WRITE_COMMAND) return this->clear_buffer_();
  auto address = this->read_address_();
  if (address == UNKNOWN_ADDRESS) return this->clear_buffer_();
  auto data_size = this->read_data_size_();
  if (!data_size.has_value()) return this->clear_buffer_();
  auto data = this->read_data_(data_size.value());
  if (!data.has_value()) return this->clear_buffer_();
  uint8_t *packet = new uint8_t[header_size + data_size.value()];
  uint8_t offset = 0;
  std::memcpy(packet + offset, inv8851_protocol, protocol_size); offset += protocol_size;
  std::memcpy(packet + offset, read_command, command_size); offset += command_size;
  std::memcpy(packet + offset, address == STATE_ADDRESS ? state_address : config_address, address_size); offset += address_size;
  std::memcpy(packet + offset, reinterpret_cast<uint8_t*>(&data_size.value()), data_size_size); offset += data_size_size;
  std::memcpy(packet + offset, data.value().data(), data_size.value()); offset += data_size.value();
  if (!this->read_crc16_(packet, header_size + data_size.value())) return this->clear_buffer_();
  if (address == STATE_ADDRESS) this->publish_state_(packet);
  else this->publish_config_(packet);
}

void Inv8851::update() {
  if (this->state_update_) this->request_state_();
  else this->request_config_();
  this->state_update_ = !this->state_update_;
}

void Inv8851::dump_config() {
  ESP_LOGCONFIG(TAG, "INV_8851:");
  #ifdef USE_BINARY_SENSOR
    LOG_BINARY_SENSOR("", "Battery connected", this->battery_binary_sensor_);
    LOG_BINARY_SENSOR("", "Buck topology initialization", this->buck_topology_initialization_binary_sensor_);
    LOG_BINARY_SENSOR("", "Bus", this->bus_binary_sensor_);
    LOG_BINARY_SENSOR("", "Bus and grid voltage match", this->bus_and_grid_voltage_match_binary_sensor_);
    LOG_BINARY_SENSOR("", "Charging", this->charging_binary_sensor_);
    LOG_BINARY_SENSOR("", "Disable utility", this->disable_utility_binary_sensor_);
    LOG_BINARY_SENSOR("", "Equalization finished", this->equalization_finished_binary_sensor_);
    LOG_BINARY_SENSOR("", "Equalization started", this->equalization_started_binary_sensor_);
    LOG_BINARY_SENSOR("", "Float charging", this->float_charging_binary_sensor_);
    LOG_BINARY_SENSOR("", "Grid PLL", this->grid_pll_binary_sensor_);
    LOG_BINARY_SENSOR("", "Inverter topology initialization", this->inverter_topology_initialization_binary_sensor_);
    LOG_BINARY_SENSOR("", "LLC topology initialization", this->llc_topology_initialization_binary_sensor_);
    LOG_BINARY_SENSOR("", "Parallel lock phase problem", this->parallel_lock_phase_binary_sensor_);
    LOG_BINARY_SENSOR("", "PV excess", this->pv_excess_binary_sensor_);
    LOG_BINARY_SENSOR("", "PV input", this->pv_input_binary_sensor_);
    LOG_BINARY_SENSOR("", "PV topology initialization", this->pv_topology_initialization_binary_sensor_);
    LOG_BINARY_SENSOR("", "System power", this->system_power_binary_sensor_);
  #endif

  #ifdef USE_TEXT_SENSOR
    LOG_TEXT_SENSOR("", "Buck topology", this->buck_topology_text_sensor_);
    LOG_TEXT_SENSOR("", "Inverter topology", this->inverter_topology_text_sensor_);
    LOG_TEXT_SENSOR("", "LLC topology", this->llc_topology_text_sensor_);
    LOG_TEXT_SENSOR("", "PV topology", this->pv_topology_text_sensor_);
  #endif

  #ifdef USE_SENSOR
    LOG_SENSOR("", "Battery charge current", this->battery_charge_current_sensor_);
    LOG_SENSOR("", "Battery voltage", this->battery_voltage_sensor_);
    LOG_SENSOR("", "BMS battery current", this->bms_battery_current_sensor_);
    LOG_SENSOR("", "BMS battery SOC", this->bms_battery_soc_sensor_);
    LOG_SENSOR("", "BMS battery voltage", this->bms_battery_voltage_sensor_);
    LOG_SENSOR("", "BMS cell 01 voltage", this->bms_cell_01_voltage_sensor_);
    LOG_SENSOR("", "BMS cell 02 voltage", this->bms_cell_02_voltage_sensor_);
    LOG_SENSOR("", "BMS cell 03 voltage", this->bms_cell_03_voltage_sensor_);
    LOG_SENSOR("", "BMS cell 04 voltage", this->bms_cell_04_voltage_sensor_);
    LOG_SENSOR("", "BMS cell 05 voltage", this->bms_cell_05_voltage_sensor_);
    LOG_SENSOR("", "BMS cell 06 voltage", this->bms_cell_06_voltage_sensor_);
    LOG_SENSOR("", "BMS cell 07 voltage", this->bms_cell_07_voltage_sensor_);
    LOG_SENSOR("", "BMS cell 08 voltage", this->bms_cell_08_voltage_sensor_);
    LOG_SENSOR("", "BMS cell 09 voltage", this->bms_cell_09_voltage_sensor_);
    LOG_SENSOR("", "BMS cell 10 voltage", this->bms_cell_10_voltage_sensor_);
    LOG_SENSOR("", "BMS cell 11 voltage", this->bms_cell_11_voltage_sensor_);
    LOG_SENSOR("", "BMS cell 12 voltage", this->bms_cell_12_voltage_sensor_);
    LOG_SENSOR("", "BMS cell 13 voltage", this->bms_cell_13_voltage_sensor_);
    LOG_SENSOR("", "BMS cell 14 voltage", this->bms_cell_14_voltage_sensor_);
    LOG_SENSOR("", "BMS cell 15 voltage", this->bms_cell_15_voltage_sensor_);
    LOG_SENSOR("", "BMS cell 16 voltage", this->bms_cell_16_voltage_sensor_);
    LOG_SENSOR("", "BTS temperature", this->bts_temperature_sensor_);
    LOG_SENSOR("", "Bus voltage", this->bus_voltage_sensor_);
    LOG_SENSOR("", "Fan 1 speed percentage", this->fan1_speed_percentage_sensor_);
    LOG_SENSOR("", "Fan 2 speed percentage", this->fan2_speed_percentage_sensor_);
    LOG_SENSOR("", "Grid current", this->grid_current_sensor_);
    LOG_SENSOR("", "Grid frequency", this->grid_frequency_sensor_);
    LOG_SENSOR("", "Grid voltage", this->grid_voltage_sensor_);
    LOG_SENSOR("", "Inverter apparent power", this->inverter_apparent_power_sensor_);
    LOG_SENSOR("", "Inverter apparent power percentage", this->inverter_apparent_power_percentage_sensor_);
    LOG_SENSOR("", "Inverter current", this->inverter_current_sensor_);
    LOG_SENSOR("", "Inverter frequency", this->inverter_frequency_sensor_);
    LOG_SENSOR("", "Inverter power percentage", this->inverter_power_percentage_sensor_);
    LOG_SENSOR("", "Inverter software version", this->inverter_software_version_sensor_);
    LOG_SENSOR("", "Inverter voltage", this->inverter_voltage_sensor_);
    LOG_SENSOR("", "Inverter voltage DC component", this->inverter_voltage_dc_component_sensor_);
    LOG_SENSOR("", "Load apparent power", this->load_apparent_power_sensor_);
    LOG_SENSOR("", "Load current", this->load_current_sensor_);
    LOG_SENSOR("", "Load power", this->load_power_sensor_);
    LOG_SENSOR("", "Log number", this->log_number_sensor_);
    LOG_SENSOR("", "Low load current", this->low_load_current_sensor_);
    LOG_SENSOR("", "NTC2 temperature", this->ntc2_temperature_sensor_);
    LOG_SENSOR("", "NTC3 temperature", this->ntc3_temperature_sensor_);
    LOG_SENSOR("", "NTC4 temperature", this->ntc4_temperature_sensor_);
    LOG_SENSOR("", "Parallel current", this->parallel_current_sensor_);
    LOG_SENSOR("", "Parallel frequency", this->parallel_frequency_sensor_);
    LOG_SENSOR("", "Parallel voltage", this->parallel_voltage_sensor_);
    LOG_SENSOR("", "PV current", this->pv_current_sensor_);
    LOG_SENSOR("", "PV power", this->pv_power_sensor_);
    LOG_SENSOR("", "PV voltage", this->pv_voltage_sensor_);
  #endif

  #ifdef USE_SELECT
    LOG_SELECT("", "Auto return", this->auto_return_select_);
    LOG_SELECT("", "Backlight", this->backlight_select_);
    LOG_SELECT("", "Battery equalization", this->battery_equalization_select_);
    LOG_SELECT("", "Battery type", this->battery_type_select_);
    LOG_SELECT("", "Buzzer", this->buzzer_select_);
    LOG_SELECT("", "Charge energy priority", this->charge_energy_priority_select_);
    LOG_SELECT("", "Fault record", this->fault_record_select_);
    LOG_SELECT("", "Frequency", this->frequency_select_);
    LOG_SELECT("", "Grid voltage range", this->grid_voltage_range_select_);
    LOG_SELECT("", "On grid", this->on_grid_select_);
    LOG_SELECT("", "Output energy priority", this->output_energy_priority_select_);
    LOG_SELECT("", "Overload restart", this->overload_restart_select_);
    LOG_SELECT("", "Overtemp restart", this->overtemp_restart_select_);
    LOG_SELECT("", "Parallel operation", this->parallel_operation_select_);
    LOG_SELECT("", "Phase", this->phase_select_);
    LOG_SELECT("", "Power buzzer", this->power_buzzer_select_);
    LOG_SELECT("", "Powersave mode", this->powersave_mode_select_);
    LOG_SELECT("", "Warning buzer", this->warning_buzer_select_);
  #endif

  #ifdef USE_NUMBER
    LOG_NUMBER("", "Battery back to util voltage", this->battery_back_to_util_voltage_number_);
    LOG_NUMBER("", "Battery bulk voltage", this->battery_bulk_voltage_number_);
    LOG_NUMBER("", "Battery float voltage", this->battery_float_voltage_number_);
    LOG_NUMBER("", "Battery charge cut-off current", this->battery_charge_cut_off_current_number_);
    LOG_NUMBER("", "Battery cut-off voltage", this->battery_cut_off_voltage_number_);
    LOG_NUMBER("", "Battery equalization interval", this->battery_equalization_interval_number_);
    LOG_NUMBER("", "Battery equalization time", this->battery_equalization_time_number_);
    LOG_NUMBER("", "Battery equalization timeout", this->battery_equalization_timeout_number_);
    LOG_NUMBER("", "Battery equalization voltage", this->battery_equalization_voltage_number_);
    LOG_NUMBER("", "Inverter maximum power", this->inverter_maximum_power_number_);
    LOG_NUMBER("", "Output frequency", this->output_frequency_number_);
    LOG_NUMBER("", "Output voltage", this->output_voltage_number_);
    LOG_NUMBER("", "Total charge current", this->total_charge_current_number_);
    LOG_NUMBER("", "Util charge current", this->util_charge_current_number_);
  #endif
}

void Inv8851::request_state_() {
  this->write_array({0x88, 0x51, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x4d, 0x08});
}

void Inv8851::request_config_() {
  this->write_array({0x88, 0x51, 0x00, 0x03, 0x02, 0x00, 0x00, 0x00, 0x4c, 0xb0});
}

void Inv8851::publish_state_(const uint8_t *resp) {
  inv8851_state_s *state = (inv8851_state_s *) resp;
  #ifdef USE_BINARY_SENSOR
    PUBLISH_STATE(this->battery_binary_sensor_, !state->no_battery);
    PUBLISH_STATE(this->buck_topology_initialization_binary_sensor_, !!state->buck_topology_initial_finished);
    PUBLISH_STATE(this->bus_binary_sensor_, !!state->bus_ok);
    PUBLISH_STATE(this->bus_and_grid_voltage_match_binary_sensor_, !!state->bus_n_grid_voltage_match);
    PUBLISH_STATE(this->charging_binary_sensor_, !state->charge_finish);
    PUBLISH_STATE(this->disable_utility_binary_sensor_, !!state->disable_utility);
    PUBLISH_STATE(this->equalization_finished_binary_sensor_, !!state->eq_charge_ready);
    PUBLISH_STATE(this->equalization_started_binary_sensor_, !!state->eq_charge_start);
    PUBLISH_STATE(this->float_charging_binary_sensor_, !!state->floating_charge);
    PUBLISH_STATE(this->grid_pll_binary_sensor_, !!state->grid_pll_ok);
    PUBLISH_STATE(this->inverter_topology_initialization_binary_sensor_, !!state->inverter_topology_initial_finished);
    PUBLISH_STATE(this->llc_topology_initialization_binary_sensor_, !!state->llc_topology_initial_finished);
    PUBLISH_STATE(this->parallel_lock_phase_binary_sensor_, !!state->parallel_lock_phase_ok);
    PUBLISH_STATE(this->pv_excess_binary_sensor_, !!state->pv_excess);
    PUBLISH_STATE(this->pv_input_binary_sensor_, !!state->pv_input_ok);
    PUBLISH_STATE(this->pv_topology_initialization_binary_sensor_, !!state->pv_topology_initial_finished);
    PUBLISH_STATE(this->system_initialization_binary_sensor_, !!state->system_initial_finished);
    PUBLISH_STATE(this->system_power_binary_sensor_, !!state->system_power);
  #endif

  #ifdef USE_TEXT_SENSOR
    PUBLISH_STATE(this->buck_topology_text_sensor_, run_mode_options[state->buck_topology]);
    PUBLISH_STATE(this->inverter_topology_text_sensor_, run_mode_options[state->inverter_topology]);
    PUBLISH_STATE(this->llc_topology_text_sensor_, run_mode_options[state->llc_topology]);
    PUBLISH_STATE(this->pv_topology_text_sensor_, run_mode_options[state->pv_topology]);
  #endif

  #ifdef USE_SENSOR
    PUBLISH_STATE(this->battery_charge_current_sensor_, state->batt_charge_current / 10.0f);
    PUBLISH_STATE(this->battery_voltage_sensor_, state->batt_voltage / 100.0f);
    PUBLISH_STATE(this->bms_battery_current_sensor_, state->bms_battery_current / 10.0f);
    PUBLISH_STATE(this->bms_battery_soc_sensor_, state->bms_battery_soc);
    PUBLISH_STATE(this->bms_battery_voltage_sensor_, state->bms_battery_voltage / 100.0f);
    PUBLISH_STATE(this->bms_cell_01_voltage_sensor_, state->bms_01cell_voltage / 100.0f);
    PUBLISH_STATE(this->bms_cell_02_voltage_sensor_, state->bms_02cell_voltage / 100.0f);
    PUBLISH_STATE(this->bms_cell_03_voltage_sensor_, state->bms_03cell_voltage / 100.0f);
    PUBLISH_STATE(this->bms_cell_04_voltage_sensor_, state->bms_04cell_voltage / 100.0f);
    PUBLISH_STATE(this->bms_cell_05_voltage_sensor_, state->bms_05cell_voltage / 100.0f);
    PUBLISH_STATE(this->bms_cell_06_voltage_sensor_, state->bms_06cell_voltage / 100.0f);
    PUBLISH_STATE(this->bms_cell_07_voltage_sensor_, state->bms_07cell_voltage / 100.0f);
    PUBLISH_STATE(this->bms_cell_08_voltage_sensor_, state->bms_08cell_voltage / 100.0f);
    PUBLISH_STATE(this->bms_cell_09_voltage_sensor_, state->bms_09cell_voltage / 100.0f);
    PUBLISH_STATE(this->bms_cell_10_voltage_sensor_, state->bms_10cell_voltage / 100.0f);
    PUBLISH_STATE(this->bms_cell_11_voltage_sensor_, state->bms_11cell_voltage / 100.0f);
    PUBLISH_STATE(this->bms_cell_12_voltage_sensor_, state->bms_12cell_voltage / 100.0f);
    PUBLISH_STATE(this->bms_cell_13_voltage_sensor_, state->bms_13cell_voltage / 100.0f);
    PUBLISH_STATE(this->bms_cell_14_voltage_sensor_, state->bms_14cell_voltage / 100.0f);
    PUBLISH_STATE(this->bms_cell_15_voltage_sensor_, state->bms_15cell_voltage / 100.0f);
    PUBLISH_STATE(this->bms_cell_16_voltage_sensor_, state->bms_16cell_voltage / 100.0f);
    PUBLISH_STATE(this->bts_temperature_sensor_, state->bts_temperature);
    PUBLISH_STATE(this->bus_voltage_sensor_, state->bus_voltage / 10.0f);
    PUBLISH_STATE(this->fan1_speed_percentage_sensor_, state->fan1_speed_percent);
    PUBLISH_STATE(this->fan2_speed_percentage_sensor_, state->fan2_speed_percent);
    PUBLISH_STATE(this->grid_current_sensor_, state->grid_current / 100.0f);
    PUBLISH_STATE(this->grid_frequency_sensor_, state->grid_freq / 100.0f);
    PUBLISH_STATE(this->grid_voltage_sensor_, state->grid_voltage / 10.0f);
    PUBLISH_STATE(this->inverter_apparent_power_sensor_, state->inv_va);
    PUBLISH_STATE(this->inverter_apparent_power_percentage_sensor_, state->inverter_va_percent);
    PUBLISH_STATE(this->inverter_current_sensor_, state->inv_current / 100.0f);
    PUBLISH_STATE(this->inverter_frequency_sensor_, state->inv_freq / 100.0f);
    PUBLISH_STATE(this->inverter_power_percentage_sensor_, state->inverter_watt_percent);
    PUBLISH_STATE(this->inverter_software_version_sensor_, state->software_version);
    PUBLISH_STATE(this->inverter_voltage_sensor_, state->inv_voltage / 10.0f);
    PUBLISH_STATE(this->inverter_voltage_dc_component_sensor_, state->inverter_voltage_dc_component);
    PUBLISH_STATE(this->load_apparent_power_sensor_, state->load_va);
    PUBLISH_STATE(this->load_current_sensor_, state->load_current / 100.0f);
    PUBLISH_STATE(this->load_power_sensor_, state->load_watt);
    PUBLISH_STATE(this->log_number_sensor_, state->log_number);
    PUBLISH_STATE(this->low_load_current_sensor_, state->low_load_current / 100.0f);
    PUBLISH_STATE(this->ntc2_temperature_sensor_, state->ntc2_temperature);
    PUBLISH_STATE(this->ntc3_temperature_sensor_, state->ntc3_temperature);
    PUBLISH_STATE(this->ntc4_temperature_sensor_, state->ntc4_temperature);
    PUBLISH_STATE(this->parallel_current_sensor_, state->parallel_current / 100.0f);
    PUBLISH_STATE(this->parallel_frequency_sensor_, state->parallel_frequency / 100.0f);
    PUBLISH_STATE(this->parallel_voltage_sensor_, state->parallel_voltage / 10.0f);
    PUBLISH_STATE(this->pv_current_sensor_, state->pv_current / 100.0f);
    PUBLISH_STATE(this->pv_power_sensor_, state->pv_power);
    PUBLISH_STATE(this->pv_voltage_sensor_, state->pv_voltage / 10.0f);
  #endif
}

void Inv8851::publish_config_(const uint8_t *resp) {
  memcpy(&this->config_, resp, inv8851_config_pkt_len);
  inv8851_config_s *config = (inv8851_config_s *) resp;
  #ifdef USE_SELECT
    PUBLISH_STATE(this->auto_return_select_, off_on_options[config->auto_return_to_default_screen]);
    PUBLISH_STATE(this->backlight_select_, off_on_options[config->backlight_on]);
    PUBLISH_STATE(this->battery_equalization_select_, off_on_options[config->battery_equalization]);
    PUBLISH_STATE(this->battery_type_select_, battery_type_options[config->battery_type]);
    PUBLISH_STATE(this->buzzer_select_, off_on_options[config->alarm_control]);
    PUBLISH_STATE(this->charge_energy_priority_select_, charge_energy_priority_options[config->charge_energy_priority]);
    PUBLISH_STATE(this->fault_record_select_, off_on_options[config->fault_record_enable]);
    PUBLISH_STATE(this->frequency_select_, frequency_options[config->frequency]);
    PUBLISH_STATE(this->grid_voltage_range_select_, grid_voltage_range_options[config->grid_voltage_range]);
    PUBLISH_STATE(this->on_grid_select_, off_on_options[config->grid_enable]);
    PUBLISH_STATE(this->output_energy_priority_select_, output_energy_priority_options[config->output_energy_priority]);
    PUBLISH_STATE(this->overload_restart_select_, off_on_options[config->output_OPP_auto_restart]);
    PUBLISH_STATE(this->overtemp_restart_select_, off_on_options[config->otp_auto_restart]);
    PUBLISH_STATE(this->parallel_operation_select_, off_on_options[config->parallel_operation]);
    PUBLISH_STATE(this->phase_select_, phase_options[config->phase]);
    PUBLISH_STATE(this->power_buzzer_select_, off_on_options[config->energy_interrupt_buzzer_on]);
    PUBLISH_STATE(this->powersave_mode_select_, off_on_options[config->powersave_on]);
    PUBLISH_STATE(this->warning_buzer_select_, off_on_options[config->warning_flag_buzer_on]);
  #endif

  #ifdef USE_NUMBER
    PUBLISH_STATE(this->battery_back_to_util_voltage_number_, config->batt_pont_back_to_util_volt / 100.0f);
    PUBLISH_STATE(this->battery_bulk_voltage_number_, config->batt_bulk_chg_voltage / 100.0f);
    PUBLISH_STATE(this->battery_float_voltage_number_, config->batt_float_chg_voltage / 100.0f);
    PUBLISH_STATE(this->battery_charge_cut_off_current_number_, config->batt_chg_cut_off_current / 10.0f);
    PUBLISH_STATE(this->battery_cut_off_voltage_number_, config->batt_cut_off_voltage / 100.0f);
    PUBLISH_STATE(this->battery_equalization_interval_number_, config->batt_eq_interval);
    PUBLISH_STATE(this->battery_equalization_time_number_, config->batt_eq_time);
    PUBLISH_STATE(this->battery_equalization_timeout_number_, config->batt_eq_timeout);
    PUBLISH_STATE(this->battery_equalization_voltage_number_, config->batt_eq_voltage / 100.0f);
    PUBLISH_STATE(this->inverter_maximum_power_number_, config->inverter_max_power);
    PUBLISH_STATE(this->output_frequency_number_, config->output_freq / 100.f);
    PUBLISH_STATE(this->output_voltage_number_, config->output_voltage / 10.0f);
    PUBLISH_STATE(this->total_charge_current_number_, config->total_chg_current / 10.0f);
    PUBLISH_STATE(this->util_charge_current_number_, config->util_chg_current / 10.0f);
  #endif
}

void Inv8851::set_select_value(const std::string type, size_t index) {
  if (type == "auto_return") this->config_.auto_return_to_default_screen = index;
  else if (type == "backlight") this->config_.backlight_on = index;
  else if (type == "battery_equalization") this->config_.battery_equalization = index;
  else if (type == "battery_type") this->config_.battery_type = index;
  else if (type == "buzzer") this->config_.alarm_control = index;
  else if (type == "charge_energy_priority") this->config_.charge_energy_priority = index;
  else if (type == "fault_record") this->config_.fault_record_enable = index;
  else if (type == "frequency") this->config_.frequency = index;
  else if (type == "grid_voltage_range") this->config_.grid_voltage_range = index;
  else if (type == "on_grid") this->config_.grid_enable = index;
  else if (type == "output_energy_priority") this->config_.output_energy_priority = index;
  else if (type == "overload_restart") this->config_.output_OPP_auto_restart = index;
  else if (type == "overtemp_restart") this->config_.otp_auto_restart = index;
  else if (type == "parallel_operation") this->config_.parallel_operation = index;
  else if (type == "phase") this->config_.phase = index;
  else if (type == "power_buzzer") this->config_.energy_interrupt_buzzer_on = index;
  else if (type == "powersave_mode") this->config_.powersave_on = index;
  else if (type == "warning_buzer") this->config_.warning_flag_buzer_on = index;
  else return;

  this->write_config_();
}

void Inv8851::set_number_value(const std::string type, float value) {
  if (type == "battery_back_to_util_voltage") this->config_.batt_pont_back_to_util_volt = value * 100.0f;
  else if (type == "battery_bulk_voltage") this->config_.batt_bulk_chg_voltage = value * 100.0f;
  else if (type == "battery_float_voltage") this->config_.batt_float_chg_voltage = value * 100.0f;
  else if (type == "battery_charge_cut_off_current") this->config_.batt_chg_cut_off_current = value * 10.0f;
  else if (type == "battery_cut_off_voltage") this->config_.batt_cut_off_voltage = value * 100.0f;
  else if (type == "battery_equalization_interval") this->config_.batt_eq_interval = value;
  else if (type == "battery_equalization_time") this->config_.batt_eq_time = value;
  else if (type == "battery_equalization_timeout") this->config_.batt_eq_timeout = value;
  else if (type == "battery_equalization_voltage") this->config_.batt_eq_voltage = value * 100.0f;
  else if (type == "inverter_maximum_power") this->config_.inverter_max_power = value;
  else if (type == "output_frequency") this->config_.output_freq = value * 100.0f;
  else if (type == "output_voltage") this->config_.output_voltage = value * 10.0f;
  else if (type == "total_charge_current") this->config_.total_chg_current = value * 10.0f;
  else if (type == "util_charge_current") this->config_.util_chg_current = value * 10.0f;
  else return;

  this->write_config_();
}

}  // namespace inv_8851
}  // namespace esphome
