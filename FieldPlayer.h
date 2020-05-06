#ifndef FIELDPLAYER_H
#define FIELDPLAYER_H
//------------------------------------------------------------------------
//
//  Name:   FieldPlayer.h
//
//  Desc:   Derived from a PlayerBase, this class encapsulates a player
//          capable of moving around a soccer game, kicking, dribbling,
//          shooting etc
//
//  Author: Mat Buckland 2003 (fup@ai-junkie.com)
//
//------------------------------------------------------------------------
#include <vector>
#include <string>
#include <algorithm>
#include <cassert>

#include "FieldPlayerStates.h"
#include "2D/Vector2D.h"
#include "FSM/StateMachine.h"
#include "PlayerBase.h"
#include "FSM/StateMachine.h"

class CSteeringBehavior;
class SoccerTeam;
class FootballGame;
class Goal;
struct Telegram;


class FieldPlayer : public PlayerBase
{
	private:

   		//an instance of the state machine class
  		StateMachine<FieldPlayer>*  m_pStateMachine;
  
	public:
  		FieldPlayer
		(
        		int id, Vector2D position, double boundingRadius, //BaseGameEntity
        		Vector2D velocity, Vector2D heading, double mass, double maxSpeed, double maxForce, double maxTurnRate,       //MovingEntity
        		SoccerTeam* soccerTeam, int homeRegion, double maxSpeedWithBall, player_role role, //PlayerBase
        		State<FieldPlayer>* startState //FieldPlayer
		);

  		~FieldPlayer();

  		//call this to update the player's position and orientation
  		void        Update();   

  		bool        HandleMessage(const Telegram& msg);

  		StateMachine<FieldPlayer>* GetFSM()const{return m_pStateMachine;}
};

#endif
