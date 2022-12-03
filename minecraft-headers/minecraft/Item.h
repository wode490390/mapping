#pragma once

#include <string>

struct Item {
	char padding[552];

//	virtual ~Item() {}
	const std::string & getFullItemName(void) const;

    bool canBeDepleted()const;
    bool isCamera()const;
    bool isDamageable()const;
    bool isElytra()const;
    bool isExplodable()const;
    bool isFireResistant()const;
    bool isHandEquipped()const;
    bool isNameTag()const;
    bool isSeed()const;
    bool isStackedByData()const;
    int getCooldownTime()const;
    HashedString getCooldownType()const;
    short getId()const;
    short getMaxDamage()const;
    int getCreativeCategory()const;

};
