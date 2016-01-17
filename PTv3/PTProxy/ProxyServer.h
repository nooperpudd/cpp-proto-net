#pragma once

class CProxyServer : public PushFramework::Server
{
public:

protected:

	virtual LogicalConnection* createLogicalConnection();
};
