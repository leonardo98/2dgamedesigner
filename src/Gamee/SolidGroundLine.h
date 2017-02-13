#ifndef SOLID_GROUND_LINE_H
#define SOLID_GROUND_LINE_H

#include "../Core/SplinePath.h"
#include "../Core/ogl/render.h"
#include "BeautyBase.h"

#define SCALE_BOX2D 60

class Besier {
public:
    QVector<FPoint> origin;
    void Path(float t, FPoint &pos);
    void Recalc(QVector<FPoint> &screen, int sectorNumber);
    void RecalcWithNumber(QVector<FPoint> &screen, int dotNumber);
    void ShiftPoint(unsigned int index, const FPoint &shift);
    float Length();
};

class SolidGroundLine : public BeautyBase
{

private:    
    
    int _sectionNumber;
    bool _drawTrianglesCoves;
    FPoint _mousePos;
    Matrix parent;
    int _width;
    int _height;
    DWORD _debugColor;
    void CalcWidthAndHeight();
    Sprite _triangles;

    QVector<FPoint> _screenDots; 
    QVector<FPoint> _renderLineDots;

    virtual void ExportToLines(QVector<FPoint> &lineDots);

    int SearchNearest(float x, float y);
    int SearchNearestLine(float x, float y);
    int SearchNearestWorld(float x, float y);
    int SearchNearestLineWorld(float x, float y);
    int CreateDot(float x, float y);
    void RemoveDot(int index);

    bool _mouseDown;
    int _dotUnderCursor;
    int _dotLastEdited;
    void GenerateTriangles();

public:

    virtual ~SolidGroundLine();
    SolidGroundLine(rapidxml::xml_node<> *xe);
    SolidGroundLine(const SolidGroundLine &l);

    virtual void Draw();
    virtual void DebugDraw(bool onlyControl);
    virtual void Update(float dt) {}
    virtual void MouseDown(const FPoint &mouse);
    virtual bool MouseMove(const FPoint &mouse);
    virtual void MouseUp(const FPoint &mouse);
    virtual bool Command(const std::string &cmd);
    virtual bool PixelCheck(const FPoint &point);
    virtual void EncapsulateAllDots(Rect &rect);
    virtual void GetAllLocalDotsRect(Rect &rect);

    virtual void SaveToXml(rapidxml::xml_node<> *xe);
    virtual std::string Type();

    virtual int Width();
    virtual int Height();

    virtual const char *GetIconTexture();

private:
    std::string _icon;
    void CollectAllBaseDots(Rect &rect);

protected:
    Besier _besier;
    Matrix parentMatrix;
};

#endif//SOLID_GROUND_LINE_H
