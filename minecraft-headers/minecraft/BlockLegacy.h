#pragma once

#include "HashedString.h"

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

	char padding[120];
	char unknown;
	char opacity;
	char brightness;
	int flameEncouragement;
	int flammability;
	float hardness;
	float blastResistance;
	char padding2[16];
	float friction;

	Block *getStateFromLegacyData(unsigned short) const;
	const NewBlockID getBlockID() const;

	std::string getFullName() const {
		return fullName.str;
	}

	float getDestroySpeed() const;

	virtual ~BlockLegacy();

    bool canBeOriginalSurface()const;
    bool canContainLiquid()const;
    bool canHurtAndBreakItem()const;
    bool canInstatick()const;
    bool isEmpty()const;
    bool isHeavy()const;
    bool isMotionBlockingBlock()const;
    bool isSolid()const;
    bool isSolidBlockingBlock()const;
    bool isStandingSign()const;
    bool isUnbreakable()const;
    bool isVanilla()const;
    bool isWaterBlocking()const;
    bool hasBlockEntity()const;
    int getCreativeCategory()const;
    int getBlockEntityType()const;
    short getBlockItemId()const;
    int getBurnOdds()const;
    int getRenderLayer()const;
    float getThickness()const;

};
