/*
 * gate_lights.cpp
 *
 *  Created on: 11 Aug 2018
 *      Author: janus
 */

#include "driveway_lights.h"

#include "stm32f1xx_hal.h"

#define REPEAT_STATE 3

typedef struct
{
	GPIO_TypeDef *port;
	uint32_t pin;
}sGatePins;

static const sGatePins gatePins[] =
{
//		{GATE_OUT0_Port, GATE_OUT0_Pin},
//		{GATE_OUT1_Port, GATE_OUT1_Pin},
//		{GATE_OUT2_Port, GATE_OUT2_Pin},
//		{GATE_OUT3_Port, GATE_OUT3_Pin},
//		{GATE_OUT4_Port, GATE_OUT4_Pin},
		{0,0}
};

void setPin(uint8_t index, GPIO_PinState state)
{
	HAL_GPIO_WritePin(gatePins[index].port, gatePins[index].pin, state);
}

DrivewayLights::DrivewayLights()
{
	mState = OFF;
	mFlashFlag = 0;
	mRepeat = 0;
	mTick = 0;
}

DrivewayLights::~DrivewayLights()
{
}

void setAll()
{
	for (int k = 0; k < 5; ++k)
	{
		setPin(k, GPIO_PIN_SET);
	}
}

void resetAll()
{
	for (int k = 0; k < 5; ++k)
	{
		setPin(k, GPIO_PIN_RESET);
	}
}

void DrivewayLights::run()
{
	//sample gate light every 400ms
	static int checkGate = 0;
	int tick = HAL_GetTick();
	if(tick < (checkGate + 400))
		return;
	checkGate = tick;

	switch(mState)
	{
	case OFF:
		mFlashFlag = 0;
		resetAll();
		break;
	case ON:
		mFlashFlag = 0;
		setAll();
		break;
	case FLASH_ON_OFF:
		if(mFlashFlag)
		{
			mFlashFlag = 0;
			setAll();
		}
		else
		{
			mFlashFlag = 1;
			resetAll();
		}
		break;
	case HOUSE_TO_STREET_OPENING:
		if(mRepeat < REPEAT_STATE)
			setPin(mFlashFlag, GPIO_PIN_RESET);
		mFlashFlag++;
		if(mFlashFlag > 4)
		{
			mFlashFlag = 0;
			if(mRepeat++ > REPEAT_STATE)
			{
				mRepeat = 0;
				mState = ON;
				break;
			}
		}
		setPin(mFlashFlag, GPIO_PIN_SET);
		break;
	case HOUSE_TO_STREET_CLOSING:
		setPin(mFlashFlag, GPIO_PIN_SET);
		mFlashFlag++;
		if(mFlashFlag > 4)
		{
			mFlashFlag = 0;

			if(mRepeat++ > REPEAT_STATE)
			{
				mRepeat = 0;
				mState = SWITCH_OFF;
				break;
			}
		}
		if(mRepeat < REPEAT_STATE)
			setPin(mFlashFlag, GPIO_PIN_RESET);
		break;
	case STREET_TO_HOUSE_OPENING:
		if(mRepeat < REPEAT_STATE)
			setPin(mFlashFlag, GPIO_PIN_RESET);
		mFlashFlag--;
		if(mFlashFlag < 0)
		{
			mFlashFlag = 4;
			if(mRepeat++ > REPEAT_STATE)
			{
				mRepeat = 0;
				mState = ON;
				break;
			}
		}
		setPin(mFlashFlag, GPIO_PIN_SET);
		break;
	case STREET_TO_HOUSE_CLOSING:
		setPin(mFlashFlag, GPIO_PIN_SET);
		mFlashFlag--;
		if(mFlashFlag < 0)
		{
			mFlashFlag = 4;
			if(mRepeat++ > REPEAT_STATE)
			{
				mRepeat = 0;
				mState = SWITCH_OFF;
				break;
			}
		}
		if(mRepeat < REPEAT_STATE)
			setPin(mFlashFlag, GPIO_PIN_RESET);
		break;
	case SWITCH_OFF:
		if(mFlashFlag == 0) //switch all lights on
		{
			setAll();
		}
		if(mFlashFlag == 101) //switch outer lights off to middle after 30s
		{
			setPin(0, GPIO_PIN_RESET);
			setPin(4, GPIO_PIN_RESET);
		}
		if(mFlashFlag == 102) //switch outer lights off to middle
		{
			setPin(1, GPIO_PIN_RESET);
			setPin(3, GPIO_PIN_RESET);
		}
		if(mFlashFlag == 103) //switch outer lights off to middle
		{
			setPin(2, GPIO_PIN_RESET);
			mState = OFF;
		}
		mFlashFlag++;
		break;
	case ON_PULSE:
		if(mFlashFlag == 0) //switch outer lights on to middle
		{
			setPin(0, GPIO_PIN_SET);
			setPin(4, GPIO_PIN_SET);
		}
		if(mFlashFlag == 1) //switch outer lights off to middle
		{
			setPin(1, GPIO_PIN_SET);
			setPin(3, GPIO_PIN_SET);
		}
		if(mFlashFlag == 2) //switch outer lights off to middle
		{
			setPin(2, GPIO_PIN_SET);
			mState = SWITCH_OFF;
		}
		mFlashFlag++;
		break;
	}
}

extern bool isDay();

void DrivewayLights::set(eLightState state)
{
	//only switch lights when it is night
	if(isDay())
	{
		printf("It is day\n");
		mState = OFF;
		return;
	}

	//when light where busy switching off
	if(mState == SWITCH_OFF)
		mFlashFlag = 0;

	//do not start flashing lights when they are on
	if(mState == ON)
	{
		if((state == HOUSE_TO_STREET_OPENING) || (state == STREET_TO_HOUSE_OPENING))
			return;
	}

	mState = state;
}

void DrivewayLights::switchOn()
{
	mState = ON_PULSE;
	mFlashFlag = 0;
}
