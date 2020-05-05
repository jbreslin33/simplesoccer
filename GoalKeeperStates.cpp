#include "GoalKeeperStates.h"
#include "footballGame.h"
#include "PlayerBase.h"
#include "Goalkeeper.h"
#include "SteeringBehaviors.h"
#include "SoccerTeam.h"
#include "Goal.h"
#include "FieldPlayer.h"
#include "Messaging/Telegram.h"
#include "Messaging/MessageDispatcher.h"
#include "SoccerMessages.h"


//uncomment to send state info to debug window
//#define GOALY_STATE_INFO_ON


//--------------------------- GlobalKeeperState -------------------------------
//-----------------------------------------------------------------------------

GlobalKeeperState* GlobalKeeperState::Instance()
{
	static GlobalKeeperState instance;

  	return &instance;
}

bool GlobalKeeperState::OnMessage(GoalKeeper* keeper, const Telegram& telegram)
{
	switch(telegram.Msg)
  	{
    		case Msg_GoHome:
    		{
      			keeper->SetDefaultHomeRegion();
      
      			keeper->GetFSM()->ChangeState(ReturnHome::Instance());
    		}

    		break;

    		case Msg_ReceiveBall:
      		{
        		keeper->GetFSM()->ChangeState(InterceptBall::Instance());
      		}

      		break;

  	}//end switch

  	return false;
}


//--------------------------- TendGoal -----------------------------------
//
//  This is the main state for the goalkeeper. When in this state he will
//  move left to right across the goalmouth using the 'interpose' steering
//  behavior to put himself between the ball and the back of the net.
//
//  If the ball comes within the 'goalkeeper range' he moves out of the
//  goalmouth to attempt to intercept it. (see next state)
//------------------------------------------------------------------------

TendGoal* TendGoal::Instance()
{
	static TendGoal instance;

	return &instance;
}

void TendGoal::Enter(GoalKeeper* keeper)
{
        if (keeper->mEnterLogs)
        {
                printf("TendGoal::Enter() ID:%d\n",keeper->ID());
        }

  	//turn interpose on
  	keeper->Steering()->InterposeOn(keeper->Team()->Game()->GoalKeeperTendingDistance);

  	//interpose will position the agent between the ball position and a target
  	//position situated along the goal mouth. This call sets the target
  	keeper->Steering()->SetTarget(keeper->GetRearInterposeTarget());
}

void TendGoal::Execute(GoalKeeper* keeper)
{
        if (keeper->mExecuteLogs)
        {
                printf("TendGoal::Execute() ID:%d\n",keeper->ID());
        }

  	//the rear interpose target will change as the ball's position changes
  	//so it must be updated each update-step 
  	keeper->Steering()->SetTarget(keeper->GetRearInterposeTarget());

  	//if the ball comes in range the keeper traps it and then changes state
  	//to put the ball back in play
  	if (keeper->BallWithinKeeperRange())
  	{
    		keeper->getBall()->Trap();

    		keeper->Game()->setKeeperHasBall(true);

    		keeper->GetFSM()->ChangeState(PutBallBackInPlay::Instance());
    		return;
  	}

  	//if ball is within a predefined distance, the keeper moves out from
  	//position to try and intercept it.
  	if (keeper->BallWithinRangeForIntercept() && !keeper->Team()->InControl())
  	{
    		keeper->GetFSM()->ChangeState(InterceptBall::Instance());
  	}

  	//if the keeper has ventured too far away from the goal-line and there
  	//is no threat from the opponents he should move back towards it
  	if (keeper->TooFarFromGoalMouth() && keeper->Team()->InControl())
  	{
    		keeper->GetFSM()->ChangeState(ReturnHome::Instance());

    		return;
  	}
}

void TendGoal::Exit(GoalKeeper* keeper)
{
        if (keeper->mExitLogs)
        {
                printf("TendGoal::Exit() ID:%d\n",keeper->ID());
        }

	keeper->Steering()->InterposeOff();
}

//------------------------- ReturnHome: ----------------------------------
//
//  In this state the goalkeeper simply returns back to the center of
//  the goal region before changing state back to TendGoal
//------------------------------------------------------------------------

ReturnHome* ReturnHome::Instance()
{
	static ReturnHome instance;

  	return &instance;
}

void ReturnHome::Enter(GoalKeeper* keeper)
{
        if (keeper->mEnterLogs)
        {
                printf("ReturnHome::Enter() ID:%d\n", keeper->ID());
        }

	keeper->Steering()->ArriveOn();
}

void ReturnHome::Execute(GoalKeeper* keeper)
{
        if (keeper->mExecuteLogs)
        {
                printf("ReturnHome::Execute() ID:%d\n", keeper->ID());
        }

	keeper->Steering()->SetTarget(keeper->HomeRegion()->Center());

  	//if close enough to home or the opponents get control over the ball,
  	//change state to tend goal
  	if (keeper->InHomeRegion() || !keeper->Team()->InControl())
  	{
   		keeper->GetFSM()->ChangeState(TendGoal::Instance());
  	}
}

void ReturnHome::Exit(GoalKeeper* keeper)
{
        if (keeper->mExitLogs)
        {
                printf("ReturnHome::Exit() ID:%d\n", keeper->ID());
        }

	keeper->Steering()->ArriveOff();
}

//----------------- InterceptBall ----------------------------------------
//
//  In this state the GP will attempt to intercept the ball using the
//  pursuit steering behavior, but he only does so so long as he remains
//  within his home region.
//------------------------------------------------------------------------

InterceptBall* InterceptBall::Instance()
{
  static InterceptBall instance;

  return &instance;
}


void InterceptBall::Enter(GoalKeeper* keeper)
{
        if (keeper->mEnterLogs)
        {
                printf("InterceptBall::Enter() ID:%d\n",keeper->ID());
        }
  
	keeper->Steering()->PursuitOn();  
}

void InterceptBall::Execute(GoalKeeper* keeper)
{ 
        if (keeper->mExecuteLogs)
        {
                printf("InterceptBall::Execute() ID:%d\n", keeper->ID());
        }

  	//if the goalkeeper moves to far away from the goal he should return to his
  	//home region UNLESS he is the closest player to the ball, in which case,
  	//he should keep trying to intercept it.
  	if (keeper->TooFarFromGoalMouth() && !keeper->isClosestPlayerOnGameToBall())
  	{
    		keeper->GetFSM()->ChangeState(ReturnHome::Instance());
    		return;
  	}
  
  	//if the ball becomes in range of the goalkeeper's hands he traps the 
  	//ball and puts it back in play
  	if (keeper->BallWithinKeeperRange())
  	{
    		keeper->getBall()->Trap();
    
    		keeper->Game()->setKeeperHasBall(true);

    		keeper->GetFSM()->ChangeState(PutBallBackInPlay::Instance());
    		return;
  	}
}

void InterceptBall::Exit(GoalKeeper* keeper)
{
        if (keeper->mExitLogs)
        {
                printf("InterceptBall::Execute() ID:%d\n", keeper->ID());
        }
  
	keeper->Steering()->PursuitOff();
}



//--------------------------- PutBallBackInPlay --------------------------
//
//------------------------------------------------------------------------

PutBallBackInPlay* PutBallBackInPlay::Instance()
{
	static PutBallBackInPlay instance;

	return &instance;
}

void PutBallBackInPlay::Enter(GoalKeeper* keeper)
{
        if (keeper->mEnterLogs)
        {
                printf("PutBallBackInPlay::Enter() ID:%d\n", keeper->ID());
        }

  	//let the team know that the keeper is in control
  	keeper->Team()->SetControllingPlayer(keeper);

  	//send all the players home
  	keeper->Team()->Opponents()->ReturnAllFieldPlayersToHome();
  	keeper->Team()->ReturnAllFieldPlayersToHome();
}

void PutBallBackInPlay::Execute(GoalKeeper* keeper)
{
	if (keeper->mExecuteLogs)
        {
                printf("PutBallBackInPlay::Execute() ID:%d\n", keeper->ID());
        }
  
	PlayerBase*  receiver = NULL;
  	Vector2D     BallTarget;
    
  	//test if there are players further forward on the field we might
  	//be able to pass to. If so, make a pass.
  	if (keeper->Team()->FindPass(keeper,
                              receiver,
                              BallTarget,
                              keeper->Game()->MaxPassingForce,
                              keeper->Game()->GoalkeeperMinPassDist))
  	{     
    		//make the pass   
    		keeper->getBall()->Kick(Vec2DNormalize(BallTarget - keeper->getBall()->Pos()),
                         keeper->Game()->MaxPassingForce);

    		//goalkeeper no longer has ball 
    		keeper->Game()->setKeeperHasBall(false);

    		//let the receiving player know the ball's comin' at him
    		Dispatcher->DispatchMsg(SEND_MSG_IMMEDIATELY,
                          keeper->ID(),
                          receiver->ID(),
                          Msg_ReceiveBall,
                          &BallTarget);
    
    		//go back to tending the goal   
    		keeper->GetFSM()->ChangeState(TendGoal::Instance());

    		return;
  	}  

  	keeper->SetVelocity(Vector2D());
}

