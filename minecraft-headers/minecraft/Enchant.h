#pragma once

#include <vector>
#include <memory>
#include "HashedString.h"

struct Enchant {
    char padding[8];
    char mEnchantType;
    int mFrequency;
    bool mIsLootable;
    int mPrimarySlots;
    int mSecondarySlots;
    int mCompatibility;
    std::string mDescription;
    HashedString mStringId;
    bool mIsDisabled;

    static std::vector<std::unique_ptr<Enchant>> mEnchants;

    bool isDisabled()const;
    bool isAvailable()const;
    bool isCompatibleWith(char type)const;
};
