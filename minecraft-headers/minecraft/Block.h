#pragma once

#include "BlockLegacy.h"
#include "CompoundTag.h"
#include "Material.h"

typedef CompoundTag BlockSerializationId;

struct Block {
	const BlockLegacy & getLegacyBlock() const {
		return *blockLegacy;
	}

	float getDestroySpeed() const;

	virtual ~Block() {}

	unsigned short data;
	BlockLegacy* blockLegacy;

	char filler2[184];
	BlockSerializationId tag;
	char filler3[32];

    Material *getMaterial()const;
};
