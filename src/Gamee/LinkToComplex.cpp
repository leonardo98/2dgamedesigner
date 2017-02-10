#include "LinkToComplex.h"
#include "../Core/Core.h"
#include "../Core/Math.h"
#include "../Core/Rect.h"
#include "Complex.h"

LinkToComplex::~LinkToComplex() {
}

void LinkToComplex::ReInit()
{
	_complex = ComplexManager::getComplex(_id);
    if (_complex)
    {
        CalcWidthAndHeight();
    }
}

LinkToComplex::LinkToComplex(rapidxml::xml_node<> *xe) 
: BeautyBase(xe)
, _shiftX(32767)
, _shiftY(32767)
{
    _id = xe->first_attribute("complex")->value();
	_complex = ComplexManager::getComplex(_id);
	if (_complex)
	{
		CalcWidthAndHeight();
	}
}

LinkToComplex::LinkToComplex(const std::string &levelName)
    : BeautyBase(FPoint(0, 0), 1, 1, 0)
    , _shiftX(32767)
    , _shiftY(32767)
{
    _id = levelName;
    _inGameType = _id;
    _complex = ComplexManager::getComplex(_id);
    if (_complex)
    {
        CalcWidthAndHeight();
    }
}

LinkToComplex::LinkToComplex(const LinkToComplex &b)
: BeautyBase(b)
{
	_id = b._id;
	_complex = ComplexManager::getComplex(_id);
	_width = b._width;
	_height = b._height;
	_shiftX = b._shiftX;
	_shiftY = b._shiftY;
}

void LinkToComplex::Draw() {
	Render::PushMatrix();
	Render::MatrixMove(Math::round(_pos.x), Math::round(_pos.y));
//	assert(fabs(_angle) < 1e-3);
//	assert(fabs(_sx - 1) < 1e-3);
//	assert(fabs(_sy - 1) < 1e-3);
    Render::MatrixRotate(_angle);
    Render::MatrixScale(_sx, _sy);
	Render::MatrixMove(_shiftX, _shiftY);
	Render::PushColorAndMul(_color);
	_complex->Draw();
	Render::PopColor();
	Render::PopMatrix();
	BeautyBase::Draw();
}

void LinkToComplex::DebugDraw(bool onlyControl) {
	if (!onlyControl) {
		Draw();
	}
	BeautyBase::DebugDraw();
}

bool LinkToComplex::PixelCheck(const FPoint &point) 
{
	Matrix localMatrix;
	localMatrix.Move(Math::round(_pos.x), Math::round(_pos.y));

	Matrix rev;
	rev.MakeRevers(localMatrix);
	FPoint r(point);
	rev.Mul(r);

	if (-_width / 2 <= r.x && r.x <= _width / 2 && - _height / 2 <= r.y && r.y <= _height / 2) {
		return _complex->GeometryCheck(r - FPoint(_shiftX, _shiftY));
	}
	return false;//BeautyBase::PixelCheck(point); 
}

void LinkToComplex::SaveToXml(rapidxml::xml_node<> *xe) {
	BeautyBase::SaveToXml(xe);
    char *name = xe->document()->allocate_string(_id.c_str());
    xe->append_attribute(xe->document()->allocate_attribute("complex", name));
    Math::Write(xe, "shiftX", _shiftX);
    Math::Write(xe, "shiftY", _shiftY);
}

std::string LinkToComplex::Type() { 
	return "LinkToComplex"; 
}

int LinkToComplex::Width() {
	return _width;
}

int LinkToComplex::Height() {
	return _height;
}

const char *LinkToComplex::GetIconTexture() {
	return NULL;
}

void LinkToComplex::Update(float dt) {
}

void LinkToComplex::CalcWidthAndHeight() {
	QVector<FPoint> dots;
	Rect rect;	
	rect.Clear();

	Render::PushMatrix();
	Render::SetMatrixUnit();
	_complex->GetAllLocalDotsRect(rect);
	Render::PopMatrix();

	rect.x1 = Math::round(rect.x1);
	rect.x2 = Math::round(rect.x2);
	rect.y1 = Math::round(rect.y1);
	rect.y2 = Math::round(rect.y2);
	_width = fabs(rect.x2 - rect.x1);
	_height = fabs(rect.y2 - rect.y1);
	_shiftX = -(rect.x2 + rect.x1) / 2;
	_shiftY = -(rect.y2 + rect.y1) / 2;
}

void LinkToComplex::EncapsulateAllDots(Rect &rect) {
	//    
	_complex->EncapsulateAllDots(rect);
	rect.x1 += (_pos.x + _shiftX);
	rect.x2 += (_pos.x + _shiftX);
	rect.y1 += (_pos.y + _shiftY);
	rect.y2 += (_pos.y + _shiftY);
}

void LinkToComplex::GetAllLocalDotsRect(Rect &rect) {
	//    
	_complex->GetAllLocalDotsRect(rect);
}
