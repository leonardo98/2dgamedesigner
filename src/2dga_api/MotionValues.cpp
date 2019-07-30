#include "../Core/MotionValues.h"
#include "Animation.h"

float MotionValues::AddKey(float time, float value)
{
    if (!(keys.size() == 0 || time > (keys.back().time + 1e-5)))
    {
        //Animation::errMessage = "check key's time - next is less then previos";
    }
    TimedKey key;
    key.time = time;
    key.value = value;
    keys.push_back(key);
    return time;
}
