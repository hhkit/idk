#include "stdafx.h"
#include "easing.h"

namespace idk::easing
{

    float linear(float t)
    {
        return t;
    }

    float sine_in(float t)
    {
        return -cosf(half_pi * t) + 1.0f;
    }

    float sine_out(float t)
    {
        return sinf(half_pi * t);
    }

}