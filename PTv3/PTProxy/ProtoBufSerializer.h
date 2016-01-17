#pragma once
class CProtoBufSerializer : public MessageFactory
{
public:
	CProtoBufSerializer()
	{
	}

protected:
	virtual SerializeResult::Type serializeMessage(OutgoingPacket& outgoingPacket, Buffer& buffer);
	virtual DeserializeResult::Type deserializeMessage(Buffer& contentBytes, IncomingPacket*& pMessage, int& nRoutingService);
	virtual bool preSerializeMessage(OutgoingPacket& outgoingPacket);
	virtual void disposeIncomingPacket(IncomingPacket* pPacket);
	virtual void disposeOutgoingPacket(OutgoingPacket* pPacket);
};

