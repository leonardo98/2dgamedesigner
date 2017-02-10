#include <QApplication>
#include "StretchTexture.h"
#include "../Core/Core.h"
#include "../Core/Math.h"
#include "../Core/InputSystem.h"
#include "TileEditorInterface.h"

StretchTexture::~StretchTexture()
{
	_mesh.GetVB().Clear();
}

StretchTexture::StretchTexture(rapidxml::xml_node<> *xe)
: SolidGroundLine(xe)
{
    _fileName = xe->first_attribute("texture")->value();
    ReplaceSlash(_fileName);
	_texture = Core::getTexture(_fileName);
    _standart = xe->first_attribute("standart") == NULL ? 0 : atoi(xe->first_attribute("standart")->value());
	_temporaryMesh.Set(_texture);
	_mesh.Set(_texture);
	_mesh.GetVB().Clear();
	DrawWithParam(&_temporaryMesh.GetVB());
	_mouseDown = false;

    rapidxml::xml_node<> *mesh = xe->first_node("mesh");
    rapidxml::xml_node<> *indexesXe = xe->first_node("indexes");
    rapidxml::xml_attribute<> *indexArray = indexesXe->first_attribute("array");
    if (mesh && indexesXe && indexArray)
    {
        int vert = atoi(mesh->first_attribute("vert")->value());
        int ind = atoi(mesh->first_attribute("ind")->value());
        _mesh.GetVB().Resize(vert, ind);
        //_dots.resize(atoi(mesh->first_attribute("vert")->value()));
        unsigned int count = 0;
        for (rapidxml::xml_node<> *i = mesh->first_node(); i; i = i->next_sibling())
        {
            sscanf(i->first_attribute("geom")->value(), "%g;%g;%g;%g"
                   , &_mesh.GetVB().VertXY(count).x
                   , &_mesh.GetVB().VertXY(count).y
                   , &_mesh.GetVB().VertUV(count).x
                   , &_mesh.GetVB().VertUV(count).y
                   );
            count++;
        }

        std::string array = indexArray->value();
        std::string::size_type start = 0;
        std::string::size_type sigma = array.find(";");
        count = 0;
        while (sigma != std::string::npos)
        {
            assert(count < ind);
            sscanf(array.substr(start, sigma - start).c_str(), "%i", &(_mesh.GetVB().Index(count)));
            start = sigma + 1;
            sigma = array.find(";", start);
            count++;
        }

    }
    else if (mesh)
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
        VertexBuffer tmpMesh;
        tmpMesh.Resize(meshXY.size(), indexes.size() / 4 * 6);
        for (unsigned int i = 0; i < meshUV.size(); ++i)
        {
            tmpMesh.VertXY(i) = meshXY[i];
            tmpMesh.VertUV(i) = meshUV[i];
        }
        for (unsigned int i = 0; i < indexes.size() / 4; ++i)
        {
            tmpMesh.Index(6 * i)     = indexes[4 * i];
            tmpMesh.Index(6 * i + 1) = indexes[4 * i + 1];
            tmpMesh.Index(6 * i + 2) = indexes[4 * i + 2];
            tmpMesh.Index(6 * i + 3) = indexes[4 * i];
            tmpMesh.Index(6 * i + 4) = indexes[4 * i + 2];
            tmpMesh.Index(6 * i + 5) = indexes[4 * i + 3];
        }
        _mesh.GetVB().Union(tmpMesh);
    }
}

StretchTexture::StretchTexture(const StretchTexture &l)
: SolidGroundLine(l)
{
	_fileName = l._fileName;
	_texture = l._texture;
	_standart = l._standart;
	_mesh = l._mesh;
	_temporaryMesh = l._temporaryMesh;
	_mouseDown = false;
}

std::string StretchTexture::Type() 
{
	return "StretchTexture"; 
}

void StretchTexture::DebugDraw(bool onlyControl) {
	
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
		SolidGroundLine::DebugDraw(true);
	}

}

void StretchTexture::Draw()
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
		_temporaryMesh.Render();// DrawWithParam(NULL);

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
		SolidGroundLine::Draw();
	}
	Render::PopColor();
}

void StretchTexture::DrawWithParam(VertexBuffer *mesh)
{
	if (mesh != NULL && mesh->Size() != 0)
	{
		mesh->Clear();
	}
	QVector<FPoint> dots;
	float ln = _besier.Length();
	if (_standart == 0)//     
	{
        int n = std::max(3, Math::round(ln / _texture->Width() * 4));
		_besier.RecalcWithNumber(dots, n);
		Render::DrawSmartStripe(dots, _texture, mesh);
	}
	else if (_standart == 1)
	{
        int n = std::max(2, Math::round(ln / _texture->Width() * 4));
		_besier.RecalcWithNumber(dots, n);
		assert(dots.size() >= 3);
		if (dots.size() == 3)
		{
			//  
			Render::DrawStripe(dots, _texture, 0.5f, 1.f, 0.25f, 0.25f, 0, 3, mesh);
		}
		else if (dots.size() == 4)
		{
			Render::DrawStripe(dots, _texture, 0.5f, 1.f, 0.25f, 0.25f, 0, 3, mesh);
			//  
			Render::DrawStripe(dots, _texture, 0.5f, 1.f, 0.25f, 0.25f, dots.size() - 3, dots.size(), mesh);
		}
		else if (dots.size() % 4 == 1)
		{
			//  
			Render::DrawStripe(dots, _texture, 0.5f, 1.f, 0.25f, -0.25f, 0, 2, mesh);
			Render::DrawStripe(dots, _texture, 0.f, 0.5f, 0.0f, 0.25f, 1, dots.size() - 1, mesh);
			Render::DrawStripe(dots, _texture, 0.5f, 1.f, 0.5f, 0.25f, dots.size() - 2, dots.size(), mesh);
		}
		else if (dots.size() % 4 == 2)
		{
			Render::DrawStripe(dots, _texture, 0.5f, 1.f, 0.25f, -0.25f, 0, 2, mesh);
			Render::DrawStripe(dots, _texture, 0.f, 0.5f, 0.0f, 0.25f, 1, dots.size() - 2, mesh);
			Render::DrawStripe(dots, _texture, 0.5f, 1.f, 0.5f, 0.25f, dots.size() - 3, dots.size() - 1, mesh);
			//   
			Render::DrawStripe(dots, _texture, 0.5f, 1.f, 0.25f, 0.25f, dots.size() - 3, dots.size(), mesh);
		}
		else if (dots.size() % 4 == 3)
		{
			//  
			Render::DrawStripe(dots, _texture, 0.5f, 1.f, 0.25f, -0.25f, 0, 2, mesh);
			Render::DrawStripe(dots, _texture, 0.f, 0.5f, 0.0f, 0.25f, 1, dots.size() - 1, mesh);
			Render::DrawStripe(dots, _texture, 0.5f, 1.f, 0.0f, 0.25f, dots.size() - 2, dots.size(), mesh);
		}
		else 
		{
			Render::DrawStripe(dots, _texture, 0.5f, 1.f, 0.25f, -0.25f, 0, 2, mesh);
			Render::DrawStripe(dots, _texture, 0.f, 0.5f, 0.0f, 0.25f, 1, dots.size() - 2, mesh);
			Render::DrawStripe(dots, _texture, 0.5f, 1.f, 0.0f, 0.25f, dots.size() - 3, dots.size() - 1, mesh);
			//   
			Render::DrawStripe(dots, _texture, 0.5f, 1.f, 0.25f, 0.25f, dots.size() - 3, dots.size(), mesh);
		}
	}
	else if (_standart == 2)
	{
        int n = std::max(2, Math::round(ln / _texture->Width() * 2));
		_besier.RecalcWithNumber(dots, n);
		Render::DrawStripe(dots, _texture, 0.f, 1.f, 0.f, 0.5f, 0, dots.size(), mesh);
	}
	else if (_standart == 3)
	{
		//   
        int n = std::max(2, Math::round(ln / _texture->Width()) * 2);
		_besier.RecalcWithNumber(dots, n);
		if (dots.size() == 3)
		{
			Render::DrawStripe(dots, _texture, 0.5f, 1.f, 0.5f, 0.5f, 0, 2, mesh);
			Render::DrawStripe(dots, _texture, 0.5f, 1.f, 0.f, 0.5f, 1, 3, mesh);
		}
		else 
		{
			Render::DrawStripe(dots, _texture, 0.5f, 1.f, 0.5f, 0.5f, 0, 2, mesh);
			Render::DrawStripe(dots, _texture, 0.f, 0.5f, 0.f, 0.5f, 1, dots.size() - 1, mesh);
			Render::DrawStripe(dots, _texture, 0.5f, 1.f, 0.f, 0.5f, dots.size() - 2, dots.size(), mesh);
		}
	}
    else if (_standart == 4)
    {
        // ничего внахлест всегда
        float x1 = std::min(_besier.origin.front().x, _besier.origin.back().x);
        float y1 = std::min(_besier.origin.front().y, _besier.origin.back().y);
        float x2 = std::max(_besier.origin.front().x, _besier.origin.back().x);
        float y2 = std::max(_besier.origin.front().y, _besier.origin.back().y);

        float fixed = 22;

        Render::DrawTexturePart(_texture,                         0, 0,                     fixed, fixed,         x1, y1, x1 + fixed, y1 + fixed, mesh);
        Render::DrawTexturePart(_texture,                     fixed, 0, _texture->Width() - fixed, fixed, x1 + fixed, y1, x2 - fixed, y1 + fixed, mesh);
        Render::DrawTexturePart(_texture, _texture->Width() - fixed, 0,         _texture->Width(), fixed, x2 - fixed, y1,         x2, y1 + fixed, mesh);

        Render::DrawTexturePart(_texture,                         0, fixed,                     fixed, _texture->Height() - fixed,         x1, y1 + fixed, x1 + fixed, y2 - fixed, mesh);
        Render::DrawTexturePart(_texture,                     fixed, fixed, _texture->Width() - fixed, _texture->Height() - fixed, x1 + fixed, y1 + fixed, x2 - fixed, y2 - fixed, mesh);
        Render::DrawTexturePart(_texture, _texture->Width() - fixed, fixed,         _texture->Width(), _texture->Height() - fixed, x2 - fixed, y1 + fixed,         x2, y2 - fixed, mesh);

        Render::DrawTexturePart(_texture,                         0, _texture->Height() - fixed,                     fixed, _texture->Height(),         x1, y2 - fixed, x1 + fixed, y2, mesh);
        Render::DrawTexturePart(_texture,                     fixed, _texture->Height() - fixed, _texture->Width() - fixed, _texture->Height(), x1 + fixed, y2 - fixed, x2 - fixed, y2, mesh);
        Render::DrawTexturePart(_texture, _texture->Width() - fixed, _texture->Height() - fixed,         _texture->Width(), _texture->Height(), x2 - fixed, y2 - fixed,         x2, y2, mesh);

    }
}

void StretchTexture::SaveMeshToXml(VertexBuffer &vb, rapidxml::xml_node<> *xe)
{
    assert(vb.Size());
    rapidxml::xml_node<> *mesh = xe->document()->allocate_node(rapidxml::node_element, "mesh");
    xe->append_node(mesh);
    Math::Write(mesh, "vert", (int)vb.Size());
    Math::Write(mesh, "ind", (int)vb.SizeIndex());
    for (unsigned int i = 0; i < vb.Size(); ++i)
    {
        rapidxml::xml_node<> *vertex = xe->document()->allocate_node(rapidxml::node_element, "vert");
        mesh->append_node(vertex);
        char buff[1000];
        sprintf(buff, "%0.0f;%0.0f;%g;%g", vb.VertXY(i).x, vb.VertXY(i).y
                                    , vb.VertUV(i).x, vb.VertUV(i).y);
        Math::Write(vertex, "geom", buff);
    }
    rapidxml::xml_node<> *indexes = xe->document()->allocate_node(rapidxml::node_element, "indexes");
    xe->append_node(indexes);

    std::string array;
    for (unsigned int i = 0; i < vb.SizeIndex(); ++i)
    {
        array += Math::IntToStr((int)vb.Index(i));
        array += ";";
    }
    Math::Write(indexes, "array", array.c_str());
}

void StretchTexture::SaveToXml(rapidxml::xml_node<> *xe)
{
	SolidGroundLine::SaveToXml(xe);
    char *copyName = xe->document()->allocate_string(_fileName.c_str());
    xe->append_attribute(xe->document()->allocate_attribute("texture", copyName));
    Math::Write(xe, "standart", _standart);
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

bool StretchTexture::Command(const std::string &cmd) {
	if (cmd == "Tab") 
	{
		_mesh.GetVB().Clear();
        _standart = (_standart + 1) % 5;
		DrawWithParam(&_temporaryMesh.GetVB());
		return true;
	}
	else if (cmd == "Mesh") 
	{
		if (_mesh.GetVB().Size())
		{
			_mesh.GetVB().Clear();
		}
		else
		{
			Render::EnableSlowUnion(true);
			DrawWithParam(&_mesh.GetVB());
			Render::EnableSlowUnion(false);
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
	if (SolidGroundLine::Command(cmd))
	{
		DrawWithParam(&_temporaryMesh.GetVB());
		return true;
	}
	return false;
}

void StretchTexture::MouseDown(const FPoint &mouse)
{
	if (_mesh.GetVB().Size() == 0)
	{
		SolidGroundLine::MouseDown(mouse);
		DrawWithParam(&_temporaryMesh.GetVB());
		return;
	}

	if (!_debugDraw) {
		return;
	}
		
	_mouseDown = true;
	_mousePos = mouse;
}

bool StretchTexture::MouseMove(const FPoint &mousePos)
{
	//if (_meshDots.size() == 0)
	{
		SolidGroundLine::MouseMove(mousePos);
		DrawWithParam(&_temporaryMesh.GetVB());
	//	return;
	}

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

void StretchTexture::MouseUp(const FPoint &mouse)
{
//	if (_meshDots.size() == 0)
	{
		SolidGroundLine::MouseUp(mouse);
		DrawWithParam(&_temporaryMesh.GetVB());
//		return;
	}
	if (_mouseDown) {
		_mouseDown = false;
	}
}

QVector<int> StretchTexture::SearchNearest(float x, float y) {
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

bool StretchTexture::Selection(const Rect& rect, bool full)
{
	_selectedDots.clear();
	for (unsigned int i = 0; i < _screenDots.size(); ++i) {
		if (rect.TestPoint(_screenDots[i].x, _screenDots[i].y)) {
			_selectedDots.push_back(i);
		}
	}
	return !full || _selectedDots.size() == _screenDots.size();
}

const char *StretchTexture::GetIconTexture() {
	return NULL;//_sprite;
}

bool StretchTexture::GeometryCheck(const FPoint &point) 
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

bool StretchTexture::PixelCheck(const FPoint &point)
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

void StretchTexture::EncapsulateAllDots(Rect &rect)
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
	//for (unsigned int i = 0; i < _screenDots.size(); ++i) {
	//	rect.Encapsulate(_screenDots[i].x, _screenDots[i].y);
	//}
}

void StretchTexture::GetAllLocalDotsRect(Rect &rect)
{
    if (_mesh.GetVB().Size())
    {
        for (unsigned int i = 0; i < _mesh.GetVB().Size(); ++i) {
            const FPoint &check = _mesh.GetVB().VertXY(i);
            rect.Encapsulate(check.x + _pos.x, check.y + _pos.y);
        }
        return;
    }
    SolidGroundLine::GetAllLocalDotsRect(rect);
}
