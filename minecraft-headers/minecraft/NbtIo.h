#pragma once

#include <memory>

#include "CompoundTag.h"

struct NbtIo {
//	static std::unique_ptr<CompoundTag> read(IDataInput &);
	static void write(const CompoundTag *, IDataOutput &);
};
