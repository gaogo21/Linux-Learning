.PHONY:all
all:tcpclient tcpserver

tcpclient:TcpClient.cc
	g++ -o $@ $^ -std=c++11
tcpserver:TcpServer.cc
	g++ -o $@ $^ -std=c++11 -lpthread

.PHONY:clean
clean:
	rm -f tcpclient tcpserver