#include <stdio.h>

#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "LPC55S69_cm33_core0.h"
#include "fsl_debug_console.h"
#include "arm_math.h"

#include "lcd.h"

#define MAXSET 100

#define DC_MOTOR_PWM(x) (CTIMER_UpdatePwmDutycycle(CTIMER1_PERIPHERAL, CTIMER1_PWM_PERIOD_CH, CTIMER1_PWM_0_CHANNEL, abs(x)))
#define READ_DC_MOTOR_DATA (GPIO_PinRead(DC_MOTOR_DATA_GPIO, DC_MOTOR_DATA_PORT, DC_MOTOR_DATA_PIN))
#define WRITE_DC_MOTOR_AIN1(x) (GPIO_PinWrite(DC_MOTOR_AIN1_GPIO, DC_MOTOR_AIN1_PORT, DC_MOTOR_AIN1_PIN, x))
#define WRITE_DC_MOTOR_AIN2(x) (GPIO_PinWrite(DC_MOTOR_AIN2_GPIO, DC_MOTOR_AIN2_PORT, DC_MOTOR_AIN2_PIN, x))

#define SET_DC_MOTOR_DIR(a, b) (WRITE_DC_MOTOR_AIN1(a), WRITE_DC_MOTOR_AIN2(b))

#define READ_ENC_INC_DATA (GPIO_PinRead(ENC_INC_DATA_GPIO, ENC_INC_DATA_PORT, ENC_INC_DATA_PIN))

volatile int32_t set = 0, motorPower=0;
volatile int32_t Position=0, oldPosition=0, divPosition=0;

volatile uint8_t divInterval=0;

volatile bool Run=false;
volatile float32_t Speed=0;

uint16_t modeColor=0;

char buff[32];

void Motor_SetPower(int32_t power, bool brake);

void int_dc_motor_pwm(uint32_t flags)
{
	divInterval++;

	if(divInterval>=100)
	{

		//CPR (Cycles per Revolution)  = 24 cycle per
		//PPR (Pulse per Revolution) =  7 (CPR / 4)
		//RR (Reduction Ratio) 1:298
		//interrupt 1Khz = 1ms

		//dp - delta Position (change of position in time)

		// rotation speed in time = dP / (PPR * RR)
		// rotation per 1ms = (dp (1ms)) / (7 * 298)
		// rotation per 1s (1000ms) = ((dp (1ms)) / (7 * 298)) * 1000
		// rotation per 1m (60s) = ((dp (1ms)) / (7 * 298)) * 1000 * 60

		divInterval = 0;
		divPosition = (Position - oldPosition); //dt
		oldPosition = Position;

		Speed = divPosition * 0.034767;
	}

	motorPower = Run ? set : 0;
	Motor_SetPower(motorPower, 1);
}

/* INT_0 callback function for the PINT component */
void int_enc_inc_clk(pint_pin_int_t pintr, uint32_t pmatch_status)
{
	if(READ_ENC_INC_DATA)
	{
		if(set < MAXSET){
			set++;
		}
	}
	else
	{
		if(set > -MAXSET){
			set--;
		}
	}

	PRINTF("%d \r\n", set);
}

/* INT_1 callback function for the PINT component */
void int_enc_inc_sw(pint_pin_int_t pintr, uint32_t pmatch_status)
{
	Run = !Run;

	PRINTF("SW \r\n");
}

/* INT_2 callback function for the PINT component */
void int_dc_motor_clk(pint_pin_int_t pintr, uint32_t pmatch_status)
{
	if(READ_DC_MOTOR_DATA){
		Position++;
	}
	else{
		Position--;
	}
}

void Motor_SetPower(int32_t power, bool brake)
{
	if(power > 100){
		power = 100;
	}

	if(power < -100){
		power = -100;
	}

	if(power>0)
	{
		SET_DC_MOTOR_DIR(0, 1);
	}
	else if(power<0)
	{
		SET_DC_MOTOR_DIR(1, 0);
	}
	else
	{
		SET_DC_MOTOR_DIR(brake, brake);
	}

	DC_MOTOR_PWM(power);
}

/*
 * @brief Application entry point.
 */

int main(void)
{
	/* Init board hardware. */

	BOARD_InitBootPins();
	BOARD_InitBootClocks();
	BOARD_InitBootPeripherals();

	#ifndef BOARD_INIT_DEBUG_CONSOLE_PERIPHERAL
	/* Init FSL debug console. */
	BOARD_InitDebugConsole();
	#endif

	LCD_Init(FLEXCOMM8_PERIPHERAL);

	PRINTF("Start \r\n");

	while(1)
	{
		modeColor = Run ? _GREEN : _RED ;

		LCD_Clear(_BLACK);

		sprintf(buff, "Set: %d", set);
		LCD_Puts(10, 10, buff, modeColor);

		sprintf(buff, "Power: %d", motorPower);
		LCD_Puts(10, 20, buff, modeColor);

		sprintf(buff, "Tacho: %d", (int)Speed);
		LCD_Puts(10, 30, buff, modeColor);

		LCD_GramRefresh();
	}

	return 0 ;
}
