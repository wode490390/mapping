#pragma once

#include <string>
struct IDataOutput {
	virtual ~IDataOutput(void) = 0;
	virtual void writeString(std::string const&) = 0;
	virtual void writeLongString(std::string const&) = 0;
	virtual void writeFloat(float) = 0;
	virtual void writeDouble(double) = 0;
	virtual void writeByte(char) = 0;
	virtual void writeShort(short) = 0;
	virtual void writeInt(int) = 0;
	virtual void writeLongLong(int64_t) = 0;
	virtual void writeBytes(const void *, size_t) = 0;
};
