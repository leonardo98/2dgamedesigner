#include <QApplication>
#include "BeautyBase.h"
#include "../Core/ogl/render.h"
#include "../Core/Core.h"
#include "../Core/Math.h"
#include "../Core/Messager.h"
#include "TileEditorInterface.h"
#include "../Core/InputSystem.h"
#include <algorithm>

const std::string &BeautyBase::GetTexturePath()
{
    static std::string s;
    return s;
}

BeautyBase::~BeautyBase() {
    delete square;
    square = NULL;
    delete center;
    center = NULL;
    delete scale;
    scale = NULL;
    delete scaleSide;
    scaleSide = NULL;
    delete rotate;
    rotate = NULL;

}

BeautyBase::BeautyBase(const BeautyBase &b)
{
    _needRegenerateIcon = false;

    _scalar = b._scalar;

    _state = edit_none;

    _visible = b._visible;
    _pos = b._pos;
    _color = b._color;
    _pointForAnything = b._pointForAnything;
    
    _angle = b._angle;

    _sx = b._sx;
    _sy = b._sy;

    _name = b._name;
    _tags = b._tags;
    _uid = b._uid;

    _canBeRotated = b._canBeRotated;
    _canBeScaled = b._canBeScaled;

    _debugDraw = false;

    InitCorners();
    _mouseDown = false;
}


BeautyBase::BeautyBase(rapidxml::xml_node<> *xe)
{
    _needRegenerateIcon = false;

    _state = edit_none;
    _visible = true;

    rapidxml::xml_attribute<> *attr = xe->first_attribute("param");
    if (attr)
    {
        int r;
        int s;
        int t1, t2;
        sscanf(attr->value(), "%f,%f,%f,%f,%f,%f,%f,%f,%i,%i,%i,%i,%x"
                , &_scalar, &(_pos.x), &(_pos.y)
                , &(_pointForAnything.x), &(_pointForAnything.y)
                , &_angle, &_sx, &_sy
                , &r, &s
                , &t1, &t2
                , &_color);

        _canBeRotated = (r == 1);
        _canBeScaled = (s == 1);
    }
    else
    {
        _pos.x = atof(xe->first_attribute("x")->value());
        _pos.y = atof(xe->first_attribute("y")->value());
        _pointForAnything.x = Math::Read(xe, "pax", 0.f);
        _pointForAnything.y = Math::Read(xe, "pay", 0.f);
        rapidxml::xml_attribute<> *tmp = xe->first_attribute("scalar");
        _scalar = (tmp != NULL) ? atof(tmp->value()) : 0.f;

        tmp = xe->first_attribute("color");
        _color = tmp ? Math::ReadColor(tmp->value()) : 0xFFFFFFFF;

        _angle = Math::Read(xe, "angle", 0.f);

        _sx = Math::Read(xe, "sx", 1.f);
        _sy = Math::Read(xe, "sy", 1.f);

        _canBeRotated = Math::Read(xe, "canBeRotated", false);
        _canBeScaled = Math::Read(xe, "canBeScaled", false);
    }

    _debugDraw = false;

    InitCorners();

    rapidxml::xml_attribute<> *name = xe->first_attribute("name");
    if (!name)
    {
        name = xe->first_attribute("inGameType");
    }
    if (name) {
        _name = name->value();
    }
    rapidxml::xml_attribute<> *tags = xe->first_attribute("tags");
    if (tags)
    {
        _tags = tags->value();
        /* API side
        std::string value(tags->value());
        std::string::size_type b = 0;
        std::string::size_type e = value.find(";");
        while (b < value.size())
        {
            _tags.insert(value.substr(b, e == std::string::npos ? e : e - b));
            b = e + 1;
            e = value.find(";", b);
        }
        */
    }
    rapidxml::xml_attribute<> *uid = xe->first_attribute("uid");
    if (uid) {
        _uid = uid->value();
    }
    _mouseDown = false;
}

void BeautyBase::SetPosition(float x, float y)
{
    _pos.x = x; 
    _pos.y = y; 
}

void BeautyBase::SetAngle(float a) { 
    _angle = a; 
}

void BeautyBase::SetScale(float sx, float sy) { 
    _sx = sx; 
    _sy = sy; 
}

void BeautyBase::ShiftTo(float dx, float dy) { 
    _pos.x += dx; 
    _pos.y += dy; 
}

void BeautyBase::SaveToXml(rapidxml::xml_node<> *xe)
{
    if (!_name.empty())
    {
        char *copy = xe->document()->allocate_string(_name.c_str());
        xe->append_attribute(xe->document()->allocate_attribute("name", copy));
    }

    if (!_tags.empty())
    {
        char *copy = xe->document()->allocate_string(_tags.c_str());
        xe->append_attribute(xe->document()->allocate_attribute("tags", copy));
    }

    if (!_uid.empty())
    {
        char *copy = xe->document()->allocate_string(_uid.c_str());
        xe->append_attribute(xe->document()->allocate_attribute("uid", copy));
    }

    char buff[2000];
    sprintf(buff, "%0.2f,%0.1f,%0.1f,%0.2f,%0.2f,%0.2f,%0.2f,%0.2f,%i,%i,%i,%i,%x"
            , _scalar, _pos.x, _pos.y
            , _pointForAnything.x, _pointForAnything.y
            , _angle, _sx, _sy
            , _canBeRotated ? 1 : 0, _canBeScaled ? 1 : 0
            , Width(), Height()
            , _color);
    {
        char *copy = xe->document()->allocate_string(buff);
        xe->append_attribute(xe->document()->allocate_attribute("param", copy));
    }
}

void BeautyBase::Draw() {
    _debugDraw = false;
}

void BeautyBase::DebugDrawPointForAnything()
{
    Render::SetFiltering(false);
    Matrix matrix = parent = Render::GetCurrentMatrix();
    Render::PushMatrix();
    Render::SetMatrixUnit();
    matrix.Move(_pos.x, _pos.y);
    {
        float x, y;
        matrix.Mul(_pointForAnything.x, _pointForAnything.y, x, y);
        x = Math::round(x);
        y = Math::round(y);
        center->Render(x - center->Width() / 2.f, y - center->Height() / 2.f);
    }
    Render::PopMatrix();
    Render::SetFiltering(TileEditorInterface::Instance()->FilteringTexture());
}

void BeautyBase::DebugDraw(bool) {
    Render::PushColor();
    Render::SetColor(0xFFFFFFFF);
    _debugDraw = true;
    {
        Render::SetFiltering(false);
        Matrix matrix = parent = Render::GetCurrentMatrix();
        Render::PushMatrix();
        Render::SetMatrixUnit();
        matrix.Move(_pos.x, _pos.y);
        matrix.Rotate(_angle);
        rotateMatrix = matrix;
        matrix.Scale(_sx, _sy);
        scaleMatrix = matrix;

        float x, y;
        matrix.Mul(0.f, 0.f, x, y);
        {
            x = Math::round(x);
            y = Math::round(y);
        }
        Rect r = Rect(- Width() / 2, - Height() / 2, Width() / 2, Height() / 2);
        {
            if (r.x1 > r.x2) {
                float t = r.x1;
                r.x1 = r.x2;
                r.x2 = t;
            }
            if (r.y1 > r.y2) {
                float t = r.y1;
                r.y1 = r.y2;
                r.y2 = t;
            }
            float alpha = (Render::GetColor() >> 24) / 255.f;
            Render::SetAlpha(Math::round(0xAF * alpha));
            FPoint corners[4];
            {                
                matrix.Mul(r.x1, r.y1, corners[0].x, corners[0].y);
                corners[0].x = Math::round(corners[0].x);
                corners[0].y = Math::round(corners[0].y);

                matrix.Mul(r.x2, r.y1, corners[1].x, corners[1].y);
                corners[1].x = Math::round(corners[1].x);
                corners[1].y = Math::round(corners[1].y);

                matrix.Mul(r.x2, r.y2, corners[2].x, corners[2].y);
                corners[2].x = Math::round(corners[2].x);
                corners[2].y = Math::round(corners[2].y);

                matrix.Mul(r.x1, r.y2, corners[3].x, corners[3].y);
                corners[3].x = Math::round(corners[3].x);
                corners[3].y = Math::round(corners[3].y);

                for (int i = 0; i < 4; ++i) {
                    if (_canBeScaled) {
                        scale->Render(corners[i].x - scale->Width() / 2.f, corners[i].y - scale->Height() / 2.f);

                        scaleBtn[i] = corners[i];
                        Render::GetCurrentMatrix().Mul(scaleBtn[i]);
                    } else {
                        scaleSide->Render(corners[i].x - scaleSide->Width() / 2.f, corners[i].y - scaleSide->Height() / 2.f);
                    }
                }
            }
            if (_canBeScaled) {
                for (int i = 0; i < 4; ++i) {
                    sideScaleBtn[i].x = (corners[i].x + corners[(i + 1) % 4].x) / 2;
                    sideScaleBtn[i].y = (corners[i].y + corners[(i + 1) % 4].y) / 2;
                    scaleSide->Render(sideScaleBtn[i].x - scaleSide->Width() / 2.f, sideScaleBtn[i].y - scaleSide->Height() / 2.f);

                    Render::GetCurrentMatrix().Mul(sideScaleBtn[i]);
                }
            }
            if (_canBeRotated) {
                Render::PushMatrix();
                Render::MatrixMove(x, y);
                
                float distance = Math::Distance(corners[0], corners[1], FPoint(x, y));
                float shiftFromCenter = distance + 40.f;
                FPoint pos(0.f, - shiftFromCenter);

                Matrix m;
                m.MakeRevers(rotateMatrix);
                FPoint r(pos);
                m.Mul(r);
                FPoint r2(0, 0);
                m.Mul(r2);

                rotateBtn = *pos.Rotate(- M_PI_2 -(r - r2).Angle());
                Render::Line(0, 0, rotateBtn.x, rotateBtn.y, 0x7F000000);


                rotate->Render( Math::round(rotateBtn.x - rotate->Width() / 2.f), Math::round(rotateBtn.y - rotate->Height() / 2.f));
                Render::GetCurrentMatrix().Mul(rotateBtn);
                Render::PopMatrix();
            }
            if (_canBeRotated) {
                center->Render(x - center->Width() / 2.f, y - center->Height() / 2.f);
            }
            Render::SetAlpha(Math::round(0xFF * alpha));
        }
        Render::PopMatrix();
        Render::SetFiltering(TileEditorInterface::Instance()->FilteringTexture());
    }
    DebugDrawPointForAnything();
    Render::PopColor();
}

bool BeautyBase::CheckScale(const FPoint &mouse)
{
    for (int i = 0; i < 4; ++i)
    {
        if ((scaleBtn[i] - mouse).Length() < 13.f)
        {
            return true;
        }
    }
    return false;
}

bool BeautyBase::CheckSideScale(const FPoint &mouse)
{
    for (int i = 0; i < 4; ++i)
    {
        if ((sideScaleBtn[i] - mouse).Length() < 13.f)
        {
            _scaleY = (i % 2) == 0;
            _scaleX = !_scaleY;
            return true;
        }
    }
    return false;
}

bool BeautyBase::Command(const std::string &cmd) 
{
    std::string position;
    if (Messager::CanCut(cmd, "create dot at ", position)) {

        FPoint mouse;
        sscanf(position.c_str(), "%f %f", &(mouse.x), &(mouse.y));

        {
            Matrix reverse;
            reverse.MakeRevers(parent);
            FPoint fp(mouse);
            reverse.Mul(fp);

            FPoint click(fp - _pos);
            static const float SIZEX = 20;
            if ((click - _pointForAnything).Length() < SIZEX)
            {
                _pointForAnything = click;
                _mouseDown = true;
                _state = edit_pointForAnything;
                _mousePos = mouse;
                return true;
            }
        }
    }
    return false; 
}

void BeautyBase::MouseDown(const FPoint &mousePos) {
    if (!_debugDraw) {
        _mousePos = mousePos;
        return;
    }
    _mouseDown = true;
    _mousePos = _mouseDownPos = mousePos;
    if (_canBeRotated
        && (rotateBtn - mousePos).Length() < 13.f) {
        _state = edit_rotate;
        return;
    } else if (_canBeScaled && CheckScale(mousePos)) {
        _state = edit_scale;
        _scaleX = true;
        _scaleY = true;
        _beforeDragScale = FPoint(_sx, _sy);
        return;
    } else if (_canBeScaled && CheckSideScale(mousePos)) {
        _state = edit_scale;
        _beforeDragScale = FPoint(_sx, _sy);
        return;
    } else {
        _state = edit_move;
        return;
    } 
}

bool BeautyBase::MouseMove(const FPoint &mousePos) {
    if (!_debugDraw) {
        return false;
    }
    bool rValue = false;
    if (_state == edit_rotate)
    {
        if ((_mousePos - mousePos).Length() > 1e-3)
        {
            Matrix reverse;
            reverse.MakeRevers(rotateMatrix);

            FPoint start(_mousePos);
            FPoint end(mousePos);
            reverse.Mul(start);
            reverse.Mul(end);

            float angleStart = atan2(start.y, start.x);
            float angleEnd = atan2(end.y, end.x);

            float angle = _angle + (angleEnd - angleStart);

            _angle = angle;

            rValue = true;
        }
    }
    else if (_state == edit_move)
    {
        Matrix reverse;
        reverse.MakeRevers(parent);

        FPoint start(_mousePos);
        FPoint end(mousePos);
        reverse.Mul(start);
        reverse.Mul(end);

        _pos.x += (end.x - start.x);
        _pos.y += (end.y - start.y);
        rValue = true;
    }
    else if (_state == edit_scale) 
    {
        if ((_mousePos - mousePos).Length() > 1e-3) {

            Matrix reverse;
            reverse.MakeRevers(scaleMatrix);

            FPoint start(_mouseDownPos);
            FPoint end(mousePos);
            reverse.Mul(start);
            reverse.Mul(end);

            float sx = (end.x / start.x);
            float sy = (end.y / start.y);

            //     -   
            if (!_scaleX) {
                sx = 1.f;
            }

            //     -   
            if (!_scaleY) {
                sy = 1.f;
            }

            //  ""    -     
            if (_scaleX && _scaleY && (QApplication::keyboardModifiers() & Qt::ShiftModifier) != 0) {
                sx = sy = std::min(sx, sy);
            }

            _sx = sx * _beforeDragScale.x;
            _sy = sy * _beforeDragScale.y;
            if ((QApplication::keyboardModifiers() & Qt::ControlModifier) != 0) {
                _sx = Math::round(_sx * 2) / 2.f;
                _sy = Math::round(_sy * 2) / 2.f;
            }

            rValue = true;
        }

    } 
    else if (_state == edit_pointForAnything)
    {
        Matrix reverse;
        reverse.MakeRevers(parent);

        FPoint start(_mousePos);
        FPoint end(mousePos);
        reverse.Mul(start);
        reverse.Mul(end);

        _pointForAnything.x += (end.x - start.x);
        _pointForAnything.y += (end.y - start.y);
        rValue = true;
    }

    _mousePos = mousePos;
    return rValue;
}

void BeautyBase::MouseUp(const FPoint &) {
    _mouseDown = false;
    _state = edit_none;
    _scaleX = false;
    _scaleY = false;
}

bool BeautyBase::ControlsCheck(const FPoint &point) {
    if (_debugDraw) {
        bool result = (rotateBtn - point).Length() < 13.f;
        result |= CheckScale(point);
        result |= CheckSideScale(point);
        return result;
    }
    return false;
}

void BeautyBase::SetColor(DWORD color) 
{ 
    _color = color; 
}

BeautyBase::BeautyBase(const FPoint &pos, float sx, float sy, float angle)
{
    _needRegenerateIcon = false;

    _scalar = 0.f;

    _state = edit_none;

    _visible = true;
    _pos = pos;
    _color = 0xFFFFFFFF;
    _pointForAnything.x = _pointForAnything.y = 0;

    _angle = angle;

    _sx = sx;
    _sy = sy;

    _name = "";
    _uid = "";

    _canBeRotated = false;
    _canBeScaled = false;

    _debugDraw = false;

    _mouseDown = false;
    InitCorners();
}

void BeautyBase::InitCorners()
{
    GLTexture2D * h_base = Core::getTexture(Core::programPath + "gfx/red.png");
    scale = new Sprite(h_base, 4, 13, 12, 12);
    scaleSide = new Sprite(h_base, 4, 1, 10, 10);
    square = new Sprite(h_base, 19, 1, 10, 10);
    rotate = new Sprite(h_base, 1, 44, 20, 20);
    center = new Sprite(h_base, 4, 29, 12, 12);
}
