#pragma once

namespace mce {
    struct Color {
        float r;
        float g;
        float b;
        float a;

        static Color fromHexString(std::string const&);

        int toARGB()const;
        int toABGR()const;
        std::string toHexString()const;
    };
}
