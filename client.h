#ifndef CLIENT_H
#define CLIENT_H

#include "2D/Vector2D.h"

class SoccerPitch;

class ClientStateMachine;

class GlobalClientState;
class InitClientState;
class WaitClientState;
class HumanClientState;
class ComputerClientState;

class Client
{
	public:
		Client(SoccerPitch* soccerPitch, int id, int port, int personId);

		void update();

		SoccerPitch* mSoccerPitch;

		int mId;
		int mPort;
		int mPersonId;

		bool mSentToClient;

		int mUp;
		int mRight;
		int mDown;
		int mLeft;
		int mRotateLeft;
		int mRotateRight;

		//joystick
		Vector2D mJoystickDirection;
		double mJoystickRotation;

		//states

		//states machines
                ClientStateMachine* mClientStateMachine;

                //player states
                GlobalClientState* mGlobalClientState;
                InitClientState* mInitClientState;
                WaitClientState* mWaitClientState;
                HumanClientState* mHumanClientState;
                ComputerClientState* mComputerClientState;


	private:
};

#endif
