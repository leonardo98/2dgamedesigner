/*
** 2D Game Animation HGE api 
** by Pakholkov Leonid am98pln@gmail.com
** Класс функции от времени
** позволяет задать список значений, которые должна принимать функция
** промежуточные значения будут расчитаны, либо линейно, либо сглаженно
**
*/

#ifndef MOTIONVALUES_H
#define MOTIONVALUES_H

#include <vector>

inline float LineMotion(float x1, float x2, float t)
{
    return x1 + (x2 - x1) * t;
}

inline float SplineMotion(float x1, float x2, float r1, float r2, float t)
{
    float res;
    float t2 = t*t;
    float t3 = t2*t;

    res = x1*(2.0f*t3-3.0f*t2+1.0f)+r1*(t3-2.0f*t2+t)+x2*(-2.0f*t3+3.0f*t2)+r2*(t3-t2);
    return res;
}

class MotionValues
{

public:

    enum Motion {
        m_line,
        m_spline,
        m_discontinuous
    };

private:

    Motion _type;

public:

    struct TimedKey {
        float time;
        float value;
        float gradient;
    };

    typedef std::vector<TimedKey> Keys;
    Keys keys;
    
    MotionValues()
    {
    }

    void Clear()
    {
        keys.clear();
    }

    float AddKey(float time, float value);

    int Value(float t, float &localT)
    {
        if (keys.front().time > t || t > keys.back().time) {
            return -1;
        }
        Keys::size_type start = 0;
        Keys::size_type end = keys.size() - 2;
        Keys::size_type middle = (start + end) / 2;
        while (!(keys[middle].time <= t && t <= keys[middle + 1].time)) {
            if (keys[middle].time > t) {
                end = middle;
            } else if (keys[middle + 1].time < t) {
                start = middle + 1;
            }
            middle = (start + end) / 2;
        }

        localT = (t - keys[middle].time) / (keys[middle + 1].time - keys[middle].time);
        return middle;
    }

    void SetType(Motion type)
    {        
        if (keys.size() == 1) {
            // для совместимости со старым форматом, 
            // который допускал наличие одного ключа
            AddKey(1.f, keys[0].value);
        };
        _type = type;
        if (_type == m_spline) {
            float g1, g2, g3;

            if (keys.size() > 1) {
                keys[0].gradient = keys[1].value - keys[0].value;
                keys[keys.size() - 1].gradient = keys[keys.size() - 1].value - keys[keys.size() - 2].value;
            }

            for (Keys::size_type i = 1; i < (keys.size()-1); i++) {
                g1 = keys[i].value - keys[i - 1].value;
                g2 = keys[i + 1].value - keys[i].value;
                g3 = g2 - g1;
                keys[i].gradient = g1 + 0.5f * g3;
            }
        }
    }

    float GetFrame(Keys::size_type i, float t)
    {
        if (_type == m_line) {
            return LineMotion(keys[i].value, keys[i+1].value, t);
        } else if (_type == m_discontinuous) {
            return keys[i].value;
        } else {
            return SplineMotion(keys[i].value, keys[i+1].value, keys[i].gradient, keys[i+1].gradient, t);
        }
    }

};

#endif//MOTIONVALUES_H
