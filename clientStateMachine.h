#ifndef CLIENTSTATEMACHINE_H
#define CLIENTSTATEMACHINE_H

class ClientState;
class Client;

class ClientStateMachine
{

	public:
		ClientStateMachine(Client* mOwner);

		Client* mOwner;

		ClientState* mGlobalState;
		ClientState* mCurrentState;
		ClientState* mPreviousState;

		void setCurrentState(ClientState* clientState);
		void setPreviousState(ClientState* clientState);
		void setGlobalState(ClientState* clientState);
		ClientState* getCurrentState();

		void update();

		void changeState(ClientState* clientState);


};

#endif

