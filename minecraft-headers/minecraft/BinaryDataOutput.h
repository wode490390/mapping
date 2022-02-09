#pragma once

#include <string>
#include "BinaryStream.h"
#include "BytesDataOutput.h"

struct BinaryDataOutput : public BytesDataOutput {
	BinaryStream *stream;

	BinaryDataOutput(){}
	BinaryDataOutput(BinaryStream &output);
	virtual void writeString(std::string const&);
	virtual void writeLongString(std::string const&);
	virtual void writeFloat(float);
	virtual void writeDouble(double);
	virtual void writeByte(char);
	virtual void writeShort(int16_t);
	virtual void writeInt(int32_t);
	virtual void writeLongLong(int64_t);
	virtual void writeBytes(const void *, size_t);
};
