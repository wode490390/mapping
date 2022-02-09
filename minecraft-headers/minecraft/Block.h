#pragma once

#include "BlockLegacy.h"
#include "CompoundTag.h"

typedef CompoundTag BlockSerializationId;

struct Block {
	const BlockLegacy & getLegacyBlock() const {
		return *blockLegacy;
	}

	float getDestroySpeed() const;

	virtual ~Block() {}

	unsigned short data;
	BlockLegacy* blockLegacy;

	char filler2[56];
	BlockSerializationId tag;
};

static_assert(offsetof(Block, tag) == 80);
