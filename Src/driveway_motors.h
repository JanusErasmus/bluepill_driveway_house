/*
 * driveway_motors.h
 *
 *  Created on: 12 Aug 2018
 *      Author: janus
 */

#ifndef SRC_DRIVEWAY_MOTORS_H_
#define SRC_DRIVEWAY_MOTORS_H_
#include "centurion_gate.h"
#include "driveway_lights.h"

class DrivewayMotors : public MotorStateChangeListener
{
	CenturionGate *mStreetGate;
	CenturionGate *mHouseGate;
	DrivewayLights *mLights;

public:
	DrivewayMotors(DrivewayLights *lights,
					CenturionGate *street_gate,
					CenturionGate *house_gate);
	virtual ~DrivewayMotors();

	void run();

	void stateChanged(CenturionGate *gate, eMotorState state);
};

#endif /* SRC_DRIVEWAY_MOTORS_H_ */
