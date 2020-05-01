#include "FieldPlayerStates.h"
#include "SoccerPitch.h"
#include "FieldPlayer.h"
#include "SteeringBehaviors.h"
#include "SoccerTeam.h"
#include "Goal.h"
#include "2D/geometry.h"
#include "SoccerBall.h"
#include "Messaging/Telegram.h"
#include "Messaging/MessageDispatcher.h"
#include "SoccerMessages.h"


//uncomment below to send state info to the debug window
#define PLAYER_STATE_INFO_ON


//************************************************************************ Global state

GlobalPlayerState* GlobalPlayerState::Instance()
{
	static GlobalPlayerState instance;
  	return &instance;
}

void GlobalPlayerState::Execute(FieldPlayer* player)                                     
{
	//if a player is in possession and close to the ball reduce his max speed
  	if((player->BallWithinReceivingRange()) && (player->isControllingPlayer()))
  	{
    		player->SetMaxSpeed(player->mPlayerMaxSpeedWithBall);
  	}
  	else
  	{
     		player->SetMaxSpeed(player->MaxSpeed());
  	}
	player->TrackBall();
}

bool GlobalPlayerState::OnMessage(FieldPlayer* player, const Telegram& telegram)
{
	switch(telegram.Msg)
  	{
  		case Msg_ReceiveBall:
    		{
      			//set the target
      			player->Steering()->SetTarget(*(static_cast<Vector2D*>(telegram.ExtraInfo)));

      			//change state 
      			player->GetFSM()->ChangeState(ReceiveBall::Instance());
      			return true;
    		}
    		break;

  		case Msg_SupportAttacker:
    		{
      			//if already supporting just return
      			if (player->GetFSM()->isInState(*SupportAttacker::Instance()))
      			{
        			return true;
      			}
      
      			//set the target to be the best supporting position
      			player->Steering()->SetTarget(player->Team()->GetSupportSpot());

      			//change the state
      			player->GetFSM()->ChangeState(SupportAttacker::Instance());

      			return true;
    		}

    		break;

 		case Msg_Wait:
    		{
      			//change the state
      			player->GetFSM()->ChangeState(Wait::Instance());
      			return true;
    		}
    		break;

  		case Msg_GoHome:
    		{
      			player->SetDefaultHomeRegion();
      
      			player->GetFSM()->ChangeState(ReturnToHomeRegion::Instance());

      			return true;
    		}

    		break;

                case Msg_PassToMe:
                {

                        //get the position of the player requesting the pass
                        FieldPlayer* receiver = static_cast<FieldPlayer*>(telegram.ExtraInfo);

                        //if the ball is not within kicking range or their is already a
                        //receiving player, this player cannot pass the ball to the player
                        //making the request.
                        if (player->Team()->Receiver() != NULL ||
                                !player->BallWithinKickingRange() )
                        {
                                return true;
                        }

                        //make the pass
			if (player->Team()->Pitch()->m_pRedTeam == player->Team())
                	{
                        	printf("PASS VIA REQUEST RED\n");
                	}
                	else
                	{
                        	printf("PASS VIA REQUEST BLUE\n");
                	}

			Vector2D v = receiver->Pos() - player->Ball()->Pos();
                        
			player->Ball()->Kick(receiver->Pos() - player->Ball()->Pos(),
                           player->Team()->Pitch()->MaxPassingForce);

			//let the receiver know a pass is coming
                        Vector2D receiverPosition;
                        receiverPosition.x = receiver->Pos().x;
                        receiverPosition.y = receiver->Pos().y;

                        //let the receiver know a pass is coming
                        Dispatcher->DispatchMsg(SEND_MSG_IMMEDIATELY,
                              player->ID(),
                              receiver->ID(),
                              Msg_ReceiveBall,
                              &receiverPosition);

                        //change state
                        player->GetFSM()->ChangeState(Wait::Instance());

                        player->FindSupport();

                        return true;
                }

    		break;

	}//end switch

  	return false;
}
                                

       

//***************************************************************************** CHASEBALL

ChaseBall* ChaseBall::Instance()
{
	static ChaseBall instance;

  	return &instance;
}

void ChaseBall::Enter(FieldPlayer* player)
{
	if (player->mEnterLogs)
	{
		printf("ChaseBall::Enter()\n");
	}
	player->mStateName = "ChaseBall";

	player->Steering()->SeekOn();

    	player->Steering()->ArriveOff();
	player->Steering()->PursuitOff();
	player->Steering()->SeparationOff();
	player->Steering()->InterposeOff();
    		
	player->TrackBall();

}

void ChaseBall::Execute(FieldPlayer* player)                                     
{
	if (player->mExecuteLogs)
	{
		printf("ChaseBall::Execute() ID:%d\n", player->ID());
	}
	
	player->TrackBall();

  	//if the ball is within kicking range the player changes state to KickBall.
  	if (player->BallWithinKickingRange())
  	{
    		player->GetFSM()->ChangeState(KickBall::Instance());
    		return;
  	}
                                                                              
  	//if the player is the closest player to the ball then he should keep
  	//chasing it
  	if (player->isClosestTeamMemberToBall())
  	{
    		player->Steering()->SetTarget(player->Ball()->Pos());
    		return;
  	}
  
  	//if the player is not closest to the ball anymore, he should return back
  	//to his home region and wait for another opportunity
  	player->GetFSM()->ChangeState(ReturnToHomeRegion::Instance());
}


void ChaseBall::Exit(FieldPlayer* player)
{
	if (player->mExitLogs)
	{
		printf("ChaseBall::Exit() ID:%d\n", player->ID());
	}
  	player->Steering()->SeekOff();
}



//*****************************************************************************SUPPORT ATTACKING PLAYER

SupportAttacker* SupportAttacker::Instance()
{
  static SupportAttacker instance;

  return &instance;
}


void SupportAttacker::Enter(FieldPlayer* player)
{
	if (player->mEnterLogs)
	{
		printf("SupportAttacker::Enter() ID:%d\n", player->ID());
	}
	player->mStateName = "SupportAttacker";
  	player->Steering()->ArriveOn();

  	player->Steering()->SetTarget(player->Team()->GetSupportSpot());
}

void SupportAttacker::Execute(FieldPlayer* player)                                     
{
	if (player->mExecuteLogs)
	{
		printf("SupportAttacker::Execute() ID:%d\n", player->ID());
	}

  	//if his team loses control go back home
  	if (!player->Team()->InControl())
  	{
    		player->GetFSM()->ChangeState(ReturnToHomeRegion::Instance()); return;
  	}	 

  	//if the best supporting spot changes, change the steering target
	//Breslin: this should be multiple states ie back support wide support depth support, permament support etc..
  	if (player->Team()->GetSupportSpot() != player->Steering()->Target())
  	{    
    		player->Steering()->SetTarget(player->Team()->GetSupportSpot());
    		player->Steering()->ArriveOn();
  	}

  	//if this player has a shot at the goal AND the attacker can pass
  	//the ball to him the attacker should pass the ball to this player
  	if( player->Team()->CanShoot(player->Pos(),
                               player->Pitch()->MaxShootingForce))
  	{
    		player->Team()->RequestPass(player);
  	}

  	//if this player is located at the support spot and his team still have
  	//possession, he should remain still and turn to face the ball
  	if (player->AtTarget())
  	{
    		player->Steering()->ArriveOff();
        
    		//the player should keep his eyes on the ball!
    		player->TrackBall();

    		player->SetVelocity(Vector2D(0,0));

    		//if not threatened by another player request a pass
    		if (!player->isThreatened())
    		{
      			player->Team()->RequestPass(player);
    		}
  	}
}


void SupportAttacker::Exit(FieldPlayer* player)
{
	if (player->mExitLogs)
	{
		printf("SupportAttacker::Exit() ID:%d\n", player->ID());
	}
  
	//set supporting player to null so that the team knows it has to 
  	//determine a new one.
  	player->Team()->SetSupportingPlayer(NULL);

  	player->Steering()->ArriveOff();
}




//************************************************************************ RETURN TO HOME REGION

ReturnToHomeRegion* ReturnToHomeRegion::Instance()
{
  static ReturnToHomeRegion instance;

  return &instance;
}


void ReturnToHomeRegion::Enter(FieldPlayer* player)
{
	if (player->mEnterLogs)
	{
		printf("ReturnToHomeRegion::Enter() ID:%d\n", player->ID());
	}
	player->mStateName = "ReturnToHomeRegion";
  
	player->Steering()->ArriveOn();

  	if (!player->HomeRegion()->Inside(player->Steering()->Target(), Region::halfsize))
  	{
    		player->Steering()->SetTarget(player->HomeRegion()->Center());
  	}
}

void ReturnToHomeRegion::Execute(FieldPlayer* player)
{
	if (player->mExecuteLogs)
	{
		printf("ReturnToHomeRegion::Execute() ID:%d\n", player->ID());
	}

  	if (player->Pitch()->GameOn())
  	{
    		//if the ball is nearer this player than any other team member  &&
    		//there is not an assigned receiver && the goalkeeper does not gave
    		//the ball, go chase it
    		if ( player->isClosestTeamMemberToBall() &&
         		(player->Team()->Receiver() == NULL) &&
         		!player->Pitch()->GoalKeeperHasBall())
    		{
      			player->GetFSM()->ChangeState(ChaseBall::Instance());
      			return;
    		}
  	}

  	//if game is on and close enough to home, change state to wait and set the 
  	//player target to his current position.(so that if he gets jostled out of 
  	//position he can move back to it)
  	if (player->Pitch()->GameOn() && player->HomeRegion()->Inside(player->Pos(),
                                                             Region::halfsize))
  	{
    		player->Steering()->SetTarget(player->Pos());
    		player->GetFSM()->ChangeState(Wait::Instance());
  	}
  
	//if game is not on the player must return much closer to the center of his
  	//home region
  	else if(!player->Pitch()->GameOn() && player->AtTarget())
  	{
    		player->GetFSM()->ChangeState(Wait::Instance());
  	}
}

void ReturnToHomeRegion::Exit(FieldPlayer* player)
{
	if (player->mExitLogs)
	{
		printf("ReturnToHomeRegion::Exit() ID:%d\n", player->ID());
	}
  	player->Steering()->ArriveOff();
}




//***************************************************************************** WAIT

Wait* Wait::Instance()
{
  static Wait instance;

  return &instance;
}


void Wait::Enter(FieldPlayer* player)
{
	if (player->mEnterLogs)
	{
		printf("Wait::Enter() ID:%d\n", player->ID());
	}
	player->mStateName = "Wait";

  
	//if the game is not on make sure the target is the center of the player's
  	//home region. This is ensure all the players are in the correct positions
	//ready for kick off
  	if (!player->Pitch()->GameOn())
  	{
    		player->Steering()->SetTarget(player->HomeRegion()->Center());
  	}
}

void Wait::Execute(FieldPlayer* player)
{    
	if (player->mExecuteLogs)
	{
		printf("Wait::Execute() ID:%d\n", player->ID());
	}

	//if the player has been jostled out of position, get back in position  
  	if (!player->AtTarget())
  	{
    		player->Steering()->ArriveOn();
    		return;
  	}
  	else
  	{
    		player->Steering()->ArriveOff();

   		 player->SetVelocity(Vector2D(0,0));

    		//the player should keep his eyes on the ball!
    		player->TrackBall();
  	}

  	//if this player's team is controlling AND this player is not the attacker
  	//AND is further up the field than the attacker he should request a pass.
  	if ( player->Team()->InControl()    &&
     		(!player->isControllingPlayer()) &&
       		player->isAheadOfAttacker() )
  	{
    		player->Team()->RequestPass(player);
    		return;
  	}

  	if (player->Pitch()->GameOn())
  	{
   		//if the ball is nearer this player than any other team member  AND
    		//there is not an assigned receiver AND neither goalkeeper has
    		//the ball, go chase it
   		if (player->isClosestTeamMemberToBall() &&
       			player->Team()->Receiver() == NULL  &&
       			!player->Pitch()->GoalKeeperHasBall())
   		{
     			player->GetFSM()->ChangeState(ChaseBall::Instance());

     			return;
   		}
 	} 
}

void Wait::Exit(FieldPlayer* player)
{
	if (player->mExitLogs)
	{
		printf("Wait::Exit() ID:%d\n", player->ID());
	}
}


//************************************************************************ KICK BALL

KickBall* KickBall::Instance()
{
  static KickBall instance;

  return &instance;
}


void KickBall::Enter(FieldPlayer* player)
{
	if (player->mEnterLogs)
	{
		printf("KickBall::Enter() ID:%d\n", player->ID());
	}
	player->mStateName = "KickBall";
  
	//let the team know this player is controlling
   	player->Team()->SetControllingPlayer(player);
		
	player->mKickCounter++;

	if (player->mKickCounter > player->mKickCounterThreshold)
	{
		player->mKickCounter = 0;
     		player->GetFSM()->ChangeState(ChaseBall::Instance());
	}
} 

void KickBall::Execute(FieldPlayer* player)
{ 
	//printf("KickBall::Execute\n");
	if (player->mExecuteLogs)
	{
		printf("KickBall::Execute() ID:%d\n", player->ID());
	}

  	//calculate the dot product of the vector pointing to the ball
  	//and the player's heading
  	Vector2D ToBall = player->Ball()->Pos() - player->Pos();
  	double   dot    = player->Heading().Dot(Vec2DNormalize(ToBall)); 

  	//cannot kick the ball if the goalkeeper is in possession or if it is 
  	//behind the player or if there is already an assigned receiver. So just
  	//continue chasing the ball
  	if (player->Team()->Receiver() != NULL   ||
      		player->Pitch()->GoalKeeperHasBall() ||
      		(dot < 0) ) 
  	{
    		player->GetFSM()->ChangeState(ChaseBall::Instance());
		return;
  	}

  	/* Attempt a shot at the goal */

  	//if a shot is possible, this vector will hold the position along the 
  	//opponent's goal line the player should aim for.
  	Vector2D    BallTarget;

  	//the dot product is used to adjust the shooting force. The more
  	//directly the ball is ahead, the more forceful the kick
  	double power = player->Pitch()->MaxShootingForce * dot;

  	//if it is determined that the player could score a goal from this position
  	//OR if he should just kick the ball anyway, the player will attempt
  	//to make the shot
  	if (player->Team()->CanShoot(player->Ball()->Pos(), power))
  	{	 
		if (player->Team()->Pitch()->m_pRedTeam == player->Team()) 
		{
			printf("SHOT RED\n");
		}
		else
		{
			printf("SHOT BLUE\n");
		}
   		//add some noise to the kick. We don't want players who are 
   		//too accurate! The amount of noise can be adjusted by altering
		
		//took out noise
   		//BallTarget = player->Ball()->AddNoiseToKick(player->Ball()->Pos(), BallTarget);

   		//this is the direction the ball will be kicked in
   		//Vector2D KickDirection = BallTarget - player->Ball()->Pos();

   		//Vector2D KickDirection = player->Ball()->Pos() - player->Team()->OpponentsGoal()->Center();
   		Vector2D KickDirection = player->Team()->OpponentsGoal()->Center() - player->Ball()->Pos();
   
   		player->Ball()->Kick(KickDirection, power);
    
   		//change state   
   		player->GetFSM()->ChangeState(Wait::Instance());
   
   		player->FindSupport();
  
   		return;
 	}

        if ( (RandFloat() < player->Pitch()->ChancePlayerAttemptsPotShot))
        {
		if (player->Team()->Pitch()->m_pRedTeam == player->Team()) 
		{
                	printf("POT SHOT RED\n");
		}
		else
		{
                	printf("POT SHOT BLUE\n");
		}

                //add some noise to the kick. We don't want players who are
                //too accurate! The amount of noise can be adjusted by altering
                BallTarget = player->Ball()->AddNoiseToKick(player->Ball()->Pos(), BallTarget);

                //this is the direction the ball will be kicked in
                Vector2D KickDirection = BallTarget - player->Ball()->Pos();

                player->Ball()->Kick(KickDirection, power);

                //change state
                player->GetFSM()->ChangeState(Wait::Instance());

                player->FindSupport();

                return;
        }


  	/* Attempt a pass to a player */

  	//if a receiver is found this will point to it
  	PlayerBase* receiver = NULL;

  	power = player->Pitch()->MaxPassingForce * dot;

  	//test if there are any potential candidates available to receive a pass
  	if (player->isThreatened()  &&
      		player->Team()->FindPass(player,
                              receiver,
                              BallTarget,
                              power,
                              player->Pitch()->MinPassDist))
  	{     
		if (player->Team()->Pitch()->m_pRedTeam == player->Team()) 
		{
			printf("PASS RED\n");
		}
		else
		{
			printf("PASS BLUE\n");
		}
    		//add some noise to the kick
    		BallTarget = player->Ball()->AddNoiseToKick(player->Ball()->Pos(), BallTarget);

    		Vector2D KickDirection = BallTarget - player->Ball()->Pos();
   
    		player->Ball()->Kick(KickDirection, power);

    		//let the receiver know a pass is coming 
    		Dispatcher->DispatchMsg(SEND_MSG_IMMEDIATELY,
                            player->ID(),
                            receiver->ID(),
                            Msg_ReceiveBall,
                            &BallTarget);                            
   
    		//the player should wait at his current position unless instruced
    		//otherwise  
    		player->GetFSM()->ChangeState(Wait::Instance());

    		player->FindSupport();

    		return;
  	}

  	//cannot shoot or pass, so dribble the ball upfield
  	else
  	{   
		if (player->Team()->Pitch()->m_pRedTeam == player->Team()) 
		{
			printf("DRIBBLE RED\n");
		}
		else
		{
			printf("DRIBBLE BLUE\n");
		}
    		player->FindSupport();

    		player->GetFSM()->ChangeState(Dribble::Instance());
  	}   
}


//*************************************************************************** DRIBBLE

Dribble* Dribble::Instance()
{
  static Dribble instance;

  return &instance;
}


void Dribble::Enter(FieldPlayer* player)
{
	if (player->mEnterLogs)
	{
		printf("Dribble::Enter() ID:%d\n", player->ID());
	}

	player->mStateName = "Dribble";

  	//let the team know this player is controlling
  	player->Team()->SetControllingPlayer(player);
}

void Dribble::Execute(FieldPlayer* player)
{
	if (player->mExecuteLogs)
	{
		printf("Dribble::Execute() ID:%d\n", player->ID());
	}
  
	double dot = player->Team()->HomeGoal()->Facing().Dot(player->Heading());

  	//if the ball is between the player and the home goal, it needs to swivel
  	// the ball around by doing multiple small kicks and turns until the player 
  	//is facing in the correct direction
  	if (dot < 0)
  	{
    		//the player's heading is going to be rotated by a small amount (Pi/4) 
    		//and then the ball will be kicked in that direction
    		Vector2D direction = player->Heading();

    		//calculate the sign (+/-) of the angle between the player heading and the 
    		//facing direction of the goal so that the player rotates around in the 
    		//correct direction
    		double angle = QuarterPi * -1 *
                 player->Team()->HomeGoal()->Facing().Sign(player->Heading());

    		Vec2DRotateAroundOrigin(direction, angle);

    		//this value works well whjen the player is attempting to control the
    		//ball and turn at the same time
    		const double KickingForce = 0.8;

    		player->Ball()->Kick(direction, KickingForce);
  	}
  	//kick the ball down the field
  	else
  	{
    		player->Ball()->Kick(player->Team()->HomeGoal()->Facing(),
                         player->Pitch()->MaxDribbleForce);  
  	}

  	//the player has kicked the ball so he must now change state to follow it
  	player->GetFSM()->ChangeState(ChaseBall::Instance());
    
  	return;  
}



//************************************************************************     RECEIVEBALL

ReceiveBall* ReceiveBall::Instance()
{
  static ReceiveBall instance;

  return &instance;
}


void ReceiveBall::Enter(FieldPlayer* player)
{
	if (player->mEnterLogs)
	{
		printf("ReceiveBall::Enter() ID:%d\n", player->ID());
	}
	player->mStateName = "ReceiveBall";
	
	//let the team know this player is receiving the ball
  	player->Team()->SetReceiver(player);
  
  	//this player is also now the controlling player
  	player->Team()->SetControllingPlayer(player);

  	//there are two types of receive behavior. One uses arrive to direct
  	//the receiver to the position sent by the passer in its telegram. The
  	//other uses the pursuit behavior to pursue the ball. 
  	//This statement selects between them dependent on the probability
  	//ChanceOfUsingArriveTypeReceiveBehavior, whether or not an opposing
  	//player is close to the receiving player, and whether or not the receiving
  	//player is in the opponents 'hot region' (the third of the pitch closest
  	//to the opponent's goal
  	const double PassThreatRadius = 70.0;

  	if (( player->InHotRegion() ||
        	RandFloat() < player->Pitch()->ChanceOfUsingArriveTypeReceiveBehavior) &&
     	!player->Team()->isOpponentWithinRadius(player->Pos(), PassThreatRadius))
  	{
    		player->Steering()->ArriveOn();
    
  	}
  	else
  	{
    		player->Steering()->PursuitOn();
  	}
}

void ReceiveBall::Execute(FieldPlayer* player)
{
	if (player->mExecuteLogs)
	{
		printf("ReceiveBall::Execute() ID:%d\n", player->ID());
	}
  
	//if the ball comes close enough to the player or if his team lose control
  	//he should change state to chase the ball
  	if (player->BallWithinReceivingRange() || !player->Team()->InControl())
  	{
    		player->GetFSM()->ChangeState(ChaseBall::Instance());
    		return;
  	}  

  	if (player->Steering()->PursuitIsOn())
  	{
    		player->Steering()->SetTarget(player->Ball()->Pos());
  	}

  	//if the player has 'arrived' at the steering target he should wait and
  	//turn to face the ball
  	if (player->AtTarget())
  	{
    		player->Steering()->ArriveOff();
    		player->Steering()->PursuitOff();
    		player->TrackBall();    
    		player->SetVelocity(Vector2D(0,0));
  	}	 
}

void ReceiveBall::Exit(FieldPlayer* player)
{
	if (player->mExitLogs)
	{
		printf("ReceiveBall::Exit() ID:%d\n", player->ID());
	}
  	player->Steering()->ArriveOff();
  	player->Steering()->PursuitOff();

  	player->Team()->SetReceiver(NULL);
}




 



