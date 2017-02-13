#ifndef GROUNDLINE_H
#define GROUNDLINE_H

#include "BeautyBase.h"

//
// 
//   
//

class GroundLine : public BeautyBase
{
private:

    int _width;
    int _height;
    void CalcWidthAndHeight();

    QVector<FPoint> _screenDots; 
    bool _mouseDown;
    int _dotUnderCursor;
    int _dotLastEdited;

    int SearchNearest(float x, float y);
    int SearchNearestLine(float x, float y);
    int SearchNearestWorld(float x, float y);
    int SearchNearestLineWorld(float x, float y);

protected:
    QVector<FPoint> _dots; //   
    DWORD _debugColor;

public:

    virtual ~GroundLine();
    GroundLine(rapidxml::xml_node<> *xe);
    GroundLine(const GroundLine &c);

    virtual void Draw();
    virtual void DebugDraw(bool onlyControl);
    virtual void Update(float dt) {}
    virtual void MouseDown(const FPoint &mouse);
    virtual bool MouseMove(const FPoint &mouse);
    virtual void MouseUp(const FPoint &mouse);
    virtual bool PixelCheck(const FPoint &point);
    virtual void EncapsulateAllDots(Rect &rect);
    virtual void GetAllLocalDotsRect(Rect &rect);
    virtual bool Command(const std::string &cmd);
    virtual void SetColor(DWORD color) { _debugColor = color; }

    virtual void SaveToXml(rapidxml::xml_node<> *xe);
    virtual std::string Type();

    virtual int Width();
    virtual int Height();

    virtual const char *GetIconTexture();
private:
    std::string _icon;
};


#endif//GROUNDLINE_H
