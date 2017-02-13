#ifndef LINK_TO_COMPLEX_H
#define LINK_TO_COMPLEX_H

#include "LevelSet.h"
#include "BeautyBase.h"

class LinkToComplex : public BeautyBase
{
private:
    std::string _id;

    LevelSet *_complex;

    int _width;
    int _height;
    int _shiftX;
    int _shiftY;

public:

    virtual ~LinkToComplex();
    LinkToComplex(const std::string &levelName);
    LinkToComplex(const LinkToComplex &b);
    LinkToComplex(rapidxml::xml_node<> *xe);

    virtual void Draw();
    virtual void DebugDraw(bool onlyControl);
    virtual void Update(float dt);
    virtual bool PixelCheck(const FPoint &point);
    virtual void EncapsulateAllDots(Rect &rect);
    virtual void GetAllLocalDotsRect(Rect &rect);
    LevelSet *GetComplex() { return _complex; }

    virtual void SaveToXml(rapidxml::xml_node<> *xe);
    virtual std::string Type();
    void ReInit();

    const std::string &Name() const { return _id; }

    virtual int Width();
    virtual int Height();

    virtual const char *GetIconTexture();
    void CalcWidthAndHeight();
    void MakeCopyOfBeauties(BeautyList &beautyList)
    {
        _complex->MakeCopyOfBeauties(beautyList);
        for (unsigned int i = 0; i < beautyList.size(); ++i) {
            beautyList[i]->ShiftTo(_pos.x + _shiftX, _pos.y + _shiftY);
        }
    }

};


#endif//LINK_TO_COMPLEX_H
