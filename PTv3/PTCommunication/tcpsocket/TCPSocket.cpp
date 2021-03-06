#include "StdAfx.h"
#include "TCPSocket.h"


#include "TCPSocketEvents.h"
#include "TCPSocketImpl.h"
#include "ResponseHandler.h"


TCPSocket::TCPSocket(bool relayToUserThread/* = false*/)
{
    this->relayTCPEvents = relayToUserThread;
    pImpl = new TCPSocketImpl(this);
}

TCPSocket::~TCPSocket()
{
    delete pImpl;
}

bool TCPSocket::connect( const char* hostAddress, unsigned int uPort )
{
    return pImpl->connect(hostAddress, uPort);
}



void TCPSocket::registerHandler( unsigned int requestId, ResponseHandler* pHandler )
{
    pImpl->registerHandler(requestId, pHandler);
}

void TCPSocket::setProtocol( PushFramework::Protocol* pProtocol )
{
    pImpl->setProtocol(pProtocol);
}

int TCPSocket::sendRequest( PushFramework::OutgoingPacket* pPacket )
{
	return pImpl->sendRequest(pPacket);
}

int TCPSocket::getStatus()
{
    return pImpl->getStatus();
}

bool TCPSocket::initializeWinsock()
{
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2,2), &wsaData);
    return true;
}

void TCPSocket::disconnect( bool waitForSend /*= true */ )
{
    pImpl->disconnect(waitForSend);
}

void TCPSocket::onPerformAutomatedJob()
{
    //
}


void TCPSocket::OnReadyToSend()
{
	//
}

const char* TCPSocket::explainLastError()
{
    return pImpl->explainLastError();
}

bool TCPSocket::isRelayTCPEvents() const
{
    return relayTCPEvents;
}

void TCPSocket::ProcessReturnedEvent( TCPSocketEvent* pEvent )
{
    if (pEvent->type == TCPSocketEvent::ConnectionEstablished)
    {
         onConnected();
    }
    if (pEvent->type == TCPSocketEvent::ConnectionClosed)
    {
        onConnectionClosed();
    }
    if (pEvent->type == TCPSocketEvent::NewMessage)
    {
        pImpl->dispatchResponse(pEvent->commandId, *pEvent->pPacket);
    }
}

void TCPSocket::PostTCPEvent( TCPSocketEvent* pEvent )
{
    //
}



