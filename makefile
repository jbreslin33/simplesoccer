-make-simplesoccer:  main.cpp
	g++ -std=c++11 -pthread -o simplesoccer main.cpp -I.
clean :
	rm simplesoccer
