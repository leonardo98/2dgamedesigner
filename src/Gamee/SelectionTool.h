#ifndef SELECTION_TOOL_H
#define SELECTION_TOOL_H

#include "../Core/ogl/render.h"
#include "../Core/rect.h"

class SelectionTool 
{
private:
    bool _mouseDown;
    bool _isSelection;
    FPoint _startPoint;
    Rect _area;
public:
    SelectionTool();
    bool OnMouseDown(const FPoint &mousePos);
    void OnMouseUp(const FPoint &mousePos);
    void OnMouseMove(const FPoint &mousePos);
    bool IsMouseOver(const FPoint &mousePos);
    void Draw();
};

#endif//SELECTION_TOOL_H
