#include <QApplication>
#include "TiledLine.h"
#include "../Core/Core.h"
#include "../Core/Math.h"
#include "../Core/InputSystem.h"
#include "TileEditorInterface.h"

TiledLine::~TiledLine()
{
    _mesh.GetVB().Clear();
}

TiledLine::TiledLine(rapidxml::xml_node<> *xe)
: GroundLine(xe)
{
    _debugColor = 0xFFFFFFFF;
    _fileName = xe->first_attribute("texture")->value();
    ReplaceSlash(_fileName);
    _texture = Core::getTexture(_fileName);
    _temporaryMesh.Set(_texture);
    _mesh.Set(_texture);
    _mesh.GetVB().Clear();
    DrawWithParam(&_temporaryMesh.GetVB());
    _mouseDown = false;
    {
        rapidxml::xml_node<> *mesh = xe->first_node("mesh");
        if (mesh)
        {
            FPoint vertex;
            FPoint vertexUV;
            QVector<FPoint> meshXY;
            QVector<FPoint> meshUV;
            for (rapidxml::xml_node<> *i = mesh->first_node(); i; i = i->next_sibling())
            {
                vertex.x = Math::Read(i, "x", 0.f);
                vertex.y = Math::Read(i, "y", 0.f);
                meshXY.push_back(vertex);
                vertexUV.x = Math::Read(i, "tx", 0.f);
                vertexUV.y = Math::Read(i, "ty", 0.f);
                meshUV.push_back(vertexUV);
            }
            rapidxml::xml_node<> *indexesXe = xe->first_node("indexes");
            //assert(indexes);
            unsigned int count = 0;
            QVector<unsigned int> indexes;
            for (rapidxml::xml_node<> *i = indexesXe->first_node(); i; i = i->next_sibling())
            {
                indexes.resize(indexes.size() + 4);
                indexes[count++] = atoi(i->first_attribute("v0")->value());
                indexes[count++] = atoi(i->first_attribute("v1")->value());
                indexes[count++] = atoi(i->first_attribute("v2")->value());
                indexes[count++] = atoi(i->first_attribute("v3")->value());
            }
            _mesh.GetVB().Resize(meshXY.size(), indexes.size() / 4 * 6);
            for (unsigned int i = 0; i < meshUV.size(); ++i)
            {
                _mesh.GetVB().VertXY(i) = meshXY[i];
                _mesh.GetVB().VertUV(i) = meshUV[i];
            }
            for (unsigned int i = 0; i < indexes.size() / 4; ++i)
            {
                _mesh.GetVB().Index(6 * i)     = indexes[4 * i];
                _mesh.GetVB().Index(6 * i + 1) = indexes[4 * i + 1];
                _mesh.GetVB().Index(6 * i + 2) = indexes[4 * i + 2];
                _mesh.GetVB().Index(6 * i + 3) = indexes[4 * i];
                _mesh.GetVB().Index(6 * i + 4) = indexes[4 * i + 2];
                _mesh.GetVB().Index(6 * i + 5) = indexes[4 * i + 3];
            }
        }
    }
}

TiledLine::TiledLine(const TiledLine &l)
: GroundLine(l)
{
    _debugColor = 0xFFFFFFFF;
    _fileName = l._fileName;
    _texture = l._texture;
    _mesh = l._mesh;
    _temporaryMesh = l._temporaryMesh;
    _mouseDown = false;
}

std::string TiledLine::Type() 
{
    return "TiledLine"; 
}

void TiledLine::DebugDraw(bool onlyControl) {
    
    if (!onlyControl) {
        Draw();
    }

    BeautyBase::DebugDrawPointForAnything();

    if (_mesh.GetVB().Size() > 0 || !TileEditorInterface::Instance()->CreateDotMode())
    {
        _debugDraw = true;
    
        Render::SetFiltering(false);
        Render::PushMatrix();

        Render::MatrixMove(_pos.x, _pos.y);
        parent = Render::GetCurrentMatrix();

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
    //else
    if (_mesh.GetVB().Size() == 0 && TileEditorInterface::Instance()->CreateDotMode())
    {
        GroundLine::DebugDraw(true);
    }

}

void TiledLine::Draw()
{
    Render::PushColorAndMul(_color);
    Render::PushMatrix();
    Render::MatrixMove(_pos.x, _pos.y);

    if (_mesh.GetVB().Size())
    {
        _mesh.Render();

        _screenDots.resize(_mesh.GetVB().Size());
        for (unsigned int i = 0; i < _mesh.GetVB().Size(); ++i) {
            _screenDots[i].x = _mesh.GetVB().VertXY(i).x;
            _screenDots[i].y = _mesh.GetVB().VertXY(i).y;
            Render::GetCurrentMatrix().Mul(_screenDots[i]);    
        }

    }
    else
    {
        _temporaryMesh.Render();

        _screenDots.resize(_temporaryMesh.GetVB().Size());
        for (unsigned int i = 0; i < _temporaryMesh.GetVB().Size(); ++i) {
            _screenDots[i].x = _temporaryMesh.GetVB().VertXY(i).x;
            _screenDots[i].y = _temporaryMesh.GetVB().VertXY(i).y;
            Render::GetCurrentMatrix().Mul(_screenDots[i]);    
        }
    }
    Render::PopMatrix();
    if (_mesh.GetVB().Size() == 0 && TileEditorInterface::Instance()->CreateDotMode())
    {
        GroundLine::Draw();
    }
    Render::PopColor();
}

void TiledLine::DrawWithParam(VertexBuffer *mesh)
{
    if (mesh != NULL && mesh->Size() != 0)
    {
        mesh->Clear();
    }
    float f = (_dots[1] - _dots[0]).Length();
    f /= _texture->Width();
    Render::DrawStripe(_dots, _texture, 0.f, 1.f, 0.f, f, 0, 2, mesh);
}

void TiledLine::SaveMeshToXml(VertexBuffer &vb, rapidxml::xml_node<> *xe)
{
    rapidxml::xml_node<> *mesh = xe->document()->allocate_node(rapidxml::node_element, "mesh");
    xe->append_node(mesh);
    Math::Write(mesh, "vert", vb.Size());
    Math::Write(mesh, "ind", vb.SizeIndex());
    for (unsigned int i = 0; i < vb.Size(); ++i)
    {
        rapidxml::xml_node<> *vertex = xe->document()->allocate_node(rapidxml::node_element, "vert");
        mesh->append_node(vertex);
        Math::Write(vertex, "x", vb.VertXY(i).x);
        Math::Write(vertex, "y", vb.VertXY(i).y);
        Math::Write(vertex, "tx", vb.VertUV(i).x);
        Math::Write(vertex, "ty", vb.VertUV(i).y);
    }
    rapidxml::xml_node<> *indexes = xe->document()->allocate_node(rapidxml::node_element, "indexes");
    xe->append_node(indexes);
    for (unsigned int i = 0; i < vb.SizeIndex(); i += 6)
    {
        rapidxml::xml_node<> *quad = xe->document()->allocate_node(rapidxml::node_element, "poly");
        indexes->append_node(quad);
        Math::Write(quad, "v0", vb.Index(i + 0));
        Math::Write(quad, "v1", vb.Index(i + 1));
        Math::Write(quad, "v2", vb.Index(i + 2));
        Math::Write(quad, "v3", vb.Index(i + 5));
    }
}

void TiledLine::SaveToXml(rapidxml::xml_node<> *xe)
{
    GroundLine::SaveToXml(xe);
    char *copyName = xe->document()->allocate_string(_fileName.c_str());
    xe->append_attribute(xe->document()->allocate_attribute("texture", copyName));
    if (_mesh.GetVB().Size())
    {
        SaveMeshToXml(_mesh.GetVB(), xe);
    }
    else
    {
        VertexBuffer vb;
        DrawWithParam(&vb);
        SaveMeshToXml(vb, xe);
    }
}

bool TiledLine::Command(const std::string &cmd) {
    if (cmd == "Mesh") 
    {
        if (_mesh.GetVB().Size())
        {
            _mesh.GetVB().Clear();
        }
        else
        {
            DrawWithParam(&_mesh.GetVB());
        }
        return true;
    }
    std::string position;
    if (_mesh.GetVB().Size() != 0 && Messager::CanCut(cmd, "create dot at ", position)) {

        FPoint mouse;
        sscanf(position.c_str(), "%f %f", &(mouse.x), &(mouse.y));
        _mousePos = mouse;

        QVector<int> index = SearchNearest(mouse.x, mouse.y);
        if (index.size() > 0) {
            for (unsigned int i = 0; i < _selectedDots.size(); ++i)
            {
                if (index[0] == _selectedDots[i])
                {
                    _dotUnderCursor = _selectedDots;
                    _mouseDown = true;
                    return true;
                }
            }
            _dotUnderCursor = _selectedDots = index;
            _mouseDown = true;
        }
        else
        {
            return BeautyBase::Command(cmd);
        }
        return _mouseDown;
    }
    if (cmd == "")
    {
        return true;
    }
    if (GroundLine::Command(cmd))
    {
        DrawWithParam(&_temporaryMesh.GetVB());
        return true;
    }
    return false;
}

void TiledLine::MouseDown(const FPoint &mouse)
{
    if (_mesh.GetVB().Size() == 0)
    {
        GroundLine::MouseDown(mouse);
        DrawWithParam(&_temporaryMesh.GetVB());
        return;
    }

    if (!_debugDraw) {
        return;
    }
        
    _mouseDown = true;
    _mousePos = mouse;
}

bool TiledLine::MouseMove(const FPoint &mousePos)
{
    GroundLine::MouseMove(mousePos);
    DrawWithParam(&_temporaryMesh.GetVB());

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

    if (_dotUnderCursor.size() == 0) {
        _pos.x += (end.x - start.x);
        _pos.y += (end.y - start.y);
    } else if ((QApplication::keyboardModifiers() & Qt::ControlModifier) != 0) {
        float deltaScale = end.Length() / start.Length();
        for (unsigned int i = 0; i < _selectedDots.size(); ++i)
        {
            _mesh.GetVB().VertXY(_selectedDots[i]).x *= deltaScale;
            _mesh.GetVB().VertXY(_selectedDots[i]).y *= deltaScale;
        }
    } else {
        float dx = (end - start).x;
        float dy = (end - start).y;
        for (unsigned int i = 0; i < _selectedDots.size(); ++i)
        {
            _mesh.GetVB().VertXY(_selectedDots[i]).x += dx;
            _mesh.GetVB().VertXY(_selectedDots[i]).y += dy;
        }
    }
    _mousePos = mousePos;
    return true;
}

void TiledLine::MouseUp(const FPoint &mouse)
{
    GroundLine::MouseUp(mouse);
    DrawWithParam(&_temporaryMesh.GetVB());
    if (_mouseDown) {
        _mouseDown = false;
    }
}

QVector<int> TiledLine::SearchNearest(float x, float y) {
    QVector<int> result;
    static const float SIZEX = 20;
    FPoint p(x, y);
    for (unsigned int i = 0; i < _screenDots.size(); ++i) {
        if ((_screenDots[i] - p).Length() < SIZEX) {
            result.push_back(i);
        }
    }
    return result;
}

bool TiledLine::Selection(const Rect& rect, bool full)
{
    _selectedDots.clear();
    for (unsigned int i = 0; i < _screenDots.size(); ++i) {
        if (rect.TestPoint(_screenDots[i].x, _screenDots[i].y)) {
            _selectedDots.push_back(i);
        }
    }
    return !full || _selectedDots.size() == _screenDots.size();
}

const char *TiledLine::GetIconTexture() {
    return NULL;
}

bool TiledLine::GeometryCheck(const FPoint &point) 
{
    Matrix localMatrix;
    localMatrix.Move(_pos.x, _pos.y);

    Matrix rev;
    rev.MakeRevers(localMatrix);

    FPoint check(point);
    rev.Mul(check);

    if (_mesh.GetVB().Size())
    {
        return _mesh.GeometryCheck(check.x, check.y); 
    }
    return _temporaryMesh.GeometryCheck(check.x, check.y); 
}

bool TiledLine::PixelCheck(const FPoint &point)
{
    Matrix localMatrix;
    localMatrix.Move(_pos.x, _pos.y);

    Matrix rev;
    rev.MakeRevers(localMatrix);

    FPoint check(point);
    rev.Mul(check);

    if (_mesh.GetVB().Size())
    {
        return _mesh.PixelCheck(check.x, check.y); 
    }
    return _temporaryMesh.PixelCheck(check.x, check.y); 
}

void TiledLine::EncapsulateAllDots(Rect &rect)
{
    if (_mesh.GetVB().Size())
    {
        for (unsigned int i = 0; i < _mesh.GetVB().Size(); ++i) {
            const FPoint &check = _mesh.GetVB().VertXY(i);
            rect.Encapsulate(check.x + _pos.x, check.y + _pos.y);
        }
        return;
    }
    for (unsigned int i = 0; i < _temporaryMesh.GetVB().Size(); ++i) {
        const FPoint &check = _temporaryMesh.GetVB().VertXY(i);
        rect.Encapsulate(check.x + _pos.x, check.y + _pos.y);
    }
}

void TiledLine::GetAllLocalDotsRect(Rect &rect)
{
    EncapsulateAllDots(rect);
}
