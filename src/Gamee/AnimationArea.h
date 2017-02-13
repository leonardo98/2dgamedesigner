#ifndef ANIMATION_AREA_H
#define ANIMATION_AREA_H

#include "../2dga_api/Animation.h"
#include "BeautyBase.h"
#include "../Core/ogl/sprite.h"

class AnimationArea : public BeautyBase
{
public:

    virtual ~AnimationArea();
    AnimationArea(const AnimationArea &b);
    AnimationArea(rapidxml::xml_node<> *xe);

    virtual void Draw();
    virtual void DebugDraw(bool onlyControl);
    virtual void Update(float dt);
    virtual bool PixelCheck(const FPoint &point);
    virtual bool Command(const std::string &cmd = "");
    virtual void EncapsulateAllDots(Rect &rect);
    virtual void GetAllLocalDotsRect(Rect &rect);
    Animation *GetAnimation() { return _animation; }

    virtual void SaveToXml(rapidxml::xml_node<> *xe);
    virtual std::string Type();

    const std::string &Name() const { return _id; }

    virtual int Width();
    virtual int Height();

    virtual const char *GetIconTexture();
    virtual float SomeFloatValue();
    virtual const std::string &GetTexturePath() { return _animation->GetTetxurePath(); }

private:
    std::string _id;
    std::string _lib;
    Animation *_animation;
    bool _play;
    float _time;
    float _startTime;
    int _width;
    int _height;
    int _shiftX;
    int _shiftY;
    void CalcWidthAndHeight();
};


#endif//ANIMATION_AREA_H
