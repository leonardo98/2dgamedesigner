#include "ClickArea.h"
#include "../Core/Math.h"
#include "../Core/Core.h"
#include "../Core/Messager.h"
#include "TileEditorInterface.h"

ClickArea::~ClickArea() {
}

ClickArea::ClickArea(const ClickArea &c) 
: BeautyBase(c)
{
    _icon = Core::programPath + "gfx/clickarea.png";
    _dots = c._dots;
    CalcWidthAndHeight();
    _mouseDown = false;
    _dotUnderCursor = _dotLastEdited = -1;
}

ClickArea::ClickArea(rapidxml::xml_node<> *xe)
: BeautyBase(xe)
{
    _icon = Core::programPath + "gfx/clickarea.png";
    rapidxml::xml_node<> *dot = xe->first_node("dot");
    while (dot != NULL) {
        _dots.push_back(FPoint(atof(dot->first_attribute("x")->value()), atof(dot->first_attribute("y")->value())));
        dot = dot->next_sibling();
    }
    CalcWidthAndHeight();
    _mouseDown = false;
    _dotUnderCursor = _dotLastEdited = -1;
}

void ClickArea::Draw() {

    Render::PushMatrix();
    Render::MatrixMove(_pos.x, _pos.y);
    parent = Render::GetCurrentMatrix();

    _screenDots = _dots;
    for (int i = 0; i < _dots.size(); ++i) {
        Render::GetCurrentMatrix().Mul(_screenDots[i]);    
        Render::Line(_dots[i].x, _dots[i].y, _dots[(i + 1) % _dots.size()].x, _dots[(i + 1) % _dots.size()].y, 0xFFFFFFFF);
    }

    Render::PopMatrix();
    BeautyBase::Draw();
}

void ClickArea::DebugDraw(bool onlyControl) {
    if (!onlyControl) {
        Draw();
    }
    {
        _debugDraw = true;
    
        Render::PushMatrix();

        Render::MatrixMove(_pos.x, _pos.y);
        for (int i = 0; i < _dots.size(); ++i) {
            Render::Line(_dots[i].x, _dots[i].y, _dots[(i + 1) % _dots.size()].x, _dots[(i + 1) % _dots.size()].y, 0xFFFFFFFF);
        }

        Render::SetMatrixUnit();

        float alpha = (Render::GetColor() >> 24) / 255.f;
        Render::SetAlpha(Math::round(0xAF * alpha));
        for (int i = 0; i < _screenDots.size(); ++i) {
            if ((i == _dotUnderCursor || i == _dotLastEdited) && TileEditorInterface::Instance()->CreateDotMode()) {
                scale->Render(_screenDots[i].x - scale->Width() / 2.f, _screenDots[i].y - scale->Height() / 2.f);
            } else {
                scaleSide->Render(_screenDots[i].x - scaleSide->Width() / 2.f, _screenDots[i].y - scaleSide->Height() / 2.f);
            }
        }
        Render::SetAlpha(Math::round(0xFF * alpha));

        Render::PopMatrix();
    }
}

bool ClickArea::PixelCheck(const FPoint &point) { 
    if (Math::Inside(point - _pos, _dots)) {
        return true;
    }
    return SearchNearest(point.x, point.y) >= 0;
}

void ClickArea::SaveToXml(rapidxml::xml_node<> *xe) {
    BeautyBase::SaveToXml(xe);
    for (int j = 0; j < _dots.size(); ++j) {
        rapidxml::xml_node<> *dot = xe->document()->allocate_node(rapidxml::node_element, "dot");
        xe->append_node(dot);
        Math::Write(dot, "x", _dots[j].x);
        Math::Write(dot, "y", _dots[j].y);
    }
}

std::string ClickArea::Type() { 
    return "ClickArea"; 
}

int ClickArea::Width() {
    return _width;
}

int ClickArea::Height() {
    return _height;
}

void ClickArea::CalcWidthAndHeight() {
    Rect rect;
    rect.Clear();
    for (unsigned int i = 0; i < _dots.size(); ++i) {
        rect.Encapsulate(_dots[i].x, _dots[i].y);
    }
    _width = fabs(rect.x2 - rect.x1);
    _height = fabs(rect.y2 - rect.y1);
}

int ClickArea::SearchNearest(float x, float y) {
    int result = -1;
    static const float SIZEX = 20;
    FPoint p(x, y);
    for (unsigned int i = 0; i < _screenDots.size() && result < 0; ++i) {
        if ((_screenDots[i] - p).Length() < SIZEX) {
            result = i;
        }
    }
    return result;
}

int ClickArea::CreateDot(float x, float y) {
    if (_dots.size() >= 50) {
        return false;
    }
    int result = -1;
    static const float SIZEX = 6;
    FPoint p(x, y);
    for (unsigned int i = 0; i < _dots.size() && result < 0; ++i) {
        if (Math::DotNearLine(_dots[i], _dots[(i + 1) % _dots.size()], p)) {
            unsigned int index = (i + 1) % _dots.size();
            if (index < _dots.size()) {
                _dots.insert(_dots.begin() + index, p);
                result = index;
            } else {
                _dots.push_back(p);
                result = _dots.size() - 1;
            }
        }
    }
    return result;
}

void ClickArea::RemoveDot(int index) {
    if (_dots.size() <= 3) {
        return;
    }
    _dots.erase(_dots.begin() + index);
}

void ClickArea::MouseDown(const FPoint &mouse) {
    _dotLastEdited = -1;

    if (!_debugDraw) {
        return;
    }
    if (!TileEditorInterface::Instance()->CreateDotMode()) {
        _dotLastEdited = _dotUnderCursor;
    }
        
    _mouseDown = true;
    _mousePos = mouse;
}

bool ClickArea::MouseMove(const FPoint &mousePos) {
    if (!TileEditorInterface::Instance()->CreateDotMode() && _mouseDown) {
        Matrix reverse;
        reverse.MakeRevers(parent);

        FPoint start(_mousePos);
        FPoint end(mousePos);
        reverse.Mul(start);
        reverse.Mul(end);

        _pos.x += (end.x - start.x);
        _pos.y += (end.y - start.y);
        _mousePos = mousePos;
        return true;
    }

    if (!_debugDraw || !_mouseDown) {
        _dotUnderCursor = SearchNearest(mousePos.x, mousePos.y);
        _mousePos = mousePos;
        return false;
    }
    Matrix reverse;
    reverse.MakeRevers(parent);

    FPoint start(_mousePos);
    FPoint end(mousePos);
    reverse.Mul(start);
    reverse.Mul(end);

    if (_dotUnderCursor < 0) {
        _pos.x += (end.x - start.x);
        _pos.y += (end.y - start.y);
    } else {
        _dots[_dotUnderCursor].x += (end.x - start.x);
        _dots[_dotUnderCursor].y += (end.y - start.y);
        CalcWidthAndHeight();
    }
    _mousePos = mousePos;
    return true;
}

void ClickArea::MouseUp(const FPoint &mouse) {
    _mouseDown = false;
    _dotUnderCursor = -1;
    _mousePos = mouse;
}

const char *ClickArea::GetIconTexture() {
    return _icon.c_str();
}

bool ClickArea::Command(const std::string &cmd) {
    std::string position;

    if (cmd == "delete dot") {
        
        if (_dotLastEdited >= 0) {
            RemoveDot(_dotLastEdited);
            _dotLastEdited = -1;
            _dotUnderCursor = -1;
        }

        return true;
    } else if (Messager::CanCut(cmd, "create dot at ", position)) {

        FPoint mouse;
        sscanf(position.c_str(), "%f %f", &(mouse.x), &(mouse.y));

        int index = SearchNearest(mouse.x, mouse.y);
        if (index >= 0) {
            _dotUnderCursor = _dotLastEdited = index;
            _mouseDown = true;
        } else {
            Matrix reverse;
            reverse.MakeRevers(parent);
            FPoint fp(mouse);
            reverse.Mul(fp);

            _dotUnderCursor = _dotLastEdited = CreateDot(fp.x, fp.y);
            _mouseDown = _dotLastEdited >= 0;
        }
        _mousePos = mouse;
        return _mouseDown;
    }
    return (cmd == "" || BeautyBase::Command(cmd));
}

void ClickArea::EncapsulateAllDots(Rect &rect) 
{
    for (unsigned int i = 0; i < _screenDots.size(); ++i) {
        //rect.Encapsulate(_screenDots[i].x,_screenDots[i] .y);
        rect.Encapsulate(_pos.x + _dots[i].x, _pos.y + _dots[i].y);
    }
}

void ClickArea::GetAllLocalDotsRect(Rect &rect) 
{
    for (unsigned int i = 0; i < _dots.size(); ++i) {
        rect.Encapsulate(_pos.x + _dots[i].x, _pos.y + _dots[i].y);
    }
}

