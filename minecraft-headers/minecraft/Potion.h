#pragma once

#include <string>
#include <vector>
#include "MobEffectInstance.h"

struct Potion {
    int mId;
    std::string mNameId;
    std::string mPrefix;
    std::vector<MobEffectInstance> mEffects;
    std::vector<std::string> mDescriptionIds;
    int mVar;

    static unsigned int getPotionCount();
    static unsigned int getPotionVariant();
    static const std::shared_ptr<Potion const> getPotion(int potionId);

    std::string getDescriptionId()const;
    unsigned int getMobEffectId()const;
};
