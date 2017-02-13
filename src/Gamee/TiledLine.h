#ifndef TILED_LINE_H
#define TILED_LINE_H

#include "GroundLine.h"
#include "../Core/ogl/textures.h"

class TiledLine
    : public GroundLine
{
public:
    ~TiledLine();
    TiledLine(rapidxml::xml_node<> *xe);
    TiledLine(const TiledLine &l);
    virtual std::string Type();
    virtual void Draw();
    virtual void DebugDraw(bool onlyControl);
    virtual void SaveToXml(rapidxml::xml_node<> *xe);
    virtual bool Command(const std::string &cmd);

    virtual void MouseDown(const FPoint &mouse);
    virtual bool MouseMove(const FPoint &mouse);
    virtual void MouseUp(const FPoint &mouse);
    virtual bool Selection(const Rect& rect, bool full = true);
    virtual const char *GetIconTexture();
    virtual bool PixelCheck(const FPoint &point);
    virtual void EncapsulateAllDots(Rect &rect);
    virtual bool GeometryCheck(const FPoint &point);
    virtual void GetAllLocalDotsRect(Rect &rect);
    virtual void SetColor(DWORD color) { _color = color; }
    bool Mesh() { return _mesh.GetVB().Size() > 0; }
    virtual const std::string &GetTexturePath() { return _fileName; }
private:
    void SaveMeshToXml(VertexBuffer &vb, rapidxml::xml_node<> *xe);
    void DrawWithParam(VertexBuffer *mesh);
    FPoint _mousePos;
    Matrix parent;
    GLTexture2D *_texture;
    std::string _fileName;
    Sprite _mesh;
    Sprite _temporaryMesh;//     
    QVector<FPoint> _screenDots; 

    bool _mouseDown;
    QVector<int> _dotUnderCursor;
    QVector<int> _selectedDots;

    QVector<int> SearchNearest(float x, float y);
};

#endif//TILED_LINE_H

