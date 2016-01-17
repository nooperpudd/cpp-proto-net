#pragma once

class ProtobufPacketImpl
{
public:
	ProtobufPacketImpl(int serviceId, protobuf::Message* message);
	virtual ~ProtobufPacketImpl(void);
public:
	virtual bool Decode(char* pBuf, unsigned int nSize);
	virtual bool Encode();
	virtual protobuf::Message& getStructuredData();
private:
	int serviceId;
	string* pEncodedStream;
	protobuf::Message* message;

public:
	string* getEncodedStream() const { return pEncodedStream; }
	int getEncodedStreamSize();
	int getServiceId() const { return serviceId; }
};

template<class T>
class ProtobufPacket : public ProtobufPacketImpl
{
public:
	ProtobufPacket(int serviceId)
		: ProtobufPacketImpl(serviceId, &data)
	{
		//
	}
	~ProtobufPacket()
	{
		//
	}
private:
	T data;
public:
	T& getData()
	{
		return data;
	}
};


class CProtobufMessage : public IncomingPacket, public OutgoingPacket
{
public:
	CProtobufMessage(int typeId);
	~CProtobufMessage();
private:
	CProtobufMessage(){}

public:
	static CProtobufMessage* CreateFromBuffer(const char* stringBuffer);
	void Encode();
	int getTypeId();
	string& getData();
private:

	int m_typeId;
	string m_data;
};

