// ***********************************
// * Acksen Safewater Controller
// * Arduino Uno SMD/ATMega328P 32-Pin TQFP
// * v1.3.0
#define SYSTEM_VERSION                  "SW FW v1.3.0"  ///< Firmware Version in character format.  Returned via Debug system if active.
// ***********************************

#define SERIAL_DEBUG  true  ///< This enables the serial debug system.  If this is set to False, no serial debug output will occur. This allows debug output to be toggled on/off for the entire project easily.

/**
 * @file Safewater.ino
 *
 * @mainpage Safewater UV-C Rainwater Sanitisation Controller
 * 
 * @section intro_sec Introduction
 * Documentation for the firmware of the UV-C Rainwater Sanitisation Controller, designed by Acksen Ltd
 * (https://www.acksen.com https://www.aquaductor.com) for the Safewater Research 
 * Initiative (https://www.safewater-research.com/).
 * 
 * @section description Description
 * This is the firmware for the UV-C Rainwater Sanitisation Controller, designed by Acksen Ltd
 * (https://www.acksen.com https://www.aquaductor.com) for the Safewater Research 
 * Initiative (https://www.safewater-research.com/).
 * 
 * This firmware is designed around the Arduino Uno SMD, or ATMega328P Microcontroller.
 * This firmware can be easily ported to other 8-bit Atmel Microcontrollers.
 * 
 * The Safewater Ultraviolet Water Purification Controller is designed to automatically provide safe,
 * sanitised and potable water for you and your family.
 * 
 * The controller automatically takes untreated water from your storage tank and treats it, to ensure that you
 * have a steady supply of clean water. No manual intervention is needed – the controller will function by
 * itself and notify you via an alarm if any problems occur.
 * 
 * This is designed for a system which:
 * - Collects rainwater into an untreated water tank.
 * - Uses a UV-C sanitisation lamp to disinfect the collected rainwater and ensure that it is safe for
 * human consumption.  A filter is also used to remove physical contaminants.
 * - Disinfected water is placed into a treated water tank.
 * - When the treated water tank reaches a lower bound float switch (Switch T1), the controller automatically refills it using water from the treated tank.
 * - When the upper bound float switch on the treated tank is triggered (Switch T2), disinfection and pumping stops.
 * - The untreated water tank features a float switch at the base (Switch S) to confirm that there is rainwater to pump and disinfect.
 * 
 * The controller features a number of safety features, such as:
 * - Warm-up period for the UV-C Lamp to ensure it has reached operating temperature
 * - Automatically ends the pumping sequence after a timeout period (based around the tank size/pump rate).
 * This is presently set for the Safewater systems deployed to Mexico and Colombia, and is altered using a external jumper on an input line.
 * - Override button to immediately start the disinfection sequence and provide water, regardless of present state.
 * - Uses Hardware Watchdog Timer on the Microcontroller to reboot if normal firmware operation is interrupted
 * - Checks the UV-C Lamp operating current while disinfecting, to ensure that it is operating correctly.
 * - Provides user alarms for UV-C Lamp Fault, Maximum Runtime Exceeded (due to e.g. Water Pump Fault), Treated Tank Float Switch Fault 
 * and Source Tank Empty Fault.
 * - Anti-cycling management
 * - Automatic retry system in case of transient faults
 *
 * @section circuit Circuit
 * - Firmware is designed to operate on an ATMega328P Microcontroller, 32-pin 
 * Surface Mount version.
 * - For PCB Layout and Schematic, please see the accompanying SafewaterHardware repository on Github.
 * (https://www.github.com/Acksen/SafewaterHardware)
 * - Further information is available on the Aquaductor website for the commercial implementation of the controller,
 * including instruction manuals, Wiring Diagrams, Troubleshooting, Installation & Faults Overview.
 * (https://www.aquaductor.com/resources)
 *
 * @section libraries Libraries
 * - AcksenButton (https://github.com/acksen/AcksenButton)
 *   - Processes button inputs to the system.
 * - AcksenIntEEPROM (https://github.com/acksen/AcksenIntEEPROM)
 *   - Used to store variables and data in the internal EEPROM in the microcontroller.
 * - AcksenPump (https://github.com/acksen/AcksenPump)  
 *   - Provides control for the Water Pump used with the Safewater Tank System.
 * - AcksenUtils (https://github.com/acksen/AcksenUtils)
 *   - Miscellaneous functions.
 * - Flash (https://github.com/schinken/Flash)
 *   - Used to store text strings in Program Memory rather than RAM.
 * - Arduino Time Library (https://github.com/PaulStoffregen/Time)
 *   - Provide time-related variable types and functions.
 *
 * @section author Author
 * - Created by Richard Phillips for Acksen Ltd on 14 Feb 2022.
 * - Modified by Richard Phillips for Acksen Ltd on 28 Jul 2022.
 *
 * Copyright (c) 2020, 2022 Acksen Ltd.
 * 
 * @section licence Licence
 * This source file is licenced using the GNU General Public License version 3. 
 * 
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

// @code
// ***********************************
// * Fuse Settings - Arduino Uno SMD/ATMega328P 32-Pin TQFP
// EXTENDED:	  0xFD
// HIGH:		    0xD6
// LOW:			    0xFF
// 
// BODLEVEL		Brown-out Detector trigger level					      2V7
// RSTDISBL		External Reset Disable								          False
// DWEN			  debugWIRE Enable									              False
// SPIEN		  Enable Serial Program and Data Downloading	    True
// WDTON		  Watchdog Timer always on							          False
// EESAVE		  EEPROM memory is preserved through Chip Erase	  True
// BOOTSZ		  Boot Size											                  256W_3F00
// BOOTRST		Select Reset Vector									            True
// CKDIV8		  Divide clock by 8									              False
// CKOUT		  Clock output										                False
// SUT_CKSEL	Select Clock Source									            EXTXOSC_8MHZ_XX_16KCK_14CK_65MS
//
// ***********************************
//
// ***********************************
// * Lock Bits (only set these after Fuse Settings)		0xC0
// * NOTE: Chip Erase will be required for further chip programming after these have been set.
// * LB				Lock bit										            PROG_VER_DISABLED
// * BLB0			Boot Lock bit 0									        LMP_SPM_DISABLE
// * BLB1			Boot Lock bit 1									        LPM_SPM_DISABLE
// ***********************************
// @endcode

// @code
// ***********************************
// * Changelog
// ***********************************
// v1.3.0 28 Jul 2022
// - Updated to latest version of Arduino core (v1.8.19)
// - Acksen Library versions updated to match initial open source release versions
//
// v1.2.7	11 Nov 2020
// - If alarm flag being set due to a fault, set it AFTER cooldown on UVC has finished, to prevent alarm beeping which can't be cancelled at present during cooldown.
//
// v1.2.6	09 Nov 2020
// - Address issue where UVC Lamp fault during UVC warmup may transition into pumping cycle before aborting later on
// - Move routines for turning UVC/Pump outputs off to end of relevant code sections, and force update alarm LED status before initiating UVC shutdown delay for clarity.
// - PUMPING_CYCLE_CURRENT_CHECK_DELAY_TIME_SEC set 0
//
// v1.2.5	29 Oct 2020
// - Reversal of Float Switch logic to original
// 
// v1.2.4	27 Oct 2020
// - Change UVC delay code during shutdown to only affect a delay, if the UVC Lamp is ON!
// - Changes to Silence Alarm code to work around alarm not being silenced
//
// v1.2.3	23 Oct 2020
// - Invert logic on float switch inputs
// - Add 1min delay between pump deactivation and lamp deactivation at all times
// - Reset the Float Switch Fault Flag when beginning UVC warmup, to allow any future faults in this area to be detected
// - Reset the Source Tank Empty fault as soon as the float goes back to normal position
//
// v1.2.2	23 Jul 2020
// - Swap Pump and UVC Lamp outputs
//
// v1.2.1	23 Jul 2020
// - Remove Pump Fault
// - Remove Pump Fault Flag
// - Check for UVC Lamp failure only during pumping cycle
//
// v1.2.0	02 Jul 2020
// - Change UVC Lamp and Pump Current Levels to 1.0/1.4V respectively
// - Change startup delay to 2s
// - Change Current Sampling to not remove high/low values from average
//
// v1.1.9	01 Jul 2020
// - Change UVC Lamp and Pump Current Levels to 0.7/1.3V respectively
// - Additional 5s delay on startup
//
// v1.1.8	01 Jul 2020
// - Change UVC Lamp and Pump Current levels to 0.6/0.8V respectively
//
// v1.1.7	01 Jul 2020
// - Change UVC Lamp and Pump Current levels to 0.8/1.1V respectively
// - Add delay into Current Sampling, such that 10 samples are taken over a 20ms window. (allow for mains interference)
//
// v1.1.6	30 Jun 2020
// - Change UVC Lamp and Pump Current levels to 0.4V each
//
// v1.1.5	29 Jun 2020
// - Skip for debugging
//
// v1.1.4	19 Jun 2020
// - Change trigger from Standby to Pumping so that state of S is ignored
// - Fix problem with Override Pumping not finishing after 30min
//
// v1.1.3	18 Jun 2020
// - Add ability to store and display multiple fault flags
// - Remove ability to reset fault flags, other than by power reset
//
// v1.1.2	05 Jun 2020
// - Resolve issue where fault flag is always cleared at the end of a lockout period
//
// v1.1.1	04 Jun 2020
// - Change Override Button functionality to provide 30 minutes of operation, ignoring float switch states
//
// v1.1.0	03 Jun 2020
// - Skip FW versions to keep in line with flowchart version
// - Add Runtime Jumper to switch between Mexico and Colombia settings in hardwareliving the dream
// - Remove temperature measurement for PCB overheat
// - Remove LED indicators for float status
// - Remove alarm LED indicator
// - Add new alarms for Max Overrun, Float Switches Fault, Source Tank Empty
// - Keep UVC Lamp energised for 1 minute after pump deactivates, after successful water treatment run
// - Change UVC Lamp Warm Up Time to 2 min and UVC Lamp Current Check delay to 1min
//
// v1.0.6	27 Apr 2020
// - Prevent the Float Level Switch indicators flashing in phase with the alarm buzzer
//
// v1.0.5	24 Apr 2020
// - Change Alarm functionality so that Alarm Buzzer only sounds for UVC Tuglad to hbe and Pump Alarms.  Indicators will show as normal.
//
// v1.0.4	22 Apr 2020
// - Update LED Outputs more often
// - Update LED Outputs after toggling alarm state
// - During many fault conditions, flash State Indicator LED in antiphase with alarm buzzer
// - Add Lockout Type flag
// - Remove seperate Override Timer
// - Add Flash Sequence on Alarm LED depending on Lockout Type, every second
//
// v1.0.3g	21 Apr 2020
// - reset timeout timings to correct runtime values, reset max temp 80c
// v1.0.3f 
// - remove pump/uvc warn led states reflecting if the pump/uvc is on or off
// v1.0.3e 
// - change pump current detection to 1.5v
// v1.0.3d	
// - change uvc current detection to 0.8V
// v1.0.3c	
// - change max temp to 250C to prevent incorrect trip
//v1.0.3b
// - Change UV-C Lamp control back to positive logic
// v1.0.3	20 Apr 2020 Rapid testing iterations
//
// v1.0.2	18 Mar 2020
// - Invert logic on relay outputs to interface with test setup
// - Allow for v1.7.4 of AcksenPump library
// - Increase pump relay switching delay to 500ms
// - Moved to Arduino v10.8.12
// - Add support for discrete Alarm LED
//
// v1.0.1	17 Feb 2020
// - Add ability to flash all indicators in turn when Alarm LEDs are being shown
// - Add Pump Current Check timing delay to allow startup to occur & settle down
// - Disable Pump Ventilation on the Water Pump
// - Only trigger water sterilisation cycle when T2 and T1 are both off
// - Set PCB Temp Fault and Level Switch fault states to not continually engage, when they've already been triggered once
// - Use AcksenButton object for float level switches, to provide debouncing
//
// v1.0.0	14 Feb 2020
// - Initial Version
//
// ***********************************
// @endcode

// ***********************************
// Libraries
// ***********************************
#include <Time.h>
#include <TimeLib.h>
#include <AcksenButton.h>
#include <Flash.h>
#include <AcksenIntEEPROM.h>
#include <AcksenPump.h>
#include <avr/wdt.h>
#include <AcksenUtils.h>

// ***********************************************
// * Arduino System and Libraries Version Checking
// ***********************************************
#if ARDUINO != 10819	// v1.8.19
#error Arduino Core
#endif

#if AcksenButton_ver != 130	// v1.3.0
#error AcksenButton
#endif

#if AcksenPump_ver != 180	// v1.8.0
#error AcksenPump
#endif

#if AcksenIntEEPROM_ver != 110	// v1.1.0
#error AcksenIntEEPROM
#endif

#if AcksenUtils_ver != 140	// v1.4.0
#error AcksenUtils
#endif

// ***********************************
// Serial Debug
// ***********************************
// Note - Ports are initialised using Serial identifier in Setup() section:
// modify this manually if any changes made here to ports in use
#define DebugSerial				  Serial  ///< Serial Port to use for Debug Output
#define DEBUG_BAUD_RATE			115200  ///< Baud Rate to use for Debug Output

// ***********************************
// I/O  
// ***********************************
#define UART_RX_IO					            0	  ///< Reserved for Serial Debug UART. Pin 30	PD0 on ATMega328P 32-Pin TQFP
#define UART_TX_IO					            1	  ///< Reserved for Serial Debug UART. Pin 31	PD1 on ATMega328P 32-Pin TQFP
#define ALARM_MAX_OVERRUN_TIME_LED_IO		2	  ///< Alarm LED to indicate Maximum Runtime Exceeded Fault. Pin 32	PD2 on ATMega328P 32-Pin TQFP
#define ALARM_PUMP_LED_IO			          3	  ///< (NOT IMPLEMENTED) Alarm LED to indicate Pump Current Fault. Pin 1	PD3 on ATMega328P 32-Pin TQFP
#define ALARM_UVC_LED_IO			          4	  ///< Alarm LED to indicate UV-C Lamp Current Fault. Pin 2	PD4 on ATMega328P 32-Pin TQFP
#define ALARM_S_TANK_LOW_LED_IO		      5	  ///< Alarm LED to indicate Source Tank Empty Fault. Pin 9	PD5 on ATMega328P 32-Pin TQFP
#define ALARM_FLOAT_ERROR_LED_IO	      6	  ///< Alarm LED to indicate Treated Tank Float Switches Fault. Pin 10	PD6 on ATMega328P 32-Pin TQFP
#define	SILENCE_ALARM_BUTTON_IO		      7	  ///< Input for Silence Alarm Button. Pin 11	PD7 on ATMega328P 32-Pin TQFP
#define PUMP_OUT_IO					            8	  ///< Output to control Water Pump. Pin 12	PB0 on ATMega328P 32-Pin TQFP
#define ALARM_BUZZER_IO				          9	  ///< Output to control Alarm Buzzer. Pin 13	PB1 on ATMega328P 32-Pin TQFP
#define UVC_OUT_IO					            10	///< Output to control UV-C Lamp. Pin 14	PB2 on ATMega328P 32-Pin TQFP
#define SPI_MOSI					              11	///< Reserved for SPI bus MOSI. Pin 15	PB3 on ATMega328P 32-Pin TQFP
#define SPI_MISO					              12	///< Reserved for SPI bus MISO. Pin 16	PB4 on ATMega328P 32-Pin TQFP
#define SPI_SCK						              13	///< Reserved for SPI bus SCK. Pin 17	PB5 on ATMega328P 32-Pin TQFP
#define CURRENT_ADC_IN				          A0	///< Current Measurement ADC Input. Pin 23	PC0 on ATMega328P 32-Pin TQFP
#define	MAX_RUNTIME_JUMPER_IN_IO		        A1	///< Maximum Runtime Jumper Input.  Used to switch between Mexico/Colombia. Pin 24	PC1 on ATMega328P 32-Pin TQFP
#define FLOAT_T2_SWITCH_IN_IO			        A2	///< Treated Tank T2 Float Switch Input. Pin 25	PC2 on ATMega328P 32-Pin TQFP
#define FLOAT_T1_SWITCH_IN_IO			        A3	///< Treated Tank T1 Float Switch Input. Pin 26	PC3 on ATMega328P 32-Pin TQFP
#define FLOAT_S_SWITCH_IN_IO			          A4	///< Untreated Tank S Float Switch Input. Pin 27	PC4 on ATMega328P 32-Pin TQFP
#define OVERRIDE_BUTTON_IN_IO			        A5	///< Override Button Input. Pin 28	PC5 on ATMega328P 32-Pin TQFP

// ***********************************
// Constants
// ***********************************

// EEPROM
#define EEPROM_ADDRESS_CHECK1					350   ///< Memory Address in internal EEPROM to store Check Bytes.  This allows us to confirm if the EEPROM has been initialised, or is blank/corrupt.
#define EEPROM_ADDRESS_CHECK2					351   ///< Memory Address in internal EEPROM to store Check Bytes.  This allows us to confirm if the EEPROM has been initialised, or is blank/corrupt.
	
#define EEPROM_SETTINGS							  360   ///< Memory Address in internal EEPROM to store system settings and data.

#define EEPROM_CHECKBYTE1_VALUE				101   ///< Value saved to Check Byte Location 1.  This is read back to confirm that EEPROM is initialised.
#define EEPROM_CHECKBYTE2_VALUE				201   ///< Value saved to Check Byte Location 2.  This is read back to confirm that EEPROM is initialised.

#define EEPROM_MAX_WRITES						  300   ///< Maximum EEPROM writes per session.


#define ALARM_BUZZER_FREQUENCY					  8	    ///< PWM Frequency Setting for Alarm Buzzer
#define ALARM_BUZZER_MARKSPACE_RATIO			127	  ///< PWM Output Mark/Space Ratio Setting for Alarm Buzzer
#define ALARM_BUZZER_ON_DELAY					    1000	///< Alarm ON time in Milliseconds
#define ALARM_BUZZER_OFF_DELAY					  1000	///< Alarm OFF time in Milliseconds

#define LOCAL_PUMP_RELAY_SWITCHING_DELAY	1000	///< Delay after switching the Pump Relay State, to allow settling, in Milliseconds

#define BUTTON_DEBOUNCE_INTERVAL				  100   ///< Debounce delay applied to button inputs, in Milliseconds

// State Machine Constants
#define CONTROL_STANDBY							      0	    ///< System is in Standby mode
#define CONTROL_UVC_WARM_UP						    1	    ///< System is Warming Up the UVC Lamp
#define CONTROL_PUMPING							      2	    ///< System is Pumping WATER
#define CONTROL_ANTI_CYCLE_LOCKOUT				3	    ///< System has been Locked Out of Automatic Control for anti-cycling purposes

// Float Switch Input States
#define FLOAT_SWITCH_OFF					        0     ///< Float Switch Input is LOW when float is not engaged
#define FLOAT_SWITCH_ON						        1     ///< Float Switch Input is HIGH which float is engaged

// UV-C Lamp Output States
#define UVC_LAMP_OFF							        0     ///< UV-C Lamp Output is LOW when not activated
#define UVC_LAMP_ON								        1     ///< UV-C Lamp Output is HIGH when not activated

#define UVC_LAMP_CURRENT_CHECK_TIME_SEC				      60	  ///< Current Check Delay for UVC Lamp in Seconds
#define UVC_LAMP_WARM_UP_TIME_SEC					          120	  ///< Warm-Up Time for UVC Lamp in Seconds
#define PUMPING_CYCLE_CURRENT_CHECK_DELAY_TIME_SEC	0	    ///< Current Check Delay for Pump in Seconds

#define	OVERRIDE_RUNTIME_MIN					              30    ///< Override Lockout Time in Minutes
#define ANTI_CYCLE_LOCKOUT_TIME_MIN				          60	  ///< Anti-Cycle Lockout Time in Minutes
#define FAULT_LOCKOUT_TIME_MIN					            30    ///< Time to engage Lockout for after Fault condition, in Minutes

// Lockout Types
#define LOCKOUT_NONE							                  0     ///< System is not locked out
#define LOCKOUT_POST_CYCLE						              1     ///< System is locked out as disinfection/pumping cycle has recently completed
#define LOCKOUT_PHYSICAL_OVERRIDE				            2     ///< System is locked out due to System Override being used (NOT IMPLEMENTED)
#define LOCKOUT_FAULT							                  3     ///< System is locked out as a fault has been detected

// Current Measurement
#define CURRENT_SAMPLES_AVERAGED_COUNT			        10    ///< Number of ADC samples taken of Current Level each time - measured as DC Voltage on the ADC Pin.
#define CURRENT_SAMPLES_LOPPED					            0     ///< Number of ADC samples that are lopped off the top/bottom of the sorted result list

#define	MAX_CURRENT_ADC_BITS					              1023  ///< Number of ADC bits (i.e. 10-bit)

#define MIN_UVC_CURRENT_LEVEL					              1.0   ///< DC Voltage level that corresponds to the normal operating Current of the UV-C Lamp.  Dropping below this level after warm-up is completed, indicates a system fault.

#define UPDATE_DISPLAY_TIMER_DELAY				          20	  ///< Update frequency of Controller LED outputs, in Milliseconds
#define UPDATE_CURRENT_TIMER_DELAY				          500	  ///< Update frequency of Current Measurement, in Milliseconds

#define MAX_RUNTIME_MEXICO						              100	  ///< Maximum Pump Runtime for Safewater system deployment in Mexico, in Minutes.  This is calculated using the Pump Flow Rate and Tank Sizes in the system.
#define MAX_RUNTIME_COLOMBIA					              60	  ///< Maximum Pump Runtime for Safewater system deployment in Colombia, in Minutes.  This is calculated using the Pump Flow Rate and Tank Sizes in the system.

#define PRE_UVC_DEACTIVATION_DELAY_S                60    ///< Time delay in Seconds, which is enforced every time we want to switch the UV-C Lamp off.  This ensures any water that has left the Untreated Tank is disinfected.

// ***********************************
// Class Declarations
// ***********************************

/**************************************************************************/
/*! 
    @brief  Class for containing system configuration variables/data, and functions for EEPROM read/write
*/
/**************************************************************************/
class AppSettings
{

public:

/**************************************************************************/
/*!
    @brief  Class initialisation.
*/
/**************************************************************************/
	AppSettings(void);

/**************************************************************************/
/*!
    @brief   Save present configuration to EEPROM.
    @return  True if EEPROM saved successfully.
             False if an error occurred.
*/
/**************************************************************************/
	bool SaveToEEPROM(void);    

/**************************************************************************/
/*!
    @brief  Load configuration from EEPROM.
    @return  True if EEPROM loaded successfully.
             False if an error occurred.
*/
/**************************************************************************/
	bool LoadFromEEPROM(void);

/**************************************************************************/
/*!
    @brief  Reset all configuration stored in EEPROM to default values.
*/
/**************************************************************************/
  void ResetToDefaults(void);

/**************************************************************************/
/*!
    @brief  Validate all configuration values stored in EEPROM against defined bounds.
    @return Returns true if validation successfully passed.
            Returns false if validation errors occured.
*/
/**************************************************************************/
	bool ValidateEEPROMValues(void);

};

/**************************************************************************/
/*! 
    @brief  Class for containing system Fault Flags
*/
/**************************************************************************/
class FaultFlags
{

public:

/**************************************************************************/
/*!
    @brief  Class initialisation.
*/
/**************************************************************************/
	FaultFlags(void);

	bool bFaultUVC = false;             ///< Flag for UV-C Lamp Fault
	bool bFaultMaxOverrunTime = false;  ///< Flag for Maximum Overrun Time Fault
	bool bFaultLevelSwitches = false;   ///< Flag for Treated Tank Float Switches Fault
	bool bFaultSTankLow = false;        ///< Flag for Source Tank Empty Fault
		
};

// ***********************************
// Variables
// ***********************************
AppSettings ControllerConfigSettings;         ///< Create Configuration Settings object
FaultFlags ControllerFaultFlags;              ///< Create Fault Flags object

AcksenIntEEPROM IntEEPROM(EEPROM_SETTINGS);   ///< Create Internal EEPROM object

AcksenPump WaterPump(PUMP_OUT_IO, -1);        ///< Create Pump Control object (Voltage Phase Sync not allocated)

volatile bool bSoundAlarmInProgress = false;  ///< Flag indicating if the Alarm is being deliberately sounded 

boolean bAlarmEnabled;                        ///< Flag indicating that Alarm is presently triggered
boolean bAlarmOutputToggleState;              ///< Flag indicating the actual state of the Alarm Buzzer Output
boolean bAlarmOverrideLatch;                  ///< Flag set when Alarms have been suppressed (e.g. by pressing Silence Alarm button)

unsigned long ulAlarmTimer = 0;               ///< Timer used to manage Alarm Output (toggling output between LOW and HIGH)

unsigned long ulUpdateDisplayTimer = 0;       ///< Timer used to manage update of Controller LED outputs, in Milliseconds
unsigned long ulUpdateCurrentTimer = 0;       ///< Timer used to manage update of Current Measurement, in Milliseconds

unsigned int uiMaxRuntimeSetting = MAX_RUNTIME_MEXICO;  ///< Flag for the Maximum Runtime of the System (set by external Jumper)

boolean bOverrideModeActive = false;          ///< Flag set when Override mode is active


AcksenButton btnFloatT2				=			AcksenButton(FLOAT_T2_SWITCH_IN_IO,		ACKSEN_BUTTON_MODE_NORMAL,		BUTTON_DEBOUNCE_INTERVAL, INPUT);     ///< Button object to monitor Treated Tank Float Switch T2
AcksenButton btnFloatT1				=			AcksenButton(FLOAT_T1_SWITCH_IN_IO,		ACKSEN_BUTTON_MODE_NORMAL,		BUTTON_DEBOUNCE_INTERVAL, INPUT);     ///< Button object to monitor Treated Tank Float Switch T1
AcksenButton btnFloatS				=			AcksenButton(FLOAT_S_SWITCH_IN_IO,			ACKSEN_BUTTON_MODE_NORMAL,		BUTTON_DEBOUNCE_INTERVAL, INPUT);     ///< Button object to monitor Untreated Tank Float T2
AcksenButton btnOverride			=			AcksenButton(OVERRIDE_BUTTON_IN_IO,		ACKSEN_BUTTON_MODE_LONGPRESS,	BUTTON_DEBOUNCE_INTERVAL, INPUT);     ///< Button object to monitor the Override button input
AcksenButton btnSilenceAlarm		=			AcksenButton(SILENCE_ALARM_BUTTON_IO,	ACKSEN_BUTTON_MODE_LONGPRESS,	BUTTON_DEBOUNCE_INTERVAL, INPUT); ///< Button object to monitor the Silence Alarm button input

AcksenUtils Utilities;    ///< Create AcksenUtils object to access misc. helper functions

	int iControllerState = CONTROL_STANDBY; ///< Controller State - what is the controller presently doing.
	int iLockoutState = LOCKOUT_NONE;       ///< Lockout State - has the system been locked out?

	time_t dtWarmUpCompleted;     ///< Timer to manage Warm Up Completion time
	time_t dtPumpingTimeout;      ///< Timer to manage Pumping Timeout/Maximum Runtime
	time_t dtLockoutTimeout;      ///< Timer to manage Lockout System
	time_t dtUVCLampCurrentCheck; ///< Timer to manage checking of UV-C Lamp Current
	time_t dtPumpCurrentCheck;    ///< Timer to manage checking of Pump Current (CHECK NOT IMPLEMENTED)
	time_t dtOverrideModeTimeout; ///< Timer to manage Override Mode

	float fLastCurrentValue;      ///< Previously measured current level (in DC Volts)
	float fCurrentValue;          ///< Present current level (in DC Volts)

/*
 * Save configuration to EEPROM
 */
void SaveEEPROMValues()
{

	Traceln(F("SaveEEPROMValues() Triggered"));

	// Save Settings to EEPROM
	ControllerConfigSettings.SaveToEEPROM();

}

/*
 * Read configuration from EEPROM.
 * Confirm if the Check Bytes are populated with expected values; if not, assume EEPROM is blank/corrupt
 * and refresh it with default values.
 * 
 * Returns True if initialisation was required.
 */
bool ReadEEPROMValues()
{

	int iEEPROMCheck1;
	int iEEPROMCheck2;

	bool bInitialiseEEPROM = false;

	// Check to see if the EEPROM has been initialised
	iEEPROMCheck1 = EEPROM.readByte(EEPROM_ADDRESS_CHECK1);
	iEEPROMCheck2 = EEPROM.readByte(EEPROM_ADDRESS_CHECK2);

	Trace(F("EEPROM PreCheck1 = "));
	Traceln(iEEPROMCheck1);
	Trace(F("EEPROM PreCheck2 = "));
	Traceln(iEEPROMCheck2);

	if ((iEEPROMCheck1 != EEPROM_CHECKBYTE1_VALUE) || (iEEPROMCheck2 != EEPROM_CHECKBYTE2_VALUE))
	{

		Traceln(F("EEPROM Init Required."));

		// Need to initialise EEPROM before reading
		InitialiseEEPROMValues();

		bInitialiseEEPROM = true;
	}
	else
	{
		Traceln(F("EEPROM Init NOT required."));

		bInitialiseEEPROM = false;
	}


	// Load Settings
	ControllerConfigSettings.LoadFromEEPROM();

	return bInitialiseEEPROM;


}

/*
 * Initialise EEPROM with default values, and set the Check Bytes to expected values
 * as confirmation that EEPROM is initialised.
 */
void InitialiseEEPROMValues()
{

	// Return Settings to Default Values
	ControllerConfigSettings.ResetToDefaults();

	// Save Settings to EEPROM
	ControllerConfigSettings.SaveToEEPROM();

	// Write EEPROM initialised flag to memory
	EEPROM.writeByte(EEPROM_ADDRESS_CHECK1, EEPROM_CHECKBYTE1_VALUE);
	EEPROM.writeByte(EEPROM_ADDRESS_CHECK2, EEPROM_CHECKBYTE2_VALUE);

}

FaultFlags::FaultFlags(void)
{

	// Initialisation Here

}

AppSettings::AppSettings(void)
{

	// Initialisation Here

}

/*
 * Implementation of AppSettings::SaveToEEPROM
 */
bool AppSettings::SaveToEEPROM(void)
{

	Traceln(F("Save App Settings to EEPROM"));

	IntEEPROM.resetPresentAddress();

  ///< Values to be written out to EEPROM go here.
  ///< The EEPROM is not presently used/populated, but reserved for future use
  ///< (e.g. configuration, statistics)
	//IntEEPROM.writeEEPROMValueBit(WaterPump.bEnablePumpVentilation);
	//IntEEPROM.writeEEPROMValueInt(WaterPump.iPumpVentilationCycles);

	return true;

}

/*
 * Implementation of AppSettings::LoadFromEEPROM
 */
bool AppSettings::LoadFromEEPROM(void)
{

	Traceln(F("Load App Settings from EEPROM"));

	IntEEPROM.resetPresentAddress();

  ///< Values to be read out from EEPROM go here.
  ///< The EEPROM is not presently used/populated, but reserved for future use
  ///< (e.g. configuration, statistics)
  
	// Read EEPROM Values
	//WaterPump.bEnablePumpVentilation = IntEEPROM.readEEPROMValueBit();
	//WaterPump.iPumpVentilationCycles = IntEEPROM.readEEPROMValueInt();

	// Validate the values, and return the result of the validation upwards
	return ValidateEEPROMValues();

}

/*
 * Implementation of AppSettings::ValidateEEPROMValues
 */
bool AppSettings::ValidateEEPROMValues(void)
{

	bool bValidateError = false;

	// Unvalidated parameters
	// bPumpVentilation

  ///< Validation of EEPROM values goes here.
  ///< The EEPROM is not presently used/populated, but reserved for future use
  ///< (e.g. configuration, statistics)
  
	// Validation
	//if (IntEEPROM.validateInt(1, 5, iPumpVentilationCycles) == false) { Traceln(F("*** Validate Error - Pump Vent Cycles")); bValidateError = true; }


	if (bValidateError == true)
	{

		Traceln(F("*** Resetting all EEPROM Parameters to defaults"));
		// Reset all EEPROM Parameters to defaults
		InitialiseEEPROMValues();
		SaveEEPROMValues();

		return false;
	}
	else
	{
		return true;
	}

}

/*
 * Implementation of AppSettings::ResetToDefaults
 */
void AppSettings::ResetToDefaults(void)
{

  ///< Initialisation of EEPROM values goes here.  Set all variables to known good defaults.
  ///< The EEPROM is not presently used/populated, but reserved for future use
  ///< (e.g. configuration, statistics)
  
	//WaterPump.bEnablePumpVentilation = PUMP_VENTILATION_ENABLED_DEFAULT;
	//WaterPump.iPumpVentilationCycles = PUMP_VENTILATION_CYCLE_COUNT_DEFAULT;

}

// ************************************************
// Setup 
// ************************************************
/**
 * The standard Arduino setup function used for setup and configuration tasks.
 * 
 * Here we:
 * - Setup the WDT (Watchdog Timer) to operate after 8 seconds and reset the system, if the WDT isn't reset regularly.
 * - Initialise the Serial Port and debug system (if SERIAL_DEBUG is defined as true).
 * - Display the amount of Free RAM on startup, to ensure sufficient left for runtime variable declaration.
 * - Initialise I/O ports on the microcontroller to input/output as required.
 * - Set PWM frequency for the Alarm Buzzer Output.
 * - Initialise EEPROM storage system and read variables.
 * - Check the status of the Maximum Runtime Jumper, and set the System to Mexico or Colombia mode appropriately.
 * - Setup internal time counting system.
 * - Light all LED indicators for a period of time, so operational status can be checked.
 * - Setup the Water Pump (relay switching delay, pump ventilation, etc).
 * - Check Free RAM again after initialisation has been carried out.
 */
void setup()
{

	// Activate Watchdog Timer
	wdt_enable(WDTO_8S);

	// Serial Port Setup
	beginDebug();

	Traceln(F("Begin Startup..."));
	Traceln(SYSTEM_VERSION);

	Trace(F("Startup FreeRAM:"));
	Traceln(Utilities.freeRam());

	// Set I/O
	InitialiseIO();
	//WaterPump.switchPumpNegativeLogic();
	digitalWrite(UVC_OUT_IO, UVC_LAMP_OFF);
	digitalWrite(PUMP_OUT_IO, WaterPump.iPumpOffState);

	// Initialise Alarm Buzzer
	setPwmFrequency(9, ALARM_BUZZER_FREQUENCY);

	// Initialise EEPROM variables
	EEPROM.setMemPool(0, EEPROMSizeATmega328);
	EEPROM.setMaxAllowedWrites(EEPROM_MAX_WRITES);

	ReadEEPROMValues();


	boolean bValidRuntimeReadingFound = false;
	unsigned int uiCheckCount = 0;

	while ((bValidRuntimeReadingFound == false) && (uiCheckCount <= 10))
	{
	
		// Check the Max Runtime Jumper
		if (digitalRead(MAX_RUNTIME_JUMPER_IN_IO) == false)
		{
			delay(50);
			if (digitalRead(MAX_RUNTIME_JUMPER_IN_IO) == false)
			{
				uiMaxRuntimeSetting = MAX_RUNTIME_MEXICO;
				bValidRuntimeReadingFound = true;
				break;
			}
		}
		else
		{
			delay(50);
			if (digitalRead(MAX_RUNTIME_JUMPER_IN_IO) == true)
			{
				uiMaxRuntimeSetting = MAX_RUNTIME_COLOMBIA;
				bValidRuntimeReadingFound = true;
				break;
			}
		}

		uiCheckCount++;

	}

	// Ensure that a default jumper value is set, if for some reason a consistent value isn't found (e.g. N/C floating jumper)
	if (bValidRuntimeReadingFound == false)
	{
		uiMaxRuntimeSetting = MAX_RUNTIME_MEXICO;
	}

	// Reset Watchdog Timer
	wdt_reset();

	// Startup Delay
	delay(2000);

	// RTCC  
	// Start internal system clock running - only used for relative rather than absolute time, so exact date/time setting not important.
	setTime(0, 0, 0, 1, 1, 2020);

	// Reset Watchdog Timer
	wdt_reset();

	// Initial display for Light Indicators
	InitialiseLights(true);
	delay(3000);

	// Deactivate Light Indicators after delay
	InitialiseLights(false);

	// Set Pump Relay Switching Delay
	WaterPump.iPumpRelaySwitchingDelay = LOCAL_PUMP_RELAY_SWITCHING_DELAY;

	// Disable Pump Ventilation
	WaterPump.bEnablePumpVentilation = false;

	Trace(F("FreeRAM after Initialisation:"));
	Traceln(Utilities.freeRam());

	Traceln(F("Startup Complete."));

}

/**
 * Setup I/O ports on the microcontroller, assigning to input/output as required.
 */
void InitialiseIO()
{

	pinMode(ALARM_MAX_OVERRUN_TIME_LED_IO, OUTPUT);
	pinMode(ALARM_PUMP_LED_IO, OUTPUT);
	pinMode(ALARM_UVC_LED_IO, OUTPUT);
	pinMode(ALARM_S_TANK_LOW_LED_IO, OUTPUT);
	pinMode(ALARM_FLOAT_ERROR_LED_IO, OUTPUT);
	pinMode(UVC_OUT_IO, OUTPUT);
	pinMode(ALARM_BUZZER_IO, OUTPUT);
	pinMode(PUMP_OUT_IO, OUTPUT);

	pinMode(CURRENT_ADC_IN, INPUT);
	pinMode(FLOAT_T2_SWITCH_IN_IO, INPUT);
	pinMode(FLOAT_T1_SWITCH_IN_IO, INPUT);
	pinMode(FLOAT_S_SWITCH_IN_IO, INPUT);
	pinMode(OVERRIDE_BUTTON_IN_IO, INPUT);
	pinMode(MAX_RUNTIME_JUMPER_IN_IO, INPUT);

}

/**
 * Light all LED indicators on the system briefly.
 * This is normally used on startup so that operators can check all LEDs are operating correctly, 
 * and visibly confirm that the system is powering up/has been reset.
 * 
 * This will also sound the alarm buzzer on startup, again for confirmation that a reset has occured.
 */
void InitialiseLights(bool bState)
{

	digitalWrite(ALARM_MAX_OVERRUN_TIME_LED_IO, bState);
	digitalWrite(ALARM_PUMP_LED_IO, bState);
	digitalWrite(ALARM_UVC_LED_IO, bState);
	digitalWrite(ALARM_S_TANK_LOW_LED_IO, bState);
	digitalWrite(ALARM_FLOAT_ERROR_LED_IO, bState);

	if (bState == true)
	{
		// Normal Startup
		SoundAlarm_Short();
		delay(50);
		SoundAlarm_Short();
		delay(50);
		SoundAlarm_Short();
		delay(50);
	}

}

// ************************************************
// Main Control Loop
// ************************************************
/**
 * The standard Arduino loop function used for repeating tasks.
 * 
 * Here we:
 * - Reset the Watchdog Timer regularly.
 * - call measureCurrent() to read the System Current.
 * - call processIO() to update the button object states, and act on any button presses.
 * - call processSoftInterrupts() to check for changes in Float status and process fault flags.
 * - call AlarmBuzzerToggle() to drive the Alarm Buzzer ON/OFF as necessary.
 * - call updateDisplay() to update LED indicators with any changes in fault flags.
 * - call WaterPump.process() to drive the pump control loop (running automatic systems like pump ventilation, etc).
 * - call processStateMachine() to update the main State Machine driving controller operation.
 */
void loop()
{

	// Reset Watchdog Timer
	wdt_reset();

	// Measure Current
	measureCurrent();

	// Check Button States
	processIO();

	// Check other Soft Interrupts
	processSoftInterrupts();

	// Update Alarm Toggle
	AlarmBuzzerToggle();

	// Update LED Display
	updateDisplay(false);

	// Run the Pump Control Loop
	WaterPump.process();

	// Process State Machine
	processStateMachine();

}

/**
 * Toggle the Alarm Buzzer ON and OFF on a timer basis, when it's active.
 */
void AlarmBuzzerToggle()
{

  // Check to see if the Alarm Timer has elpased, and we need to update Alarm Buzzer state
	if (ulAlarmTimer <= millis())
	{

		// Alarm Toggle Triggered
		if (bAlarmEnabled == true)
		{

			// Display the Alarm LED in normal flash sequence
			if (bAlarmOutputToggleState == true)
			{
				if (bAlarmOverrideLatch == false)
				{ 
					analogWrite(ALARM_BUZZER_IO, ALARM_BUZZER_MARKSPACE_RATIO); 
				}
				else
				{
					// Alarm overridden - sounder shouldn't be operating!
					analogWrite(ALARM_BUZZER_IO, 0);
				}
				ulAlarmTimer = millis() + ALARM_BUZZER_ON_DELAY;
			}
			else
			{
				analogWrite(ALARM_BUZZER_IO, 0); 
				ulAlarmTimer = millis() + ALARM_BUZZER_OFF_DELAY;
			}

		}
		else
		{

			// Alarm NOT active
		
			// Ensure the alarm is disabled 
			if (bSoundAlarmInProgress == false)
			{
				analogWrite(ALARM_BUZZER_IO, 0);
			}

			ulAlarmTimer = millis() + ALARM_BUZZER_OFF_DELAY;

		}

		// Toggle the Alarm State regardless of alarm being active or not - use this to flash the other alarm LED indicators as needed
		bAlarmOutputToggleState = not bAlarmOutputToggleState;

		// Force LED Update
		updateDisplay(true);

	}

}

/**
 * Drive the Alarm Buzzer to produce a short 'beep' sound.
 * Setting bSoundAlarmInProgress here prevents the main Alarm Buzzer processing system from affecting this operation.
 */
void SoundAlarm_Short()
{

	// Output Alarm Buzzer for 50ms
	bSoundAlarmInProgress = true;
	analogWrite(ALARM_BUZZER_IO, ALARM_BUZZER_MARKSPACE_RATIO);
 
	delay(50);
	
	analogWrite(ALARM_BUZZER_IO, 0);
	bSoundAlarmInProgress = false;

}

/**
 * Drive the Alarm Buzzer to produce a short 'beep' sound.
 * Setting bSoundAlarmInProgress here prevents the main Alarm Buzzer processing system from affecting this operation.
 */
void MenuBeep()
{

	// Output Alarm Buzzer for 25ms 
	bSoundAlarmInProgress = true;
	analogWrite(ALARM_BUZZER_IO, ALARM_BUZZER_MARKSPACE_RATIO);

	delay(25);

	analogWrite(ALARM_BUZZER_IO, 0);
	bSoundAlarmInProgress = false;

}

/**
 * Update the button object states, and act on any button presses.
 * 
 * Here we:
 * - Process presses of the Silence Alarm button.
 * - Process presses of the Override button, which will move the system immediately into disinfection mode
 * when in standby.
 */
void processIO()
{

	btnFloatT2.refreshStatus();
	btnFloatT1.refreshStatus();
	btnFloatS.refreshStatus();
	btnOverride.refreshStatus();
	btnSilenceAlarm.refreshStatus();

  // Check to see if the Silence Alarm button has been pressed
	if (btnSilenceAlarm.onPressed() == true)
	{

		Traceln(F("Silence Alarm Button Pressed..."));

		// Ensure that the buzzer is silenced immediately!
		analogWrite(ALARM_BUZZER_IO, 0);
		delay(200);

		// Indicate to user that the button has been pressed
		SoundAlarm_Short();
		delay(50);
		SoundAlarm_Short();
		delay(50);                                                                                                                                                                                                                                                                                                                                                                                                
		SoundAlarm_Short();
		delay(50);

		// Latch the Override to suppress any future alarms
		bAlarmOverrideLatch = true;

		// Disable any present alarms!
		bAlarmEnabled = false;
	}


	// Check to see if the Override Switch has been pressed, if in standby mode - this will override any other ongoing operations!
	if ((btnOverride.onPressed() == true) && (iControllerState == CONTROL_STANDBY))
	{
		Traceln(F("Override Button Pressed..."));

		bOverrideModeActive = true;

		// Move into UVC Warm-Up Mode
		Traceln(F("*** Control set to UVC Lamp Warm Up"));
		iControllerState = CONTROL_UVC_WARM_UP;

		// Turn on the UVC Lamp
		digitalWrite(UVC_OUT_IO, UVC_LAMP_ON);

		// Set Timer for advancing to Pumping Stage after Warm-Up Mode completed
		dtWarmUpCompleted = now() + UVC_LAMP_WARM_UP_TIME_SEC;
		dtUVCLampCurrentCheck = now() + UVC_LAMP_CURRENT_CHECK_TIME_SEC;

	}

}

/**
 * Check for changes in Float switch status and process fault flags.
 * 
 * We review faults to see if they can now be cleared and system returned to normal, and also trigger a 
 * Treated Tank Float Switch fault if fault conditions are met.
 */
void processSoftInterrupts()
{

	if (bOverrideModeActive == true)
	{
		// Bypass processing of Soft Interrupts due to Override mode being active
		return;
	}

	// Source Tank Level Switch Check
	if (ControllerFaultFlags.bFaultSTankLow == true)
	{

		// Check to see if the Source Tank has gone back to normal level in the interim - allowing us to cancel any fault and related alarms.
		if (btnFloatS.getButtonState() == FLOAT_SWITCH_ON)
		{
			// Switch has gone back to normal position!
			Traceln(F("***CLEARING Source Tank Empty Fault due to change in switch position"));
			ControllerFaultFlags.bFaultSTankLow = false;

			// Check to see if the Alarm is currently active
			if (bAlarmEnabled == true)
			{

				// Check to see if we can deactivate the alarm!
				if (
					(ControllerFaultFlags.bFaultUVC == false)
					&&
					(ControllerFaultFlags.bFaultMaxOverrunTime == false)
					&&
					(ControllerFaultFlags.bFaultLevelSwitches == false)
					&&
					(ControllerFaultFlags.bFaultSTankLow == false)
					)
				{
					// Alarm can now be deactivated!
					bAlarmEnabled = false;

					// Alarm will turn itself off automatically next time update routine is executed
				}

			}

			//  LED Display updated immediately
			updateDisplay(true);

		}

	}


	// Water Level Switch Check
	if (
		(btnFloatT2.getButtonState() == FLOAT_SWITCH_ON)
		&&
		(btnFloatT1.getButtonState() == FLOAT_SWITCH_OFF)
		&&
		(ControllerFaultFlags.bFaultLevelSwitches == false)
		)
	{

		// This should never happen - level switches should never be in conflict

		// Setup Lockout Condition
		Traceln(F("Control set to Anti Cycle Lockout"));
		iControllerState = CONTROL_ANTI_CYCLE_LOCKOUT;

		// Deactivate Override Mode
		bOverrideModeActive = false;


		// Set Lockout Expiry Time
		dtLockoutTimeout = now() + (FAULT_LOCKOUT_TIME_MIN * 60);
		iLockoutState = LOCKOUT_FAULT;

		Traceln(F("*** Level Switch Fault Mode Engaged"));
		ControllerFaultFlags.bFaultLevelSwitches = true;

		// LED Display updated immediately
		updateDisplay(true);

		// Deactivate Pump/UVC Lamp
		WaterPump.turnOff();
		PreUVCDeactivationDelay();
		digitalWrite(UVC_OUT_IO, UVC_LAMP_OFF);

		// Sound Alarm
		if (bAlarmOverrideLatch == false)
		{
			bAlarmEnabled = true;
		}

		// End Function without further code execution
		return;

	}

}

/**
 * Set the PWM output frequency.
 * This function will need reviewed/updated if used on different microcontrollers to ensure correct operation, 
 * especially outside the 8-bit AVR family, as MCU specific Timer Control Registers are being set (TCCR0B/TCCR1B/TCCR2B).
 * 
 * Presently designed for use with the ATMega328P.
 */
void setPwmFrequency(int iPin, int iDivisor)
{

	byte bMode;

	if (iPin == 5 || iPin == 6 || iPin == 9 || iPin == 10)
	{

		switch (iDivisor)
		{
  		case 1: bMode = 0x01; break;
  		case 8: bMode = 0x02; break;
  		case 64: bMode = 0x03; break;
  		case 256: bMode = 0x04; break;
  		case 1024: bMode = 0x05; break;
  		default: return;
		}

		if ((iPin == 5) || (iPin == 6))
		{
			TCCR0B = (TCCR0B & 0b11111000) | bMode;
		}
		else
		{
			TCCR1B = (TCCR1B & 0b11111000) | bMode;
		}

	}
	else if ((iPin == 3) || (iPin == 11))
	{

		switch (iDivisor)
		{
  		case 1: bMode = 0x01; break;
  		case 8: bMode = 0x02; break;
  		case 32: bMode = 0x03; break;
  		case 64: bMode = 0x04; break;
  		case 128: bMode = 0x05; break;
  		case 256: bMode = 0x06; break;
  		case 1024: bMode = 0x07; break;
  		default: return;
		}

		TCCR2B = TCCR2B & (0b11111000 | bMode);

	}

}

/**
 * Update the main State Machine driving controller operation.
 * 
 * This function uses iControllerState to drive what the system is doing next, based on timers and system inputs.
 * 
 * Possible States:
 * CONTROL_STANDBY                   System is in Standby mode
 * CONTROL_UVC_WARM_UP               System is Warming Up the UVC Lamp
 * CONTROL_PUMPING                   System is Pumping WATER
 * CONTROL_ANTI_CYCLE_LOCKOUT        System has been Locked Out of Automatic Control for anti-cycling purposes
 * 
 * Standby Mode:
 * - Switch into operating mode if the Treated Tank needs more water, and the Source Tank has water to treat.
 * - Otherwise ensure that the Pump and UV-C Lamp are turned OFF.
 * 
 * Warming up UV-C Lamp Mode:
 * - After a period of time has elapsed to let the UV-C lamp get to normal operating level, start checking the UV-C
 * Lamp Current to confirm correct operation.
 * - Fault is flagged if current below specified levels, and disinfection aborted before pumping takes place.
 * - If the period for Warming Up has been met, move to Pumping Mode.
 * 
 * Pumping Mode:
 * - Continuously check that the UV-C Lamp Current is sufficient, otherwise abort and flag fault
 * - Check to see if the Source Tank has emptied, if so, end Pumping and set fault flag
 * - Check to see if the Treated Tank has been filled, if so, end Pumping sequence normally, moving to post-pumping anti-cycling Lockout mode
 * - If Maximum Runtime has been exceeded (possible Pump Fault), end Pumping and set fault flag
 * 
 * Anti Cycle Lockout Mode:
 * - Move back to Standby mode once the lockout timer has elapsed.
 */
void processStateMachine()
{

	// Main Control Loop
	if (iControllerState == CONTROL_STANDBY)
	{

		// Check to see if the T2 Level Switch on the Treated Tank has gone off, and S Level Switch indicates there's water to treat
		if (
			(btnFloatT2.getButtonState() == FLOAT_SWITCH_OFF)
			&&
			(btnFloatT1.getButtonState() == FLOAT_SWITCH_OFF)
			)
		{

			// Ensure that Override Mode is not active
			bOverrideModeActive = false;

			// Deactivate the Float Switch Fault flag - this will allow any faults to be redetected later.
			ControllerFaultFlags.bFaultLevelSwitches = false;

			// Move into UVC Warm-Up Mode
			Traceln(F("*** Control set to UVC Lamp Warm Up"));
			iControllerState = CONTROL_UVC_WARM_UP;

			// Turn on the UVC Lamp
			digitalWrite(UVC_OUT_IO, UVC_LAMP_ON);

			// Set Timer for advancing to Pumping Stage after Warm-Up Mode completed
			dtWarmUpCompleted = now() + UVC_LAMP_WARM_UP_TIME_SEC;
			dtUVCLampCurrentCheck = now() + UVC_LAMP_CURRENT_CHECK_TIME_SEC;

		}
		else
		{
		
			// Ensure that Pump and UVC Lamp are turned OFF.
			WaterPump.turnOff();
			digitalWrite(UVC_OUT_IO, UVC_LAMP_OFF);

		}

	}
	else if (iControllerState == CONTROL_UVC_WARM_UP)
	{

		// Check to see if the UVC Lamp Current Check Timer has been triggered
		if (now() >= dtUVCLampCurrentCheck)
		{

			// Check to see if the UVC Lamp Current is sufficent
			if (fCurrentValue >= (float)MIN_UVC_CURRENT_LEVEL)
			{
				// Level OK!
				//Trace(F("*** UVC Lamp Current OK! Measured="));
				//Traceln(fCurrentValue);
			}
			else
			{

				// UVC Lamp Current is below minimum levels
				Trace(F("*** Control set to Anti Cycle Lockout! Measured="));
				Traceln(fCurrentValue);
				iControllerState = CONTROL_ANTI_CYCLE_LOCKOUT;

				// Deactivate Override Mode
				bOverrideModeActive = false;


				// Set Lockout Expiry Time
				dtLockoutTimeout = now() + (FAULT_LOCKOUT_TIME_MIN * 60);
				iLockoutState = LOCKOUT_FAULT;

				Traceln(F("*** UVC Lamp Fault Mode Engaged"));
				ControllerFaultFlags.bFaultUVC = true;

				// LED Display updated immediately
				updateDisplay(true);

				// Deactivate Pump/UVC Lamp
				WaterPump.turnOff();
				PreUVCDeactivationDelay();
				digitalWrite(UVC_OUT_IO, UVC_LAMP_OFF);

				// Sound Alarm
				if (bAlarmOverrideLatch == false)
				{
					bAlarmEnabled = true;
				}

			}

		}

		// Check to see if the Warm Up Time has been exceeded - and confirm that we're still in warm-up mode, and haven't been taken out of it due to fault!
		if ((now() >= dtWarmUpCompleted) && (iControllerState == CONTROL_UVC_WARM_UP))
		{

			Traceln(F("Control set to Pumping"));
			iControllerState = CONTROL_PUMPING;

			WaterPump.ToggleState();

			dtPumpCurrentCheck = now() + PUMPING_CYCLE_CURRENT_CHECK_DELAY_TIME_SEC;

			// Set Backup Max Timer for Pumping Stage 
			dtPumpingTimeout = now() + (uiMaxRuntimeSetting * 60);

			if (bOverrideModeActive == true)
			{
				// If in Override mode, set the Override Max Runtime
				dtOverrideModeTimeout = now() + (OVERRIDE_RUNTIME_MIN * 60);
			}

		}

	}
	else if (iControllerState == CONTROL_PUMPING)
	{

		// Check to see if the UVC Lamp Current is sufficient, if our startup delay has been exceeded
		if ((fCurrentValue < (float)MIN_UVC_CURRENT_LEVEL) && (now() >= dtPumpCurrentCheck))
		{

			// Current is below minimum levels

			// System has already failed at this stage before.. need to flag up a Pump Fault!
			Traceln(F("Control set to Anti Cycle Lockout"));
			iControllerState = CONTROL_ANTI_CYCLE_LOCKOUT;

			// Deactivate Override Mode
			bOverrideModeActive = false;


			// Set Lockout Expiry Time
			dtLockoutTimeout = now() + (FAULT_LOCKOUT_TIME_MIN * 60);
			iLockoutState = LOCKOUT_FAULT;

			Traceln(F("*** UVC Lamp Fault Mode Engaged"));
			ControllerFaultFlags.bFaultUVC = true;

			// LED Display updated immediately
			updateDisplay(true);

			// Deactivate Pump/UVC Lamp
			WaterPump.turnOff();
			PreUVCDeactivationDelay();
			digitalWrite(UVC_OUT_IO, UVC_LAMP_OFF);

			// Sound Alarm
			if (bAlarmOverrideLatch == false)
			{
				bAlarmEnabled = true;
			}

		}
		// Check to see if the Source Tank has run down
		else if ((btnFloatS.getButtonState() == FLOAT_SWITCH_OFF) && (bOverrideModeActive == false))
		{

			// Pumping is complete - the source tank is near empty

			Traceln(F("Control set to Anti Cycle Lockout"));
			iControllerState = CONTROL_ANTI_CYCLE_LOCKOUT;

			// Deactivate Override Mode
			bOverrideModeActive = false;

			// Set Lockout Expiry Time
			dtLockoutTimeout = now() + (FAULT_LOCKOUT_TIME_MIN * 60);
			iLockoutState = LOCKOUT_FAULT;

			Traceln(F("***Source Tank Empty Mode Engaged"));
			ControllerFaultFlags.bFaultSTankLow = true;

			// LED Display updated immediately
			updateDisplay(true);

			// Deactivate Pump/UVC Lamp
			WaterPump.turnOff();
			PreUVCDeactivationDelay();
			digitalWrite(UVC_OUT_IO, UVC_LAMP_OFF);

			// Sound Alarm
			if (bAlarmOverrideLatch == false)
			{
				bAlarmEnabled = true;
			}

		}
		// Check to see if Completion condition has been met for Pumping
		else if (
					(
						(btnFloatT2.getButtonState() == FLOAT_SWITCH_ON)
						&&
						(btnFloatT1.getButtonState() == FLOAT_SWITCH_ON)
					)
					&&
					(bOverrideModeActive == false)
				)
		{

			// Pumping is complete - the Treated tank is Full

			// Go to Override Lockout mode to prevent cycling
			Traceln(F("Control set to Anti Cycle Lockout"));
			iControllerState = CONTROL_ANTI_CYCLE_LOCKOUT;

			// Deactivate Override Mode
			bOverrideModeActive = false;

			// Set Lockout Expiry Time
			dtLockoutTimeout = now() + (ANTI_CYCLE_LOCKOUT_TIME_MIN * 60);
			iLockoutState = LOCKOUT_POST_CYCLE;

			// Latch the Override to suppress any future alarms
			bAlarmOverrideLatch = false;

			// Disable any present alarms
			bAlarmEnabled = false;

			// LED Display updated immediately
			updateDisplay(true);

			// Deactivate Pump/UVC Lamp
			WaterPump.turnOff();
			PreUVCDeactivationDelay();
			digitalWrite(UVC_OUT_IO, UVC_LAMP_OFF);

		}
		// Check to see if completition condition met in Override Mode
		else if (
					(now() >= dtOverrideModeTimeout)
					&&
					(bOverrideModeActive == true)
				)
		{

			// Pumping is complete - the Treated tank is Full

			// Deactivate Override Mode
			bOverrideModeActive = false;

			// Go to Override Lockout mode to prevent cycling
			Traceln(F("Control set to Anti Cycle Lockout"));
			iControllerState = CONTROL_ANTI_CYCLE_LOCKOUT;

			// Set Lockout Expiry Time
			dtLockoutTimeout = now() + (ANTI_CYCLE_LOCKOUT_TIME_MIN * 60);
			iLockoutState = LOCKOUT_POST_CYCLE;

			// LED Display updated immediately
			updateDisplay(true);

			// Deactivate Pump/UVC Lamp
			WaterPump.turnOff();
			PreUVCDeactivationDelay();
			digitalWrite(UVC_OUT_IO, UVC_LAMP_OFF);

		}
		else if ((now() >= dtPumpingTimeout) && (bOverrideModeActive == false))
		{

			// Pumping Timeout has been exceeded - this should not be possible given relative tank capacities, so indicates a fault condition.

			Traceln(F("Control set to Anti Cycle Lockout"));
			iControllerState = CONTROL_ANTI_CYCLE_LOCKOUT;

			// Deactivate Override Mode
			bOverrideModeActive = false;

			// Set Lockout Expiry Time
			dtLockoutTimeout = now() + (FAULT_LOCKOUT_TIME_MIN * 60);
			iLockoutState = LOCKOUT_FAULT;

			Traceln(F("***Max Overrun Time Fault Mode Engaged"));
			ControllerFaultFlags.bFaultMaxOverrunTime = true;

			// LED Display updated immediately
			updateDisplay(true);

			// Deactivate Pump/UVC Lamp
			WaterPump.turnOff();
			PreUVCDeactivationDelay();
			digitalWrite(UVC_OUT_IO, UVC_LAMP_OFF);

			// Sound Alarm
			if (bAlarmOverrideLatch == false)
			{
				bAlarmEnabled = true;
			}

		}

	}
	else if (iControllerState == CONTROL_ANTI_CYCLE_LOCKOUT)
	{

		// Deactivate Override Mode
		bOverrideModeActive = false;


		// Check to see if Override Lockout Time has expired
		if (now() >= dtLockoutTimeout)
		{
			// Move back to normal operating mode
			Traceln(F("Control set to Standby"));
			iControllerState = CONTROL_STANDBY;
			iLockoutState = LOCKOUT_NONE;

			// Deactivate Override Mode
			bOverrideModeActive = false;

		}

		// LED Display updated immediately
		updateDisplay(true);

		// Deactivate Pump/UVC Lamp
		WaterPump.turnOff();
		PreUVCDeactivationDelay();
		digitalWrite(UVC_OUT_IO, UVC_LAMP_OFF);

	}


}

/**
 * Measure Current on a timer basis.
 * 
 * This carries out multiple ADC samples, sorts them into ascending order, lops outliers from start/end
 * of array (if set to do so) and finds an average value.
 * 
 * We use this technique to ensure the ADC sample is as accurate as possible, as it can be easily affected
 * by system noise and other interference factors.
 * 
 * The ADC value is then converted to a voltage from 0-5V, which is used later to confirm the 
 * UV-C Lamp has reached operating current.
 * 
 * This could be converted to a current value, however knowing the measured voltage produced by 
 * the minimum operating current level is sufficient for our purposes.
 */
void measureCurrent()
{

	// Check to see Update Current Timer has expired
	if (ulUpdateCurrentTimer > millis())
	{
		return;
	}
	else
	{
		// Update next refresh time
		ulUpdateCurrentTimer = millis() + UPDATE_CURRENT_TIMER_DELAY;
	}

	unsigned int uiCurrentValue;
	float fCurrentADCVoltage;
	unsigned int uiSampledValuesArray[CURRENT_SAMPLES_AVERAGED_COUNT];

	// Read ADC Values
	for (int x = 0; x < CURRENT_SAMPLES_AVERAGED_COUNT; x++)
	{
		delay(2);
		uiSampledValuesArray[x] = analogRead(CURRENT_ADC_IN);
	}

	// Sort the results array
	Utilities.sortUIntArray(uiSampledValuesArray, CURRENT_SAMPLES_AVERAGED_COUNT);

	// Average the result. ignoring the lowest and highest value
	uiCurrentValue = 0;
	for (int x = CURRENT_SAMPLES_LOPPED; x < CURRENT_SAMPLES_AVERAGED_COUNT - CURRENT_SAMPLES_LOPPED; x++)
	{
		uiCurrentValue = uiCurrentValue + uiSampledValuesArray[x];
	}
	uiCurrentValue = uiCurrentValue / (CURRENT_SAMPLES_AVERAGED_COUNT - (CURRENT_SAMPLES_LOPPED * 2));

	// Transform the Average Current Value into Voltage
	fCurrentADCVoltage = ((float)uiCurrentValue / (float)MAX_CURRENT_ADC_BITS) * (float)5.0;	// Assumes 5V ADC

	// Calculate the Current from the Voltage
	fLastCurrentValue = fCurrentValue;
	fCurrentValue = fCurrentADCVoltage;

	//Serial.print(F("Current="));
	//Serial.print(fCurrentValue);
	//Serial.println(F("V"));

}

/**
 * Update the LED system outputs on a timer basis.
 * 
 * This uses the status of Fault Flags to set the LEDs HIGH or LOW.
 */
void updateDisplay(bool bForceUpdate)
{

	// Check to see Update Display Timer has expired
	if ((ulUpdateDisplayTimer > millis()) && (bForceUpdate == false))
	{
		return;
	}
	else
	{
		// Update next refresh time
		ulUpdateDisplayTimer = millis() + UPDATE_DISPLAY_TIMER_DELAY;
	}

	digitalWrite(ALARM_PUMP_LED_IO, false); // NOT IMPLEMENTED
	digitalWrite(ALARM_UVC_LED_IO, ControllerFaultFlags.bFaultUVC);
	digitalWrite(ALARM_MAX_OVERRUN_TIME_LED_IO, ControllerFaultFlags.bFaultMaxOverrunTime);
	digitalWrite(ALARM_S_TANK_LOW_LED_IO, ControllerFaultFlags.bFaultSTankLow);
	digitalWrite(ALARM_FLOAT_ERROR_LED_IO, ControllerFaultFlags.bFaultLevelSwitches);

}

/**
 * This function keeps the UV-C Lamp turned ON for a period of time after use,
 * to ensure that any water present is disinfected.
 * 
 * The Pump will be turned off first, then after the time delay provided by this function, 
 * the UV-C Lamp turned OFF.
 * 
 * This ensures that any water which has left the Untreated Tank will be disinfected.
 * 
 * If the UV-C Lamp is already OFF, no delay is applied.  This delay only occurs when transitioning
 * from ON to OFF state.
 */
void PreUVCDeactivationDelay(void)
{

	// Check to see if the delay is needed - i.e. the UVC Lamp is turned on.
	if (digitalRead(UVC_OUT_IO) == UVC_LAMP_OFF)
	{
		// No need for delay - lamp already turned off!
		return;
	}

	unsigned int uiDelayCount = 0;

	while (uiDelayCount < PRE_UVC_DEACTIVATION_DELAY_S)
	{

		// Reset Watchdog Timer
		wdt_reset();

		delay(1000);

		// Update Alarm Toggle
		AlarmBuzzerToggle();

		uiDelayCount++;

	}

}
