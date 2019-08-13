/*
 * motor_state_change_listener.h
 *
 *  Created on: 12 Aug 2018
 *      Author: janus
 */

#ifndef INC_MOTOR_STATE_CHANGE_LISTENER_H_
#define INC_MOTOR_STATE_CHANGE_LISTENER_H_

#include "Utils/utils.h"

class CenturionGate;

class MotorStateChangeListener
{
public:
	enum eMotorState
	{
		UNKNOWN = 0,
		CLOSED,
		OPEN,
		OPENING,
		CLOSING,
		COURTESY_ON,
		MAINS_FAILURE,
		BATTERY_LOW,
		OBSTRUCTION
	};

	MotorStateChangeListener(){};
	virtual ~MotorStateChangeListener(){};

	virtual void stateChanged(CenturionGate *gate, eMotorState state) = 0;

	static const char *stateToString(eMotorState state)
	{
		switch(state)
		{
		case UNKNOWN:
			return "UNKNOWN";
		break;
		case CLOSED:
			return GREEN("CLOSED");
		break;
		case OPEN:
			return RED("OPEN");
		break;
		case OPENING:
			return "OPENING";
		break;
		case CLOSING:
			return "CLOSING";
		break;
		case COURTESY_ON:
			return "COURTESY_ON";
		break;
		case MAINS_FAILURE:
			return CYAN("MAINS_FAILURE");
		break;
		case BATTERY_LOW:
			return "BATTERY_LOW";
		break;
		case OBSTRUCTION:
			return "OBSTRUCTION";
		break;
		}

		return "";
	}
};



#endif /* INC_MOTOR_STATE_CHANGE_LISTENER_H_ */
