#ifndef BEAUTY_H
#define BEAUTY_H

#include "BeautyBase.h"

class Beauty : public BeautyBase
{
public:

    virtual ~Beauty();
    Beauty(const Beauty &b);
    Beauty(rapidxml::xml_node<> *xe);
    Beauty(const std::string &imagePath);
    Beauty(const GLTexture2D *texture, float x, float y, float width, float height);

    virtual void Draw();
    virtual void DebugDraw(bool onlyControl);
    virtual void Update(float ) {}
    virtual bool PixelCheck(const FPoint &point);
    virtual bool GeometryCheck(const FPoint &point);
    virtual void EncapsulateAllDots(Rect &rect);
    virtual void GetAllLocalDotsRect(Rect &rect);

    virtual void SaveToXml(rapidxml::xml_node<> *xe);
    virtual std::string Type();

    virtual int Width();
    virtual int Height();

    virtual const char *GetIconTexture();
    virtual const std::string &GetTexturePath() { return _fileName; }

private:
    std::string _fileName;
    Sprite *_sprite;
};


#endif//BEAUTY_H
