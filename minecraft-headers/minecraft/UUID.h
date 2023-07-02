#pragma once

#include <string>

namespace mce {
    struct UUID {
        long long Data[2];

        static mce::UUID EMPTY;

        static mce::UUID fromString(std::string const&);
        static mce::UUID seedFromString(std::string const&);
        static bool canParse(std::string const&);

        std::string asString()const;
        bool isEmpty()const;
    };
}
