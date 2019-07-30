#include "AnimationArea.h"
#include "../Core/Core.h"
#include "../Core/Math.h"
#include "../Core/rect.h"

AnimationArea::~AnimationArea() {
}

AnimationArea::AnimationArea(rapidxml::xml_node<> *xe)
: BeautyBase(xe)
{
    _lib = xe->first_attribute("file")->value();
    _id = xe->first_attribute("id")->value();
    if (!AnimationManager::Load(_lib))
    {
        _animation = NULL;
        return;
    }
    _animation = AnimationManager::getAnimation(_id);
    if (_animation == NULL)
    {
        return;
    }
    _play = false;
    rapidxml::xml_attribute<> *tmp = xe->first_attribute("start");
    _startTime = tmp ? atof(tmp->value()) : 0.f;
    _time = _startTime;
    CalcWidthAndHeight();

}

AnimationArea::AnimationArea(const AnimationArea &b)
: BeautyBase(b)
{
    _lib = b._lib;
    _id = b._id;
    _startTime = b._startTime;
    _time = _startTime;
    _animation = AnimationManager::getAnimation(_id);
    _play = false;
    _width = b._width;
    _height = b._height;
    _shiftX = b._shiftX;
    _shiftY = b._shiftY;
}

void AnimationArea::Draw() {
    if (!_animation)
    {
        return;
    }
    Render::PushMatrix();
    Render::MatrixMove(_pos.x, _pos.y);
    Render::MatrixRotate(_angle);
    Render::MatrixScale(_sx, _sy);
    Render::MatrixMove(_shiftX, _shiftY);
    Render::PushColorAndMul(_color);
    _animation->Draw(_time);
    Render::PopColor();
    Render::PopMatrix();
    BeautyBase::Draw();
}

void AnimationArea::DebugDraw(bool onlyControl) {
    if (!onlyControl) {
        Draw();
    }
    BeautyBase::DebugDraw();
}

bool AnimationArea::PixelCheck(const FPoint &point) 
{
    Matrix localMatrix;
    localMatrix.Move(_pos.x, _pos.y);
    localMatrix.Rotate(_angle);
    localMatrix.Scale(_sx, _sy);
    localMatrix.Move(_shiftX, _shiftY);
    Matrix rev;
    rev.MakeRevers(localMatrix);
    FPoint check(point);
    rev.Mul(check);
    return _animation->PixelCheck(check);// || BeautyBase::PixelCheck(point);
}

void AnimationArea::SaveToXml(rapidxml::xml_node<> *xe) {
    BeautyBase::SaveToXml(xe);
    char *file = xe->document()->allocate_string(_lib.c_str());
    char *id = xe->document()->allocate_string(_id.c_str());
    xe->append_attribute(xe->document()->allocate_attribute("file", file));
    xe->append_attribute(xe->document()->allocate_attribute("id", id));
    Math::Write(xe, "shiftX", _shiftX);
    Math::Write(xe, "shiftY", _shiftY);
    Math::Write(xe, "start", _startTime);
}

std::string AnimationArea::Type() { 
    return "Animation"; 
}

int AnimationArea::Width() {
    return _width;
}

int AnimationArea::Height() {
    return _height;
}

const char *AnimationArea::GetIconTexture() {
    return NULL;
}

void AnimationArea::Update(float dt) {
    if (_animation && _play) {
        _time += dt / _animation->Time();
        if (_time >= 1.f) {
            _time = _startTime;
            _play = false;
        }
    }
}

float AnimationArea::SomeFloatValue() 
{ 
    return _startTime; 
}

bool AnimationArea::Command(const std::string &cmd) {
    if (cmd == "play") {
        _play = true;
        _time = 0.f;
        return true;
    }
    if (cmd.substr(0, 5) == "setup") {
        _play = false;
        _time = _startTime = atof(cmd.substr(5).c_str());
        return true;
    }
    return (cmd == "" || BeautyBase::Command(cmd));
}

void AnimationArea::CalcWidthAndHeight() {
    QVector<FPoint> dots;
    Rect rect;    
    rect.Clear();

    Render::PushMatrix();
    Render::SetMatrixUnit();
    Render::MatrixRotate(_angle);
    _animation->PreDraw(0.01f);
    _animation->EncapsulateAllDots(0.1f, rect);

    _width = fabs(rect.x2 - rect.x1);
    _height = fabs(rect.y2 - rect.y1);
    _shiftX = -(rect.x2 + rect.x1) / 2;
    _shiftY = -(rect.y2 + rect.y1) / 2;

    float p = 0.f;
    while (p < 1.f && (_width <= 0.f || _width > 9999
           || _height <= 0.f || _height > 9999))
    {
        _animation->EncapsulateAllDots(p, rect);
        p += 0.1f;
        _width = fabs(rect.x2 - rect.x1);
        _height = fabs(rect.y2 - rect.y1);
        _shiftX = -(rect.x2 + rect.x1) / 2;
        _shiftY = -(rect.y2 + rect.y1) / 2;
    }

    Render::PopMatrix();
}

void AnimationArea::EncapsulateAllDots(Rect &rect) {
    _animation->PreDraw(0.1f);
    _animation->EncapsulateAllDots(0.1f, rect);
    rect.Move(_pos.x + _shiftX, _pos.y + _shiftY);
}

void AnimationArea::GetAllLocalDotsRect(Rect &rect) 
{
    Matrix unit;
    unit.Move(_pos.x, _pos.y);
    unit.Rotate(_angle);
    unit.Scale(_sx, _sy);
    unit.Move(_shiftX, _shiftY);

    _animation->PreDraw(0.1f);
    _animation->EncapsulateAllDots(0.1f, rect);
}
