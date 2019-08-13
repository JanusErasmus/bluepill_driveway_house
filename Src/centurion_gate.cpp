/*
 * centurion_gate.cpp
 *
 *  Created on: 11 Aug 2018
 *      Author: janus
 */
#include <string.h>
#include <Utils/utils.h>

#include "centurion_gate.h"

CenturionGate::CenturionGate() : mIndex(0),
						mListener(0),
						mState(MotorStateChangeListener::UNKNOWN),
						mErrorIndex(0)
{
}

CenturionGate::~CenturionGate()
{
}

void CenturionGate::appendSample(uint8_t state)
{
	mSamples[mIndex] = state;
	mIndex = (mIndex + 1) % 128;

	//start flashing A.S.A.P
	if((mState == MotorStateChangeListener::OPEN) && !state)
	{
		setState(MotorStateChangeListener::CLOSING);
	}
	if((mState == MotorStateChangeListener::CLOSED) && state)
	{
		setState(MotorStateChangeListener::OPENING);
	}
}

MotorStateChangeListener::eMotorState CenturionGate::getState()
{
	return mState;
}

MotorStateChangeListener::eMotorState CenturionGate::checkErrorCode(uint8_t *samples)
{
	MotorStateChangeListener::eMotorState error = MotorStateChangeListener::UNKNOWN;
	int silence = -1;
	int zeroCount = 0;
	for (int k = 0; k < 128; ++k)
	{
		if(samples[k] == 0)
		{
			zeroCount++;
		}
		else if(zeroCount >= 30) //check if we have 15 zeros before this one (about a 750ms of darkness)
		{
			silence = k;
			break;
		}
		else
			zeroCount = 0;
	}

	if(silence > 0)
	{
		//printf("250-> %d\n", 128 - silence);
		//diag_dump_buf(&lastSeq[silence], 128 - silence);

		uint16_t sample250 = 0;
		int shift = 0;
		for (int k = silence + 5; k < 128; )
		{
			//printf("last[%d] = %02X\n", k, lastSeq[k]);
			if(samples[k] == 1)
				sample250 |= (1 << shift);

			shift++;
			if(shift > 15)
				break;

			k += 10;
		}
		//printf("250: %04X\n", sample250);
		switch(sample250)
		{
		case 0x05: //0b0101 is two flashes
			error = MotorStateChangeListener::MAINS_FAILURE;
			break;
		case 0x15: //0b00010101 is three flashes
			error = MotorStateChangeListener::BATTERY_LOW;
			break;
		default:
			break;
		}
	}

	if(error != MotorStateChangeListener::UNKNOWN)
	{
			mPrevError[mErrorIndex] = error;
			mErrorIndex++;
			if(mErrorIndex > 2)
				mErrorIndex = 0;

			//we need three of the same errors to report it
			bool valid = true;
			for (int k = 0; k < 3; ++k)
			{
				if(error != mPrevError[k])
					valid = false;
			}

			if(!valid)
				error = MotorStateChangeListener::UNKNOWN;
	}
	return error;
}

MotorStateChangeListener::eMotorState CenturionGate::checkPosition(uint8_t *samples)
{
	//align with first transition
	int transition = -1;
	bool thisState = samples[0];
	for (int k = 0; k < 128; k++)
	{
		if(thisState != samples[k])
		{
			transition = k;
			break;
		}
	}

	if(transition > 0)
	{
		//printf("150-> %d\n", 128 - transition);
		//diag_dump_buf(&lastSeq[transition], 128 - transition);

		uint8_t sample150 = 0;
		int shift = 0;
		for (int k = transition + 3; k < 128; )
		{
			if(samples[k] == 1)
				sample150 |= (1 << shift);

			shift++;
			if(shift > 7)
				break;

			k += 6;
		}
		//printf("150: %02X\n", sample150);

		if((sample150 == 0x33) || (sample150 == 0xCC))
			return MotorStateChangeListener::OPENING;

		if((sample150 == 0xAA) || (sample150 == 0x55))
			return MotorStateChangeListener::CLOSING;

		return MotorStateChangeListener::UNKNOWN;
	}

	//the gate is open or close
	{
		bool close = true;
		bool open = true;
		for (int k = 0; k < 128; ++k) {
			if(samples[k] == 0x01)
				close = false;

			if(samples[k] == 0x00)
				open = false;
		}

		if(close && !open)
			return MotorStateChangeListener::CLOSED;

		if(!close && open)
			return MotorStateChangeListener::OPEN;
	}


	return MotorStateChangeListener::UNKNOWN;
}

void CenturionGate::run()
{
	//fill last sequence with last 128 states (25ms x 128 = 3.2s)
	uint8_t lastSeq[128];
	memset(lastSeq, 0, 128);
	int index = mIndex;
	for (int k = 127; k >= 0; k--)
	{
		lastSeq[k] = mSamples[index];
		index--;
		if(index < 0)
			index = 127;
	}

	//check gate position
	MotorStateChangeListener::eMotorState state = checkPosition(lastSeq);

	//if not known, see if it was an error code
	if(state == MotorStateChangeListener::UNKNOWN)
		state = checkErrorCode(lastSeq);

	//only set state when it is known
	if(state != MotorStateChangeListener::UNKNOWN)
		setState(state);

}

void CenturionGate::setState(MotorStateChangeListener::eMotorState state)
{
	if(state == mState)
		return;

	mState = state;
	//printf("Gate state changed %s\n", MotorStateChangeListener::stateToString(state));

	if(mListener)
		mListener->stateChanged(this, state);
}


