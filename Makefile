hellomake:
	g++ -std=c++11 -w -DLOG_TO_STDOUT=1 -odataserver cppWebSockets/Util.cpp cppWebSockets/WebSocketServer.cpp dataserver.cc samples.cc chrysler_demo.cpp -lwebsockets -lsqlite3
