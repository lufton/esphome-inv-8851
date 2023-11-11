# 8851 inverter protocol integration ESPHome component

## Compatible inverters
* PowMr POW-HVM6.5K-48V (tested by [@lufton](https://github.com/lufton))
* PowMr POW-HVM4.5K-24V
* Simular inverters that utilizes the same DTU (WBS1-V001)

## Capabilities
This component will let you monitor (sensors and binary sensors) and control (numbers and selects) compatible inverter without need of installing Chinese software and depend on any third-party services.

### Minimal configuration
```yaml
uart:
  - baud_rate: 9600

inv_8851:

sensor:
  - platform: inv_8851
    battery_voltage:
      name: "Battery voltage"
    ...
```
Full list of available entities [is here](#available-entities).

The only configuration you may need to set is `update_interval` (witch tells how often to request state and configuration from inverter, default value is 5s and should suite most situations) of `inv_8851` component.
```yaml
inv_8851:
  update_interval: 30s
```

## 24v vs 48v version
24v and 48v files are different only in comfiguration of some entities witch has no sense for 24v version (bms_cell_09_voltage — bms_cell_16_voltage) and minimum/maximum values for some number entities (as they depend on voltage). So in theory flashing "wrong" configuration shouldn't make any harm to ESP or inverter.

## ...-local.yaml
This project build to be as simple is possible, so in most cases you woudn't need those files. Regular `.yaml` file will download latest stable version from this repository and use it upon build process. But if you want to customize configuration or you plan to change files in `packages` or `components` directories, than you probably want to use those.

## Flashing DTU WBS1-V001
You can flash WBS1-V001 using this component (use corresponding `...-example.yaml` file). This way you can use original DTU with much more comfortable and easy way. This configuration also supports onboard LED indication.
In order to flash original DTU follow next steps:
1. Disassemble DTU (it has 4 screws in the corners under the foamy sticker on the bottom)
2. Solder pin headers of jump wires to +5V, GND1, BOOT1, RXD1, TXD1 pads (they are labeled)
3. Connect USB to TTL module VCC → +5V, RX → TXD1, TX → RXD1 and GND → GND1 with BOOT1 pin connected to GND1
4. Dump and store original firmware using [esptool](https://github.com/espressif/esptool/releases/tag/v4.6.2) (replace `COM1` with valid port number):<br>
`esptool.exe -p COM1 -b 460800 read_flash 0x0 0x400000 DTU.bin`
5. Flash ESPHome with corresponding configuration
6. Disconnect USB to TTL module, assemble DTU and connect to inverter
7. In order to restore original firmware connect DTU and USB to TTL module with BOOT pin connected to GND and run command (replace `COM1` and `DTU.bin` with valid port number and firmware dump file backed up in step 4 path):<br>
`esptool.exe -p COM1 -b 460800 write_flash 0x0 DTU.bin`

## Flashing ESP32 or ESP8266
You can flash ESP32 or ESP8266 using this component the same way you flash your other ESPHome project. Read [official guides](https://esphome.io/guides/) if you have any questions.

## Available entities
### Sensors
| ID                                 	| Description 	| Unit 	| Resolution 	|
|------------------------------------	|-------------	|------	|----------	|
| battery_charge_current             	|             	| A    	| 0.1      	|
| battery_voltage                    	|             	| V    	| 0.01     	|
| bms_battery_current                	|             	| A    	| 0.1      	|
| bms_battery_soc                    	|             	| %    	| 1        	|
| bms_battery_voltage                	|             	| V    	| 0.01     	|
| bms_cell_01_voltage                	|             	| V    	| 0.01     	|
| bms_cell_02_voltage                	|             	| V    	| 0.01     	|
| bms_cell_03_voltage                	|             	| V    	| 0.01     	|
| bms_cell_04_voltage                	|             	| V    	| 0.01     	|
| bms_cell_05_voltage                	|             	| V    	| 0.01     	|
| bms_cell_06_voltage                	|             	| V    	| 0.01     	|
| bms_cell_07_voltage                	|             	| V    	| 0.01     	|
| bms_cell_08_voltage                	|             	| V    	| 0.01     	|
| bms_cell_09_voltage                	|             	| V    	| 0.01     	|
| bms_cell_10_voltage                	|             	| V    	| 0.01     	|
| bms_cell_11_voltage                	|             	| V    	| 0.01     	|
| bms_cell_12_voltage                	|             	| V    	| 0.01     	|
| bms_cell_13_voltage                	|             	| V    	| 0.01     	|
| bms_cell_14_voltage                	|             	| V    	| 0.01     	|
| bms_cell_15_voltage                	|             	| V    	| 0.01     	|
| bms_cell_16_voltage                	|             	| V    	| 0.01     	|
| bts_temperature                    	|             	| °C   	| 1        	|
| bus_voltage                        	|             	| V    	| 0.1      	|
| fan1_speed_percentage              	|             	| %    	| 1        	|
| fan2_speed_percentage              	|             	| %    	| 1        	|
| grid_current                       	|             	| A    	| 0.01     	|
| grid_frequency                     	|             	| Hz   	| 0.01     	|
| grid_voltage                       	|             	| V    	| 0.1      	|
| inverter_apparent_power            	|             	| VA   	| 1        	|
| inverter_apparent_power_percentage 	|             	| %    	| 1        	|
| inverter_current                   	|             	| A    	| 0.01     	|
| inverter_frequency                 	|             	| Hz   	| 0.01     	|
| inverter_power_percentage          	|             	| %    	| 1        	|
| inverter_software_version          	|             	| N/A  	| 1        	|
| inverter_voltage                   	|             	| V    	| 0.1      	|
| inverter_voltage_dc_component      	|             	| ?    	| ?        	|
| load_apparent_power                	|             	| VA   	| 1        	|
| load_current                       	|             	| A    	| 0.01     	|
| load_power                         	|             	| W    	| 1        	|
| log_number                         	|             	| N/A  	| 1        	|
| low_load_current                   	|             	| A    	| 0.01     	|
| ntc2_temperature                   	|             	| °C   	| 1        	|
| ntc3_temperature                   	|             	| °C   	| 1        	|
| ntc4_temperature                   	|             	| °C   	| 1        	|
| parallel_current                   	|             	| A    	| 0.01     	|
| parallel_frequency                 	|             	| Hz   	| 0.01     	|
| parallel_voltage                   	|             	| V    	| 0.1      	|
| pv_current                         	|             	| A    	| 0.01     	|
| pv_power                           	|             	| W    	| 1        	|
| pv_voltage                         	|             	| V    	| 0.1      	|

### Binary sensors
| ID                          	| Description 	|
|-----------------------------	|-------------	|
| battery_charging            	|             	|
| battery_connected           	|             	|
| bus_problem                 	|             	|
| float_charging              	|             	|
| grid_pll_problem            	|             	|
| grid_power                  	|             	|
| parallel_lock_phase_problem 	|             	|
| pv_excess                   	|             	|
| pv_input_problem            	|             	|
| system_power                	|             	|

### Selects
| ID                     	| Description 	| Options                                                  	|
|------------------------	|-------------	|----------------------------------------------------------	|
| auto_return            	|             	| OFF<br>     ON                                           	|
| backlight              	|             	| OFF<br>     ON                                           	|
| battery_equalization   	|             	| OFF<br>     ON                                           	|
| battery_type           	|             	| AGM<br>     Flooded<br>     User-defined<br>     Library 	|
| buzzer                 	|             	| OFF<br>     ON                                           	|
| charge_energy_priority 	|             	| PV & Grid<br>     PV > Grid<br>     PV only              	|
| fault_record           	|             	| OFF<br>     ON                                           	|
| frequency              	|             	| 50Hz<br>     60Hz                                        	|
| grid_voltage_range     	|             	| APL<br>     UPS                                          	|
| on_grid                	|             	| OFF<br>     ON                                           	|
| output_energy_priority 	|             	| PV > Grid > Battery<br>     PV > Battery > Grid          	|
| overload_restart       	|             	| OFF<br>     ON                                           	|
| overtemp_restart       	|             	| OFF<br>     ON                                           	|
| parallel_operation     	|             	| OFF<br>     ON                                           	|
| phase                  	|             	| A<br>     B<br>     C                                    	|
| power_buzzer           	|             	| OFF<br>     ON                                           	|
| powersave_mode         	|             	| OFF<br>     ON                                           	|

### Numbers
| ID                             	| Description 	| Unit   	| Resolution 	|
|--------------------------------	|-------------	|--------	|------------	|
| battery_back_to_util_voltage   	|             	| V      	| 0.1        	|
| battery_bulk_voltage           	|             	| V      	| 0.1        	|
| battery_float_voltage          	|             	| V      	| 0.1        	|
| battery_charge_cut_off_current 	|             	| A      	| 0.1        	|
| battery_cut_off_voltage        	|             	| V      	| 0.1        	|
| battery_equalization_interval  	|             	| Day    	| 1          	|
| battery_equalization_time      	|             	| Minute 	| 1          	|
| battery_equalization_timeout   	|             	| Minute 	| 1          	|
| battery_equalization_voltage   	|             	| V      	| 0.1        	|
| output_frequency               	|             	| Hz     	| 1          	|
| output_voltage                 	|             	| V      	| 1          	|
| total_charge_current           	|             	| A      	| 0.1        	|
| util_charge_current            	|             	| A      	| 0.1        	|

## Original firmware dump
[Here](https://github.com/lufton/esphome-inv-8851/blob/main/DTU.bin) you can find original firmware dump file. Restore procedure described in the last step of [Flashing DTU WBS1-V001](#flashing-dtu-wbs1-v001) section.

## Thanks
* [@leodesigner](https://github.com/leodesigner) for his work on [powmr4500_comm](https://github.com/leodesigner/powmr4500_comm)
* [@aquaforum](https://github.com/aquaforum) for his work on [c++ header file](https://github.com/leodesigner/powmr4500_comm/blob/main/include/inv8851.h) and protocol reverse engineering
