#ifndef INC_DEFAULTS_HPP_
#define INC_DEFAULTS_HPP_

#define TIMER_CLOCK_FREQ 137500000
#define NXT_TIME_PS_PER_SEC 1000000000000ll
#define TICK_PERIOD_PS 6400000

static uint16_t const CONTROL_LOOP_FREQ = 500;
static uint16_t const MICROSTEPS = 64;
static float const DEFAULT_STEP = 1.0f;
static float const DEFAULT_ACCELERATION_TIME_INV = 4.0f;  // 0.25s
static float const DEFAULT_MAX_VELOCITY = 2000.0f;
static float const DEFAULT_STANDARD_VELOCITY = 200.0f;
static float const DEFAULT_HYSTERESIS = 0.0f;
static uint16_t const DEFAULT_MOTOR_CURRENT = 300;  // mA
static uint32_t const BACKUP_SRAM_TOKEN = 0xdeadbeef;

static float const CONTROL_LOOP_PERIOD = 1.0f / CONTROL_LOOP_FREQ; // to avoid slow float division later
static float const MICROSTEP_SIZE = 1.0f / MICROSTEPS; // to avoid slow float division later

static int32_t const LIMIT_POWER_2 = 4;
static int32_t const LIMIT_MAX = 10000;
static int32_t const LIMIT_NEW_COMP = (LIMIT_MAX << LIMIT_POWER_2);
static int32_t const LIMIT_OLD_FRACTION = ((2 << LIMIT_POWER_2) - 1);
static int32_t const LIMIT_THRS_LOW = 3000;
static int32_t const LIMIT_THRS_HIGH = 7000;


//// time step between minimal step loop iteration.
//// ... = [timer interval] * NO_OF_MOTORS
//static uint32_t const TICK_PERIOD_NS = (2600 * NO_OF_MOTORS);


#endif /* INC_DEFAULTS_HPP_ */
