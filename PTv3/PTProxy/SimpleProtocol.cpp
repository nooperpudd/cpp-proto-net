#include "stdafx.h"
#include "SimpleProtocol.h"

#define SignatureStart 16018
#define SignatureEnd 16108

typedef unsigned char       BYTE;

CSimpleProtocol::CSimpleProtocol()
{
}


CSimpleProtocol::~CSimpleProtocol()
{
}

EncodeResult::Type CSimpleProtocol::encodeContent(ProtocolContext& context, Buffer& inputBuffer, Buffer& outputBuffer)
{
	unsigned int nTotalSize = 4 + inputBuffer.getDataSize() + 2;

	if (outputBuffer.getRemainingSize() < nTotalSize)
	{
		return EncodeResult::InsufficientBufferSpace;
	}

	unsigned int sStart = SignatureStart;
	unsigned int packetLen = nTotalSize;
	outputBuffer.append((char*)&sStart, 2);
	outputBuffer.append((char*)&packetLen, 2);

	outputBuffer.append(inputBuffer);

	unsigned int sEnd = SignatureEnd;
	outputBuffer.append((char*)&sEnd, 2);


	return EncodeResult::Success;
}

DecodeResult::Type CSimpleProtocol::tryDecode(ProtocolContext& context, Buffer& outputBuffer)
{
	Buffer& inputbuffer = context.getDataBuffer();

	if (inputbuffer.getDataSize() < 6)
		return DecodeResult::WantMoreData;

	unsigned int sStart = 0;
	unsigned int packetLen = 0;
	unsigned int sEnd = 0;

	for (int i = 0; i<2; i++)
	{
		*((BYTE*)(&sStart) + i) = inputbuffer.getBuffer()[i];
		*((BYTE*)(&packetLen) + i) = inputbuffer.getBuffer()[i + 2];
	}

	if (sStart != SignatureStart)
		return DecodeResult::Failure;

	outputBuffer.clearBytes();

	if (packetLen > outputBuffer.getCapacity())
		return DecodeResult::Failure;


	if (inputbuffer.getDataSize() < packetLen)
		return DecodeResult::WantMoreData;

	for (int i = 0; i<2; i++)
		*((BYTE*)(&sEnd) + i) = inputbuffer.getBuffer()[packetLen - 2 + i];

	if (sEnd != SignatureEnd)
		return DecodeResult::Failure;

	outputBuffer.append(inputbuffer.getBuffer(4), packetLen - 6);

	inputbuffer.pop(packetLen);//TODO. verify this is correct.

	return DecodeResult::Content;
}

void CSimpleProtocol::startSession(ProtocolContext& context, Buffer& outgoingBytes)
{
	outgoingBytes.clearBytes();
	context.setInitialized();
}

ProtocolContext* CSimpleProtocol::createNewProtocolContext()
{
	return new ProtocolContext();
}

bool CSimpleProtocol::readData(ProtocolContext& context, Buffer& incomingBytes)
{
	return context.getDataBuffer().append(incomingBytes);
}
