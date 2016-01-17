#include "stdafx.h"
#include "ProxyServer.h"
#include "SimpleProtocol.h"
#include "ProtoBufSerializer.h"

#define PROXY_APP_VERSION "1.0"

int main(int argc, char* argv[])
{
	cout << "PTStation Proxy v" << PROXY_APP_VERSION << endl;

	CProxyServer theServer;
	CSimpleProtocol simpleProtocol;
	CProtoBufSerializer protobufSerializer;
	
	theServer.setMessageFactory(&protobufSerializer);
	
	ListenerOptions lOptions;
	lOptions.pProtocol = &simpleProtocol;

	theServer.createListener(10010, &lOptions);

	/*
	QueueOptions qOptions;
	qOptions.priority = 10;
	qOptions.packetsQuota = 4;

	theServer.CreateQueue("queue1", qOptions);
	*/

	theServer.start();
	return 0;
}

