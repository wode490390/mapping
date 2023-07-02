#pragma once

#include <map>
#include <string>

struct BlockActor {
    static std::map<std::string, int> mIdClassMap;
    static std::map<int, std::string> mClassIdMap;
};
