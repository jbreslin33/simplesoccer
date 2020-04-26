#include "clientStateMachine.h"
#include "clientState.h"
#include <iostream>

ClientStateMachine::ClientStateMachine(Client* owner)
{
	mOwner = owner;

	mGlobalState = nullptr;
	mCurrentState = nullptr;
	mPreviousState = nullptr;
}

void ClientStateMachine::update()
{
	if (mGlobalState)
	{
		mGlobalState->execute(mOwner);
	}	

	if (mCurrentState)
	{
		mCurrentState->execute(mOwner);
	}	
}

ClientState* ClientStateMachine::getCurrentState()
{
	return mCurrentState;
}

void ClientStateMachine::setCurrentState(ClientState* clientState)
{
	mCurrentState = clientState;
}

void ClientStateMachine::setPreviousState(ClientState* clientState)
{
	mPreviousState = clientState;
}

void ClientStateMachine::setGlobalState(ClientState* clientState)
{
	mGlobalState = clientState;
}

void ClientStateMachine::changeState(ClientState* newState)
{
	if (mCurrentState == nullptr)
	{

	}
	else
	{
		mPreviousState = mCurrentState;

        	if(mCurrentState)
        	{
        		mCurrentState->exit(mOwner);
        	}
	}

        mCurrentState = newState;

        if(mCurrentState)
        {
        	mCurrentState->enter(mOwner);
	}
}

