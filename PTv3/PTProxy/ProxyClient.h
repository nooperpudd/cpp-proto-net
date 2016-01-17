#pragma once

class CProxyClient : public LogicalConnection
{
public:
	CProxyClient();
	~CProxyClient();

protected:

	virtual void handleRequest(IncomingPacket* pRequest);

	virtual Login::Type processLogin(LoginData& loginData) override;

	virtual void Recycle();
};

