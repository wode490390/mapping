#pragma once

#include <memory>
#include <vector>
#include "Color.h"

struct Material {
    static std::vector<std::unique_ptr<Material>> mMaterials;

    int mType;
    bool mNeverBuildable;
    bool mAlwaysDestroyable;
    bool mLiquid;
    float mTranslucency;
    bool mBlocksMotion;
    bool mBlocksPrecipitation;
    bool mSolid;
    bool mSuperHot;

    bool isSolidBlocking()const;
    bool isSuperHot()const;
    bool isTopSolid(bool includeWater, bool includeLeaves)const;
};
