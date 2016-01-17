#include "stdafx.h"
#include "ProtoBufSerializer.h"
#include "ProtobufMessage.h"

void CProtoBufSerializer::disposeIncomingPacket(IncomingPacket* pPacket)
{
	CProtobufMessage* pMessage = (CProtobufMessage*)pPacket;
	delete pMessage;
}

void CProtoBufSerializer::disposeOutgoingPacket(OutgoingPacket* pPacket)
{
	CProtobufMessage* pMessage = (CProtobufMessage*)pPacket;
	delete pMessage;
}

SerializeResult::Type CProtoBufSerializer::serializeMessage(OutgoingPacket& outgoingPacket, Buffer& buffer)
{
	CProtobufMessage& message = (CProtobufMessage&)outgoingPacket;

	message.Encode();

	if (buffer.getCapacity() < (message.getData().length() + 1))
	{
		return SerializeResult::InsufficientBufferSpace; //TODO. Verify this.
	}

	buffer.append(message.getData().c_str(), message.getData().length());

	char endChar = 0;
	buffer.append(&endChar, 1);

	return SerializeResult::Success;
}

DeserializeResult::Type CProtoBufSerializer::deserializeMessage(Buffer& contentBytes, IncomingPacket*& pMessage, int& nRoutingService)
{
	if (contentBytes.getDataSize() == 0)
	{
		return DeserializeResult::DiscardContent;
	}

	char endChar = contentBytes.getAt(contentBytes.getDataSize() - 1);

	if (endChar != 0)
	{
		return DeserializeResult::Failure;
	}

	CProtobufMessage* message = CProtobufMessage::CreateFromBuffer(contentBytes.getBuffer());

	if (message != NULL)
	{
		return DeserializeResult::Failure;
	}

	pMessage = (IncomingPacket*)message;
	nRoutingService = message->getTypeId();

	return DeserializeResult::Success;
}

bool CProtoBufSerializer::preSerializeMessage(OutgoingPacket& outgoingPacket)
{
	CProtobufMessage& message = (CProtobufMessage&)outgoingPacket;
	message.Encode();
	return true;
}