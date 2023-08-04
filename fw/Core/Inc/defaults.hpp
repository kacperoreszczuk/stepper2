#ifndef INC_DEFAULTS_HPP_
#define INC_DEFAULTS_HPP_

#define TIMER_CLOCK_FREQ 137500000
#define NXT_SUPERRESOLUTION_FACTOR 10
#define LOWEST_NXT_PERIOD 550  // 250 kHz, fastest possible switching speed
#define IDLE_NXT_PERIOD 55000  // 2.5 kHz, idle time delay if NXT tick must be further in time than MAX_NXT_PERIOD

static uint16_t const CONTROL_LOOP_FREQ = 500;
static uint16_t const MICROSTEPS = 64;
static float const DEFAULT_STEP = 0.005f;
static float const DEFAULT_ACCELERATION_TIME_INV = 4.0f;  // 0.25s
static float const DEFAULT_MAX_VELOCITY = 10.0f;
static float const DEFAULT_STANDARD_VELOCITY = 1.0f;
static float const DEFAULT_HYSTERESIS = 0.0f;
static uint16_t const DEFAULT_MOTOR_CURRENT = 300;  // mA
static uint32_t const BACKUP_SRAM_TOKEN = 0xdeadbeef;

static float const CONTROL_LOOP_PERIOD = 1.0f / CONTROL_LOOP_FREQ; // to avoid slow float division later
static float const MICROSTEP_SIZE = 1.0f / MICROSTEPS; // to avoid slow float division later
static float const LIMIT_EXP_AVG_ALPHA = 0.1f; // Exponential moving average - alpha parameter
static float const LIMIT_THRESHOLD = 0.2f; // threshold for low state (1-x for high state)


//// time step between minimal step loop iteration.
//// ... = [timer interval] * NO_OF_MOTORS
//static uint32_t const TICK_PERIOD_NS = (2600 * NO_OF_MOTORS);


#endif /* INC_DEFAULTS_HPP_ */
