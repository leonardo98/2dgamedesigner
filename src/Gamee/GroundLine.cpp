#include <QApplication>
#include "GroundLine.h"
#include "../Core/Math.h"
#include "../Core/Core.h"
#include "../Core/Messager.h"
#include "../Core/InputSystem.h"
#include "TileEditorInterface.h"

GroundLine::~GroundLine() {
}

GroundLine::GroundLine(const GroundLine &c) 
: BeautyBase(c)
{
	_debugColor = _color;
	_icon = Core::programPath + "gfx/black.png";
	_dots = c._dots;
	CalcWidthAndHeight();
	_mouseDown = false;
	_dotUnderCursor = _dotLastEdited = -1;
}

GroundLine::GroundLine(rapidxml::xml_node<> *xe)
: BeautyBase(xe)
{
	_debugColor = _color;
	_icon = Core::programPath + "gfx/black.png";
    rapidxml::xml_node<> *dot = xe->first_node("dot");
	while (dot != NULL) {
        _dots.push_back(FPoint(atof(dot->first_attribute("x")->value()), atof(dot->first_attribute("y")->value())));
        dot = dot->next_sibling("dot");
	}
	CalcWidthAndHeight();
	_mouseDown = false;
	_dotUnderCursor = _dotLastEdited = -1;
}

void GroundLine::Draw() {
	Render::PushMatrix();
	Render::MatrixMove(_pos.x, _pos.y);

	_screenDots = _dots;
	for (unsigned int i = 0; i < _dots.size(); ++i) {
		Render::GetCurrentMatrix().Mul(_screenDots[i]);	
		Render::Line(_dots[i].x, _dots[i].y, _dots[(i + 1) % _dots.size()].x, _dots[(i + 1) % _dots.size()].y, _debugColor);
	}

	Render::PopMatrix();
	BeautyBase::Draw();
}

void GroundLine::DebugDraw(bool onlyControl) {
	if (!onlyControl) {
		Draw();
	}
	{
		_debugDraw = true;
	
		Render::PushMatrix();

		Render::MatrixMove(_pos.x, _pos.y);
		Matrix matrix = parent = Render::GetCurrentMatrix();
		for (unsigned int i = 0; i < _dots.size(); ++i) {
			Render::Line(_dots[i].x, _dots[i].y, _dots[(i + 1) % _dots.size()].x, _dots[(i + 1) % _dots.size()].y, _debugColor);
		}

		Render::SetMatrixUnit();

		float alpha = (Render::GetColor() >> 24) / 255.f;
		Render::SetAlpha(Math::round(0xAF * alpha));
		for (unsigned int i = 0; i < _screenDots.size(); ++i) {
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

bool GroundLine::PixelCheck(const FPoint &point) { 
	return SearchNearestWorld(point.x, point.y) >= 0 || SearchNearestLineWorld(point.x, point.y) >= 0;
}

void GroundLine::SaveToXml(rapidxml::xml_node<> *xe) {
	BeautyBase::SaveToXml(xe);
	for (unsigned int j = 0; j < _dots.size(); ++j) {
        rapidxml::xml_node<> *dot = xe->document()->allocate_node(rapidxml::node_element, "dot");
        xe->append_node(dot);
        Math::Write(dot, "x", _dots[j].x);
        Math::Write(dot, "y", _dots[j].y);
	}
}

std::string GroundLine::Type() { 
	return "GroundLine"; 
}

int GroundLine::Width() {
	return _width;
}

int GroundLine::Height() {
	return _height;
}

void GroundLine::CalcWidthAndHeight() {
	Rect rect;
	rect.Clear();
	for (unsigned int i = 0; i < _dots.size(); ++i) {
		rect.Encapsulate(_dots[i].x, _dots[i].y);
	}
	_width = fabs(rect.x2 - rect.x1);
	_height = fabs(rect.y2 - rect.y1);
}

int GroundLine::SearchNearest(float x, float y) {
	int result = -1;
	static const float SIZEX = 10;
	FPoint p(x, y);
	for (unsigned int i = 0; i < _screenDots.size() && result < 0; ++i) {
		if ((_screenDots[i] - p).Length() < SIZEX) {
			result = i;
		}
	}
	return result;
}

int GroundLine::SearchNearestWorld(float x, float y) {
	int result = -1;
	static const float SIZEX = 10;
	FPoint p(x, y);
	p -= _pos;
	for (unsigned int i = 0; i < _dots.size() && result < 0; ++i) {
		if ((_dots[i] - p).Length() < SIZEX) {
			result = i;
		}
	}
	return result;
}

void GroundLine::MouseDown(const FPoint &mouse) {
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

bool GroundLine::MouseMove(const FPoint &mousePos) {
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
        bool rValue = BeautyBase::MouseMove(mousePos);
        _mousePos = mousePos;
        return rValue;
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
        if ((QApplication::keyboardModifiers() & Qt::ControlModifier) != 0)
		{
			assert(_dots.size() == 2);
			if (fabs(_dots[_dotUnderCursor].x - _dots[1 - _dotUnderCursor].x) < fabs(_dots[_dotUnderCursor].y - _dots[1 - _dotUnderCursor].y))
			{
				_dots[_dotUnderCursor].x = _dots[1 - _dotUnderCursor].x;
			}
			else
			{
				_dots[_dotUnderCursor].y = _dots[1 - _dotUnderCursor].y;
			}
		}
		CalcWidthAndHeight();
	}
	_mousePos = mousePos;
    return true;
}

void GroundLine::MouseUp(const FPoint &mouse)
{
    BeautyBase::MouseUp(mouse);
	_mouseDown = false;
	_dotUnderCursor = -1;
	_mousePos = mouse;
}

const char *GroundLine::GetIconTexture() {
	return _icon.c_str();
}

void GroundLine::EncapsulateAllDots(Rect &rect) {
	for (unsigned int i = 0; i < _screenDots.size(); ++i) {
		//rect.Encapsulate(_screenDots[i].x, _screenDots[i].y);
		rect.Encapsulate(_pos.x + _dots[i].x, _pos.y + _dots[i].y);
	}
}

void GroundLine::GetAllLocalDotsRect(Rect &rect) {
	for (unsigned int i = 0; i < _dots.size(); ++i) {
		rect.Encapsulate(_pos.x + _dots[i].x, _pos.y + _dots[i].y);
	}
}

int GroundLine::SearchNearestLine(float x, float y) 
{
	FPoint p(x, y);
	for (int i = 0; i < static_cast<int>(_screenDots.size()) - 1; ++i) {
		if (Math::DotNearLine(_screenDots[i], _screenDots[i + 1], p)) {
			return i / 6;
		}
	}
	return -1;
}

int GroundLine::SearchNearestLineWorld(float x, float y) 
{
	FPoint p(x, y);
	p -= _pos;
	for (int i = 0; i < static_cast<int>(_dots.size()) - 1; ++i) {
		if (Math::DotNearLine(_dots[i], _dots[i + 1], p)) {
			return i / 6;
		}
	}
	return -1;
}

bool GroundLine::Command(const std::string &cmd) {
	std::string position;

	if (Messager::CanCut(cmd, "create dot at ", position)) {

		FPoint mouse;
        sscanf(position.c_str(), "%f %f", &(mouse.x), &(mouse.y));

		int index = SearchNearest(mouse.x, mouse.y);
		if (index >= 0) {
			_dotUnderCursor = _dotLastEdited = index;
			_mouseDown = true;
		}
		_mousePos = mouse;
		return _mouseDown;
	}
	return (cmd == "" || BeautyBase::Command(cmd));
}
