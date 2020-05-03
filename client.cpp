#include <iostream>

#include "clientStateMachine.h"
#include "clientStates.h"
#include "FootballGame.h"
#include "client.h"

Client::Client(FootballGame* footballGame, int id, int port, int personId)
{
	mFootballGame = footballGame;

	mId = id;
	mPort = port;
	mPersonId = personId;

	mSentToClient = false;
	printf("client %d listening on port %d\n",id,port);

	mUp = 0;
	mRight = 0;
	mDown = 0;
	mLeft = 0;
	mRotateLeft = 0;
	mRotateRight = 0;

	//joystick
	mJoystickRotation = 0;

        //state machines
        mClientStateMachine = new ClientStateMachine(this);    //setup the state machine

        //player states
        mGlobalClientState = new GlobalClientState();
        mInitClientState = new InitClientState();
        mWaitClientState = new WaitClientState();
        mHumanClientState = new HumanClientState();
        mComputerClientState = new ComputerClientState();

        //set states
        //player states
        mClientStateMachine->setCurrentState(nullptr);
        mClientStateMachine->setPreviousState(nullptr);
        mClientStateMachine->setGlobalState(mGlobalClientState);
        mClientStateMachine->changeState(mInitClientState);

}

void Client::update()
{
        mClientStateMachine->update();
}
