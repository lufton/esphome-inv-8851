#pragma once

#include "inv8851.h"
#include "esphome/core/component.h"
#include "esphome/components/uart/uart.h"

#ifdef USE_BINARY_SENSOR
#include "esphome/components/binary_sensor/binary_sensor.h"
#endif
#ifdef USE_TEXT_SENSOR
#include "esphome/components/text_sensor/text_sensor.h"
#endif
#ifdef USE_SENSOR
#include "esphome/components/sensor/sensor.h"
#endif
#ifdef USE_SELECT
#include "esphome/components/select/select.h"
#endif
#ifdef USE_NUMBER
#include "esphome/components/number/number.h"
#endif

namespace esphome {
namespace inv_8851 {

static const char *const TAG = "inv_8851";
enum BatteryType { AGM = 0, FLOODED = 1, USER = 2, LIB = 3 };
enum ChargeEnergyPriority { CSO = 0, SNU = 1, OSO = 2 };
enum Frequency { FIFTY = 0, SIXTY = 1 };
enum GridVoltageRange { UPS = 0, APL = 1 };
enum OffOn { OFF = 0, ON = 1 };
enum OutputEnergyPriority { SUB = 0, SBU = 1 };
enum Phase { A = 0, B = 1, C = 2 };

class Inv8851 : public PollingComponent, public uart::UARTDevice {
 public:
   #ifdef USE_BINARY_SENSOR
    SUB_BINARY_SENSOR(battery)
    SUB_BINARY_SENSOR(buck_topology_initialization)
    SUB_BINARY_SENSOR(bus)
    SUB_BINARY_SENSOR(bus_and_grid_voltage_match)
    SUB_BINARY_SENSOR(charging)
    SUB_BINARY_SENSOR(disable_utility)
    SUB_BINARY_SENSOR(equalization_finished)
    SUB_BINARY_SENSOR(equalization_started)
    SUB_BINARY_SENSOR(float_charging)
    SUB_BINARY_SENSOR(grid_pll)
    SUB_BINARY_SENSOR(inverter_topology_initialization)
    SUB_BINARY_SENSOR(llc_topology_initialization)
    SUB_BINARY_SENSOR(parallel_lock_phase)
    SUB_BINARY_SENSOR(pv_excess)
    SUB_BINARY_SENSOR(pv_input)
    SUB_BINARY_SENSOR(pv_topology_initialization)
    SUB_BINARY_SENSOR(system_initialization)
    SUB_BINARY_SENSOR(system_power)
  #endif

  #ifdef USE_TEXT_SENSOR
    SUB_TEXT_SENSOR(buck_topology)
    SUB_TEXT_SENSOR(inverter_topology)
    SUB_TEXT_SENSOR(llc_topology)
    SUB_TEXT_SENSOR(pv_topology)
  #endif

  #ifdef USE_SENSOR
    SUB_SENSOR(battery_charge_current)
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

  #ifdef USE_SELECT
    SUB_SELECT(auto_return)
    SUB_SELECT(backlight)
    SUB_SELECT(battery_equalization)
    SUB_SELECT(battery_type)
    SUB_SELECT(buzzer)
    SUB_SELECT(charge_energy_priority)
    SUB_SELECT(fault_record)
    SUB_SELECT(frequency)
    SUB_SELECT(grid_voltage_range)
    SUB_SELECT(on_grid)
    SUB_SELECT(output_energy_priority)
    SUB_SELECT(overload_restart)
    SUB_SELECT(overtemp_restart)
    SUB_SELECT(parallel_operation)
    SUB_SELECT(phase)
    SUB_SELECT(power_buzzer)
    SUB_SELECT(powersave_mode)
    SUB_SELECT(warning_buzer)
  #endif

  #ifdef USE_NUMBER
    SUB_NUMBER(battery_back_to_util_voltage)
    SUB_NUMBER(battery_bulk_voltage)
    SUB_NUMBER(battery_float_voltage)
    SUB_NUMBER(battery_charge_cut_off_current)
    SUB_NUMBER(battery_cut_off_voltage)
    SUB_NUMBER(battery_equalization_interval)
    SUB_NUMBER(battery_equalization_time)
    SUB_NUMBER(battery_equalization_timeout)
    SUB_NUMBER(battery_equalization_voltage)
    SUB_NUMBER(inverter_maximum_power)
    SUB_NUMBER(output_frequency)
    SUB_NUMBER(output_voltage)
    SUB_NUMBER(total_charge_current)
    SUB_NUMBER(util_charge_current)
  #endif

  void setup() override;

  void loop() override;

  void update() override;

  void dump_config() override;

  void set_select_value(const std::string type, size_t index);
  
  void set_number_value(const std::string type, float value);

 protected:
  #define PUBLISH_STATE(entity, state) \
  if (entity != nullptr) entity->publish_state(state);

  void clear_buffer_();

  bool is_crc_valid_(uint8_t *raw, uint8_t length);

  void request_state_();
  
  void request_config_();

  void publish_state_(const uint8_t *resp);
  
  void publish_config_(const uint8_t *resp);

  inv8851_config_s config_;

  bool state_update_ = true;

  uint32_t last_read_{0};
};

}  // namespace inv_8851
}  // namespace esphome
