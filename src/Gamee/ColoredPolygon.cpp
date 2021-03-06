#include <QApplication>
#include "ColoredPolygon.h"
#include "../Core/Math.h"
#include "../Core/Core.h"
#include "../Core/Messager.h"
#include "../Core/InputSystem.h"
#include "../Core/ogl/vertexbuffer.h"
#include "TileEditorInterface.h"
#include <math.h>

ColoredPolygon::~ColoredPolygon() {
}

ColoredPolygon::ColoredPolygon(const ColoredPolygon &c) 
: BeautyBase(c)
{
    _dots = c._dots;
    _texture = c._texture;
    _texturePath = c._texturePath;
    _textureScale = c._textureScale;
    _textureAngle = c._textureAngle;
    _textureTransform.Unit();
    if (_texture)
    {
        _textureTransform.Scale( _textureScale / _texture->Width()
            , _textureScale / _texture->Height());
        _textureTransform.Rotate(_textureAngle);
    }
    CalcWidthAndHeight();
    GenerateTriangles();
    _mouseDown = false;
    _dotUnderCursor.clear();
    _selectedDots.clear();
}

ColoredPolygon::ColoredPolygon(rapidxml::xml_node<> *xe)
: BeautyBase(xe)
{
    rapidxml::xml_attribute<> *tmp = xe->first_attribute("texture");
    if (tmp)
    {
        _texturePath = tmp->value();
        ReplaceSlash(_texturePath);
        _texture = Core::getTexture(_texturePath);
        _textureScale = Math::Read(xe, "textureScale", 1.f);
        _textureAngle = Math::Read(xe, "textureAngle", 0.f);
        _textureTransform.Unit();
        _textureTransform.Scale( _textureScale / _texture->Width(), _textureScale / _texture->Height());
        _textureTransform.Rotate(_textureAngle);

    }
    else
    {
        _texture = 0;
        _textureScale = 1.f;
        _textureAngle = 0.f;
    }

    rapidxml::xml_node<> *dot = xe->first_node("dot");
    if (dot == NULL)
    {
        rapidxml::xml_attribute<> *size = xe->first_attribute("size");
        if (size)
        {
            _dots.resize(atoi(size->value()));
            std::string array = xe->first_attribute("dots")->value();
            std::string::size_type start = 0;
            std::string::size_type sigma = array.find(";");
            for (unsigned int j = 0; j < _dots.size(); ++j)
            {
                sscanf(array.substr(start, sigma - start).c_str(), "%g %g", &_dots[j].x, &_dots[j].y);
                start = sigma + 1;
                sigma = array.find(";", start);
            }
        }
        else
        {
            rapidxml::xml_node<> *mesh = xe->first_node("mesh");
            if (mesh)
            {
                int vert = atoi(mesh->first_attribute("vert")->value());
                int ind = atoi(mesh->first_attribute("ind")->value());
                _triangles.GetVB().Resize(vert, ind);
                _dots.resize(vert);
                //_dots.resize(atoi(mesh->first_attribute("vert")->value()));
                unsigned int count = 0;
                for (rapidxml::xml_node<> *i = mesh->first_node(); i; i = i->next_sibling())
                {
                    sscanf(i->first_attribute("geom")->value(), "%g;%g;%g;%g"
                           , &_triangles.GetVB().VertXY(count).x
                           , &_triangles.GetVB().VertXY(count).y
                           , &_triangles.GetVB().VertUV(count).x
                           , &_triangles.GetVB().VertUV(count).y
                           );
                    _dots[count] = _triangles.GetVB().VertXY(count);
                    count++;
                }
            }
        }
    }
    else
    {
        while (dot != NULL) {
            _dots.push_back(FPoint(atof(dot->first_attribute("x")->value()), atof(dot->first_attribute("y")->value())));
            dot = dot->next_sibling("dot");
        }
    }
    CalcWidthAndHeight();
    GenerateTriangles();
    _mouseDown = false;
    _dotUnderCursor.clear();
    _selectedDots.clear();
}

void ColoredPolygon::Draw() {

    Render::PushMatrix();
    Render::MatrixMove(_pos.x, _pos.y);

    DrawTriangles();

    _screenDots = _dots;
    for (unsigned int i = 0; i < _dots.size(); ++i) {
        Render::GetCurrentMatrix().Mul(_screenDots[i]);    
    }
    if (_triangles.GetVB().Size() == 0) 
    {
        for (unsigned int i = 0; i < _dots.size(); ++i) {
            Render::Line(_dots[i].x, _dots[i].y, _dots[(i + 1) % _dots.size()].x, _dots[(i + 1) % _dots.size()].y, 0xFFFFFFFF);
        }
    }

    Render::PopMatrix();
    BeautyBase::Draw();
}

void ColoredPolygon::DebugDraw(bool onlyControl) {
    if (!onlyControl) {
        Draw();
    }
    {
        _debugDraw = true;
    
        Render::SetFiltering(false);
        Render::PushMatrix();

        Render::MatrixMove(_pos.x, _pos.y);
        parent = Render::GetCurrentMatrix();
        for (unsigned int i = 0; i < _dots.size(); ++i) {
            Render::Line(_dots[i].x, _dots[i].y, _dots[(i + 1) % _dots.size()].x, _dots[(i + 1) % _dots.size()].y, 0xFFFFFFFF);
        }

        Render::SetMatrixUnit();

        float alpha = (Render::GetColor() >> 24) / 255.f;
        Render::SetAlpha(Math::round(0xAF * alpha));
        std::set<int> drawBig;
        for (unsigned int i = 0; i < _dotUnderCursor.size(); ++i)
        {
            drawBig.insert(_dotUnderCursor[i]);
        }
        for (unsigned int i = 0; i < _selectedDots.size(); ++i)
        {
            drawBig.insert(_selectedDots[i]);
        }

        for (unsigned int i = 0; i < _screenDots.size(); ++i) {
            if (drawBig.end() != drawBig.find(i)) {
                scale->Render(_screenDots[i].x - scale->Width() / 2.f, _screenDots[i].y - scale->Height() / 2.f);
            } else {
                scaleSide->Render(_screenDots[i].x - scaleSide->Width() / 2.f, _screenDots[i].y - scaleSide->Height() / 2.f);
            }
        }
        Render::SetAlpha(Math::round(0xFF * alpha));

        Render::PopMatrix();
        Render::SetFiltering(TileEditorInterface::Instance()->FilteringTexture());
    }
}

bool ColoredPolygon::PixelCheck(const FPoint &point) { 
    if (Math::Inside(point - _pos, _dots)) {
        return true;
    }
    return SearchNearest(point.x, point.y) >= 0;
}

void ColoredPolygon::SaveToXml(rapidxml::xml_node<> *xe) {
    BeautyBase::SaveToXml(xe);
    if (_texture != 0)
    {
        char *copyName = xe->document()->allocate_string(_texturePath.c_str());
        xe->append_attribute(xe->document()->allocate_attribute("texture", copyName));
        Math::Write(xe, "textureScale", _textureScale);
        Math::Write(xe, "textureAngle", _textureAngle);
    }
    if (_triangles.GetVB().Size())
    {
        rapidxml::xml_node<> *mesh = xe->document()->allocate_node(rapidxml::node_element, "mesh");
        xe->append_node(mesh);
        Math::Write(mesh, "vert", (int)_triangles.GetVB().Size());
        Math::Write(mesh, "ind", (int)_triangles.GetVB().SizeIndex());
        for (unsigned int i = 0; i < _triangles.GetVB().Size(); ++i)
        {
            rapidxml::xml_node<> *vertex = xe->document()->allocate_node(rapidxml::node_element, "vert");
            mesh->append_node(vertex);
            char buff[1000];
            sprintf(buff, "%0.0f;%0.0f;%g;%g", _triangles.GetVB().VertXY(i).x, _triangles.GetVB().VertXY(i).y
                                        , _triangles.GetVB().VertUV(i).x, _triangles.GetVB().VertUV(i).y);
            Math::Write(vertex, "geom", buff);
        }
        rapidxml::xml_node<> *indexes = xe->document()->allocate_node(rapidxml::node_element, "indexes");
        xe->append_node(indexes);

        std::string array;
        for (unsigned int i = 0; i < _triangles.GetVB().SizeIndex(); ++i)
        {
            array += Math::IntToStr((int)_triangles.GetVB().Index(i));
            array += ";";
        }
        Math::Write(indexes, "array", array.c_str());
    }
    else
    {
        Math::Write(xe, "size", (int)_dots.size());
        std::string array;
        for (unsigned int j = 0; j < _dots.size(); ++j)
        {
            char buff[100];
            sprintf(buff, "%g %g;", _dots[j].x, _dots[j].y);
            array += buff;
        }
        Math::Write(xe, "dots", array.c_str());
     }
}

std::string ColoredPolygon::Type() { 
    return "ColoredPolygon"; 
}

int ColoredPolygon::Width() {
    return _width;
}

int ColoredPolygon::Height() {
    return _height;
}

void ColoredPolygon::CalcWidthAndHeight() {
    Rect rect;
    rect.Clear();
    for (unsigned int i = 0; i < _dots.size(); ++i) {
        rect.Encapsulate(_dots[i].x, _dots[i].y);
    }
    _width = fabs(rect.x2 - rect.x1);
    _height = fabs(rect.y2 - rect.y1);
}

void ColoredPolygon::GenerateTriangles()
{
    Math::GenerateTriangles(_dots, _triangles, _color, _texture, &_textureTransform);
}

void ColoredPolygon::DrawTriangles() {
    Render::PushColorAndMul(_color);
    if (_texture)
        _texture->bind();
    _triangles.Render();
    if (_texture)
        _texture->unbind();
    Render::PopColor();
}

int ColoredPolygon::SearchNearest(float x, float y)
{
    static const float SIZEX = 20;
    FPoint p(x, y);
    for (unsigned int i = 0; i < _screenDots.size(); ++i)
    {
        if ((_screenDots[i] - p).Length() < SIZEX)
        {
            return i;
        }
    }
    return -1;
}

void ColoredPolygon::EncapsulateAllDots(Rect &rect) {
    for (unsigned int i = 0; i < _screenDots.size(); ++i) {
        //rect.Encapsulate(_screenDots[i].x, _screenDots[i].y);
        rect.Encapsulate(_pos.x + _dots[i].x, _pos.y + _dots[i].y);
    }
}

void ColoredPolygon::GetAllLocalDotsRect(Rect &rect) {
    for (unsigned int i = 0; i < _dots.size(); ++i) {
        rect.Encapsulate(_pos.x + _dots[i].x, _pos.y + _dots[i].y);
    }
}


int ColoredPolygon::CreateDot(float x, float y) {
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
    if (result >= 0) {
        GenerateTriangles();
    }
    return result;
}

void ColoredPolygon::RemoveDot(QVector<int> index) {
    if (_dots.size() <= 3) {
        return;
    }
    QVector<FPoint> tmp;
    for (int i = 0; i < _dots.size(); ++i)
    {
        int j = 0;
        for (; j < index.size() && i != index[j]; ++j);
        if (j >= index.size())
        {
            tmp.push_back(_dots[i]);
        }
    }
    _dots = tmp;
}

void ColoredPolygon::MouseDown(const FPoint &mouse) {
    _selectedDots.clear();

    if (!_debugDraw) {
        return;
    }
    if (!TileEditorInterface::Instance()->CreateDotMode()) {
        _selectedDots = _dotUnderCursor;
    }
        
    _mouseDown = true;
    _mousePos = mouse;
}

bool ColoredPolygon::MouseMove(const FPoint &mousePos) {
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
        int r = SearchNearest(mousePos.x, mousePos.y);
        if (r == -1)
        {
            _dotUnderCursor.clear();
        }
        else
        {
            _dotUnderCursor = QVector<int>(1, r);
        }
        _mousePos = mousePos;
        return false;
    }
    Matrix reverse;
    reverse.MakeRevers(parent);

    FPoint start(_mousePos);
    FPoint end(mousePos);
    reverse.Mul(start);
    reverse.Mul(end);

    if (_dotUnderCursor.size() == 0) {
        _pos.x += (end.x - start.x);
        _pos.y += (end.y - start.y);
    } else {
        for (unsigned int i = 0; i < _dotUnderCursor.size(); ++i)
        {
            _dots[_dotUnderCursor[i]].x += (end.x - start.x);
            _dots[_dotUnderCursor[i]].y += (end.y - start.y);
        }
        CalcWidthAndHeight();
    }
    _mousePos = mousePos;
    return true;
}

void ColoredPolygon::MouseUp(const FPoint &mouse) {
    _mouseDown = false;
    _dotUnderCursor.clear();
    GenerateTriangles();
}

const char *ColoredPolygon::GetIconTexture() {
    return NULL;
}

bool ColoredPolygon::Command(const std::string &cmd) {
    std::string position;

    if (cmd == "Minus") 
    {
        _textureScale += 0.1f;
        _textureTransform.Unit();
        if (_texture)
        {
            _textureTransform.Scale( _textureScale / _texture->Width(), _textureScale / _texture->Height());
            _textureTransform.Rotate(_textureAngle);
        }
        GenerateTriangles();
        return true;
    }
    if (cmd == "Plus") 
    {
        _textureScale = std::max(0.1f, _textureScale - 0.1f);
        _textureTransform.Unit();
        if (_texture)
        {
            _textureTransform.Scale( _textureScale / _texture->Width(), _textureScale / _texture->Height());
            _textureTransform.Rotate(_textureAngle);
        }
        GenerateTriangles();
        return true;
    }
    if (cmd == "Rigth")
    {
        _textureAngle += M_PI / 6.f;
        _textureTransform.Unit();
        if (_texture)
        {
            _textureTransform.Scale( _textureScale / _texture->Width(), _textureScale / _texture->Height());
            _textureTransform.Rotate(_textureAngle);
        }
        GenerateTriangles();
        return true;
    }
    if (cmd == "Left")
    {
        _textureAngle -= M_PI / 6.f;
        _textureTransform.Unit();
        if (_texture)
        {
            _textureTransform.Scale( _textureScale / _texture->Width(), _textureScale / _texture->Height());
            _textureTransform.Rotate(_textureAngle);
        }
        GenerateTriangles();
        return true;
    }
    if (cmd == "delete dot")
    {
        if (_selectedDots.size() > 0)
        {
            RemoveDot(_selectedDots);
            _selectedDots.clear();
            _dotUnderCursor.clear();
            GenerateTriangles();
        }

        return true;
    } 
    if (Messager::CanCut(cmd, "create dot at ", position)) {

        FPoint mouse;
        sscanf(position.c_str(), "%f %f", &(mouse.x), &(mouse.y));

        int index = SearchNearest(mouse.x, mouse.y);
        if (index >= 0) {
            int i = 0;
            for (; i < _selectedDots.size() && _selectedDots[i] != index; ++i)
            {}
            if (i >= _selectedDots.size())
            {
                _dotUnderCursor = _selectedDots = QVector<int>(1, index);
            }
            else
            {
                _dotUnderCursor = _selectedDots;
            }
            _mouseDown = true;
        } else {
            Matrix reverse;
            reverse.MakeRevers(parent);
            FPoint fp(mouse);
            reverse.Mul(fp);

            int result = CreateDot(fp.x, fp.y);
            if (result >= 0)
            {
                _dotUnderCursor = _selectedDots = QVector<int>(1, result);
            }
            else
            {
                _dotUnderCursor.clear();
                _selectedDots.clear();
            }
            _mouseDown = _selectedDots.size() > 0;
        }
        _mousePos = mouse;
        return _mouseDown;
    }
    return (cmd == "" || BeautyBase::Command(cmd));
}

bool ColoredPolygon::GeometryCheck(const FPoint &point) {
    FPoint p(point - _pos);
    return Math::Inside(p, _dots); 
}

bool ColoredPolygon::Selection(const Rect& rect, bool full)
{
    _selectedDots.clear();
    for (unsigned int i = 0; i < _screenDots.size(); ++i) {
        if (rect.TestPoint(_screenDots[i].x, _screenDots[i].y)) {
            _selectedDots.push_back(i);
        }
    }
    return !full || _selectedDots.size() == _screenDots.size();
}
