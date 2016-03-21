.PHONY: clean

CXX=g++
CXXFLAGS=-Wall -g
BIN=librptp.a
INCLUDE=-I./include/
INCLUDEDIR=./include
HEADERS=./rptp
SRC=./src
LIB=./lib
OBJS=InetAddress.o PollPoller.o Socket.o Rio.o TcpConnection.o TcpServer.o Timer.o TimerThread.o
STARD=-std=c++11 -rdynamic
$(BIN):$(OBJS)
	ar -crv $@ -o $^
	chmod +x $@
	cp -r $(INCLUDEDIR) $(HEADERS)
	cp $(BIN) $(LIB)
%.o:$(SRC)/%.cc
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -o $@ $(STARD)

clean:
	rm *.o $(BIN)
	rm -rf $(HEADERS)
