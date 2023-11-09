#pragma once

#include "inv8851.h"
#include "esphome/core/component.h"
#include "esphome/components/uart/uart.h"
#ifdef USE_SENSOR
#include "esphome/components/sensor/sensor.h"
#endif
#ifdef USE_BINARY_SENSOR
#include "esphome/components/binary_sensor/binary_sensor.h"
#endif

namespace esphome {
namespace inv_8851 {

class INV_8851 : public PollingComponent, public uart::UARTDevice {
 public:
  #ifdef USE_SENSOR
    SUB_SENSOR(battery_charge_current)
    SUB_SENSOR(battery_type)
    SUB_SENSOR(battery_voltage)
    SUB_SENSOR(bms_battery_current)
    SUB_SENSOR(bms_battery_soc)
    SUB_SENSOR(bms_battery_voltage)
    SUB_SENSOR(bms_cell_01_voltage)
    SUB_SENSOR(bms_cell_02_voltage)
    SUB_SENSOR(bms_cell_03_voltage)
    SUB_SENSOR(bms_cell_04_voltage)
    SUB_SENSOR(bms_cell_05_voltage)
    SUB_SENSOR(bms_cell_06_voltage)
    SUB_SENSOR(bms_cell_07_voltage)
    SUB_SENSOR(bms_cell_08_voltage)
    SUB_SENSOR(bms_cell_09_voltage)
    SUB_SENSOR(bms_cell_10_voltage)
    SUB_SENSOR(bms_cell_11_voltage)
    SUB_SENSOR(bms_cell_12_voltage)
    SUB_SENSOR(bms_cell_13_voltage)
    SUB_SENSOR(bms_cell_14_voltage)
    SUB_SENSOR(bms_cell_15_voltage)
    SUB_SENSOR(bms_cell_16_voltage)
    SUB_SENSOR(bts_temperature)
    SUB_SENSOR(bus_voltage)
    SUB_SENSOR(fan1_speed_percentage)
    SUB_SENSOR(fan2_speed_percentage)
    SUB_SENSOR(grid_current)
    SUB_SENSOR(grid_frequency)
    SUB_SENSOR(grid_voltage)
    SUB_SENSOR(inverter_apparent_power)
    SUB_SENSOR(inverter_apparent_power_percentage)
    SUB_SENSOR(inverter_current)
    SUB_SENSOR(inverter_frequency)
    SUB_SENSOR(inverter_power_percentage)
    SUB_SENSOR(inverter_software_version)
    SUB_SENSOR(inverter_voltage)
    SUB_SENSOR(inverter_voltage_dc_component)
    SUB_SENSOR(load_apparent_power)
    SUB_SENSOR(load_current)
    SUB_SENSOR(load_power)
    SUB_SENSOR(log_number)
    SUB_SENSOR(low_load_current)
    SUB_SENSOR(ntc2_temperature)
    SUB_SENSOR(ntc3_temperature)
    SUB_SENSOR(ntc4_temperature)
    SUB_SENSOR(parallel_current)
    SUB_SENSOR(parallel_frequency)
    SUB_SENSOR(parallel_voltage)
    SUB_SENSOR(pv_current)
    SUB_SENSOR(pv_power)
    SUB_SENSOR(pv_voltage)
  #endif
  #ifdef USE_BINARY_SENSOR
    SUB_BINARY_SENSOR(battery_charging)
    SUB_BINARY_SENSOR(battery_connected)
    SUB_BINARY_SENSOR(bus_problem)
    SUB_BINARY_SENSOR(float_charging)
    SUB_BINARY_SENSOR(grid_pll_problem)
    SUB_BINARY_SENSOR(grid_power)
    SUB_BINARY_SENSOR(parallel_lock_phase_problem)
    SUB_BINARY_SENSOR(pv_excess)
    SUB_BINARY_SENSOR(pv_input_problem)
    SUB_BINARY_SENSOR(system_power)
  #endif

  void setup() override;

  void loop() override;

  void update() override;

  void dump_config() override;

 protected:
  #define PUBLISH_STATE(entity, state) \
  if (entity != nullptr) entity->publish_state(state);

  void clear_buffer_();

  bool is_crc_valid_(uint8_t *raw, uint8_t length);

  void request_state_();
  
  void request_config_();

  void publish_state_();
  
  void publish_config_();

  inv8851_state_s *state_;

  inv8851_config_s *config_;

  bool state_update_ = true;

  uint32_t last_read_{0};
};

}  // namespace dtu_wbs1
}  // namespace esphome
