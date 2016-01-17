#include "stdafx.h"
#include "ProxyServer.h"
#include "ProxyClient.h"

LogicalConnection* CProxyServer::createLogicalConnection()
{
	return new CProxyClient();
}