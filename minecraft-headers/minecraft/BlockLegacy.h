#pragma once

#include "HashedString.h"
#include "Color.h"

struct Block;

struct NewBlockID {
	NewBlockID(const NewBlockID&);

	unsigned short value;
};

struct BlockLegacy {

	std::string descriptionId;
	HashedString baseName;
	std::string namespaceName;
	HashedString fullName;

	char padding[172];
	char unknown560_288;
	char isWater;
    char isPumpkin;
    char notLava;
    char unused560_292;
    char mLightBlock;
    char mLightEmission;
	int flammability;
    int mBurnOdds;
	bool lavaFlammable;
	float hardness;
	float blastResistance;
    Color mMapColor;
	float friction;
    int unknown560_336;
    int id;

	Block *getStateFromLegacyData(unsigned short) const;
	const NewBlockID getBlockID() const;

	std::string getFullName() const {
		return fullName.str;
	}

	float getDestroySpeed() const;

	virtual ~BlockLegacy();

    Block *getRenderBlock()const;
    bool canBeOriginalSurface()const;
    bool canContainLiquid()const;
    bool canHurtAndBreakItem()const;
    bool canInstatick()const;
    bool isAir()const;
    bool isEmpty()const;
    bool isHeavy()const;
    bool isLavaFlammable()const;
    bool isMotionBlockingBlock()const;
    bool isSolid()const;
    bool isSolidBlockingBlock()const;
    bool isVanilla()const;
    bool isWaterBlocking()const;
    bool hasBlockEntity()const;
    bool shouldRandomTick()const;
    int getCreativeCategory()const;
    int getBlockEntityType()const;
    short getBlockItemId()const;
    int getBurnOdds()const;
    int getFlameOdds()const;
    int getRenderLayer()const;
    float getThickness()const;

};

static_assert(offsetof(BlockLegacy, isWater) == 341);
