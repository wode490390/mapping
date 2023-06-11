#pragma once

#include "HashedString.h"
#include "Color.h"
#include "BaseGameVersion.h"

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

	char padding[168];
	char unknown560_288;
	char isWater;
    char isPumpkin;
    char notLava;
    char unused560_292;
    unsigned char mLightBlock;
    unsigned char mLightEmission;
    /// mFlameOdds
	int flammability;
    int mBurnOdds;
	bool lavaFlammable;
    /// mDestroySpeed
	float hardness;
    /// mExplosionResistance
	float blastResistance;
    mce::Color mMapColor;
    /// mFriction
	float friction;
    int unknown560_336;
    char isUnknownBlock;
    unsigned short id;
    BaseGameVersion mMinRequiredBaseGameVersion;
    bool mIsVanilla;

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
    float getExplosionResistance()const;

};

static_assert(offsetof(BlockLegacy, isWater) == 337);
