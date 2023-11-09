#include "inv_8851.h"
#include "esphome/core/log.h"
#include <cinttypes>

namespace esphome {
namespace inv_8851 {

static const char *const TAG = "inv_8851";

void INV_8851::clear_buffer_() {
  while (this->available()) this->read();
  this->last_read_ = millis();
}

bool INV_8851::is_crc_valid_(uint8_t *raw, uint8_t length) {
  uint16_t computed_crc = crc16(raw, length - 2);
  uint16_t remote_crc = raw[length - 1] << 8 | raw[length - 2];
  if (computed_crc == remote_crc) ESP_LOGV(TAG, "CRC sum is valid.");
  else ESP_LOGE(TAG, "CRC sum is invalid, check your wiring.");
  return computed_crc == remote_crc;
}

void INV_8851::setup() {
  this->clear_buffer_();
  this->update();
}

void INV_8851::loop() {
  if (this->available() == 0 || millis() - this->last_read_ < 100) return;
  else if (this->available() > inv8851_state_pkt_len) {
    ESP_LOGW(TAG, "There was a message with length %d, that is too long.", this->available());
    this->clear_buffer_();
    return;
  } else this->last_read_ = millis();

  if (this->available() == inv8851_state_pkt_len) {
    auto resp = *this->read_array<inv8851_state_pkt_len>();
    ESP_LOGV(TAG, "Received %d bytes of state response.", inv8851_state_pkt_len);
    if (!this->is_crc_valid_(resp.data(), inv8851_state_pkt_len)) return;
    if (resp[9] << 24 | resp[10] << 16 | resp[11] << 8 | resp[12] == 0x0000) {
      ESP_LOGV(TAG, "This is indeed a state response.");
      this->state_ = (inv8851_state_s *) &resp;
      this->publish_state_();
    } else ESP_LOGW(TAG, "This doesn't look like a state response.");
  } else if (this->available() == inv8851_config_pkt_len) {
    auto resp = *this->read_array<inv8851_config_pkt_len>();
    ESP_LOGV(TAG, "Received %d bytes of config response.", inv8851_config_pkt_len);
    if (!this->is_crc_valid_(resp.data(), inv8851_config_pkt_len)) return;
    if (resp[9] << 24 | resp[10] << 16 | resp[11] << 8 | resp[12] == 0x0200) {
      ESP_LOGV(TAG, "This is indeed a config response.");
      this->config_ = (inv8851_config_s *) &resp;
      this->publish_config_();
    } else ESP_LOGW(TAG, "This doesn't look like a config response.");
  }
}

void INV_8851::update() {
  if (this->state_update_) this->request_state_();
  else this->request_config_();
  this->state_update_ = !this->state_update_;
}

void INV_8851::dump_config() {
  ESP_LOGCONFIG(TAG, "DTU_WBS1:");
  #ifdef USE_BINARY_SENSOR
    LOG_BINARY_SENSOR("", "Battery charging", this->battery_charging_binary_sensor_);
    LOG_BINARY_SENSOR("", "Battery connected", this->battery_connected_binary_sensor_);
    LOG_BINARY_SENSOR("", "Bus problem", this->bus_problem_binary_sensor_);
    LOG_BINARY_SENSOR("", "Float charging", this->float_charging_binary_sensor_);
    LOG_BINARY_SENSOR("", "Grid PLL problem", this->grid_pll_problem_binary_sensor_);
    LOG_BINARY_SENSOR("", "Grid power", this->grid_power_binary_sensor_);
    LOG_BINARY_SENSOR("", "Parallel lock phase problem", this->parallel_lock_phase_problem_binary_sensor_);
    LOG_BINARY_SENSOR("", "PV excess", this->pv_excess_binary_sensor_);
    LOG_BINARY_SENSOR("", "PV input problem", this->pv_input_problem_binary_sensor_);
    LOG_BINARY_SENSOR("", "System power", this->system_power_binary_sensor_);
  #endif
  #ifdef USE_SENSOR
    LOG_SENSOR("", "Battery charge current", this->battery_charge_current_sensor_);
    LOG_SENSOR("", "Battery voltage", this->battery_voltage_sensor_);
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
}

void INV_8851::request_state_() {
  this->write_array({0x88, 0x51, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x4d, 0x08});
}

void INV_8851::request_config_() {
  this->write_array({0x88, 0x51, 0x00, 0x03, 0x02, 0x00, 0x00, 0x00, 0x4c, 0xb0});
}

void INV_8851::publish_state_() {
  #ifdef USE_BINARY_SENSOR
    PUBLISH_STATE(this->battery_charging_binary_sensor_, !this->state_->charge_finish);
    PUBLISH_STATE(this->battery_connected_binary_sensor_, !this->state_->no_battery);
    PUBLISH_STATE(this->bus_problem_binary_sensor_, !this->state_->bus_ok);
    PUBLISH_STATE(this->float_charging_binary_sensor_, !!this->state_->floating_charge);
    PUBLISH_STATE(this->grid_pll_problem_binary_sensor_, !this->state_->grid_pll_ok);
    PUBLISH_STATE(this->grid_power_binary_sensor_, !this->state_->disable_utility);
    PUBLISH_STATE(this->parallel_lock_phase_problem_binary_sensor_, !this->state_->parallel_lock_phase_ok);
    PUBLISH_STATE(this->pv_excess_binary_sensor_, !!this->state_->pv_excess);
    PUBLISH_STATE(this->pv_input_problem_binary_sensor_, !this->state_->pv_input_ok);
    PUBLISH_STATE(this->system_power_binary_sensor_, !!this->state_->system_power);
  #endif
  #ifdef USE_SENSOR
    PUBLISH_STATE(this->battery_charge_current_sensor_, this->state_->batt_charge_current / 10.0f);
    PUBLISH_STATE(this->battery_voltage_sensor_, this->state_->batt_voltage / 100.0f);
    PUBLISH_STATE(this->bms_battery_current_sensor_, this->state_->bms_battery_current / 10.0f);
    PUBLISH_STATE(this->bms_battery_soc_sensor_, this->state_->bms_battery_soc);
    PUBLISH_STATE(this->bms_battery_voltage_sensor_, this->state_->bms_battery_voltage / 100.0f);
    PUBLISH_STATE(this->bms_cell_01_voltage_sensor_, this->state_->bms_01cell_voltage / 100.0f);
    PUBLISH_STATE(this->bms_cell_02_voltage_sensor_, this->state_->bms_02cell_voltage / 100.0f);
    PUBLISH_STATE(this->bms_cell_03_voltage_sensor_, this->state_->bms_03cell_voltage / 100.0f);
    PUBLISH_STATE(this->bms_cell_04_voltage_sensor_, this->state_->bms_04cell_voltage / 100.0f);
    PUBLISH_STATE(this->bms_cell_05_voltage_sensor_, this->state_->bms_05cell_voltage / 100.0f);
    PUBLISH_STATE(this->bms_cell_06_voltage_sensor_, this->state_->bms_06cell_voltage / 100.0f);
    PUBLISH_STATE(this->bms_cell_07_voltage_sensor_, this->state_->bms_07cell_voltage / 100.0f);
    PUBLISH_STATE(this->bms_cell_08_voltage_sensor_, this->state_->bms_08cell_voltage / 100.0f);
    PUBLISH_STATE(this->bms_cell_09_voltage_sensor_, this->state_->bms_09cell_voltage / 100.0f);
    PUBLISH_STATE(this->bms_cell_10_voltage_sensor_, this->state_->bms_10cell_voltage / 100.0f);
    PUBLISH_STATE(this->bms_cell_11_voltage_sensor_, this->state_->bms_11cell_voltage / 100.0f);
    PUBLISH_STATE(this->bms_cell_12_voltage_sensor_, this->state_->bms_12cell_voltage / 100.0f);
    PUBLISH_STATE(this->bms_cell_13_voltage_sensor_, this->state_->bms_13cell_voltage / 100.0f);
    PUBLISH_STATE(this->bms_cell_14_voltage_sensor_, this->state_->bms_14cell_voltage / 100.0f);
    PUBLISH_STATE(this->bms_cell_15_voltage_sensor_, this->state_->bms_15cell_voltage / 100.0f);
    PUBLISH_STATE(this->bms_cell_16_voltage_sensor_, this->state_->bms_16cell_voltage / 100.0f);
    PUBLISH_STATE(this->bts_temperature_sensor_, this->state_->bts_temperature);
    PUBLISH_STATE(this->bus_voltage_sensor_, this->state_->bus_voltage / 10.0f);
    PUBLISH_STATE(this->fan1_speed_percentage_sensor_, this->state_->fan1_speed_percent);
    PUBLISH_STATE(this->fan2_speed_percentage_sensor_, this->state_->fan2_speed_percent);
    PUBLISH_STATE(this->grid_current_sensor_, this->state_->grid_current / 100.0f);
    PUBLISH_STATE(this->grid_frequency_sensor_, this->state_->grid_freq / 100.0f);
    PUBLISH_STATE(this->grid_voltage_sensor_, this->state_->grid_voltage / 10.0f);
    PUBLISH_STATE(this->inverter_apparent_power_sensor_, this->state_->inv_va);
    PUBLISH_STATE(this->inverter_apparent_power_percentage_sensor_, this->state_->inverter_va_percent);
    PUBLISH_STATE(this->inverter_current_sensor_, this->state_->inv_current / 100.0f);
    PUBLISH_STATE(this->inverter_frequency_sensor_, this->state_->inv_freq / 100.0f);
    PUBLISH_STATE(this->inverter_power_percentage_sensor_, this->state_->inverter_watt_percent);
    PUBLISH_STATE(this->inverter_software_version_sensor_, this->state_->softvare_version);
    PUBLISH_STATE(this->inverter_voltage_sensor_, this->state_->inv_voltage / 10.0f);
    PUBLISH_STATE(this->inverter_voltage_dc_component_sensor_, this->state_->inverter_voltage_dc_component);
    PUBLISH_STATE(this->load_apparent_power_sensor_, this->state_->load_va);
    PUBLISH_STATE(this->load_current_sensor_, this->state_->load_current / 100.0f);
    PUBLISH_STATE(this->load_power_sensor_, this->state_->load_watt);
    PUBLISH_STATE(this->log_number_sensor_, this->state_->log_number);
    PUBLISH_STATE(this->low_load_current_sensor_, this->state_->low_load_current / 100.0f);
    PUBLISH_STATE(this->ntc2_temperature_sensor_, this->state_->ntc2_temperature);
    PUBLISH_STATE(this->ntc3_temperature_sensor_, this->state_->ntc3_temperature);
    PUBLISH_STATE(this->ntc4_temperature_sensor_, this->state_->ntc4_temperature);
    PUBLISH_STATE(this->parallel_current_sensor_, this->state_->parallel_current / 100.0f);
    PUBLISH_STATE(this->parallel_frequency_sensor_, this->state_->parallel_frequency / 100.0f);
    PUBLISH_STATE(this->parallel_voltage_sensor_, this->state_->parallel_voltage / 10.0f);
    PUBLISH_STATE(this->pv_current_sensor_, this->state_->pv_current / 100.0f);
    PUBLISH_STATE(this->pv_power_sensor_, this->state_->pv_power);
    PUBLISH_STATE(this->pv_voltage_sensor_, this->state_->pv_voltage / 10.0f);
  #endif
}
  
void INV_8851::publish_config_() {
  #ifdef USE_SENSOR
    PUBLISH_STATE(this->battery_type_sensor_, this->config_->battery_type);
  #endif
}

}  // namespace dtu_wbs1
}  // namespace esphome
