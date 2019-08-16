#include "BeautyText.h"
#include "../Core/Core.h"
#include "../Core/Math.h"
#include "../Core/Messager.h"

#include <QCoreApplication>

BeautyText::~BeautyText()
{
}

BeautyText::BeautyText(rapidxml::xml_node<> *xe)
: BeautyBase(xe)
{
    std::string programPath = QCoreApplication::applicationDirPath().toStdString();
    _icon = programPath + "/gfx/beautytext.png";
    _fileName = xe->first_attribute("font")->value();
    _font = Core::getFont(_fileName.c_str());
    _align = atoi(xe->first_attribute("align")->value());
    _hInterval = (float)atof(xe->first_attribute("hInterval")->value());
    _vSpacing = (float)atof(xe->first_attribute("vSpacing")->value());

    SetText(xe->first_attribute("text")->value());
}

BeautyText::BeautyText(const std::string &imagePath)
: BeautyBase(FPoint(0, 0), 1.f, 1.f, 0)
{
    std::string programPath = QCoreApplication::applicationDirPath().toStdString();
    _icon = programPath + "/gfx/beautytext.png";
    _canBeRotated = true;
    _canBeScaled = true;
    _fileName = imagePath;
    _font = Core::getFont(_fileName.c_str());
    SetText("Test Test Test\nTest Test\nTest");
    _align = 0;
    _hInterval = 1.f;
    _vSpacing = 0.f;
}

BeautyText::BeautyText(const GLTexture2D *texture, float x, float y, float /*width*/, float /*height*/)
    : BeautyBase(FPoint(x, y), 1.f, 1.f, 0)
{
    std::string programPath = QCoreApplication::applicationDirPath().toStdString();
    _icon = programPath + "/gfx/beautytext.png";
    _canBeRotated = true;
    _canBeScaled = true;
    _fileName = "";
    _font = NULL;
    SetText("Test Test Test\nTest Test\nTest");
    _align = 0;
    _hInterval = 1.f;
    _vSpacing = 0.f;
}

std::string BeautyText::SaveString()
{
    std::string stringToSave(_text);
    for (uint i = 0; i < stringToSave.size(); ++i)
    {
        if (stringToSave[i] == '\n')
        {
            stringToSave[i] = '#';
        }
    }
    return stringToSave;
}

void BeautyText::SaveToXml(rapidxml::xml_node<> *xe)
{
    BeautyBase::SaveToXml(xe);
    char *copyName = xe->document()->allocate_string(_fileName.c_str());
    xe->append_attribute(xe->document()->allocate_attribute("font", copyName));
    Math::Write(xe, "align", _align);
    copyName = xe->document()->allocate_string(SaveString().c_str());
    xe->append_attribute(xe->document()->allocate_attribute("text", copyName));
    Math::Write(xe, "hInterval", _hInterval);
    Math::Write(xe, "vSpacing", _vSpacing);
}

void BeautyText::Draw()
{
    Render::PushMatrix();
    Render::MatrixMove(_pos.x, _pos.y);
    Render::MatrixRotate(_angle);
    Render::MatrixScale(_sx, _sy);
    Render::MatrixMove(0, Math::round(- (Height() - _font->GetStringHeight(NULL)) / 2));
    if (_align == -1)
        Render::MatrixMove(Math::round(- Width() / 2), 0);
    else if (_align == 1)
        Render::MatrixMove(Math::round( Width()) / 2, 0);
    Render::PushColorAndMul(_color);
    if (_font)
    {
        _font->SetSpacing(_hInterval);
        _font->SetTracking(_vSpacing);
        if (_text.size())
            _font->Render(0, 0, _align, _text.c_str());
        else
            _font->Render(0, 0, _align, "Write you text here!");
    }
    Render::PopColor();
    Render::PopMatrix();
    BeautyBase::Draw();
}

void BeautyText::DebugDraw(bool onlyControl) {
    if (!onlyControl) {
        Draw();
    }
    BeautyBase::DebugDraw();
}

bool BeautyText::PixelCheck(const FPoint &point)
{
    if (GeometryCheck(point))
    {
        Matrix localMatrix;
        localMatrix.Move(_pos.x, _pos.y);
        localMatrix.Rotate(_angle);
        localMatrix.Scale(_sx, _sy);
        localMatrix.Move(Math::round(- Width() / 2), Math::round(- Height() / 2));

        Matrix rev;
        rev.MakeRevers(localMatrix);

        FPoint check(point);
        rev.Mul(check);

        return true;
    }
    return false;
}

void BeautyText::EncapsulateAllDots(Rect &rect)
{
    FPoint a(       0,        0);
    FPoint b( Width(),        0);
    FPoint c( Width(), Height());
    FPoint d(       0, Height());

    Matrix localMatrix;
    localMatrix.Move(_pos.x, _pos.y);
    localMatrix.Rotate(_angle);
    localMatrix.Scale(_sx, _sy);
    localMatrix.Move(Math::round(- Width() / 2), Math::round(- Height() / 2));

    localMatrix.Mul(a);
    localMatrix.Mul(b);
    localMatrix.Mul(c);
    localMatrix.Mul(d);

    rect.Encapsulate(a.x, a.y);
    rect.Encapsulate(b.x, b.y);
    rect.Encapsulate(c.x, c.y);
    rect.Encapsulate(d.x, d.y);
}


void BeautyText::GetAllLocalDotsRect(Rect &rect)
{
    FPoint a(       0,        0);
    FPoint b( Width(),        0);
    FPoint c( Width(), Height());
    FPoint d(       0, Height());

    Matrix m;
    m.Move(_pos.x, _pos.y);
    m.Rotate(_angle);
    m.Scale(_sx, _sy);
    m.Move(Math::round(- Width() / 2), Math::round(- Height() / 2));

    m.Mul(a);
    m.Mul(b);
    m.Mul(c);
    m.Mul(d);

    rect.Encapsulate(a.x, a.y);
    rect.Encapsulate(b.x, b.y);
    rect.Encapsulate(c.x, c.y);
    rect.Encapsulate(d.x, d.y);
}

bool BeautyText::GeometryCheck(const FPoint &point)
{
    Matrix localMatrix;
    localMatrix.Move(_pos.x, _pos.y);
    localMatrix.Rotate(_angle);
    localMatrix.Scale(_sx, _sy);
    localMatrix.Move(Math::round(- Width() / 2), Math::round(- Height() / 2));

    Matrix rev;
    rev.MakeRevers(localMatrix);

    FPoint check(point);
    rev.Mul(check);

    return 0 <= check.x && check.x <= Width() && 0 <= check.y && check.y <= Height();
}

std::string BeautyText::Type() {
    return "BeautyText";
}

int BeautyText::Width() {
    return _width;
}

int BeautyText::Height() {
    return _height;
}

BeautyText::BeautyText(const BeautyText &b)
: BeautyBase(b)
{
    std::string programPath = QCoreApplication::applicationDirPath().toStdString();
    _icon = programPath + "/gfx/beautytext.png";
    _fileName = b._fileName;
    _font = b._font;
    _text = b._text;
    _width = b._width;
    _height = b._height;

    _align = b._align;
    _hInterval = b._hInterval;
    _vSpacing = b._vSpacing;
}

const char *BeautyText::GetIconTexture() {
    return _icon.c_str();
}

void BeautyText::SetText(const std::string &text)
{
    _text = text;
    for (uint i = 0; i < _text.size(); ++i)
    {
        if (_text[i] == '#')
        {
            _text[i] = '\n'; // multiline
        }
    }
    if (_font)
    {
        _font->SetSpacing(_hInterval);
        _font->SetTracking(_vSpacing);
    }
    _width = _font ? _font->GetStringWidth(_text.c_str()) : 100;
    _height = _font ? _font->GetStringHeight(_text.c_str()) : 50;
}

bool BeautyText::Command(const std::string &cmd)
{
    std::string value;
    if (Messager::CanCut(cmd, "align:", value))
    {
        _align = atoi(value.c_str());
        return true;
    }
    return BeautyBase::Command(cmd);
}

void BeautyText::SetVertInterval(float value)
{
    _hInterval = value;
    if (_font)
        _font->SetSpacing(value);
    _height = _font ? _font->GetStringHeight(_text.c_str()) : 50;
}
void BeautyText::SetHorSpacing(float value)
{
    _vSpacing = value;
    if (_font)
        _font->SetTracking(value);
    _width = _font ? _font->GetStringWidth(_text.c_str()) : 100;
}
