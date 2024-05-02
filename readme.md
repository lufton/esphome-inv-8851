# 8851 inverter protocol integration ESPHome component

## Denial of Responsibility
**Disclaimer:** This project is provided "as-is" without any warranty or support of any kind. By using this software, you agree that:
1. **No Warranty:** The project is provided without any warranty, expressed or implied. The entire risk of using the project is assumed by you, the user.
2. **No Support:** The author(s) of this project are under no obligation to provide support, updates, or maintenance. Issues and pull requests may be addressed at the sole discretion of the project contributors.
3. **Use at Your Own Risk:** The project may involve experimental features or third-party dependencies, and the user is responsible for ensuring its compatibility with their system. The author(s) are not responsible for any damage, data loss, or other consequences that may arise from the use of this project.
4. **No Guarantee of Compatibility:** The project may not be compatible with all environments, hardware, or software configurations. It is the user's responsibility to verify compatibility before use.
5. **Security Considerations:** The project may not have undergone a comprehensive security review. Users are advised to exercise caution and not use this project in critical or security-sensitive applications.
6. **Third-Party Dependencies:** This project may rely on third-party libraries, frameworks, or tools. The user is responsible for complying with the licenses and terms associated with these dependencies.

By using this project, you acknowledge and agree to these terms. If you do not agree with these terms, do not use or contribute to this project.

## Compatible inverters
* PowMr POW-HVM6.5K-48V (tested by [@lufton](https://github.com/lufton))
* PowMr POW-HVM4.5K-24V
* Simular inverters that utilize the same DTU (WBS1-V001)

## Capabilities
This component will let you monitor (sensors, binary sensors and text sensors) and control (numbers and selects) compatible inverter without need of installing Chinese software and depend on any third-party services.

## Why to use this component
* No need to use original software. Chinese mobile app doen't work stable, is slow and not reliable.
* No need to send you data to custom server. All data sent from DTU with original firmware are stored with open access MQTT server. That is potential security breach, that can lead to data leak and even your inverter configuration changes witch can damage you hardware.
* Some extra configuration parameters that are absent in the app, like `output_frequency`, `warning_buzzer`, `inverter_maximum_power`.
* Possiblity to set some configuration parameters more accurately, like `battery_charge_cut_off_current`, `output_voltage`, `total_charge_current`, `util_charge_current`. You can set tham as any number from the range in comparison to inverter's menu where you can only select values from predefined options.
* Simple integration with 3rd party services like Home Assistant.
* Local automations.

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
24v and 48v files are different only in configuration of some entities witch has no sense for 24v version (bms_cell_09_voltage — bms_cell_16_voltage) and minimum/maximum values for some number entities (as they depend on voltage). So in theory flashing "wrong" configuration shouldn't make any harm to ESP or inverter.

## ...-local.yaml
This project build to be as simple is possible, so in most cases you woudn't need those files. Regular `.yaml` file will download latest stable version from this repository and use it upon build process. But if you want to customize configuration or you plan to change files in `packages` or `components` directories, than you probably want to use those.

## inverter_maximum_power
First of all there are two places you can find `inverter_maximum_power` parameter:
* Under `substitutions` section of example files
* Under `number` configuration section

The reason for that parameter present in `substitutions` section is to limit `max_value` parameter for `inverter_maximum_power` number entity. This is needed to prevent user from setting to high value for that entity.

Number entity `inverter_maximum_power` sets software limitaion for inverter's maximum power (total power inverter consumes from grid including battery charging power). Setting value that is higher than factory-default (4500W for 4.5kW inverter version and 6500W for 6.5kW inverter version) for that number entity can lead to potential damage. From other hand you can set software limitaion lower than factory-default to protect your inverter from working in conditions close to maximum possible and prolong it's life. In case of exceeding 110% of `inverter_maximum_power` value, inverter will turn off with error code of `07`.

**Please make sure you understand what you're doing before adjusting this parameter.**

## Flashing DTU WBS1-V001
You can flash WBS1-V001 using this component (use corresponding `dtu-wbs1-v001...-example.yaml` file). This way you can use original DTU with much more comfortable and easy way. This configuration also supports onboard LED indication.
In order to flash original DTU follow next steps:
1. Disassemble DTU (it has 4 screws in the corners under the foamy sticker on the bottom)
2. Solder pin headers of jump wires to +5V, GND1, BOOT1, RXD1, TXD1 pads (they are labeled)
3. Connect USB to TTL module VCC → +5V, RX → TXD1, TX → RXD1 and GND → GND1 with BOOT1 pin connected to GND1
4. Dump and store original firmware using [esptool](https://github.com/espressif/esptool/releases/tag/v4.6.2) (replace `COM1` with valid port number):<br>
`esptool.exe -p COM1 -b 460800 read_flash 0x0 0x400000 DTU.bin`
5. Flash ESPHome with corresponding configuration
6. Disconnect USB to TTL module, assemble DTU and connect to inverter
7. In order to restore original firmware connect DTU and USB to TTL module with BOOT1 pin connected to GND1 and run command (replace `COM1` and `DTU.bin` with valid port number and firmware dump file backed up in step 4 path):<br>
`esptool.exe -p COM1 -b 460800 write_flash 0x0 DTU.bin`

## Flashing ESP32 or ESP8266
You can flash ESP32 or ESP8266 using this component the same way you flash your other ESPHome projects. Read [official guides](https://esphome.io/guides/) if you have any questions.

## Available entities
### Sensors
| ID                                 	| Description                                                                                      	| Unit 	| Res. 	|
|------------------------------------	|--------------------------------------------------------------------------------------------------	|------	|------	|
| battery_charge_current             	| Maximum current to charge battery. It will start decreasing when battery enter absorption stage. 	| A    	| 0.1  	|
| battery_voltage                    	|                                                                                                  	| V    	| 0.01 	|
| bms_battery_current                	|                                                                                                  	| A    	| 0.1  	|
| bms_battery_soc                    	|                                                                                                  	| %    	| 1    	|
| bms_battery_voltage                	|                                                                                                  	| V    	| 0.01 	|
| bms_cell_01_voltage                	|                                                                                                  	| V    	| 0.01 	|
| bms_cell_02_voltage                	|                                                                                                  	| V    	| 0.01 	|
| bms_cell_03_voltage                	|                                                                                                  	| V    	| 0.01 	|
| bms_cell_04_voltage                	|                                                                                                  	| V    	| 0.01 	|
| bms_cell_05_voltage                	|                                                                                                  	| V    	| 0.01 	|
| bms_cell_06_voltage                	|                                                                                                  	| V    	| 0.01 	|
| bms_cell_07_voltage                	|                                                                                                  	| V    	| 0.01 	|
| bms_cell_08_voltage                	|                                                                                                  	| V    	| 0.01 	|
| bms_cell_09_voltage                	|                                                                                                  	| V    	| 0.01 	|
| bms_cell_10_voltage                	|                                                                                                  	| V    	| 0.01 	|
| bms_cell_11_voltage                	|                                                                                                  	| V    	| 0.01 	|
| bms_cell_12_voltage                	|                                                                                                  	| V    	| 0.01 	|
| bms_cell_13_voltage                	|                                                                                                  	| V    	| 0.01 	|
| bms_cell_14_voltage                	|                                                                                                  	| V    	| 0.01 	|
| bms_cell_15_voltage                	|                                                                                                  	| V    	| 0.01 	|
| bms_cell_16_voltage                	|                                                                                                  	| V    	| 0.01 	|
| bts_temperature                    	|                                                                                                  	| °C   	| 1    	|
| bus_voltage                        	|                                                                                                  	| V    	| 0.1  	|
| fan1_speed_percentage              	|                                                                                                  	| %    	| 1    	|
| fan2_speed_percentage              	|                                                                                                  	| %    	| 1    	|
| grid_current                       	|                                                                                                  	| A    	| 0.01 	|
| grid_frequency                     	|                                                                                                  	| Hz   	| 0.01 	|
| grid_voltage                       	|                                                                                                  	| V    	| 0.1  	|
| inverter_apparent_power            	|                                                                                                  	| VA   	| 1    	|
| inverter_apparent_power_percentage 	|                                                                                                  	| %    	| 1    	|
| inverter_current                   	|                                                                                                  	| A    	| 0.01 	|
| inverter_frequency                 	|                                                                                                  	| Hz   	| 0.01 	|
| inverter_power_percentage          	|                                                                                                  	| %    	| 1    	|
| inverter_software_version          	|                                                                                                  	| N/A  	| 1    	|
| inverter_voltage                   	|                                                                                                  	| V    	| 0.1  	|
| inverter_voltage_dc_component      	|                                                                                                  	| ?    	| ?    	|
| load_apparent_power                	|                                                                                                  	| VA   	| 1    	|
| load_current                       	|                                                                                                  	| A    	| 0.01 	|
| load_power                         	|                                                                                                  	| W    	| 1    	|
| log_number                         	|                                                                                                  	| N/A  	| 1    	|
| low_load_current                   	|                                                                                                  	| A    	| 0.01 	|
| ntc2_temperature                   	|                                                                                                  	| °C   	| 1    	|
| ntc3_temperature                   	|                                                                                                  	| °C   	| 1    	|
| ntc4_temperature                   	|                                                                                                  	| °C   	| 1    	|
| parallel_current                   	|                                                                                                  	| A    	| 0.01 	|
| parallel_frequency                 	|                                                                                                  	| Hz   	| 0.01 	|
| parallel_voltage                   	|                                                                                                  	| V    	| 0.1  	|
| pv_current                         	|                                                                                                  	| A    	| 0.01 	|
| pv_power                           	|                                                                                                  	| W    	| 1    	|
| pv_voltage                         	|                                                                                                  	| V    	| 0.1  	|

### Binary sensors
| ID                               	| Description                                                       	|
|----------------------------------	|-------------------------------------------------------------------	|
| battery                          	| Indicates if battery is connected                                 	|
| buck_topology_initialization     	| Indicates if buck topology is initialized                         	|
| bus                              	| ?                                                                 	|
| bus_and_grid_voltage_match       	| ?                                                                 	|
| charging                         	| Indicates if battery is still charging                            	|
| disable_utility                  	| ? Indicates if inverter works in bypass mode                      	|
| equalization_finished            	| Indicates if battery equalization program is finished             	|
| equalization_started             	| Indicates if battery equalization program is started              	|
| float_charging                   	| Indeicates if battery is in float charging mode                   	|
| grid_pll                         	| Indicates if inverter phase is locked                             	|
| inverter_topology_initialization 	| Indicates if inverter's topology is initialized                   	|
| llc_topology_initialization      	| Indicates if LLC topology is initialized                          	|
| parallel_lock_phase              	| Indicates if phases of inverters connected in parallel are locked 	|
| pv_excess                        	| Indicates if there is PV power excess avaiable                    	|
| pv_input                         	| Indicates if PV is connected                                      	|
| pv_topology_initialization       	| Indicates if PV topology is initialized                           	|
| system_initialization            	| Indicates if all inverter's systems are initialized               	|
| system_power                     	| Indicates if inverter is turned on using button on left side      	|

### Text sensors
| ID                	| Description             	|
|-------------------	|-------------------------	|
| buck_topology     	| Buck topology state     	|
| inverter_topology 	| Inverter topology state 	|
| llc_topology      	| LLC topology state      	|
| pv_topology       	| PV topology state       	|

### Selects
| ID                     	| Description                                                                                                	| Options                                                                                                             	| Menu 	|
|------------------------	|------------------------------------------------------------------------------------------------------------	|---------------------------------------------------------------------------------------------------------------------	|------	|
| auto_return            	| Auto return to home screen after 1 minute of inactivity                                                    	| OFF<br>     ON                                                                                                      	| 19   	|
| backlight              	| Backlight always ON mode                                                                                   	| OFF<br>     ON                                                                                                      	| 20   	|
| battery_equalization   	| Battery equalization program activation                                                                    	| OFF<br>     ON                                                                                                      	| 33   	|
| battery_type           	| Battery type selection. If User-defined type is selected, then additional   voltage parameters can be set. 	| AGM<br>     Flooded<br>     User-defined<br>     Library                                                            	| 05   	|
| buzzer                 	| Buzzer that beep every time you press any screen button                                                    	| OFF<br>     ON                                                                                                      	| 18   	|
| charge_energy_priority 	| Charger source priority                                                                                    	| PV & Grid — PV and Grid at   the same time<br>     PV > Grid — Priority for PV, then Grid<br>     PV only — Only PV 	| 16   	|
| fault_record           	| Record fault code                                                                                          	| OFF<br>     ON                                                                                                      	| 25   	|
| frequency              	| Default frequency output mode, can be adjusted with `output_frequency`   number                            	| 50Hz<br>     60Hz                                                                                                   	| 09   	|
| grid_voltage_range     	| Grid voltage range in witch inverter will work, otherwise will switch to   battery                         	| APL — 90-265V<br>     UPS — 170-265V                                                                                	| 03   	|
| on_grid                	| Send produced enery to Grid                                                                                	| OFF<br>     ON                                                                                                      	| 10   	|
| output_energy_priority 	| Which source to use first: Grid or PV                                                                      	| PV > Grid > Battery — Grid   over battery<br>     PV > Battery > Grid — Battery over grid                           	| 01   	|
| overload_restart       	| Automatic restart after overload protection                                                                	| OFF<br>     ON                                                                                                      	| 06   	|
| overtemp_restart       	| Automatic restart after over temperature protection                                                        	| OFF<br>     ON                                                                                                      	| 07   	|
| parallel_operation     	| Parallel operation mode                                                                                    	| OFF<br>     ON                                                                                                      	| 14   	|
| phase                  	| Witch phase inverter is connected to                                                                       	| A<br>     B<br>     C                                                                                               	| 15   	|
| power_buzzer           	| Buzzer that beep every time Grid becomes unavailable                                                       	| OFF<br>     ON                                                                                                      	| 22   	|
| powersave_mode         	| Power save mode, will pulse output voltage to determine and start if load   is connected                   	| OFF<br>     ON                                                                                                      	| 04   	|
| warning_buzzer         	| Buzzer that beep every time inverter raise warning                                                         	| OFF<br>     ON                                                                                                      	| N/A  	|

### Numbers
| ID                             	| Description                                                                                                          	| Unit 	| Res. 	| Range                   	| Menu 	|
|--------------------------------	|----------------------------------------------------------------------------------------------------------------------	|------	|------	|-------------------------	|------	|
| battery_back_to_util_voltage   	| Voltage point at witch inverter will switch to Grid in PV > Battery   > Grid mode                                    	| V   	| 0.1  	| 20…29.2<br>     40…58.4 	| 13   	|
| battery_bulk_voltage           	| Fully charged battery voltage                                                                                        	| V   	| 0.1  	| 24…29.2<br>     48…58.4 	| 26   	|
| battery_float_voltage          	| Floating charging mode voltage                                                                                       	| V   	| 0.1  	| 24…29.2<br>     48…58.4 	| 27   	|
| battery_charge_cut_off_current 	| Charge current at witch inverted assumes that battery is fully charged   and switches into floating charging mode    	| A   	| 0.1  	| 2…20                    	| 12   	|
| battery_cut_off_voltage        	| Minimum battery voltage, inverter will disconnect battery at this point                                              	| V   	| 0.1  	| 20…24<br>     40…48     	| 29   	|
| battery_equalization_interval  	| How often battery equalization program will be triggered                                                             	| Day 	| 1    	| 0…90                    	| 37   	|
| battery_equalization_time      	| How long battery equalization program will run                                                                       	| Min. 	| 1    	| 5…900                   	| 35   	|
| battery_equalization_timeout   	| How long battery equalization program can run before terminating                                                     	| Min. 	| 1    	| 5…900                   	| 36   	|
| battery_equalization_voltage   	| Battery equalization voltage                                                                                         	| V   	| 0.1  	| 25…29.5<br>     50…59   	| 34   	|
| inverter_maximum_power         	| Inverter maximum output power                                                                                        	| W   	| 1    	| 0…4500<br>     0…6500   	| N/A  	|
| output_frequency               	| Output frequency, that can be in range 50-55Hz for 50Hz mode and 60-55Hz   for 60Hz mode                             	| Hz  	| 1    	| 50…55<br>     60…55     	| N/A  	|
| output_voltage                 	| Output voltage, resets to closest lower number that is multiple of 5   (234V → 230V, 229V → 225V) after exiting menu 	| V   	| 1    	| 100…240                 	| 08   	|
| total_charge_current           	| Maximum battery charge current equals PV current + Grid current                                                      	| A   	| 0.1  	| 0…130<br>     0…150     	| 02   	|
| util_charge_current            	| Maximum current used from Grid to charge battery                                                                     	| A   	| 0.1  	| 0…110<br>     0…140     	| 11   	|

## WBS1-V001 simplified schematics
![WBS1-V001 simplified schematics](resources/WBS1-V001.svg?raw=true "WBS1-V001")
<details>
  <summary>WBS1-V001 module pictures</summary>
  <img alt="WBS1-V001" src="resources/WBS1-V001.jpg?raw=true" />
  <img alt="WBS1-V001 screws" src="resources/WBS1-V001_screws.jpg?raw=true" />
  <img alt="WBS1-V001 parts" src="resources/WBS1-V001_parts.jpg?raw=true" />
  <img alt="WBS1-V001 PCB (top)" src="resources/WBS1-V001_PCB_top.jpg?raw=true" />
  <img alt="WBS1-V001 PCB (bottom)" src="resources/WBS1-V001_PCB_bottom.jpg?raw=true" />
</details>

## Original firmware dump
[Here](https://github.com/lufton/esphome-inv-8851/blob/main/DTU.bin) you can find original firmware dump file. Restore procedure described in the last step of [Flashing DTU WBS1-V001](#flashing-dtu-wbs1-v001) section.

## Thanks
* [@leodesigner](https://github.com/leodesigner) for his work on [powmr4500_comm](https://github.com/leodesigner/powmr4500_comm)
* [@aquaforum](https://github.com/aquaforum) for his work on [c++ header file](https://github.com/leodesigner/powmr4500_comm/blob/main/include/inv8851.h) and protocol reverse engineering
