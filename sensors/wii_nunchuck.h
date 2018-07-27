/* This code is made available for anyone to use but
 * please leave this header intact.
 *
 * Communication codes and decryption method provided by:
 * http://wiibrew.org/wiki/Wiimote/Extension_Controllers#Nunchuk
 *
 * This library was produced by Sean D. Cherbone (scherbone@gmail.com)
 * Enjoy!
 */

#include <avr/twi_utils.h>

//**************************USER AREA***************************

/* The const values below correspond to unique controller positions
 * as returned by the wii_nunchuck_get methods. Compare these values
 * to those returned by the stated get methods. Use these to find
 * the percentage the controller is moved along the given axis.
 *
 * The acceleration axis are as follows:
 *                    
 *                         +Z
 *                         /\
 *                          |
 *                          |
 *                        |000|
 *            _____________|0|_____
 *           /                      \0
 * -Y <---- /           > X <       /0 ----> +Y
 *          \       /-----------\__/
 *           \_____/        |
 *                          |
 *                         \/
 *                         -Z
 *
 * +X is out of screen, -X is into screen.
 *
 * The TWI interface on an Atmega1284 can be used without
 * additional/external pull-up resistors.
 */

// Neutral, full-left and full-right acceleration values along the X-axis
extern const unsigned short WII_NUNCHUCK_MID_ACCELX;
extern const unsigned short WII_NUNCHUCK_ACCELX_FULL_LEFT;
extern const unsigned short WII_NUNCHUCK_ACCELX_FULL_RIGHT;

// Neutral, full-left and full-right acceleration values along the Y-axis
extern const unsigned short WII_NUNCHUCK_MID_ACCELY;
extern const unsigned short WII_NUNCHUCK_ACCELY_FULL_BACK;
extern const unsigned short WII_NUNCHUCK_ACCELY_FULL_FORWARD;

// Neutral, full-left and full-right acceleration values along the Z-axis
extern const unsigned short WII_NUNCHUCK_MID_ACCELZ;
extern const unsigned short WII_NUNCHUCK_ACCELZ_FULL_UP;
extern const unsigned short WII_NUNCHUCK_ACCELZ_FULL_DOWN;

// Center, full-left and full-right values for the joystick X-axis
extern const unsigned char WII_NUNCHUCK_MID_JOYX;
extern const unsigned char WII_NUNCHUCK_JOYX_FULL_LEFT;
extern const unsigned char WII_NUNCHUCK_JOYX_FULL_RIGHT;

// Center, full-left and full-right values for the joystick Y-axis
extern const unsigned char WII_NUNCHUCK_MID_JOYY;
extern const unsigned char WII_NUNCHUCK_JOYY_FULL_UP;
extern const unsigned char WII_NUNCHUCK_JOYY_FULL_DOWN;

// The voltage that will be used to communicate with the nunchuck
enum WII_NUNCHUCK_VOLTAGES { WII_NUNCHUCK_5V, WII_NUNCHUCK_3_3V };

// Expected clock frequency values for TWI communication
enum WII_NUNCHUCK_CLOCK { _8MHz, _16MHz, _20MHz };

/** Initialize the TWI interface to work with the nunchuck.
 *  @param clockFrequency	WII_NUNCHUCK_BLOCK enum value to indicate the uC clock speed.
 *  @param voltage			WII_NUNCHUCK_VOLTAGES enum value.
 *  @return					1 for successful init, 0 for failed init
 */
unsigned signed char wii_nunchuck_init_twi(int clockFrequency, int voltage);

/** Initialize the nunchuck response interpreter so it can properly convert
 *  response values to real values.
 *  @param usingNintendoBrand	Is this a Nintendo manufactured device? 0 == false, all other == true.
 *  @return						1 for successful init, 0 for failed init
 */
unsigned signed char wii_nunchuck_init(unsigned char usingNintendoBrand);

/** Sends the first read request to the nunchuck. The first
 *  request takes longer than updates.
 *  @return	1 for successful start request sent, 0 for failure
 */
unsigned signed char wii_nunchuck_start_read();

/** Set the controller's current position and orientation as the neutral/0 position.
 *  Give the read process time to complete then retrieve the results using the get methods.
 *  @return	1 for success, 0 for failure
 */
unsigned signed char wii_nunchuck_calibrate();

/** Initiate another reading after the initial "start" read.
 *  Updates are faster than the initial read. Give the read process
 *  time to complete then retrieve the results using the get methods.
 *  @return	1 for success, 0 for failure
 */
unsigned signed char wii_nunchuck_update();

/** After a read or update, get the joystick X axis position.
 *  Compare against the appropriate JOYX const values above.
 *  @return	Position of the joystick along its X axis
 */
char wii_nunchuck_get_joyX();

/** After a read or update, get the joystick Y axis position.
 *  Compare against the appropriate JOYY const values above.
 *  @return	Position of the joystick along its Y axis
 */
char wii_nunchuck_get_joyY();

/** After a read or update, get the C button press value.
 *  @return	1 == pressed, 0 == released
 */
unsigned char wii_nunchuck_get_btnC();

/** After a read or update, get the Z button press value.
 *  @return	1 == pressed, 0 == released
 */
unsigned char wii_nunchuck_get_btnZ();

/** After a read or update, get the acceleration along the controller's X axis.
 *  Compare against the appropriate ACCELX const values above.
 *  @return	Acceleration of the controller along its X axis
 */
short wii_nunchuck_get_accelX();

/** After a read or update, get the acceleration along the controller's Y axis.
 *  Compare against the appropriate ACCELY const values above.
 *  @return	Acceleration of the controller along its Y axis
 */
short wii_nunchuck_get_accelY();

/** After a read or update, get the acceleration along the controller's Z axis.
 *  Compare against the appropriate ACCELZ const values above.
 *  @return	Acceleration of the controller along its Z axis
 */
short wii_nunchuck_get_accelZ();
//****************************END USER AREA**************************************

const unsigned short WII_NUNCHUCK_MID_ACCELX = 512;
const unsigned short WII_NUNCHUCK_ACCELX_FULL_LEFT = 300;
const unsigned short WII_NUNCHUCK_ACCELX_FULL_RIGHT = 740;

const unsigned short WII_NUNCHUCK_MID_ACCELY = 512;
const unsigned short WII_NUNCHUCK_ACCELY_FULL_BACK = 280;
const unsigned short WII_NUNCHUCK_ACCELY_FULL_FORWARD = 720;

const unsigned short WII_NUNCHUCK_MID_ACCELZ = 512;
const unsigned short WII_NUNCHUCK_ACCELZ_FULL_UP = 760;
const unsigned short WII_NUNCHUCK_ACCELZ_FULL_DOWN = 320;

const unsigned char WII_NUNCHUCK_MID_JOYX = 128;
const unsigned char WII_NUNCHUCK_JOYX_FULL_LEFT = 35;
const unsigned char WII_NUNCHUCK_JOYX_FULL_RIGHT = 228;

const unsigned char WII_NUNCHUCK_MID_JOYY = 128;
const unsigned char WII_NUNCHUCK_JOYY_FULL_UP = 220;
const unsigned char WII_NUNCHUCK_JOYY_FULL_DOWN = 27;

const unsigned char WII_NUNCHUCK_ADDRESS = 0x52;
const unsigned char WII_NUNCHUCK_DECODE_KEY = 0x17;
const unsigned char WII_NUNCHUCK_INIT_CODE_1 = 0x40;
const unsigned char WII_NUNCHUCK_INIT_CODE_2 = 0x00;
const unsigned char WII_NUNCHUCK_INIT_CODE_ALT_1 = 0xF0;
const unsigned char WII_NUNCHUCK_INIT_CODE_ALT_2 = 0x55;
const unsigned char WII_NUNCHUCK_INIT_CODE_ALT_3 = 0xFB;
const unsigned char WII_NUNCHUCK_INIT_CODE_ALT_4 = 0x00;
const unsigned char WII_NUNCHUCK_INIT_READ = 0x00;

// GLOBAL VARIABLES FOR LIBRARY FUNCTION ACCESS ONLY
static unsigned char wii_nunchuck_usingNintendoNunchuck = 1;

static unsigned char wii_nunchuck_0joyX = 128; //WII_NUNCHUCK_MID_JOYX;
static unsigned char wii_nunchuck_0joyY = 128; //WII_NUNCHUCK_MID_JOYY;
static unsigned short wii_nunchuck_0accelX = 512; //WII_NUNCHUCK_MID_ACCELX;
static unsigned short wii_nunchuck_0accelY = 512; //WII_NUNCHUCK_MID_ACCELY;
static unsigned short wii_nunchuck_0accelZ = 512; //WII_NUNCHUCK_ACCELZ_FULL_UP;

/* Data order (bytes 0 - 5):
 *	0: MSB [AccelZ 1, AccelZ 0, AccelY 1, AccelY 0, AccelX 1, AccelX 0, c-button, z-button] LSB
 *	1: Joystick X
 *	2: Joystick Y
 *	3: Accelerometer X (bits 9 to 2 of 10)
 *	4: Accelerometer Y (bits 9 to 2 of 10)
 *	5: Accelerometer Z (bits 9 to 2 of 10)
 */
 
static unsigned short wii_nunchuck_data_buf[6];

//-----------------FUNCTION DEFINITIONS---------------------

unsigned char wii_nunchuck_decrypt_data(unsigned char data) {
	return (data ^ WII_NUNCHUCK_DECODE_KEY) + WII_NUNCHUCK_DECODE_KEY;
}

unsigned char wii_nunchuck_init_twi(int clockFrequency, int voltage) {
	if(voltage != WII_NUNCHUCK_5V && voltage != WII_NUNCHUCK_3_3V)
		return 0;
	//static const unsigned long MIN_WII_CLK = 100000; // 100 kHz for 3.3V
	//static const unsigned long MAX_WII_CLK = 400000; // 400 kHz for 5V

	// How did I come up with these bit rate values? Well they work...
	if(clockFrequency == _8MHz) {
		if(voltage == WII_NUNCHUCK_5V)
			twi_init(TWI_PRESCALER_ONE, 152);
		else
			twi_init(TWI_PRESCALER_4TH, 158);
	} else if(clockFrequency == _16MHz) {
		if(voltage == WII_NUNCHUCK_5V)
			twi_init(TWI_PRESCALER_4TH, 76);
		else
			twi_init(TWI_PRESCALER_16TH, 79);
	} else if(clockFrequency == _20MHz) {
		if(voltage == WII_NUNCHUCK_5V)
			twi_init(TWI_PRESCALER_4TH, 95);
		else
			twi_init(TWI_PRESCALER_64TH, 25);
	} else {
		return 0;
	}

	return 1;
}

unsigned char wii_nunchuck_init(unsigned char usingNintendoBrand) {
	twi_sendStart();

	unsigned char initCode1, initCode2;
	if(usingNintendoBrand) {
		initCode1 = WII_NUNCHUCK_INIT_CODE_1;
		initCode2 = WII_NUNCHUCK_INIT_CODE_2;
		wii_nunchuck_usingNintendoNunchuck = 1;
	} else {
		initCode1 = WII_NUNCHUCK_INIT_CODE_ALT_1;
		initCode2 = WII_NUNCHUCK_INIT_CODE_ALT_2;
		wii_nunchuck_usingNintendoNunchuck = 0;
	}
	
	twi_waitOnBusy();
	if(!twi_confirmStatus(TWI_MSTR_STAT_START_TRANSMITTED))
		return 0;
	twi_addressSlave(WII_NUNCHUCK_ADDRESS, TWI_WRITE);
	twi_waitOnBusy();
	if(!twi_confirmStatus(TWI_MSTR_STAT_SLA_W_ACK))
		return 0;
	twi_transmitUchar(initCode1);
	twi_waitOnBusy();
	if(!twi_confirmStatus(TWI_MSTR_STAT_DATA_SEND_ACK))
		return 0;
	twi_transmitUchar(initCode2);
	twi_waitOnBusy();
	if(!twi_confirmStatus(TWI_MSTR_STAT_DATA_SEND_ACK))
		return 0;
	twi_sendStop();
	
	if(!usingNintendoBrand) {
		twi_sendStart();
		twi_waitOnBusy();
		if(!twi_confirmStatus(TWI_MSTR_STAT_START_TRANSMITTED))
			return 0;
		twi_addressSlave(WII_NUNCHUCK_ADDRESS, TWI_WRITE);
		twi_waitOnBusy();
		if(!twi_confirmStatus(TWI_MSTR_STAT_SLA_W_ACK))
			return 0;
		twi_transmitUchar(WII_NUNCHUCK_INIT_CODE_ALT_3);
		twi_waitOnBusy();
		if(!twi_confirmStatus(TWI_MSTR_STAT_DATA_SEND_ACK))
			return 0;
		twi_transmitUchar(WII_NUNCHUCK_INIT_CODE_ALT_4);
		twi_waitOnBusy();
		if(!twi_confirmStatus(TWI_MSTR_STAT_DATA_SEND_ACK))
			return 0;
		twi_sendStop();
	}
	return 1;
}

unsigned char wii_nunchuck_calibrate() {
	if(!wii_nunchuck_update())
		return 0;
	wii_nunchuck_0joyX = wii_nunchuck_data_buf[0];
	wii_nunchuck_0joyY = wii_nunchuck_data_buf[1];
	wii_nunchuck_0accelX = ((wii_nunchuck_data_buf[2] << 2) | ((wii_nunchuck_data_buf[5] >> 2) & 0x03));
	wii_nunchuck_0accelY = ((wii_nunchuck_data_buf[3] << 2) | ((wii_nunchuck_data_buf[5] >> 4) & 0x03));
	wii_nunchuck_0accelZ = ((wii_nunchuck_data_buf[4] << 2) | ((wii_nunchuck_data_buf[5] >> 6) & 0x03));
	return 1;
}

unsigned char wii_nunchuck_start_read() {
	twi_sendStart();
	twi_waitOnBusy();
	if(!twi_confirmStatus(TWI_MSTR_STAT_START_TRANSMITTED))
		return 0;
	twi_addressSlave(WII_NUNCHUCK_ADDRESS, TWI_WRITE);
	twi_waitOnBusy();
	if(!twi_confirmStatus(TWI_MSTR_STAT_SLA_W_ACK))
		return 0;
	twi_transmitUchar(WII_NUNCHUCK_INIT_READ);
	twi_waitOnBusy();
	if(!twi_confirmStatus(TWI_MSTR_STAT_DATA_SEND_ACK))
		return 0;
	twi_sendStop();
	return 1;
}

unsigned char wii_nunchuck_update() {
	twi_sendStart();
	twi_waitOnBusy();
	if(!twi_confirmStatus(TWI_MSTR_STAT_START_TRANSMITTED))
		return 0;
	twi_addressSlave(WII_NUNCHUCK_ADDRESS, TWI_READ);
	twi_waitOnBusy();
	if(!twi_confirmStatus(TWI_MSTR_STAT_SLA_R_ACK))
		return 0;
	twi_get_slvTransmitterStatus();
	twi_waitOnBusy();
	//if(!twi_confirmStatus(TWI_SLV_STAT_SLA_R_ACK))
	//	return 0;
	
	unsigned char i;
	unsigned char dat;
	for(i = 0; i < 6; ++i) {
		if(i == 5)
			dat = twi_receiveNack();
		else
			dat = twi_receiveAck();
		if(wii_nunchuck_usingNintendoNunchuck)
			dat = wii_nunchuck_decrypt_data(dat);
		wii_nunchuck_data_buf[i] = dat;
		twi_waitOnBusy();
	}
	
	twi_sendStop();
	return 1;
}

char wii_nunchuck_get_joyX() {
	return wii_nunchuck_data_buf[0] - wii_nunchuck_0joyX;
}

char wii_nunchuck_get_joyY() {
	return wii_nunchuck_data_buf[1] - wii_nunchuck_0joyY;
}

unsigned char wii_nunchuck_get_btnC() {
	return !((wii_nunchuck_data_buf[5] >> 1) & 0x01);
}

unsigned char wii_nunchuck_get_btnZ() {
	return !(wii_nunchuck_data_buf[5] & 0x01);
}

short wii_nunchuck_get_accelX() {
	return ((wii_nunchuck_data_buf[2] << 2) | ((wii_nunchuck_data_buf[5] >> 2) & 0x03)) - wii_nunchuck_0accelX;
}

short wii_nunchuck_get_accelY() {
	return ((wii_nunchuck_data_buf[3] << 2) | ((wii_nunchuck_data_buf[5] >> 4) & 0x03)) - wii_nunchuck_0accelY;
}

short wii_nunchuck_get_accelZ() {
	return ((wii_nunchuck_data_buf[4] << 2) | ((wii_nunchuck_data_buf[5] >> 6) & 0x03)) - wii_nunchuck_0accelZ;
}
