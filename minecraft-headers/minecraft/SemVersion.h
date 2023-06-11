#pragma once

#include <string>

struct SemVersion {
    unsigned short mMajor;
    unsigned short mMinor;
    unsigned short mPatch;
    std::string mPreRelease;
    std::string mBuildMeta;
    std::string mFullVersionString;
    bool mValidVersion;
    bool mAnyVersion;

    SemVersion();

    bool isValid()const;
    bool isAnyVersion()const;
    unsigned short getMajor()const;
    unsigned short getMinor()const;
    unsigned short getPatch()const;
};
