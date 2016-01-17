#include "stdafx.h"
#include "ProtobufMessage.h"

#include <boost/asio/buffer.hpp>


ProtobufPacketImpl::ProtobufPacketImpl(int serviceId, protobuf::Message* message)
{
	this->serviceId = serviceId;
	this->message = message;
	pEncodedStream = NULL;
}

ProtobufPacketImpl::~ProtobufPacketImpl(void)
{
	delete pEncodedStream;
}

bool ProtobufPacketImpl::Decode(char* pBuf, unsigned int nSize)
{
	if (nSize == 0)
		return true;
	return getStructuredData().ParseFromArray(pBuf, nSize);
}

bool ProtobufPacketImpl::Encode()
{
	if (pEncodedStream)
		return true;
	pEncodedStream = new std::string();
	if (!getStructuredData().SerializeToString(pEncodedStream))
	{
		delete pEncodedStream;
		pEncodedStream = NULL;
		return false;
	}

	getStructuredData().ByteSize();
	return true;
}

protobuf::Message& ProtobufPacketImpl::getStructuredData()
{
	return (*message);
}

int ProtobufPacketImpl::getEncodedStreamSize()
{
	return getStructuredData().GetCachedSize();
}


CProtobufMessage::CProtobufMessage(int typeId)
	: m_typeId(typeId)
{
}


CProtobufMessage::~CProtobufMessage()
{
}

CProtobufMessage * CProtobufMessage::CreateFromBuffer(const char * stringBuffer)
{

	/*
	XMLNode xmlRoot = XMLNode::parseString(stringBuffer, "root");
	if (xmlRoot.isEmpty())
	{
		return NULL;
	}

	const char* val = xmlRoot.getAttribute("typeId");
	if (!val)
		return NULL;

	XMLMessage* message = new XMLMessage();
	message->xmlRoot = xmlRoot;

	return message;
	*/
	return nullptr;
}

void CProtobufMessage::Encode()
{
	
	boost::asio::const_buffer bufType = boost::asio::buffer(&m_typeId, sizeof(int));
	vector<boost::asio::const_buffer> buffers = { bufType, boost::asio::buffer(m_data) };
	vector<unsigned char> wholeMsg(boost::asio::buffer_size(buffers));
	boost::asio::buffer_copy(boost::asio::buffer(wholeMsg), buffers);
}

int CProtobufMessage::getTypeId()
{
	return m_typeId;
}

string & CProtobufMessage::getData()
{
	return m_data;
}
