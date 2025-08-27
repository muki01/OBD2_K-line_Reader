//-------------------------------------------------------------------------------------//
// PIDs (https://en.wikipedia.org/wiki/OBD-II_PIDs)
//-------------------------------------------------------------------------------------//

const byte init_OBD = 0x81;  // Init ISO14230

// Modes
const uint8_t read_LiveData = 0x01;              // Show current live data
const uint8_t read_FreezeFrame = 0x02;           // Show freeze frame data
const uint8_t read_storedDTCs = 0x03;            // Show stored Diagnostic Trouble Codes (DTCs)
const uint8_t clear_DTCs = 0x04;                 // Clear Diagnostic Trouble Codes and stored values
const uint8_t test_OxygenSensors = 0x05;         // Test results, oxygen sensor monitoring (non-CAN only)
const uint8_t test_OtherComponents = 0x06;       // Test results, other component/system monitoring (for CAN)
const uint8_t read_pendingDTCs = 0x07;           // Show pending Diagnostic Trouble Codes
const uint8_t control_OnBoardComponents = 0x08;  // Control operation of on-board component/system
const uint8_t read_VehicleInfo = 0x09;           // Request vehicle information
const uint8_t read_PermanentDTCs = 0x0A;         // Show permanent Diagnostic Trouble Codes


// PIDs for Vehicle Info
const byte supported_VehicleInfo = 0x00;  // Read Supported Vehicle Info
const byte read_VIN_Count = 0x01;         // Read VIN Count
const byte read_VIN = 0x02;               // Read VIN
const byte read_ID_Length = 0x03;         // Read Calibration ID Length
const byte read_ID = 0x04;                // Read Calibration ID
const byte read_ID_Num_Length = 0x05;     // Read Calibration ID Number Length
const byte read_ID_Num = 0x06;            // Read Calibration ID Number

// PIDs for Live Data and Freeze Frame
const byte SUPPORTED_PIDS_1_20 = 0x00;               // bit encoded
const byte MONITOR_STATUS_SINCE_DTC_CLEARED = 0x01;  // bit encoded          suported
const byte FREEZE_DTC = 0x02;                        //                      suported
const byte FUEL_SYSTEM_STATUS = 0x03;                // bit encoded          suported
const byte ENGINE_LOAD = 0x04;                       // %
const byte ENGINE_COOLANT_TEMP = 0x05;               // °C
const byte SHORT_TERM_FUEL_TRIM_BANK_1 = 0x06;       // %   suported
const byte LONG_TERM_FUEL_TRIM_BANK_1 = 0x07;        // %   suported
const byte SHORT_TERM_FUEL_TRIM_BANK_2 = 0x08;       // %
const byte LONG_TERM_FUEL_TRIM_BANK_2 = 0x09;        // %
const byte FUEL_PRESSURE = 0x0A;                     // kPa
const byte INTAKE_MANIFOLD_ABS_PRESSURE = 0x0B;      // kPa
const byte ENGINE_RPM = 0x0C;                        // rpm
const byte VEHICLE_SPEED = 0x0D;                     // km/h
const byte TIMING_ADVANCE = 0x0E;                    // ° before TDC
const byte INTAKE_AIR_TEMP = 0x0F;                   // °C
const byte MAF_FLOW_RATE = 0x10;                     // g/s
const byte THROTTLE_POSITION = 0x11;                 // %
const byte COMMANDED_SECONDARY_AIR_STATUS = 0x12;    // bit encoded
const byte OXYGEN_SENSORS_PRESENT_2_BANKS = 0x13;    // bit encoded    suported
const byte OXYGEN_SENSOR_1_A = 0x14;                 // V %            suported
const byte OXYGEN_SENSOR_2_A = 0x15;                 // V %            suported
const byte OXYGEN_SENSOR_3_A = 0x16;                 // V %
const byte OXYGEN_SENSOR_4_A = 0x17;                 // V %
const byte OXYGEN_SENSOR_5_A = 0x18;                 // V %
const byte OXYGEN_SENSOR_6_A = 0x19;                 // V %
const byte OXYGEN_SENSOR_7_A = 0x1A;                 // V %
const byte OXYGEN_SENSOR_8_A = 0x1B;                 // V %
const byte OBD_STANDARDS = 0x1C;                     // bit encoded  suported
const byte OXYGEN_SENSORS_PRESENT_4_BANKS = 0x1D;    // bit encoded
const byte AUX_INPUT_STATUS = 0x1E;                  // bit encoded
const byte RUN_TIME_SINCE_ENGINE_START = 0x1F;       // sec

const byte SUPPORTED_PIDS_21_40 = 0x20;                   // bit encoded
const byte DISTANCE_TRAVELED_WITH_MIL_ON = 0x21;          // km           suported
const byte FUEL_RAIL_PRESSURE = 0x22;                     // kPa
const byte FUEL_RAIL_GAUGE_PRESSURE = 0x23;               // kPa
const byte OXYGEN_SENSOR_1_B = 0x24;                      // ratio V
const byte OXYGEN_SENSOR_2_B = 0x25;                      // ratio V
const byte OXYGEN_SENSOR_3_B = 0x26;                      // ratio V
const byte OXYGEN_SENSOR_4_B = 0x27;                      // ratio V
const byte OXYGEN_SENSOR_5_B = 0x28;                      // ratio V
const byte OXYGEN_SENSOR_6_B = 0x29;                      // ratio V
const byte OXYGEN_SENSOR_7_B = 0x2A;                      // ratio V
const byte OXYGEN_SENSOR_8_B = 0x2B;                      // ratio V
const byte COMMANDED_EGR = 0x2C;                          // %
const byte EGR_ERROR = 0x2D;                              // %
const byte COMMANDED_EVAPORATIVE_PURGE = 0x2E;            // %
const byte FUEL_TANK_LEVEL_INPUT = 0x2F;                  // %
const byte WARMUPS_SINCE_CODES_CLEARED = 0x30;            // count
const byte DISTANCE_TRAVELED_SINCE_CODES_CLEARED = 0x31;  // km
const byte EVAPORATION_SYSTEM_VAPOR_PRESSURE = 0x32;      // Pa
const byte ABSOLUTE_BAROMETRIC_PRESSURE = 0x33;           // kPa
const byte OXYGEN_SENSOR_1_C = 0x34;                      // ratio mA
const byte OXYGEN_SENSOR_2_C = 0x35;                      // ratio mA
const byte OXYGEN_SENSOR_3_C = 0x36;                      // ratio mA
const byte OXYGEN_SENSOR_4_C = 0x37;                      // ratio mA
const byte OXYGEN_SENSOR_5_C = 0x38;                      // ratio mA
const byte OXYGEN_SENSOR_6_C = 0x39;                      // ratio mA
const byte OXYGEN_SENSOR_7_C = 0x3A;                      // ratio mA
const byte OXYGEN_SENSOR_8_C = 0x3B;                      // ratio mA
const byte CATALYST_TEMP_BANK_1_SENSOR_1 = 0x3C;          // °C
const byte CATALYST_TEMP_BANK_2_SENSOR_1 = 0x3D;          // °C
const byte CATALYST_TEMP_BANK_1_SENSOR_2 = 0x3E;          // °C
const byte CATALYST_TEMP_BANK_2_SENSOR_2 = 0x3F;          // °C

const byte SUPPORTED_PIDS_41_60 = 0x40;              // bit encoded
const byte MONITOR_STATUS_THIS_DRIVE_CYCLE = 0x41;   // bit encoded
const byte CONTROL_MODULE_VOLTAGE = 0x42;            // V
const byte ABS_LOAD_VALUE = 0x43;                    // %
const byte FUEL_AIR_COMMANDED_EQUIV_RATIO = 0x44;    // ratio
const byte RELATIVE_THROTTLE_POSITION = 0x45;        // %
const byte AMBIENT_AIR_TEMP = 0x46;                  // °C
const byte ABS_THROTTLE_POSITION_B = 0x47;           // %
const byte ABS_THROTTLE_POSITION_C = 0x48;           // %
const byte ABS_THROTTLE_POSITION_D = 0x49;           // %
const byte ABS_THROTTLE_POSITION_E = 0x4A;           // %
const byte ABS_THROTTLE_POSITION_F = 0x4B;           // %
const byte COMMANDED_THROTTLE_ACTUATOR = 0x4C;       // %
const byte TIME_RUN_WITH_MIL_ON = 0x4D;              // min
const byte TIME_SINCE_CODES_CLEARED = 0x4E;          // min
const byte MAX_VALUES_EQUIV_V_I_PRESSURE = 0x4F;     // ratio V mA kPa
const byte MAX_MAF_RATE = 0x50;                      // g/s
const byte FUEL_TYPE = 0x51;                         // ref table
const byte ETHANOL_FUEL_PERCENT = 0x52;              // %
const byte ABS_EVAP_SYS_VAPOR_PRESSURE = 0x53;       // kPa
const byte EVAP_SYS_VAPOR_PRESSURE = 0x54;           // Pa
const byte SHORT_TERM_SEC_OXY_SENS_TRIM_1_3 = 0x55;  // %
const byte LONG_TERM_SEC_OXY_SENS_TRIM_1_3 = 0x56;   // %
const byte SHORT_TERM_SEC_OXY_SENS_TRIM_2_4 = 0x57;  // %
const byte LONG_TERM_SEC_OXY_SENS_TRIM_2_4 = 0x58;   // %
const byte FUEL_RAIL_ABS_PRESSURE = 0x59;            // kPa
const byte RELATIVE_ACCELERATOR_PEDAL_POS = 0x5A;    // %
const byte HYBRID_BATTERY_REMAINING_LIFE = 0x5B;     // %
const byte ENGINE_OIL_TEMP = 0x5C;                   // °C
const byte FUEL_INJECTION_TIMING = 0x5D;             // °
const byte ENGINE_FUEL_RATE = 0x5E;                  // L/h
const byte EMISSION_REQUIREMENTS = 0x5F;             // bit encoded

const byte SUPPORTED_PIDS_61_80 = 0x60;            // bit encoded
const byte DEMANDED_ENGINE_PERCENT_TORQUE = 0x61;  // %
const byte ACTUAL_ENGINE_TORQUE = 0x62;            // %
const byte ENGINE_REFERENCE_TORQUE = 0x63;         // Nm
const byte ENGINE_PERCENT_TORQUE_DATA = 0x64;      // %
const byte AUX_INPUT_OUTPUT_SUPPORTED = 0x65;      // bit encoded

const byte SUPPORTED_PIDS_81_100 = 0x80;            // bit encoded