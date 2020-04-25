-make-simplesoccer:  main.cpp  utility.cpp server.cpp SoccerPitch.cpp FieldPlayer.cpp GoalKeeperStates.cpp SoccerMessages.cpp SoccerTeam.cpp SupportSpotCalculator.cpp FieldPlayerStates.cpp Goalkeeper.cpp PlayerBase.cpp SoccerBall.cpp SteeringBehaviors.cpp TeamStates.cpp Game/BaseGameEntity.cpp Game/EntityManager.cpp Messaging/MessageDispatcher.cpp misc/FrameCounter.cpp   
	g++ -std=c++11 -pthread -o simplesoccer main.cpp utility.cpp server.cpp SoccerPitch.cpp FieldPlayer.cpp GoalKeeperStates.cpp SoccerMessages.cpp SoccerTeam.cpp SupportSpotCalculator.cpp FieldPlayerStates.cpp Goalkeeper.cpp PlayerBase.cpp SoccerBall.cpp SteeringBehaviors.cpp TeamStates.cpp Game/BaseGameEntity.cpp Game/EntityManager.cpp Messaging/MessageDispatcher.cpp misc/FrameCounter.cpp -I.
clean :
	rm simplesoccer
