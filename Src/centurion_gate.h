/*
 * centurion_gate.h
 *
 *  Created on: 11 Aug 2018
 *      Author: janus
 */

#ifndef SRC_CENTURION_GATE_H_
#define SRC_CENTURION_GATE_H_
#include "stm32f1xx_hal.h"

#include "motor_state_change_listener.h"

class CenturionGate
{
	int mIndex;
	uint8_t mSamples[128];
	MotorStateChangeListener *mListener;
	MotorStateChangeListener::eMotorState mState;
	int mErrorIndex;
	MotorStateChangeListener::eMotorState mPrevError[3];

	MotorStateChangeListener::eMotorState checkErrorCode(uint8_t *samples);
	MotorStateChangeListener::eMotorState checkPosition(uint8_t *samples);
	void setState(MotorStateChangeListener::eMotorState state);

public:
	CenturionGate();
	virtual ~CenturionGate();

	void setStateListener(MotorStateChangeListener *listener){ mListener = listener; }

	void appendSample(uint8_t state);

	void run();

	MotorStateChangeListener::eMotorState getState();
	static const char *printState(MotorStateChangeListener::eMotorState state);
};

#endif /* SRC_CENTURION_GATE_H_ */
