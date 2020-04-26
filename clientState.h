#ifndef CLIENTSTATE_H
#define CLIENTSTATE_H

#include "clientState.h"

class Client;

class ClientState
{
	public: 
		ClientState();
		virtual void enter(Client* player);
		virtual void execute(Client* player);
		virtual void exit(Client* player);

	private:


};

#endif
