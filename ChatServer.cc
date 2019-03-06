#include <iostream>
#include <pthread.h>
#include "ChatServer.hpp"

void* RunProduct(void *arg)
{
	pthread_detach(pthread_self());
	ChatServer *sp = (ChatServer *)arg;
	for(;;)
	{
		sp->Product();
	}
}

void* RunConsume(void *arg)
{
	pthread_detach(pthread_self());
	ChatServer *sp = (ChatServer *)arg;
	for(;;)
	{
		sp->Consume();
	}
}

static void Usage(std::string proc)
{
    std::cout << "Usage: " << proc << " tcp_port udp_port " << std::endl;
}

// ./ChatServer  tcp_port  udp_port
int main(int argc, char* argv[])
{
    if(argc != 3){
        Usage(argv[0]);
        exit(1);
    }

    int tcp_port = atoi(argv[1]);
    int udp_port = atoi(argv[2]);

    ChatServer *sp = new ChatServer(tcp_port, udp_port);
    sp->InitServer();

	pthread_t p, c;
	pthread_create(&p, NULL, RunProduct, sp);
	pthread_create(&c, NULL, RunConsume, sp);

    sp->Start();

    return 0;
}
