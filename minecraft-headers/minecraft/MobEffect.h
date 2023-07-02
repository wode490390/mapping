#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include "HashedString.h"
#include "Color.h"

struct MobEffect {
    char padding[8];
    unsigned int mId;
    bool mIsHarmful;
    mce::Color mColor;
    std::string mDescriptionId;
    bool mIcon;
    float mDurationModifier;
    bool mIsDisabled;
    std::string mResourceName;
    std::string mIconName;
    bool mEffectVisible;
    HashedString mComponentName;

    static MobEffect *getById(unsigned int effectId);
    static MobEffect *getByName(std::string const&);
    static std::string getNameById(unsigned int effectId);
};
