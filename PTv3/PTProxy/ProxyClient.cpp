#include "stdafx.h"
#include "ProxyClient.h"


CProxyClient::CProxyClient()
{
}


CProxyClient::~CProxyClient()
{
}

void CProxyClient::handleRequest(IncomingPacket * pRequest)
{
}

Login::Type CProxyClient::processLogin(LoginData & loginData)
{
	return Login::Type();
}

void CProxyClient::Recycle()
{
}
