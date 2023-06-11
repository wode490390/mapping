#pragma once
#include <functional>
#include "Block.h"
#include "BlockLegacy.h"
#include "HashedString.h"
#include "Memory.h"

struct BlockTypeRegistry{
	static void forEachBlock(std::function<bool (const BlockLegacy &)>);
	static WeakPtr<BlockLegacy> lookupByName(const HashedString &, bool);
	static const Block * lookupByName(const HashedString &, int, bool);
};
