/*
 * gate_lights.h
 *
 *  Created on: 11 Aug 2018
 *      Author: janus
 */

#ifndef SRC_DRIVEWAY_LIGHTS_H_
#define SRC_DRIVEWAY_LIGHTS_H_

class DrivewayLights {
public:
	enum eLightState
	{
		ON,
		OFF,
		FLASH_ON_OFF,
		HOUSE_TO_STREET_OPENING,
		HOUSE_TO_STREET_CLOSING,
		STREET_TO_HOUSE_OPENING,
		STREET_TO_HOUSE_CLOSING,
		SWITCH_OFF,
		ON_PULSE
	};
private:
	eLightState mState;
	int mFlashFlag;
	int mRepeat;
	int mTick;
public:
	DrivewayLights();
	virtual ~DrivewayLights();

	void run();
	void set(eLightState state);

	void switchOn();
};

#endif /* SRC_DRIVEWAY_LIGHTS_H_ */
