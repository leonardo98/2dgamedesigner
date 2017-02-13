#include <QApplication>
#include "SolidGroundLine.h"
#include "../Core/Math.h"
#include "../Core/Core.h"
#include "../Core/InputSystem.h"
#include "TileEditorInterface.h"

#define SPLINE_OFFSET 30

void Besier::Path(float t, FPoint &pos)
{
    assert(origin.size() >= 4 && ((int)origin.size() - 1) % 3 == 0);
    if (t == 1.f)
    {
        pos = origin.back();
        return;
    }
    assert(0.f <= t && t < 1.f);
    
    int segmentAmount = origin.size() / 3;
    int segment = static_cast<int>(t * segmentAmount);

    t = t * segmentAmount - segment;

    FPoint &p0 = origin[segment * 3];
    FPoint &p1 = origin[segment * 3 + 1];
    FPoint &p2 = origin[segment * 3 + 2];
    FPoint &p3 = origin[segment * 3 + 3];

    pos = ((1 - t) * (1 - t) * (1 - t) * p0
            + 3 * t * (1 - t) * (1 - t) * p1
            + 3 * t * t * (1 - t) * p2
            + t * t * t * p3);
}

void Besier::ShiftPoint(unsigned int index, const FPoint &shift) 
{
    if (index % 3 == 0) { //   -    
        if (index > 0) {
            origin[index - 1] += shift;
        }
        origin[index] += shift;
        if (index + 1 < origin.size()) {
            origin[index + 1] += shift;
        }
    } else if (index % 3 == 1) { //   -     
        origin[index] += shift;
        if ((QApplication::keyboardModifiers() & Qt::ControlModifier) == 0 && index >= 2)
        {
            float len = (origin[index - 2] - origin[index - 1]).Length();
            FPoint p(origin[index] - origin[index - 1]);
            origin[index - 2] = -len / p.Length() * p + origin[index - 1];
        }
    } else if (index % 3 == 2) { //   -     
        origin[index] += shift;
        if ((QApplication::keyboardModifiers() & Qt::AltModifier) == 0 && index < origin.size() - 2)
        {
            float len = (origin[index + 2] - origin[index + 1]).Length();
            FPoint p(origin[index] - origin[index + 1]);
            origin[index + 2] = -len / p.Length() * p + origin[index + 1];
        }
    } else {
        assert(false);
    }
}

void Besier::Recalc(QVector<FPoint> &screen, int sectorNumber) {
    assert(((int)origin.size() - 1) % 3 == 0);
    screen.clear();
    for (unsigned int i = 0; i < origin.size() / 3; ++i) {
        FPoint p0 = origin[i * 3];
        FPoint p1 = origin[i * 3 + 1];
        FPoint p2 = origin[i * 3 + 2];
        FPoint p3 = origin[i * 3 + 3];
        for (int k = 0; k < sectorNumber; ++k) {
            float t = static_cast<float>(k) / sectorNumber;
            screen.push_back((1 - t) * (1 - t) * (1 - t) * p0
                            + 3 * t * (1 - t) * (1 - t) * p1
                            + 3 * t * t * (1 - t) * p2
                            + t * t * t * p3);
        }
    }
    screen.push_back(origin.back());
}

void Besier::RecalcWithNumber(QVector<FPoint> &screen, int dotNumber)
{
    screen.clear();

    FPoint dot;
    QVector<FPoint> tmp;
    Recalc(tmp, 10);
    float lenFull = 0.f;
    for (int i = 0; i < static_cast<int>(tmp.size()) - 1; ++i) 
    {
        lenFull += (tmp[i] - tmp[i + 1]).Length();
    }
    screen.push_back(tmp.front());
    int k = 0;
    float len = 0.f;
    for (int i = 1; i < dotNumber; ++i) 
    {
        float searchValue = i * lenFull / dotNumber;
        assert(k + 1 < tmp.size());
        float lenCurrent = (tmp[k + 1] - tmp[k]).Length();
        while (len + lenCurrent < searchValue) 
        {
            len += lenCurrent;
            ++k;
            lenCurrent = (tmp[k + 1] - tmp[k]).Length();
        }
        dot = ((searchValue - len) / lenCurrent) * (tmp[k + 1] - tmp[k]) + tmp[k];
        screen.push_back(dot);
    }
    screen.push_back(tmp.back());
}

float Besier::Length()
{
    QVector<FPoint> tmp;
    Recalc(tmp, 10);
    float lenFull = 0.f;
    for (int i = 0; i < static_cast<int>(tmp.size()) - 1; ++i) 
    {
        lenFull += (tmp[i] - tmp[i + 1]).Length();
    }
    return lenFull;
}


void SolidGroundLine::Draw() {    
    assert(((int)_besier.origin.size() - 1) % 3 == 0);

    Render::PushMatrix();
    Render::MatrixMove(_pos.x, _pos.y);
    parentMatrix = Render::GetCurrentMatrix();
    if (_drawTrianglesCoves)
    {
        _triangles.Render();
    }

    _besier.Recalc(_renderLineDots, 6);
    for (unsigned int i = 1; i < _renderLineDots.size(); ++i) {
        Render::Line(_renderLineDots[i - 1].x, _renderLineDots[i - 1].y
                    , _renderLineDots[i].x, _renderLineDots[i].y, _debugColor);
    }
    for (unsigned int i = 0; i < _renderLineDots.size(); ++i) {
        Render::GetCurrentMatrix().Mul(_renderLineDots[i]);    
    }
    _screenDots.clear();
    for (unsigned int i = 0; i < _besier.origin.size(); ++i) {
        _screenDots.push_back(_besier.origin[i]);
        Render::GetCurrentMatrix().Mul(_screenDots.back());    
    }

    Render::PopMatrix();
}


void SolidGroundLine::DebugDraw(bool onlyControl) {
    
    if (!onlyControl) {
        Draw();
    }

    {
        _debugDraw = true;
    
        Render::SetFiltering(false);
        Render::PushMatrix();

        Render::MatrixMove(_pos.x, _pos.y);
        Matrix matrix = parent = Render::GetCurrentMatrix();

        Render::SetMatrixUnit();

        float alpha = (Render::GetColor() >> 24) / 255.f;
        Render::SetAlpha(Math::round(0xAF * alpha));


        int underCursor = _dotUnderCursor;
        if (underCursor % 3 == 1) {
            underCursor = underCursor - 1;
        } else if (underCursor % 3 == 2) {
            underCursor = underCursor + 1;
        }

        int lastEdited = _dotLastEdited;
        if (lastEdited % 3 == 1) {
            lastEdited = lastEdited - 1;
        } else if (lastEdited % 3 == 2) {
            lastEdited = lastEdited + 1;
        }
        for (unsigned int i = 0; i < _screenDots.size(); ++i) {
            if (i % 3 == 0) {
                if ((i == underCursor || i == lastEdited) && TileEditorInterface::Instance()->CreateDotMode()) {

                    if (i == lastEdited) {
                        if (i > 0) {
                            Render::Line(_screenDots[i].x, _screenDots[i].y
                                            , _screenDots[i - 1].x, _screenDots[i - 1].y, 0x8FFFFFFF);
                        }
                        if (i + 1 < _screenDots.size()) {
                            Render::Line(_screenDots[i].x, _screenDots[i].y
                                            , _screenDots[i + 1].x, _screenDots[i + 1].y, 0x8FFFFFFF);
                        }
                    }

                    scale->Render(_screenDots[i].x - scale->Width() / 2.f, _screenDots[i].y - scale->Height() / 2.f);
                } else {
                    scaleSide->Render(_screenDots[i].x - scaleSide->Width() / 2.f, _screenDots[i].y - scaleSide->Height() / 2.f);
                }
            }
            if (i % 3 != 0 && lastEdited >= 0 && (i == (lastEdited - 1) || i == (lastEdited + 1))) {
                square->Render(_screenDots[i].x - square->Width() / 2.f, _screenDots[i].y - square->Height() / 2.f);
            }
        }
        Render::SetAlpha(Math::round(0xFF * alpha));

        Render::PopMatrix();
        Render::SetFiltering(TileEditorInterface::Instance()->FilteringTexture());
    }
}

int SolidGroundLine::SearchNearest(float x, float y) {
    int result = -1;
    float SIZEX = 20;
    FPoint p(x, y);
    for (unsigned int i = 0; i < _screenDots.size(); ++i) {
        if ((_screenDots[i] - p).Length() < SIZEX) {
            result = i; 
            SIZEX = (_screenDots[i] - p).Length();
        }
    }
    return result;
}

int SolidGroundLine::SearchNearestLine(float x, float y) {

    FPoint p(x, y);
    for (int i = 0; i < static_cast<int>(_renderLineDots.size()) - 1; ++i) {
        if (Math::DotNearLine(_renderLineDots[i], _renderLineDots[i + 1], p)) {
            return i / _sectionNumber;
        }
    }
    return -1;
}

int SolidGroundLine::SearchNearestWorld(float x, float y) {
    int result = -1;
    static const float SIZEX = 20;
    FPoint p(x, y);
    p -= _pos;
    for (unsigned int i = 0; i < _besier.origin.size() && result < 0; ++i) {
        if ((_besier.origin[i] - p).Length() < SIZEX) {
            result = i;
        }
    }
    return result;
}

int SolidGroundLine::SearchNearestLineWorld(float x, float y) 
{
    QVector<FPoint> dots;
    _besier.Recalc(dots, 6);
    FPoint p(x, y);
    p -= _pos;
    for (int i = 0; i < static_cast<int>(dots.size()) - 1; ++i) {
        if (Math::DotNearLine(dots[i], dots[i + 1], p)) {
            return i / _sectionNumber;
        }
    }
    return -1;
}
int SolidGroundLine::CreateDot(float x, float y) {
    if (_besier.origin.size() >= 50) {
        return -1;
    }
    int result = 3 * SearchNearestLine(x, y);

    if (result >= 0) {

        Matrix reverse;
        reverse.MakeRevers(parent);

        FPoint fp(x, y);
        reverse.Mul(fp);

        int index = result + 1;
        if (index < (int)_besier.origin.size()) {
            QVector<FPoint> spline = _besier.origin;
            _besier.origin.clear();
            for (int i = 0; i < index + 1; ++i) {
                _besier.origin.push_back(spline[i]);
            }
            FPoint prev = (fp - _besier.origin.back());
            prev *= (SPLINE_OFFSET / prev.Length());
            _besier.origin.push_back(fp - prev);
            int r = _besier.origin.size();
            _besier.origin.push_back(fp);
            _besier.origin.push_back(fp + prev);
            for (int i = index + 1; i < (int)spline.size(); ++i) {
                _besier.origin.push_back(spline[i]);
            }
            return r;
        } else {
            _besier.origin.push_back(_besier.origin.back() + FPoint(-SPLINE_OFFSET, 0));
            _besier.origin.push_back(fp + FPoint(-SPLINE_OFFSET, 0));
            _besier.origin.push_back(fp);
        }
        return index;
    }
    
    return -1;
}

void SolidGroundLine::RemoveDot(int index) {
    if (_besier.origin.size() <= 4 || index % 3 != 0) {
        return;
    }
    QVector<FPoint> spline = _besier.origin;
    _besier.origin.clear();
    if (index == spline.size() - 1) {
        index--;
    }
    for (int i = 0; i < (index) - 1; ++i) {
        _besier.origin.push_back(spline[i]);
    }
    if (index == 0) {
        index++;
    }
    for (int i = (index + 1) + 1; i < (int)spline.size(); ++i) {
        _besier.origin.push_back(spline[i]);
    }
}

void SolidGroundLine::ExportToLines(QVector<FPoint> &lineDots) {
    _besier.Recalc(lineDots, 6);
}

std::string SolidGroundLine::Type() {
    return "SolidGroundLine";
}

void SolidGroundLine::SaveToXml(rapidxml::xml_node<> *xe) {
    BeautyBase::SaveToXml(xe);
    Math::Write(xe, "size", (int)_besier.origin.size());
    std::string array;
    for (unsigned int j = 0; j < _besier.origin.size(); ++j)
    {
        char buff[100];
        sprintf(buff, "%g %g;", _besier.origin[j].x, _besier.origin[j].y);
        array += buff;
    }
    Math::Write(xe, "dots", array.c_str());

    char *copyColor = xe->document()->allocate_string(Math::ColorToString(_debugColor).c_str());
    xe->append_attribute(xe->document()->allocate_attribute("debug_color", copyColor));
    Math::Write(xe, "besier", 1);
    Math::Write(xe, "section", _sectionNumber);
    if (_drawTrianglesCoves)
    {
        Math::Write(xe, "drawTrianglesCoves", 1);
    }
}

SolidGroundLine::SolidGroundLine(const SolidGroundLine &l) 
: BeautyBase(l)
{
    _debugColor = l._debugColor;
    _sectionNumber = l._sectionNumber;
    _mouseDown = false;
    _icon = Core::programPath + "gfx/spline.png";

    _besier.origin = l._besier.origin;
    _drawTrianglesCoves = l._drawTrianglesCoves;

    CalcWidthAndHeight();

    _mouseDown = false;
    _dotUnderCursor = -1;
    _dotLastEdited = -1;
}

SolidGroundLine::~SolidGroundLine() {
}

SolidGroundLine::SolidGroundLine(rapidxml::xml_node<>* xe) 
: BeautyBase(xe)
{
    rapidxml::xml_attribute<> *tmp = xe->first_attribute("debug_color");
    if (tmp)
    {
        _debugColor = Math::ReadColor(tmp->value());
    }
    else
    {
        _debugColor = _color;
        _color = 0xFFFFFFFF;
    }
    rapidxml::xml_attribute<> *size = xe->first_attribute("size");
    if (size)
    {
        _besier.origin.resize(atoi(size->value()));
        std::string array = xe->first_attribute("dots")->value();
        std::string::size_type start = 0;
        std::string::size_type sigma = array.find(";");
        for (unsigned int j = 0; j < _besier.origin.size(); ++j)
        {
            sscanf(array.substr(start, sigma - start).c_str(), "%g %g", &_besier.origin[j].x, &_besier.origin[j].y);
            start = sigma + 1;
            sigma = array.find(";", start);
        }
    }
    else
    {
        bool besier = xe->first_attribute("besier") != NULL;
        rapidxml::xml_node<> *dot = xe->first_node("dot");
        while (dot != NULL) {
            FPoint p(atof(dot->first_attribute("x")->value()), atof(dot->first_attribute("y")->value()));
            if (!besier) {
                if (_besier.origin.size() != 0) {
                    _besier.origin.push_back(_besier.origin.back() + FPoint(SPLINE_OFFSET, 0));
                    _besier.origin.push_back(p + FPoint(-SPLINE_OFFSET, 0));
                }
                _besier.origin.push_back(p);
            } else {
                _besier.origin.push_back(p);
            }
            dot = dot->next_sibling("dot");
        }
    }

    _icon = Core::programPath + "gfx/spline.png";

    CalcWidthAndHeight();

    _mouseDown = false;
    _dotUnderCursor = -1;
    _dotLastEdited = -1;

    {
        rapidxml::xml_attribute<> *tmp = xe->first_attribute("drawTrianglesCoves");
        _drawTrianglesCoves = tmp != NULL;
        tmp = xe->first_attribute("section");
        if (tmp)
        {
            _sectionNumber = atoi(tmp->value());
        }
        else
        {
            _sectionNumber = 6;
        }
    }
    GenerateTriangles();
}

void SolidGroundLine::GenerateTriangles() {
    _besier.Recalc(_renderLineDots, _sectionNumber);
    if (_drawTrianglesCoves)
    {
        Math::GenerateTriangles(_renderLineDots, _triangles, _debugColor);
    }
}

void SolidGroundLine::MouseDown(const FPoint &mouse) {
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

bool SolidGroundLine::MouseMove(const FPoint &mousePos) {

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
        _besier.ShiftPoint(_dotUnderCursor, end - start);
        //xPoses[_dotUnderCursor] += (end.x - start.x);
        //yPoses.keys[_dotUnderCursor].first += (end.y - start.y);
        CalcWidthAndHeight();
    }
    _mousePos = mousePos;
    return true;
}

void SolidGroundLine::MouseUp(const FPoint &mousePos) {
    if (_mouseDown) {
        _mouseDown = false;
    }
    _dotUnderCursor = -1;
    BeautyBase::MouseUp(mousePos);
}

//FPoint SolidGroundLine::GetStart() {
//    return _besier.origin.front();
//}
//
//FPoint SolidGroundLine::GetFinish() {
//    return _besier.origin.back();
//}

int SolidGroundLine::Width() {
    return _width;
}

int SolidGroundLine::Height() {
    return _height;
}

void SolidGroundLine::CalcWidthAndHeight() {
    Rect rect;
    rect.Clear();
    CollectAllBaseDots(rect);
    _width = fabs(rect.x2 - rect.x1);
    _height = fabs(rect.y2 - rect.y1);
}

void SolidGroundLine::EncapsulateAllDots(Rect &rect) 
{
    QVector<FPoint> dots;
    _besier.Recalc(dots, 6);
    for (unsigned int i = 1; i < _screenDots.size(); ++i) {
        //rect.Encapsulate(_screenDots[i].x, _screenDots[i].y);
        rect.Encapsulate(_pos.x + dots[i].x, _pos.y + dots[i].y);
    }
}

void SolidGroundLine::GetAllLocalDotsRect(Rect &rect) {
    QVector<FPoint> dots;
    _besier.Recalc(dots, 6);
    for (unsigned int i = 1; i < dots.size(); ++i) {
        rect.Encapsulate(_pos.x + dots[i].x, _pos.y + dots[i].y);
    }
}

void SolidGroundLine::CollectAllBaseDots(Rect &rect) {
    for (unsigned int j = 0; j < _besier.origin.size(); ++j) {
        rect.Encapsulate(_besier.origin[j].x, _besier.origin[j].y); 
    }
}

const char *SolidGroundLine::GetIconTexture() {
    return _icon.c_str();
}

bool SolidGroundLine::Command(const std::string &cmd) {
    std::string position;

    if (cmd == "delete dot") {
        
        if (_dotLastEdited >= 0) {
            RemoveDot(_dotLastEdited);
            _dotLastEdited = -1;
            _dotUnderCursor = -1;
            GenerateTriangles();
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
            _dotUnderCursor = _dotLastEdited = CreateDot(mouse.x, mouse.y);
            if (_dotUnderCursor == -1)
            {
                return (cmd == "" || BeautyBase::Command(cmd));
            }
            _mouseDown = true;
            GenerateTriangles();
        }
        _mousePos = mouse;
        return true;
    }
    return (cmd == "" || BeautyBase::Command(cmd));
}

bool SolidGroundLine::PixelCheck(const FPoint &point) { 
    if (_drawTrianglesCoves)
    {
        Matrix reverse;
        reverse.MakeRevers(parent);

        FPoint p(point);
        reverse.Mul(p);
        if (Math::Inside(p, _renderLineDots))
        {
            return true;
        }
    }
    return SearchNearestWorld(point.x, point.y) >= 0 || SearchNearestLineWorld(point.x, point.y) >= 0;
}
