#include "Beauty.h"
#include "../Core/Core.h"
#include "../Core/Math.h"

Beauty::~Beauty() {
	delete _sprite;
}

Beauty::Beauty(rapidxml::xml_node<> *xe)
: BeautyBase(xe)
{
    _fileName = xe->first_attribute("texture")->value();
    ReplaceSlash(_fileName);
    _sprite = new Sprite(Core::getTexture(_fileName));
}

Beauty::Beauty(const std::string &imagePath)
: BeautyBase(FPoint(0, 0), 1.f, 1.f, 0)
{
    _canBeRotated = true;
    _canBeScaled = true;
    _fileName = imagePath;
    _sprite = new Sprite(Core::getTexture(imagePath));
}

Beauty::Beauty(const GLTexture2D *texture, float x, float y, float width, float height)
    : BeautyBase(FPoint(0, 0), 1.f, 1.f, 0)
{
    _canBeRotated = true;
    _canBeScaled = true;
    _fileName = "";
    _sprite = new Sprite(texture, x, y, width, height);
}

void Beauty::SaveToXml(rapidxml::xml_node<> *xe) {
	BeautyBase::SaveToXml(xe);
    char *copyName = xe->document()->allocate_string(_fileName.c_str());
    xe->append_attribute(xe->document()->allocate_attribute("texture", copyName));
}

void Beauty::Draw() {
	Render::PushMatrix();
	Render::MatrixMove(_pos.x, _pos.y);
	Render::MatrixRotate(_angle);
	Render::MatrixScale(_sx, _sy);
	Render::MatrixMove(Math::round(- Width() / 2), Math::round(- Height() / 2));
	Render::PushColorAndMul(_color);
	_sprite->Render();
	Render::PopColor();
	Render::PopMatrix();
	BeautyBase::Draw();
}

void Beauty::DebugDraw(bool onlyControl) {
	if (!onlyControl) {
		Draw();
	}
	BeautyBase::DebugDraw();
}

bool Beauty::PixelCheck(const FPoint &point) 
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

		return _sprite->PixelCheck(check.x, check.y); 
	}
	return false;
}

void Beauty::EncapsulateAllDots(Rect &rect) 
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


void Beauty::GetAllLocalDotsRect(Rect &rect) 
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

bool Beauty::GeometryCheck(const FPoint &point) 
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

std::string Beauty::Type() { 
	return "Beauty"; 
}

int Beauty::Width() {
	return _sprite->Width();
}

int Beauty::Height() {
	return _sprite->Height();
}

Beauty::Beauty(const Beauty &b)
: BeautyBase(b)
{
	_fileName = b._fileName;
	_sprite = new Sprite(Core::getTexture(_fileName));
}

const char *Beauty::GetIconTexture() {
	return NULL;//_sprite;
}
