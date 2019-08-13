/*
 * driveway_motors.cpp
 *
 *  Created on: 12 Aug 2018
 *      Author: janus
 */

#include "driveway_motors.h"

DrivewayMotors::DrivewayMotors(DrivewayLights *lights,
								CenturionGate *street_gate,
								CenturionGate *house_gate) :
								mStreetGate(street_gate),
								mHouseGate(house_gate),
								mLights(lights)
{
	if(mStreetGate)
		mStreetGate->setStateListener(this);

	if(mHouseGate)
		mHouseGate->setStateListener(this);
}

DrivewayMotors::~DrivewayMotors()
{
}

void DrivewayMotors::run()
{
    //sample gate light every 3.2 seconds second
    static int checkGate = 0;
    int tick = HAL_GetTick();
    if(tick < (checkGate + 3200))
    	return;

  	  checkGate = tick;

  	if(mStreetGate)
  		mStreetGate->run();

	if(mHouseGate)
		mHouseGate->run();
}

extern void reportNow();

void DrivewayMotors::stateChanged(CenturionGate* gate, eMotorState state)
{
	if(mStreetGate)
	{
		if(gate == mStreetGate)
		{
			printf("StreetGate: %s\n", stateToString(state));

			if(mLights)
			{
				if((state == OPENING) || (state == OPEN))
					mLights->set(DrivewayLights::STREET_TO_HOUSE_OPENING);
				if((state == CLOSING) || (state == CLOSED))
					mLights->set(DrivewayLights::STREET_TO_HOUSE_CLOSING);
			}
		}
	}

	if(mHouseGate)
	{
		if(gate == mHouseGate)
		{
			printf("HouseGate: %s\n", stateToString(state));

			if(mLights)
			{
				if((state == OPENING) || (state == OPEN))
					mLights->set(DrivewayLights::HOUSE_TO_STREET_OPENING);
				if((state == CLOSING) || (state == CLOSED))
					mLights->set(DrivewayLights::HOUSE_TO_STREET_CLOSING);
			}
		}
	}
	//do not report moving states
	//if(!((state == CLOSING) || (state == OPENING)))
	reportNow();
}
