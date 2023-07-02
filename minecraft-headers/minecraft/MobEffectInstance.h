#pragma once

#include <string>
#include "HashedString.h"
#include "CompoundTag.h"

struct MobEffectInstance {
    unsigned int mId;
    int mDuration;
    int mDurationEasy;
    int mDurationNormal;
    int mDurationHard;
    int mAmplifier;
    bool mDisplayOnScreenTextureAnimation;
    bool mAmbient;
    bool mNoCounter;
    bool mEffectVisible;

    MobEffectInstance(MobEffectInstance const&);
    MobEffectInstance(MobEffectInstance &&);
    MobEffectInstance(unsigned int id);
    MobEffectInstance(unsigned int id, int duration);
    MobEffectInstance(unsigned int id, int duration, int amplifier);
    MobEffectInstance(unsigned int id, int duration, int amplifier, bool ambient, bool effectVisible, bool displayAnimation);
    MobEffectInstance(unsigned int id, int duration, int durationEasy, int durationNormal, int durationHard, int amplifier, bool ambient, bool effectVisible, bool displayAnimation);

    void update(const MobEffectInstance *takeOver)const;
    void setDuration(int duration)const;
    int getAmplifier()const;
    float getSplashDuration()const;
    float getLingerDuration()const;
    MobEffectInstance *getSplashEffect()const;
    bool isAmbient()const;
    bool isEffectVisible()const;
    std::string getDescriptionId()const;
    std::string getDisplayName()const;
    HashedString getComponentName()const;
    int getDifficulityDuration(int difficulty)const;
    void setDifficulityDuration(int difficulty, int duration)const;
    CompoundTag* *save()const;
    MobEffectInstance *load(const CompoundTag *tag)const;
    void tryInitializeMobEffectData()const;
};
