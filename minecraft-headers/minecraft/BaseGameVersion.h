#pragma once

#include <string>
#include "SemVersion.h"

struct BaseGameVersion : SemVersion {
    char padding[6];
    char unknown112;

    std::string asString()const;
};
