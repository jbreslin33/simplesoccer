-make-simplesoccer:  main.cpp  utility.cpp server.cpp game.cpp footballGame.cpp footballPitch.cpp FieldPlayer.cpp GoalKeeperStates.cpp SoccerMessages.cpp SoccerTeam.cpp SupportSpotCalculator.cpp FieldPlayerStates.cpp Goalkeeper.cpp PlayerBase.cpp SoccerBall.cpp SteeringBehaviors.cpp TeamStates.cpp Game/MovingEntity.cpp Game/BaseGameEntity.cpp Game/EntityManager.cpp Messaging/MessageDispatcher.cpp misc/FrameCounter.cpp client.cpp clientState.cpp clientStateMachine.cpp clientStates.cpp  Game/Region.cpp Goal.cpp
	g++ -std=c++11 -pthread -o simplesoccer main.cpp utility.cpp server.cpp game.cpp footballGame.cpp footballPitch.cpp FieldPlayer.cpp GoalKeeperStates.cpp SoccerMessages.cpp SoccerTeam.cpp SupportSpotCalculator.cpp FieldPlayerStates.cpp Goalkeeper.cpp PlayerBase.cpp SoccerBall.cpp SteeringBehaviors.cpp TeamStates.cpp Game/MovingEntity.cpp Game/BaseGameEntity.cpp Game/EntityManager.cpp Messaging/MessageDispatcher.cpp misc/FrameCounter.cpp client.cpp clientState.cpp clientStateMachine.cpp clientStates.cpp Game/Region.cpp Goal.cpp -I.
clean :
	rm simplesoccer
