#pragma once

namespace Core {
    struct Random {
        unsigned int mSeed;

        Random(unsigned int seed, bool);

        void _setSeed(unsigned int seed)const;
        unsigned int _genRandInt32()const;
        unsigned int _genUniformRandomInt()const;
        int nextInt()const;
        int nextInt(int n)const;
        long long nextLong()const;
        bool nextBoolean()const;
        float nextFloat()const;
        double nextDouble()const;
        float nextGaussian()const;
        double nextGaussianDouble()const;
        void consumeCount(unsigned int)const;
        Random *fork()const;
    };
}
