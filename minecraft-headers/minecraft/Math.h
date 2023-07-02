#pragma once

namespace mce {
    struct Math {
        static int clamp(int v, int low, int high);
        static float clamp(float v, float low, float high);
        static float absMax(float a, float b);
        static float approach(float current, float target, float increment);
        static float approachDegrees(float current, float target, float increment);
        static float atan(float x);
        static float atan2(float dy, float dx);
        static int ceil(float v);
        static float clampRotate(float current, float target, float maxDelta);
        static float clampedLerp(float min, float max, float factor);
        static float clampedMap(float, float, float, float, float);
        static float cos(float f);
        static float cos(float *radian);
        static float degreesDifferenceAbs(float angleA, float angleB);
        static bool doubleEqualsEpsilon(double a, double b);
        static float fastInvSqrt(float x);
        static unsigned int fastRandom();
        static bool floatEqualsEpsilon(float a, float b);
        static int floor(float v);
        static float hermiteBlend(float t);
        static float invSqrt(float x);
        static float inverseLerp(float, float, float);
        static bool isNan(float f);
        static bool isNan(double);
        static bool isWrappedRotationEpsilonEqual(float, float, float);
        static float lerpRotate(float from, float to, float a);
        static float map(float, float, float, float, float);
        static void safeIncrement(int *i);
        static float signum(float a);
        static float sin(float f);
        static float sin(float *radian);
        static float snapRotationToCardinal(float deg);
        static float sqrt(float x);
        static float wrapDegrees(float input);
        static float wrapDegrees(float *degrees);
        static float wrapRadians(float angle);
    };
}
