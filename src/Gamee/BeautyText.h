#ifndef BEAUTYTEXT_H
#define BEAUTYTEXT_H

#include "BeautyBase.h"
#include "../Core/CoreFont.h"

class BeautyText
    : public BeautyBase
{
public:

    void SetText(const std::string &text);
    virtual ~BeautyText();
    BeautyText(const BeautyText &b);
    BeautyText(rapidxml::xml_node<> *xe);
    BeautyText(const std::string &imagePath);
    BeautyText(const GLTexture2D *texture, float x, float y, float width, float height);

    virtual void Draw();
    virtual void DebugDraw(bool onlyControl);
    virtual void Update(float ) {}
    virtual bool PixelCheck(const FPoint &point);
    virtual bool GeometryCheck(const FPoint &point);
    virtual void EncapsulateAllDots(Rect &rect);
    virtual void GetAllLocalDotsRect(Rect &rect);
    virtual bool Command(const std::string &cmd);

    virtual void SaveToXml(rapidxml::xml_node<> *xe);
    virtual std::string Type();

    virtual int Width();
    virtual int Height();

    virtual const char *GetIconTexture();
    virtual const std::string &GetTexturePath() { return _fileName; }
    int Align() { return _align; }
    void SetVertInterval(float value);
    void SetHorSpacing(float value);
    float VertInterval() { return _hInterval; }
    float HorSpacing() { return _vSpacing; }
    const std::string &Text() { return _text; }
    std::string SaveString();

private:
    std::string _fileName;
    std::string _icon;
    std::string _text;
    int _align;
    float _hInterval;
    float _vSpacing;
    CoreFont *_font;
    float _width;
    float _height;
};

#endif // BEAUTYTEXT_H
