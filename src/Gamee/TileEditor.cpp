#include <QApplication>
#include <QBitmap>
#include <QMimeData>
#include <QMouseEvent>
#include <QDir>

#include "../Core/Core.h"
#include "../Core/ogl/render.h"
#include "../Core/Math.h"
#include "../Core/types.h"
#include "../Core/InputSystem.h"
#include "../Core/rapidxml/rapidxml_print.hpp"
#include "../Atlas/DataContainer.h"

#include "TileEditor.h"
#include "BeautyText.h"
#include "ColoredPolygon.h"
#include "SolidGroundLine.h"
#include "StretchTexture.h"
#include "ClickArea.h"
#include "GroundLine.h"
#include "AnimationArea.h"
#include "TiledLine.h"
#include "LinkToComplex.h"
#include "TileEditorInterface.h"
#include "Complex.h"
#include "GMessageBox.h"

#define DEC 0.01f
#define HALFBORDER 0.0025f

#define CURRENTBEAUTY _currents.beauty

TileEditor *TileEditor::instance = NULL;

bool IsLine(BeautyBase *one)
{
	return (one->Type() == "GroundLine" 
			|| one->Type() == "SolidGroundLine"
			|| one->Type() == "ClickArea");
}

int TileEditor::round(float a) {
	int b = static_cast<int>(a);
	return (a - b) >= 0.5f ? b + 1 : b;
} 

void TileEditor::LoadTemplates(const rapidxml::xml_document<> &doc)
{
	{
        rapidxml::xml_node<> *elem = doc.first_node();
        if (elem == NULL || elem->first_node("Definitions") == NULL)
        {
            return;
        }
        elem = elem->first_node("Definitions")->first_node();

		std::string typeName;
		std::string caption;
		while (elem != NULL) {
			BeautyBase *b = NULL;
            typeName = elem->name();
			caption = "";
			if (typeName == "ColoredPolygon") {
				b = new ColoredPolygon(elem);
				caption = "polygon";
			} else if (typeName == "GroundLine") {
				b = new GroundLine(elem);
				caption = "Way";
			} else if (typeName == "ClickArea") {
				b = new ClickArea(elem);
				caption = "clickarea";
			} else if (typeName == "Beauty") {
				Beauty *a;
				b = a = new Beauty(elem);
                std::string tex(a->GetTexturePath().substr(a->GetTexturePath().rfind("/") + 1));
                caption = "\"" + tex + "\"";
            } else if (typeName == "BeautyText") {
                BeautyText *a;
                b = a = new BeautyText(elem);
                std::string tex(a->GetTexturePath().substr(a->GetTexturePath().rfind("/") + 1));
                caption = "\"" + tex + "\"";
            } else if (typeName == "Animation") {
				AnimationArea *a;
				b = a = new AnimationArea(elem);
				if (a->GetAnimation() == NULL)
				{
					LOG("Can't load animation " + a->Name());
					delete a;
                    b = NULL;
				}
                else
                {
                    caption = "Animation \"" + a->Name() + "\"";
                }
			} else if (typeName == "GroundSpline") {
				b = new SolidGroundLine(elem);
				caption = "Spline Way";
			} else if (typeName == "LinkToComplex") {
				LinkToComplex * c = new LinkToComplex(elem);
				if (c->GetComplex() == NULL)
				{
					delete c;
					b = NULL;
				}
				else
				{
					b = c;
				}
				caption = "Complex";
			} else if (typeName == "SolidGroundLine") {
				b = new SolidGroundLine(elem);
				caption = "Spline Way";
			} else if (typeName == "StretchTexture") {
				b = new StretchTexture(elem);
				caption = "StretchTexture";
			} else if (typeName == "TiledLine") {
				b = new TiledLine(elem);
				caption = "TiledLine";
			} else {
                LOG(typeName);
				assert(false);
			}
			if (b)
			{
                caption = (b->UserString() != "" ? ("\"" + b->UserString() + "\" ") : "") + caption;
				_collection.push_back(b);
                TileEditorInterface::Instance()->GetCollectionControl()->AddItem(b, caption);
			}
            elem = elem->next_sibling();
		}
	}
}

bool TileEditor::SaveTemplates(const std::string &filename)
{
    rapidxml::xml_document<> doc;

    {
        rapidxml::xml_node<>* decl = doc.allocate_node(rapidxml::node_declaration);
        decl->append_attribute(doc.allocate_attribute("version", "1.0"));
        decl->append_attribute(doc.allocate_attribute("encoding", "utf-8"));
        decl->append_attribute(doc.allocate_attribute("standalone", "no"));
        doc.append_node(decl);
    }
    rapidxml::xml_node<> *mainRoot = doc.allocate_node(rapidxml::node_element, "fileroot");
    doc.append_node(mainRoot);
    rapidxml::xml_node<> *files = doc.allocate_node(rapidxml::node_element, "files");
    mainRoot->append_node(files);
    Core::SaveDataToXml(files);
    rapidxml::xml_node<> *root = doc.allocate_node(rapidxml::node_element, "Definitions");
    mainRoot->append_node(root);

	for (Collection::iterator i = _collection.begin(), e = _collection.end(); i != e; i++) 
	{
        char *name = doc.allocate_string((*i)->Type().c_str());
        rapidxml::xml_node<> *beauty = doc.allocate_node(rapidxml::node_element, name);
        root->append_node(beauty);
        (*i)->SaveToXml(beauty);
    }

    // Save to file
    std::ofstream file_stored(Core::Resource_MakePath(filename.c_str()));
    file_stored << doc;
    file_stored.close();
    doc.clear();
    return true;//doc.SaveFile();
}

TileEditor::TileEditor(QWidget *parent)
    : QGLWidget(parent)
    , _currentSelectionMode(false)
    , test_beauty(NULL)
    , _viewScale(1.f)
    , _screenOffset((SCREEN_WIDTH = 1024) / 2
                    , (SCREEN_HEIGHT = 768) / 2  )
    , _worldOffset(0.f, 0.f)
    , Messager("tile_editor")
    , _currentLevel(NOT_DEFINED_NAME)
    , _mouseDown(false)
    , SLIDER_SCALE(1.2f)
    , SLIDER_MIN(0.2f)
    , _pushCopyOnMouseUp(false)
    //, _meshBtnState(false, false)
    , _buttonsState(bs_nothing)
{
	_mouseMoveAction = mouse_none;
	_worldOffset.x = - 10 + _screenOffset.x;
	_worldOffset.y = - 10 + _screenOffset.y;
	assert(instance == NULL);
	instance = this;
    _clearColor = Math::ReadColor( TileEditorInterface::Instance()->settings.value("background", "0xFF7F7F7F").toString().toStdString() );
    _viewportWidth = TileEditorInterface::Instance()->settings.value("viewportwidth", 320).toInt();
    _viewportHeight = TileEditorInterface::Instance()->settings.value("viewportheight", 480).toInt();
	_currents.beautyUnderCursor = NULL;
//	ClearSelectionList();
    setAcceptDrops(true);
    setFocusPolicy(Qt::ClickFocus);
    makeCurrent();
    fbo = new QGLFramebufferObject(64, 64);

    GLTexture2D * h_base = Core::getTexture(Core::programPath + "gfx/buttons.png");
    _meshBtn       = new Beauty(h_base,   0,  0, 60, 32);
    _splineBtn     = new Beauty(h_base,  64,  0, 60, 32);
    _playBtn       = new Beauty(h_base, 128,  0, 60, 32);
    _scaleTexUpBtn = new Beauty(h_base, 192,  0, 60, 32);
    _editBtn       = new Beauty(h_base,   0, 32, 60, 32);
    _rotLeftBtn   = new Beauty(h_base,  64, 32, 60, 32);
    _rotRigthBtn    = new Beauty(h_base, 128, 32, 60, 32);
    _scaleTexDnBtn = new Beauty(h_base, 192, 32, 60, 32);
    _ungroupBtn    = new Beauty(h_base,   0, 64, 60, 32);
    _stretchModeBtn   = new Beauty(h_base,  64, 64, 60, 32);

    SetButtonsState(bs_nothing);
}

void TileEditor::LoadLevelSet()
{
    std::ifstream myfile(Core::Resource_MakePath(Core::storeXML.c_str()));
    _bufferForDoc.clear();
    _bufferForDoc = std::vector<char>(std::istreambuf_iterator<char>(myfile), std::istreambuf_iterator<char>());
    if (_bufferForDoc.size() == 0)
    {
        LOG("Error! Levels file not found! " + Core::Resource_MakePath(Core::storeXML.c_str()));
        return;
    }

    _bufferForDoc.push_back('\0');
    Parse(_doc, _bufferForDoc, Core::storeXML.c_str());
}

TileEditor::~TileEditor()
{
    delete fbo;

    Clear();

    delete _meshBtn;
    delete _splineBtn;
    delete _playBtn;
    delete _scaleTexUpBtn;
    delete _editBtn;
    delete _rotRigthBtn;
    delete _rotLeftBtn;
    delete _scaleTexDnBtn;
    delete _ungroupBtn;
    delete _stretchModeBtn;
}

void TileEditor::Clear()
{
	ClearLevel();
	ComplexManager::UnloadAll();
    AnimationManager::UnloadAll();
	Core::Unload();
	for (Collection::iterator i = _collection.begin(), e = _collection.end(); i != e; i++) {
		delete (*i);
	}
	_collection.clear();
	if (TileEditorInterface::Instance())
	{
        TileEditorInterface::Instance()->GetCollectionControl()->RemoveChildren();
	}
}

void TileEditor::wheelEvent(QWheelEvent *event)
{
    _lastMousePos.x = event->x();
    _lastMousePos.y = event->y();
#ifdef WIN32
    OnMouseWheel(event->delta() / 2000.f);
#else
    OnMouseWheel(event->delta() / 200.f);
#endif
}

void TileEditor::SwitchToCustomEditMode()
{
    if (GetButtonsState() != bs_standart && GetButtonsState() != bs_nothing)
    {
        if (Selection().size() == 1)
        {
            SetButtonsState(bs_standart);
        }
        else
        {
            SetButtonsState(bs_nothing);
        }
        return;
    }

    if (Selection().size() != 1)
    {
        return;
    }
    if (Selection()[0]->Type() == "LinkToComplex")
    {
        SetButtonsState(bs_complex);
    }
    else if (Selection()[0]->Type() == "Animation")
    {
        SetButtonsState(bs_animation);
    }
    else if (Selection()[0]->Type() == "ColoredPolygon")
    {
        SetButtonsState(bs_polygon_mesh);
    }
    else if (Selection()[0]->Type() == "Beauty")
    {
        SetButtonsState(bs_beauty);
    }
    else if (Selection()[0]->Type() == "BeautyText")
    {
        SetButtonsState(bs_beauty_text);
    }
    else if (Selection()[0]->Type() == "ClickArea"
             || Selection()[0]->Type() == "GroundLine"
             || Selection()[0]->Type() == "SolidGroundLine")
    {
        SetButtonsState(bs_dots_edit);
    }
    else if (Selection()[0]->Type() == "TiledLine")
    {
        if (static_cast<TiledLine*>(Selection()[0])->Mesh())
        {
            SetButtonsState(bs_tiled_mesh);
        }
        else
        {
            SetButtonsState(bs_tiled_line);
        }
    }
    else if (Selection()[0]->Type() == "StretchTexture")
    {
        if (static_cast<StretchTexture*>(Selection()[0])->Mesh())
        {
            SetButtonsState(bs_stretch_mesh);
        }
        else
        {
            SetButtonsState(bs_stretch_spline);
        }
    }
}

void TileEditor::BackspaceSwitch()
{
    if (!_splineBtn->Visible() && !_meshBtn->Visible())
    {
        return;
    }
    for (unsigned int i = 0; i < _currents.beauty.size(); ++i) {
        _currents.beauty[i]->Command("Mesh");
    }
    PushCopyToRedoUndoManager();
    if (_splineBtn->Visible())
    {
        if (GetButtonsState() == bs_stretch_mesh)
        {
            SetButtonsState(bs_stretch_spline);
        }
        else if (GetButtonsState() == bs_tiled_mesh)
        {
            SetButtonsState(bs_tiled_line);
        }
    }
    else if (_meshBtn->Visible())
    {
        if (GetButtonsState() == bs_stretch_spline)
        {
            SetButtonsState(bs_stretch_mesh);
        }
        else if (GetButtonsState() == bs_tiled_line)
        {
            SetButtonsState(bs_tiled_mesh);
        }
    }
}

void TileEditor::OnMouseDown(const FPoint &mousePos)
{	
    if (_editBtn->Visible() && _editBtn->PixelCheck(mousePos))
    {
        SwitchToCustomEditMode();
        return;
    }
    if (_playBtn->Visible() && _playBtn->PixelCheck(mousePos))
    {
        TileEditorInterface::Instance()->TestAnimation();
        return;
    }
    if (_ungroupBtn->Visible() && _ungroupBtn->PixelCheck(mousePos))
    {
        UngroupComplex();
        return;
    }
    if (_splineBtn->Visible() && _splineBtn->PixelCheck(mousePos))
    {
        BackspaceSwitch();
        return;
    }
    if (_meshBtn->Visible() && _meshBtn->PixelCheck(mousePos))
    {
        BackspaceSwitch();
        return;
    }
    if (_stretchModeBtn->Visible() && _stretchModeBtn->PixelCheck(mousePos))
    {
        for (unsigned int i = 0; i < _currents.beauty.size(); ++i) {
            _currents.beauty[i]->Command("Tab");
        }
        PushCopyToRedoUndoManager();
        return;
    }
    if (_rotRigthBtn->Visible() && _rotRigthBtn->PixelCheck(mousePos))
    {
        for (unsigned int i = 0; i < _currents.beauty.size(); ++i) {
            _currents.beauty[i]->Command("Rigth");
        }
        PushCopyToRedoUndoManager();
        return;
    }
    if (_rotLeftBtn->Visible() && _rotLeftBtn->PixelCheck(mousePos))
    {
        for (unsigned int i = 0; i < _currents.beauty.size(); ++i) {
            _currents.beauty[i]->Command("Left");
        }
        PushCopyToRedoUndoManager();
        return;
    }
    if (_scaleTexUpBtn->Visible() && _scaleTexUpBtn->PixelCheck(mousePos))
    {
        for (unsigned int i = 0; i < _currents.beauty.size(); ++i) {
            _currents.beauty[i]->Command("Plus");
        }
        PushCopyToRedoUndoManager();
        return;
    }
    if (_scaleTexDnBtn->Visible() && _scaleTexDnBtn->PixelCheck(mousePos))
    {
        for (unsigned int i = 0; i < _currents.beauty.size(); ++i) {
            _currents.beauty[i]->Command("Minus");
        }
        PushCopyToRedoUndoManager();
        return;
    }
    _mouseDown = true;
	_lastMousePos = _mouseDownPos = mousePos;
	_mouseMovingMode = 0.f;
	FPoint worldClickPos = ScreenToWorld(mousePos);

	FPoint fp(mousePos.x, mousePos.y);

    if ((QApplication::keyboardModifiers() & Qt::Key_Space) != 0)
	{
		_mouseMoveAction = mouse_dragging_world;
		return;
	}

	if (TileEditorInterface::Instance()->CreateDotMode() && _currents.beauty.size() == 1) {
		char buff[100];
		sprintf(buff, "create dot at %f %f", fp.x, fp.y);
		if (_currents.beauty[0]->Command(buff)) {
			_pushCopyOnMouseUp = true;
			_mouseMoveAction = mouse_moving_beauty;
		}
		else
		{
			_selectionTool.OnMouseDown(FPoint(fp.x, fp.y));
			_mouseMoveAction = mouse_select_region;
		}
		return;
	}

	{//          
		BeautyBase *clicked = ControlsAtPoint(mousePos);
		if (clicked)
		{
			clicked->MouseDown(mousePos);
			_mouseMoveAction = mouse_moving_beauty_control;
			return;
		}
	}
	{//    
		BeautyBase *clicked = BeautyAtPoint(worldClickPos);
        if ((QApplication::keyboardModifiers() & Qt::ControlModifier) != 0) {
			_mouseMoveAction = mouse_select_region;
			_selectionTool.OnMouseDown(FPoint(fp.x, fp.y));
			if (!clicked)
			{
				return;
			}
			//        -   
			for (unsigned int i = 0; i < _currents.beauty.size(); ++i) {
                if (_currents.beauty[i] == clicked)
                {
                    // remove "cliked" from list
					_currents.beauty.erase(_currents.beauty.begin() + i);
					_pushCopyOnMouseUp = true;
					ReadSelectedInGameType();
					return;
				}
			}
			PushBeautyToSelectionList(clicked);
			_pushCopyOnMouseUp = true;
		} else {
			if (_currents.beauty.size() == 1 && _currents.beauty.back() == clicked)
			{
				_currents.beauty[0]->MouseDown(fp);
				_mouseMoveAction = mouse_moving_beauty;
			}
			else if (clicked)
			{
				//        -   
				for (unsigned int i = 0; i < _currents.beauty.size(); ++i) {
					if (_currents.beauty[i] == clicked) {
						_mouseMoveAction = mouse_moving_beauty;
						return;
					}
				}
                ClearSelectionList();
                PushBeautyToSelectionList(clicked);
				_currents.beauty[0]->MouseDown(fp);
				_pushCopyOnMouseUp = true;
				_mouseMoveAction = mouse_moving_beauty;
			}
			else
			{
				_selectionTool.OnMouseDown(FPoint(fp.x, fp.y));
				_mouseMoveAction = mouse_select_region;
			}
		}
	}
}

void TileEditor::OnMouseUp()
{
	_mouseMovingMode = 0.f;
	_mouseDown = false;
	if (_currents.beauty.size() == 1) {
		_currents.beauty[0]->MouseUp(_lastMousePos);
	}
	if (_pushCopyOnMouseUp) {
        PushCopyToRedoUndoManager();
		_pushCopyOnMouseUp = false;
	}
	_mouseMoveAction = mouse_none;
}

void TileEditor::OnMouseMove(const FPoint &mousePos)
{
	FPoint newMmouseWorld = ScreenToWorld(mousePos);

	if (_mouseMoveAction == mouse_dragging_world) {
		_worldOffset -= (mousePos - _lastMousePos) / _viewScale;
	} else if (_mouseMoveAction == mouse_moving_beauty/* || _mouseMoveAction == mouse_moving_group*/) {
		if ((mousePos - _mouseDownPos).Length() > 10 || _mouseMovingMode >= 0.2f) {
			_mouseMovingMode = 0.2f;

			if (!TileEditorInterface::Instance()->CreateDotMode()) {
				for (unsigned int i = 0; i < _currents.beauty.size(); ++i) {
					_currents.beauty[i]->ShiftTo(newMmouseWorld.x - _mouseWorld.x, newMmouseWorld.y - _mouseWorld.y);
				}
                TileEditorInterface::Instance()->UpdateProperties();
			}
			else
			{
				for (unsigned int i = 0; i < _currents.beauty.size(); ++i) {
					_currents.beauty[i]->MouseMove(mousePos);
				}
			}
			if (_currents.beauty.size()) {
				_pushCopyOnMouseUp = true;
			}
		}
	}
	else if (_mouseMoveAction == mouse_moving_beauty_control)
	{
		for (unsigned int i = 0; i < _currents.beauty.size(); ++i) {
            _pushCopyOnMouseUp |= _currents.beauty[i]->MouseMove(mousePos);
		}
        TileEditorInterface::Instance()->UpdateProperties();
	}
	if (!_mouseDown && !TileEditorInterface::Instance()->CreateDotMode()) {//    
		_currents.beautyUnderCursor = BeautyAtPoint(newMmouseWorld);
	}

	_lastMousePos = mousePos;
	_mouseWorld = newMmouseWorld;
}

BeautyBase *TileEditor::BeautyAtPoint(const FPoint &worldPos)
{
	BeautyBase *resultBeauty = NULL;
	for (int i = (int)(_level.beauties.size()) - 1; i >= 0 && resultBeauty == NULL; --i) {
		if (IsLine(_level.beauties[i]) && _level.beauties[i]->PixelCheck(worldPos)) {
			resultBeauty = _level.beauties[i];
		}
	}
	if (!resultBeauty) {
		for (int i = (int)(_level.beauties.size()) - 1; i >= 0 && resultBeauty == NULL; --i) 
		{
			if (_level.beauties[i]->PixelCheck(worldPos)) 
			{
				resultBeauty = _level.beauties[i];
			}
		}
	}
	return resultBeauty;
}

BeautyBase *TileEditor::ControlsAtPoint(const FPoint &screenPos)
{
	if (_currents.beauty.size() == 1)
	{
		if (_currents.beauty[0]->ControlsCheck(screenPos)) 
		{
			return _currents.beauty[0];
		}
	}
	return NULL;
}

bool TileEditor::OnMouseWheel(float value)
{
	FPoint fp = ScreenToWorld(_lastMousePos);
    _screenOffset = _lastMousePos;
	_worldOffset = fp;
	float MIN = 1 / 32.f;
	float MAX = 4.f;
    _viewScale += value * _viewScale;
//	if (direction > 0 && _viewScale < MAX) {
//		_viewScale *= 1.09f;
//	} else if (direction < 0 && _viewScale > MIN) {
//		_viewScale *= 0.9f;
//	}
    _viewScale = std::min(MAX, std::max(MIN, _viewScale));
    _worldOffset = ScreenToWorld(FPoint(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2));
	_screenOffset = FPoint(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
	return true;
}	

bool TileEditor::IsMouseOver(const FPoint &mousePos) {
	return true;
}

void TileEditor::paintGL()
{
    if (test_beauty)
    {
        // рисую в fbo, а потом читаю его пиксели, иначе вылезают артефакты
        makeCurrent();
        fbo->bind();
        glClearColor(0.0f, 0.0f, 0.0f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable ( GL_BLEND );
        //**************************
        Rect rect;
        Render::PushMatrix();
        Render::SetMatrixUnit();
        Render::MatrixMove(0, Height() - 64);
        Render::DrawBar(0, 0, 64, 64, 0xFFAFAFAF);
        //base->EncapsulateAllDots(rect);
        test_beauty->GetAllLocalDotsRect(rect);
        float scale = std::min(1.f, std::min(64.f / (rect.x2 - rect.x1), 64.f / (rect.y2 - rect.y1)));
        //float maxShift = std::max(rect.x1, rect.y1) * scale;
        Render::MatrixMove(32, 32);
        Render::MatrixScale(scale, -scale);
        Render::SetFiltering(true);
        test_beauty->Draw();
        Render::SetFiltering(TileEditorInterface::Instance()->FilteringTexture());
        Render::PopMatrix();
        glDisable(GL_BLEND);
        glFlush();
        fbo->release();

        glEnable(GL_TEXTURE_2D);
        glDisable(GL_BLEND);
        glBindTexture(GL_TEXTURE_2D, fbo->texture());
        glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, image_buffer);
        glBindTexture(GL_TEXTURE_2D, 0);
        return;
    }
    Draw();
}

void TileEditor::Draw()
{

    glClearColor(0.50f, 0.50f, 0.50f, 1.0f);
    glBlendFunc(GL_SRC_ALPHA ,GL_ONE_MINUS_SRC_ALPHA);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    glEnable ( GL_BLEND );

    Render::SetFiltering(TileEditorInterface::Instance()->FilteringTexture());
	{ //  
        Render::BeginCachingLine();
		Render::PushMatrix();
		if (TileEditorInterface::Instance()->AlphaBlend())
		{
			Render::DrawBar(2, 2, Width()- 4, Height() - 4, 0xFF000000);
		}
		else
		{
			Render::DrawBar(2, 2, Width()- 4, Height() - 4, _clearColor);
		}
		Render::MatrixMove(_screenOffset.x, _screenOffset.y);
		Render::MatrixScale(_viewScale, _viewScale);
		Render::MatrixMove(-_worldOffset.x, -_worldOffset.y);

		if (TileEditorInterface::Instance()->AlphaBlend())
		{
			Render::SetBlendMode(1);
		}
		for (unsigned int i = 0; i < _level.beauties.size(); ++i) {
			_level.beauties[i]->Draw();
		}
		Render::SetBlendMode(0);
		if (_currents.beautyUnderCursor) {
			Render::SetAlpha(0x5F);
			_currents.beautyUnderCursor->DebugDraw(true);
			Render::SetAlpha(0xFF);
		}
		if (_currents.beauty.size()) {
			Render::SetAlpha(0xAF);
			for (unsigned int i = 0; i < _currents.beauty.size(); ++i) {
				_currents.beauty[i]->DebugDraw(true);
			}
			Render::SetAlpha(0xFF);
		}
		if (TileEditorInterface::Instance()->NetVisible()) {// 
			Matrix m;
			m.MakeRevers(Render::GetCurrentMatrix());
			float startX = 0.f;
			float startY = 0.f;
			m.Mul(startX, startY);
			float endX = 2000.f;
			float endY = 1000.f;
			m.Mul(endX, endY);
			float STEP = 64.f;
			float x = static_cast<int>(startX / STEP) * STEP;
			while (x < endX) {
				Render::Line(x, startY, x, endY, 0x0FFFFFFF);
				x += STEP;
			}
			float y = static_cast<int>(startY / STEP) * STEP;
			while (y < endY) {
				Render::Line(startX, y, endX, y, 0x0FFFFFFF);
				y += STEP;
			}
		} 
        Render::EndCachingLine();
		_walkThrough.Render();
		if (TileEditorInterface::Instance()->ViewportVisible()) {
			Render::Line(0, 0, _viewportWidth, 0, 0x7FFF0000);
			Render::Line(_viewportWidth, 0, _viewportWidth, _viewportHeight, 0x7FFF0000);
			Render::Line(_viewportWidth, _viewportHeight, 0, _viewportHeight, 0x7FFF0000);
			Render::Line(0, _viewportHeight, 0, 0, 0x7FFF0000);
		}
		Render::DrawCachingLine();
		Render::PopMatrix();
	}
	// 
	char buff[10];
	Math::FloatToChar(_viewScale, buff);
	Render::PrintString(940, 0, "", buff);
	_selectionTool.Draw();
	Render::SetFiltering(TileEditorInterface::Instance()->FilteringTexture());
    {
        float x = _widgetWidth / 2 - (64 * _commandButtons.size()) / 2;
        for (unsigned int i = 0; i < _commandButtons.size(); ++i)
        {
            if (i == 0 && GetButtonsState() != bs_standart)
            {
                _commandButtons[i]->SetColor(0xFF7F7F7F);
            }
            else
            {
                _commandButtons[i]->SetColor(0xFFFFFFFF);
            }
            _commandButtons[i]->SetPosition(x, 30);
            _commandButtons[i]->Draw();
            x += 64;
        }
    }
//    if (GetEditBtnState().visible)
//    {
//        if (GetEditBtnState().IsPressed())
//        {
//            _editBtn->SetColor(0xFF7F7F7F);
//        }
//        else
//        {
//            _editBtn->SetColor(0xFFFFFFFF);
//        }
//        if (_playBtn->Visible())
//        {
//            float x = _widgetWidth / 2 - 64 / 2;
//            _editBtn->SetPosition(x, 30);
//            _editBtn->Draw();
//            x += 64;
//            _playBtn->SetPosition(x, 30);
//            _playBtn->Draw();
//        }
//        else if (_ungroupBtn->Visible())
//        {
//            float x = _widgetWidth / 2 - 64 / 2;
//            _editBtn->SetPosition(x, 30);
//            _editBtn->Draw();
//            x += 64;
//            _ungroupBtn->SetPosition(x, 30);
//            _ungroupBtn->Draw();
//        }
//        else if (_ungroupBtn->Visible())
//        {
//            float x = _widgetWidth / 2 - 64 / 2;
//            _editBtn->SetPosition(x, 30);
//            _editBtn->Draw();
//            x += 64;
//            _ungroupBtn->SetPosition(x, 30);
//            _ungroupBtn->Draw();
//        }
//        else if (GetEditBtnState().IsPressed()
//                 && Selection().size() == 1
//                 && Selection()[0]->Type() == "StretchTexture")
//        {
//            if (static_cast<StretchTexture*>(Selection()[0])->Mesh())
//            {
//                float x = _widgetWidth / 2 - 64 / 2;
//                _editBtn->SetPosition(x, 30);
//                _editBtn->Draw();
//                x += 64;
//                _splineBtn->SetPosition(x, 30);
//                _splineBtn->Draw();
//            }
//            else
//            {
//                float x = _widgetWidth / 2 - (64 * 2) / 2;
//                _editBtn->SetPosition(x, 30);
//                _editBtn->Draw();
//                x += 64;
//                _meshBtn->SetPosition(x, 30);
//                _meshBtn->Draw();
//                x += 64;
//                _stretchModeBtn->SetPosition(x, 30);
//                _stretchModeBtn->Draw();
//            }
//        }
//        else
//        {
//            float x = _widgetWidth / 2;
//            _editBtn->SetPosition(x, 30);
//            _editBtn->Draw();
//        }
//    }
}

FPoint TileEditor::ScreenToWorld(const FPoint &screenPos) {
    return (screenPos - _screenOffset) / _viewScale + _worldOffset;
}

FPoint TileEditor::WorldToScreen(const FPoint &worldPos) {
	return (worldPos - _worldOffset) * _viewScale + _screenOffset;
}

void TileEditor::DeletePressed()
{
    if (TileEditorInterface::Instance()->CreateDotMode()) {
        if (_currents.beauty.size() == 1) {
            if (_currents.beauty[0]->Command("delete dot")) {
                PushCopyToRedoUndoManager();
            }
        }
    } else {
        RemoveSelection();
    }
}

void  TileEditor::keyPressEvent ( QKeyEvent * event )
{
    int iKey = event->key();
    if ((QApplication::keyboardModifiers() & Qt::ControlModifier) != 0 && iKey == Qt::Key_Return)
    {
        //TileEditorInterface::Instance()->StartGameFunction();
        return;
    }
    else if (iKey == Qt::Key_Backspace)
    {
        BackspaceSwitch();
        return;
    }
    else if (iKey == Qt::Key_Home)
    {
        _viewScale = 1.f;
        if ((QApplication::keyboardModifiers() & Qt::ControlModifier) != 0) {
            _worldOffset.x = - 10 + _screenOffset.x;
            _worldOffset.y = - 10 + _screenOffset.y;
        }
        return;
    }
    else if (Selection().size() == 1
             && (QApplication::keyboardModifiers() & Qt::ControlModifier) != 0
             && iKey == Qt::Key_D)
    {
        SwitchToCustomEditMode();
    }
    else if (_currents.beauty.size() > 0) {
        if (iKey == Qt::Key_Delete) {
            return;
        } else if (iKey == Qt::Key_R) {
            for (unsigned int i = 0; i < _currents.beauty.size(); ++i) {
                _currents.beauty[i]->Command("Tab");
            }
            PushCopyToRedoUndoManager();
            return;
        } else if (iKey == Qt::Key_Plus) {
            for (unsigned int i = 0; i < _currents.beauty.size(); ++i) {
                _currents.beauty[i]->Command("Plus");
            }
            PushCopyToRedoUndoManager();
            return;
        } else if (iKey == Qt::Key_Minus) {
            for (unsigned int i = 0; i < _currents.beauty.size(); ++i) {
                _currents.beauty[i]->Command("Minus");
            }
            PushCopyToRedoUndoManager();
            return;
        } else if (iKey == Qt::Key_Backspace) {
            for (unsigned int i = 0; i < _currents.beauty.size(); ++i) {
                _currents.beauty[i]->Command("Mesh");
            }
            PushCopyToRedoUndoManager();
            return;
        } else if (iKey == Qt::Key_Left) {
            float shift = 1 / _viewScale;
            if ((QApplication::keyboardModifiers() & Qt::ShiftModifier) != 0)
            {
                shift = 10;
            }
            if ((QApplication::keyboardModifiers() & Qt::ControlModifier) != 0 || (QApplication::keyboardModifiers() & Qt::ShiftModifier) != 0)
            {
                FPoint pos;
                for (unsigned int i = 0; i < _currents.beauty.size(); ++i) {
                    pos = _currents.beauty[i]->GetPos();
                    pos.x = Math::round(pos.x / shift) * shift;
                    _currents.beauty[i]->SetPosition(pos.x, pos.y);
                }
            }
            for (unsigned int i = 0; i < _currents.beauty.size(); ++i) {
                _currents.beauty[i]->ShiftTo(-shift, 0);
            }
            if (_currents.beauty.size() == 1)
            {
                TileEditorInterface::Instance()->UpdateProperties();
            }
            PushCopyToRedoUndoManager();
            return;
        } else if (iKey == Qt::Key_Right) {
            float shift = 1 / _viewScale;
            if ((QApplication::keyboardModifiers() & Qt::ShiftModifier) != 0)
            {
                shift = 10;
            }
            if ((QApplication::keyboardModifiers() & Qt::ControlModifier) != 0 || (QApplication::keyboardModifiers() & Qt::ShiftModifier) != 0)
            {
                FPoint pos;
                for (unsigned int i = 0; i < _currents.beauty.size(); ++i) {
                    pos = _currents.beauty[i]->GetPos();
                    pos.x = Math::round(pos.x / shift) * shift;
                    _currents.beauty[i]->SetPosition(pos.x, pos.y);
                }
            }
            for (unsigned int i = 0; i < _currents.beauty.size(); ++i) {
                _currents.beauty[i]->ShiftTo(shift, 0);
            }
            if (_currents.beauty.size() == 1)
            {
                TileEditorInterface::Instance()->UpdateProperties();
            }
            PushCopyToRedoUndoManager();
            return;
        } else if (iKey == Qt::Key_Up) {
            float shift = 1 / _viewScale;
            if ((QApplication::keyboardModifiers() & Qt::ShiftModifier) != 0)
            {
                shift = 10;
            }
            if ((QApplication::keyboardModifiers() & Qt::ControlModifier) != 0 || (QApplication::keyboardModifiers() & Qt::ShiftModifier) != 0)
            {
                FPoint pos;
                for (unsigned int i = 0; i < _currents.beauty.size(); ++i) {
                    pos = _currents.beauty[i]->GetPos();
                    pos.y = Math::round(pos.y / shift) * shift;
                    _currents.beauty[i]->SetPosition(pos.x, pos.y);
                }
            }
            for (unsigned int i = 0; i < _currents.beauty.size(); ++i) {
                _currents.beauty[i]->ShiftTo(0, -shift);
            }
            if (_currents.beauty.size() == 1)
            {
                TileEditorInterface::Instance()->UpdateProperties();
            }
            PushCopyToRedoUndoManager();
            return;
        } else if (iKey == Qt::Key_Down) {
            float shift = 1 / _viewScale;
            if ((QApplication::keyboardModifiers() & Qt::ShiftModifier) != 0)
            {
                shift = 10;
            }
            if ((QApplication::keyboardModifiers() & Qt::ControlModifier) != 0 || (QApplication::keyboardModifiers() & Qt::ShiftModifier) != 0)
            {
                FPoint pos;
                for (unsigned int i = 0; i < _currents.beauty.size(); ++i) {
                    pos = _currents.beauty[i]->GetPos();
                    pos.y = Math::round(pos.y / shift) * shift;
                    _currents.beauty[i]->SetPosition(pos.x, pos.y);
                }
            }
            for (unsigned int i = 0; i < _currents.beauty.size(); ++i) {
                _currents.beauty[i]->ShiftTo(0, shift);
            }
            if (_currents.beauty.size() == 1)
            {
                TileEditorInterface::Instance()->UpdateProperties();
            }
            PushCopyToRedoUndoManager();
            return;
        } else if (iKey == Qt::Key_PageUp && _currents.beauty.size() == 1) {
            if ((QApplication::keyboardModifiers() & Qt::ControlModifier) != 0) {
                for (unsigned int i = 0; i < _level.beauties.size(); ++i) {
                    if (_level.beauties[i] == _currents.beauty[0] && (i + 1) < _level.beauties.size()) {
                        for (unsigned int k = i; (k + 1) < _level.beauties.size(); ++k) {
                            BeautyBase *tmp = _level.beauties[k];
                            _level.beauties[k] = _level.beauties[k + 1];
                            _level.beauties[k + 1] = tmp;
                        }

                        break;
                    }
                }
            } else {
                for (unsigned int i = 0; i < _level.beauties.size(); ++i) {
                    if (_level.beauties[i] == _currents.beauty[0] && (i + 1) < _level.beauties.size()) {
                        BeautyBase *tmp = _level.beauties[i];
                        _level.beauties[i] = _level.beauties[i + 1];
                        _level.beauties[i + 1] = tmp;
                        break;
                    }
                }
            }
            PushCopyToRedoUndoManager();
            return;
        } else if (iKey == Qt::Key_PageDown && _currents.beauty.size() == 1) {
            if ((QApplication::keyboardModifiers() & Qt::ControlModifier) != 0) {
                for (unsigned int i = 0; i < _level.beauties.size(); ++i) {
                    if (_level.beauties[i] == _currents.beauty[0] && i > 0) {
                        for (unsigned int k = i; k > 0; --k) {
                            BeautyBase *tmp = _level.beauties[k];
                            _level.beauties[k] = _level.beauties[k - 1];
                            _level.beauties[k - 1] = tmp;
                        }
                        break;
                    }
                }
            } else {
                for (unsigned int i = 0; i < _level.beauties.size(); ++i) {
                    if (_level.beauties[i] == _currents.beauty[0] && i > 0) {
                        BeautyBase *tmp = _level.beauties[i];
                        _level.beauties[i] = _level.beauties[i - 1];
                        _level.beauties[i - 1] = tmp;
                        break;
                    }
                }
            }
            PushCopyToRedoUndoManager();
        }
    }
}

void TileEditor::Update(float deltaTime) {	
	if (_mouseDown) {
		_mouseMovingMode += deltaTime;
	}
	for (unsigned int i = 0; i < _level.beauties.size(); ++i) {
		_level.beauties[i]->Update(deltaTime);
	}
    while (_needIcons.begin() != _needIcons.end())
    {
        QIcon *icon = NULL;
        DrawBeautyInBuffer(_needIcons.front().first, icon);
        if (icon)
        {
            _needIcons.front().second->setIcon(0, *icon);
            delete icon;
        }
        _needIcons.erase(_needIcons.begin());
    }
}

void TileEditor::NewLevelYes() 
{
    _currentLevel = NOT_DEFINED_NAME;
	TileEditorInterface::Instance()->UpdateTitle();
	_screenOffset = FPoint(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
	_worldOffset = FPoint(0.f, 0.f);
	_viewScale = 1.f;
	ClearLevel();
	TileEditorInterface::Instance()->Changes();
}

void TileEditor::Save()
{
    if (GetCurrentLevel() == NOT_DEFINED_NAME)
    {
        SaveAsNew();
    }
    else
    {
        PreSaveLevel(GetCurrentLevel());
    }
    if (GetCurrentLevel() != NOT_DEFINED_NAME && ComplexManager::isComplex(GetCurrentLevel()))
    {
        // объект является комплексом
        // обновляем ему иконку в списке
        *ComplexManager::getComplex(GetCurrentLevel()) = _level;

        LinkToComplex *b = NULL;
        for (Collection::iterator i = _collection.begin(), e = _collection.end(); i != e; i++)
        {
            if ((*i)->Type() == "LinkToComplex" && static_cast<LinkToComplex *>(*i)->Name() == GetCurrentLevel())
            {
                b = static_cast<LinkToComplex *>(*i);
                b->ReInit();

                for (int i = 0; i < TileEditorInterface::Instance()->GetCollectionControl()->topLevelItemCount(); ++i)
                {
                    QTreeWidgetItem *item = TileEditorInterface::Instance()->GetCollectionControl()->topLevelItem (i);
                    if (item && item->data(0, Qt::UserRole).value<CustomDataClass>().beauty == b)
                    {
                        TileEditor::Instance()->NeedBeautyIcon(b, item);
                    }
                }
                break;
            }
        }
    }
}

void TileEditor::SaveAsNew()
{
    TileEditorInterface::Instance()->EnterNameDialog(mode_save_as, "Enter name of level", TileEditor::Instance()->GetCurrentLevel());
}

void TileEditor::OverwriteYes() 
{
	SaveLevel(_saveLevelName, _level);
	_saveLevelName = _preSaveLevelName;
	_saveLevelXml = _preSaveLevelXml;
}

void TileEditor::SaveAsComplex()
{
    if (_currentLevel == NOT_DEFINED_NAME)
    {
        Save();
    }
    if (_currentLevel == NOT_DEFINED_NAME)
    {
        return;
    }
    if (ComplexManager::isComplex(_currentLevel))
    {
        return;
    }
    CreateLinkToComplex();
}

void TileEditor::PreSaveLevel(const std::string &msg)
{
    rapidxml::xml_node<> *xe = _doc.first_node()->first_node();
	std::string s;
    while (xe != NULL && xe->first_attribute("id")->value() != msg) {
        s = xe->first_attribute("id")->value();
        xe = xe->next_sibling();
	}
	_saveLevelXml = xe;
	if (xe == NULL) {
        rapidxml::xml_node<> *level = _doc.allocate_node(rapidxml::node_element, "level");
        char *copy = _doc.allocate_string(msg.c_str());
        level->append_attribute(_doc.allocate_attribute("id", copy));
        _doc.first_node()->append_node(level);
		xe = level; 
		_saveLevelXml = xe;
		SaveLevel(msg.c_str(), _level);
		return;
	} else if (_currentLevel != msg) {
		_preSaveLevelName = msg;
		_preSaveLevelXml = xe;
//		GMessageBoxYesNoShow(std::string("Are you sure?\nDo you want overwrite " + msg + "?").c_str())->onPress.Add(Instance(), &TileEditor::OverwriteYes);
		return;
	}
	SaveLevel(msg, _level);
}

void TileEditor::OnMessage(const std::string &message) {
	std::string msg;
	if (CanCut(message, "LoadLevel", msg)) {
		LoadLevel(msg);
		return;
	}
	if (CanCut(message, "PreSaveLevel", msg)) {
		PreSaveLevel(msg);
		return;
	}
	if (message == "test") {
		for (unsigned int i = 0; i < _currents.beauty.size(); ++i) {
			_currents.beauty[i]->Command("play");
		}
	} else if (message == "rewind") {
		for (unsigned int i = 0; i < _currents.beauty.size(); ++i) {
			_currents.beauty[i]->Command("rewind");
		}
//	} else if (message == "load templates") {
//		LoadTemplates(Core::projectXML);
	} else if (message == "new") {
        if (GMessageBoxYesNoShow("Are you sure?\nDelete all objects?") == QMessageBox::Yes)
        {
            NewLevelYes();
        }
	} else if (message == "open") {
        if (_doc.first_node() == NULL)
        {
            return;
        }
       TileEditorInterface::Instance()->OnMessage("prefix LoadLevel");
        rapidxml::xml_node<> *xe = _doc.first_node()->first_node();
		std::string s;
		while (xe) {			
            s = xe->first_attribute("id")->value();
            TileEditorInterface::Instance()->OnMessage("add " + s);
            xe = xe->next_sibling();
		}
		if (_currentLevel != "") {
            TileEditorInterface::Instance()->OnMessage("select " + _currentLevel);
		}
	} else if (message == "save") {
		Messager::SendMsg("Interface", "prefix PreSaveLevel");
        rapidxml::xml_node<> *xe = _doc.first_node()->first_node();
		std::string s;
		while (xe) {
            s = xe->first_attribute("id")->value();
			Messager::SendMsg("Interface", "add " + s);
            xe = xe->next_sibling();
		}
		if (_currentLevel != "") {
			Messager::SendMsg("Interface", "select " + _currentLevel);
		}
	} else {
		assert(message == "ok");
	}
}

void TileEditor::SaveLevelToXml(rapidxml::xml_node<> *xe, LevelSet &level)
{
    rapidxml::xml_node<> *beautyList = xe->document()->allocate_node(rapidxml::node_element, "Beauties");
    xe->append_node(beautyList);
    for (BeautyList::iterator i = level.beauties.begin(), e = level.beauties.end(); i != e; ++i)
	{
        char *name = xe->document()->allocate_string((*i)->Type().c_str());
        rapidxml::xml_node<> *beauty = xe->document()->allocate_node(rapidxml::node_element, name);
        beautyList->append_node(beauty);
        (*i)->SaveToXml(beauty);
	}
}

void TileEditor::SaveLevel(const std::string &levelName, LevelSet &level)
{
    // level previos version removing
    rapidxml::xml_node<> *elem = _saveLevelXml->first_node();
    while (elem != NULL) {
        rapidxml::xml_node<> *remove = elem;
        elem = elem->next_sibling();
        _saveLevelXml->remove_node(remove);
    }

    // level saving
    if (TileEditorInterface::Instance()->SaveCopyToFile())
    {
        SaveLevelToFile(levelName, level);
    }
    else
    {
        SaveLevelToXml(_saveLevelXml, level);
    }

    rapidxml::xml_node<> *word = _saveLevelXml->document()->allocate_node(rapidxml::node_element, "word");
    _saveLevelXml->append_node(word);
    Math::Write(word, "x", _worldOffset.x);
    Math::Write(word, "y", _worldOffset.y);
    Math::Write(word, "scale", _viewScale);

    std::ofstream file_stored(Core::Resource_MakePath(Core::storeXML.c_str()));
    file_stored << _doc;
    file_stored.close();

    TileEditorInterface::Instance()->Saved();

    _currentLevel = levelName;
    TileEditorInterface::Instance()->UpdateTitle();

}

void TileEditor::SaveLevelToFile(const std::string &levelName, LevelSet &level)
{
    rapidxml::xml_document<> doc;

    rapidxml::xml_node<>* decl = doc.allocate_node(rapidxml::node_declaration);
    decl->append_attribute(doc.allocate_attribute("version", "1.0"));
    decl->append_attribute(doc.allocate_attribute("encoding", "utf-8"));
    decl->append_attribute(doc.allocate_attribute("standalone", "no"));
    doc.append_node(decl);

    rapidxml::xml_node<> *root = doc.allocate_node(rapidxml::node_element, "level");
    doc.append_node(root);

    SaveLevelToXml(root, level);

    // Save to file
    std::string fileName(Core::storeDir.empty() ? Core::storeXML + "_files" : Core::storeDir);
    QDir dir(fileName.c_str());
    if (!dir.exists())
    {
        dir.mkpath(".");
    }
    fileName += ("/" + levelName + ".xml");
    std::ofstream file_stored(Core::Resource_MakePath(fileName.c_str()));
    file_stored << doc;
    file_stored.close();
    doc.clear();
}

void TileEditor::ClearLevel() {
	_level.Clear();

	ClearSelectionList();
	_currents.beautyUnderCursor = NULL;

	TurnOffWalkThrough();
}

bool TileEditor::LoadLevel(const std::string &id, LevelSet &level) 
{
	level.Clear();

    if (Core::storeDir.size() == 0 || !level.LoadFromFile(Core::storeDir + "/" + id + ".xml"))
    {
        rapidxml::xml_node<> *xe = _doc.first_node()->first_node();
        while (xe != NULL && xe->first_attribute("id")->value() != id) {
            xe = xe->next_sibling();
        }
        if (xe != NULL)
        {
            level.LoadFromXml(xe);
        }
        return xe != NULL;
    }
    // успешно загрузились из файла
    return true;
}

rapidxml::xml_node<> * TileEditor::LoadLevel(const std::string &msg) 
{
	ClearLevel();

    rapidxml::xml_node<> *xe = _doc.first_node()->first_node();
    while (xe != NULL && xe->first_attribute("id")->value() != msg) {
        xe = xe->next_sibling();
    }
    assert(xe != NULL);

    // level loading
    if (Core::storeDir.size() == 0 || !_level.LoadFromFile(Core::storeDir + "/" + msg + ".xml"))
    {
        _level.LoadFromXml(xe);
    }

    redoUndoManager.Clear();
    PushCopyToRedoUndoManager();

    rapidxml::xml_node<> *word = xe->first_node("word");
	if (word) {
        _worldOffset.x = atof(word->first_attribute("x")->value());
        _worldOffset.y = atof(word->first_attribute("y")->value());
        _viewScale = atof(word->first_attribute("scale")->value());
	} else {
		_worldOffset.x = 0.f;
		_worldOffset.y = 0.f;
		_viewScale = 1.f;
	}

	_currentLevel = msg;
	TileEditorInterface::Instance()->UpdateTitle();

	return xe;
}

void TileEditor::ReadSelectedInGameType()
{
    if (TileEditorInterface::Instance() == NULL) {
		return;
	} 

    if (TileEditorInterface::Instance()->EmptyProperties() || !IsSelection())
    {
        TileEditorInterface::Instance()->ItemSelected();
    }
    else
    {
        TileEditorInterface::Instance()->UpdateProperties();
    }

//	TileEditorInterface::Instance()->UpdateProperties();


    std::string resultStr;
    bool sameStr = true;
    DWORD resultColor = 0xFFFFFFFF;
    bool sameColor = true;
    float resultFloat = 0.f;
    bool sameFloat = true;
    for (unsigned int i = 0; i < _currents.beauty.size(); ++i) {
        if (resultStr == "" && _currents.beauty[i]->UserString() != "") {
            resultStr = _currents.beauty[i]->UserString();
        } else if (sameStr && resultStr != "" && _currents.beauty[i]->UserString() != resultStr) {
            sameStr = false;
        }
        if (resultColor == 0xFFFFFFFF && _currents.beauty[i]->GetColor() != 0xFFFFFFFF) {
            resultColor = _currents.beauty[i]->GetColor();
        } else if (sameColor && resultColor != 0xFFFFFFFF && _currents.beauty[i]->GetColor() != resultColor) {
            sameColor = false;
        }
        if (resultFloat == 0.f && _currents.beauty[i]->SomeFloatValue() != 0.f) {
            resultFloat = _currents.beauty[i]->SomeFloatValue();
        } else if (sameFloat && resultFloat != 0.f && _currents.beauty[i]->SomeFloatValue() != resultFloat) {
            sameFloat = false;
        }
    }
    //TileEditorInterface::Instance()->UserString()->setText(resultStr.c_str());
    if (!sameStr)
    {
//        QPalette palette;
//        palette.setColor(QPalette::Text, Qt::gray);
//        TileEditorInterface::Instance()->UserString()->setPalette(palette);

        std::string str;
        str = "Multiple selection";
        TileEditorInterface::Instance()->SetStatusBar(str);
    }
    else
    {
//        QPalette palette;
//        palette.setColor(QPalette::Text, Qt::black);
        //TileEditorInterface::Instance()->UserString()->setPalette(palette);
        std::string str;
        if (_currents.beauty.size() == 0)
        {
            str = "Nothing is selected. " + Math::IntToStr(_level.beauties.size()) + " objects in scene.";
        }
        else
        {
            int sameTypeCounter = 0;
            for (unsigned int i = 0; i < _level.beauties.size(); ++i)
            {
                if (_level.beauties[i]->UserString() == resultStr)
                {
                    ++sameTypeCounter;
                }
            }
            str = "Selected " + Math::IntToStr(_currents.beauty.size()) + "/" + Math::IntToStr(sameTypeCounter) + " " + resultStr;
        }
        TileEditorInterface::Instance()->SetStatusBar(str);
    }

//    if (!sameColor) {
//        TileEditorInterface::Instance()->ColorPickButton()->setValue(QColor(0xFF7F7F7F));
//    } else {
//        TileEditorInterface::Instance()->ColorPickButton()->setValue(QColor(resultColor));
//    }

//    if (!sameFloat) {
//        TileEditorInterface::Instance()->HorizontalSlider()->setValue(0);
//    } else {
//        TileEditorInterface::Instance()->HorizontalSlider()->setValue(resultFloat * 100);
//    }

}

void TileEditor::SetInGameTypeForSelected() {
	for (unsigned int i = 0; i < _currents.beauty.size(); ++i) {
        //_currents.beauty[i]->UserString() = TileEditorInterface::Instance()->UserString()->text().c_str();
	}
    QPalette palette;
    palette.setColor(QPalette::Text, Qt::black);
    //TileEditorInterface::Instance()->UserString()->setPalette(palette);
    PushCopyToRedoUndoManager();
}

void TileEditor::SetColorForSelected() {
    //Gwen::Color color = TileEditorInterface::Instance()->ColorPickButton()->GetColor();
    DWORD inColor = 0xFFFFFFFF;//color.a << 24 | color.r << 16 | color.g << 8 | color.b;
	for (unsigned int i = 0; i < _currents.beauty.size(); ++i) {
		_currents.beauty[i]->SetColor( inColor );
	}
	//     ..   
    //PushCopyToRedoUndoManager();
}

void TileEditor::SetSliderValueForSelected() 
{
	std::string cmd("setup");

    float value = TileEditorInterface::Instance()->HorizontalSlider()->value() / 100.f;
	char buff[20];
	Math::FloatToChar(value, buff);
	cmd += buff;

	for (unsigned int i = 0; i < _currents.beauty.size(); ++i) {
		_currents.beauty[i]->Command( cmd );
	}
}

std::string TileEditor::GetCurrentLevel() {
	return (_currentLevel.size() ? _currentLevel : NOT_DEFINED_NAME);
}

void TileEditor::PushCopyToRedoUndoManager() {
	redoUndoManager.PushCopy(&_level, CURRENTBEAUTY);
}

void TileEditor::DeleteLevel(const std::string &name)
{
    rapidxml::xml_node<> *xe = _doc.first_node()->first_node();
    while (xe != NULL && xe->first_attribute("id")->value() != name) {
        xe = xe->next_sibling();
	}
	if (xe)
	{
        _doc.first_node()->remove_node(xe);

        std::ofstream file_stored(Core::Resource_MakePath(Core::storeXML.c_str()));
        file_stored << _doc;
        file_stored.close();
    }
    if (Core::storeDir.size())
    {
        QDir r;
        //bool check =
        r.remove((Core::storeDir + "/" + name + ".xml").c_str());
        //RemoveFile(Core::storeDir + "/" + name + ".xml");
    }
}

bool TileEditor::IsLevelExist(const std::string &name)
{
    rapidxml::xml_node<> *xe = _doc.first_node()->first_node();
    while (xe != NULL && xe->first_attribute("id")->value() != name) {
        xe = xe->next_sibling();
	}
	return (xe != NULL);
}

void TileEditor::Rename(const std::string &name) {
	if (name.size() == 0 || _currentLevel == "") {
		return;
	}

    if (Core::storeDir.size())
    {
        QDir r;
        //bool check =
        r.rename((Core::storeDir + "/" + _currentLevel + ".xml").c_str(), (Core::storeDir + "/" + name + ".xml").c_str());
    }

    rapidxml::xml_node<> *xe = _doc.first_node()->first_node();
	std::string s;
    while (xe != NULL && xe->first_attribute("id")->value() != _currentLevel) {
        xe = xe->next_sibling();
	}
	assert(xe != NULL);
    rapidxml::xml_attribute<> *id = xe->first_attribute("id");
    char *copyName = xe->document()->allocate_string(name.c_str());
    id->value(copyName);
	_currentLevel = name;	

    std::ofstream file_stored(Core::Resource_MakePath(Core::storeXML.c_str()));
    file_stored << _doc;
    file_stored.close();
}

void TileEditor::CreateLinkToComplex()
{
    LinkToComplex *b = new LinkToComplex(_currentLevel);
    std::string caption = (b->UserString() != "" ? ("\"" + b->UserString() + "\" ") : "") + "Complex";
    _collection.push_back(b);
    TileEditorInterface::Instance()->GetCollectionControl()->AddItem(b, caption);

    SaveTemplates(Core::projectXML);
}

void TileEditor::CreateFolder()
{

}

void TileEditor::ItemDublicate()
{
//    int selectionIndex = TileEditorInterface::Instance()->GetCollectionControl()->currentRow();
//    if(selectionIndex < 0 || selectionIndex >= TileEditorInterface::Instance()->GetCollectionControl()->count())
//    {
//        return;
//    }
//    QTreeWidgetItem *item = TileEditorInterface::Instance()->GetCollectionControl()->item(selectionIndex);
    QTreeWidgetItem *item = TileEditorInterface::Instance()->GetCollectionControl()->currentItem();
    int selectionIndex = TileEditorInterface::Instance()->GetCollectionControl()->indexOfTopLevelItem(item);

    if (item->parent())
        selectionIndex = item->parent()->indexOfChild(item);

    if (item == NULL || selectionIndex == -1)
    {
       return;
    }
    CustomDataClass data = item->data(0, Qt::UserRole).value<CustomDataClass>();
    BeautyBase *origin = data.beauty;

	BeautyBase *b = MakeCopy(origin);

	std::string typeName = b->Type();
	std::string caption = "";
	if (typeName == "ColoredPolygon") {
		caption = "polygon";
	} else if (typeName == "GroundLine") {
		caption = "Way";
	} else if (typeName == "ClickArea") {
		caption = "clickarea";
	} else if (typeName == "Beauty") {
		Beauty *a = static_cast<Beauty *>(b);
        std::string tex(a->GetTexturePath().substr(a->GetTexturePath().rfind("/") + 1));
        caption = "\"" + tex + "\"";
    } else if (typeName == "BeautyText") {
        BeautyText *a = static_cast<BeautyText *>(b);
        std::string tex(a->GetTexturePath().substr(a->GetTexturePath().rfind("/") + 1));
        caption = "\"" + tex + "\"";
    } else if (typeName == "Animation") {
		AnimationArea *a = static_cast<AnimationArea *>(b);
		caption = "Animation \"" + a->Name() + "\"";
	} else if (typeName == "GroundSpline") {
		caption = "Spline Way";
	} else if (typeName == "LinkToComplex") {
		caption = "Complex";
	} else if (typeName == "SolidGroundLine") {
		caption = "Spline Way";
	} else if (typeName == "StretchTexture") {
		caption = "StretchTexture";
	} else {
		assert(false);
	}
    caption = (b->UserString() != "" ? ("\"" + b->UserString() + "\" ") : "") + caption;
	_collection.push_back(b);
    TileEditorInterface::Instance()->GetCollectionControl()->AddItem(b, caption, selectionIndex, item->parent());

    SaveTemplates(Core::projectXML);
}

void TileEditor::ClearSelectionList()
{
	_currents.beauty.clear();
	ReadSelectedInGameType();
    SetButtonsState(bs_nothing);
}

void TileEditor::PushBeautyToSelectionList(BeautyBase *b)
{
	BeautyList::iterator insertBefore = _currents.beauty.begin();
	for (BeautyList::iterator i = _level.beauties.begin(), e = _level.beauties.end()
				; i != e && insertBefore != _currents.beauty.end() && (*i) != b
				; ++i)
	{
		if ((*insertBefore) == (*i))
		{
			insertBefore++;
		}
		else if (b == (*i))
		{
			break;
		}
	}

	_currents.beauty.insert( insertBefore, b );
	ReadSelectedInGameType();

    if (Selection().size() == 1)
    {
        SetButtonsState(bs_standart);
    }
    else
    {
        SetButtonsState(bs_nothing);
    }
}

void TileEditor::ItemRemove(BeautyBase *base)
{
	for (Collection::iterator i = _collection.begin(), e = _collection.end(); i != e; i++) 
	{
		if ((*i) == base)
		{
			delete (*i);
			_collection.erase(i);
			break;
		}
	}

    SaveTemplates(Core::projectXML);
}

void TileEditor::ItemProperties()
{
}

bool TileEditor::CreateBeautyWithPng(std::string filePng)
{

    if (filePng.size() > 4 && (filePng.substr(filePng.size() - 4) == ".png" || filePng.substr(filePng.size() - 4) == ".PNG")) {
        std::string dirAndPath = Math::ProcessTexture(filePng.c_str(), Core::workDir.substr(0, Core::workDir.size() - 1));
        if (dirAndPath.size())
        {
            Beauty *b = new Beauty(Core::workDir + dirAndPath);
            std::string tex(b->GetTexturePath().substr(b->GetTexturePath().rfind("/") + 1));
            std::string caption = "\"" + tex + "\"";
            caption = (b->UserString() != "" ? ("\"" + b->UserString() + "\" ") : "") + caption;
            _collection.push_back(b);
            TileEditorInterface::Instance()->GetCollectionControl()->AddItem(b
                , "No" + Math::IntToStr(_collection.size()) + " " + caption);
            return true;
        }
    }
    return false;
}

void TileEditor::ProcessSelectionArea(Rect rect)
{
	if (_mouseMoveAction != mouse_select_region)
	{
		return;
	}
	if (TileEditorInterface::Instance()->CreateDotMode()) {
		if (_currents.beauty.size() == 1) {
			_currents.beauty[0]->Selection(rect, false);
		}
	}
	else 
	{
		std::set<BeautyBase *> old;
        if ((QApplication::keyboardModifiers() & Qt::ControlModifier) != 0)
		{
			for (unsigned int i = 0; i < _currents.beauty.size(); ++i) {
				old.insert(_currents.beauty[i]);
			}
		}
		ClearSelectionList();
		Rect beautyRect;
		FPoint a1(rect.x1, rect.y1);
		FPoint a2(rect.x2, rect.y2);
		a1 = ScreenToWorld(a1);
		a2 = ScreenToWorld(a2);
		Rect selection;
		selection.Encapsulate(a1.x, a1.y);
		selection.Encapsulate(a2.x, a2.y);
		std::set<BeautyBase *> sel;
		for (unsigned int i = 0; i < _level.beauties.size(); ++i) {
			beautyRect.Clear();
			_level.beauties[i]->EncapsulateAllDots(beautyRect);
            if ((!_currentSelectionMode && selection.Intersect(&beautyRect))
                    || (_currentSelectionMode && selection.Contain(&beautyRect)))
			{
				sel.insert(_level.beauties[i]);
			}
		}
		sel.insert(old.begin(), old.end());
		for (std::set<BeautyBase *>::iterator i = sel.begin(); i != sel.end(); ++i) {
			PushBeautyToSelectionList(*i);
		}
		if (_currents.beauty.size())
		{
            PushCopyToRedoUndoManager();
		}
	}
}

bool TileEditor::TryLoadWalkThrough()
{
	return _walkThrough.Init(); 
}

void TileEditor::TurnOffWalkThrough()
{
    _walkThrough.Clear();
    if (TileEditorInterface::Instance() && TileEditorInterface::Instance()->GetWalkThroughBtn() && TileEditorInterface::Instance()->GetWalkThroughBtn()->isChecked())
    {
        TileEditorInterface::Instance()->GetWalkThroughBtn()->setChecked(false);
    }
}

void TileEditor::SetCurrentBeautyGeometry(bool move, float x, float y, bool rotate, float a, bool scale, float sx, float sy)
{
	if (_currents.beauty.size() == 1)
	{
		if (move)
            _currents.beauty[0]->SetPosition(x, y);
		if (rotate && _currents.beauty[0]->Rotated())
			_currents.beauty[0]->SetAngle(a);
		if (scale && _currents.beauty[0]->Scaled())
			_currents.beauty[0]->SetScale(sx, sy);
		PushCopyToRedoUndoManager();
	}
}

void TileEditor::SetCurrentBeautyUserData(bool move, float x, float y, bool scalar, float a, const char *id)
{
	if (_currents.beauty.size() == 1)
	{
		if (move)
			_currents.beauty[0]->PointData() = FPoint(x, y);
		if (scalar)
			_currents.beauty[0]->FloatData() = a;
		_currents.beauty[0]->GetUnicOrLinkedId() = id;
		PushCopyToRedoUndoManager();
	}
}

void TileEditor::UngroupComplex()
{
    _currents.beautyUnderCursor = NULL;
    if (_currents.beauty.size() == 1 && _currents.beauty[0]->Type() == "LinkToComplex")
	{
		LinkToComplex *c = static_cast<LinkToComplex *>(_currents.beauty[0]);
		BeautyList bl;
		c->MakeCopyOfBeauties(bl);

		BeautyList::iterator i = _level.beauties.begin();
		for (; i != _level.beauties.end() && (*i) != c; ++i);
		        
        //_level.beauties.insert(i, bl.begin(), bl.end()); - QVector так не умеет
        BeautyList help(bl);
        while (help.begin() != help.end())
        {
            i = _level.beauties.insert(i, help.back());
            help.pop_back();
        }
        // замена insert
		
		i = _level.beauties.begin();
		for (; i != _level.beauties.end() && (*i) != c; ++i);
	
		assert(i != _level.beauties.end());
		delete (*i);
		_level.beauties.erase(i);


        ClearSelectionList();
		for (unsigned int i = 0; i < bl.size(); ++i)
		{
			PushBeautyToSelectionList(bl[i]);
		}
		PushCopyToRedoUndoManager();
	}
}

void TileEditor::PushTexturesToAtlas()
{
	for (unsigned int i = 0; i < _level.beauties.size(); ++i)
	{
		if (_level.beauties[i]->GetTexturePath().size())
		{
			Agregator::PushFileName(_level.beauties[i]->GetTexturePath());
		}
	}
}

void TileEditor::PushLevelsTexturesToAtlas(QVector<std::string> list)
{
	for (unsigned int i = 0; i < list.size(); ++i) 
	{
        rapidxml::xml_node<> *xe = _doc.first_node()->first_node();
        while (xe != NULL && xe->first_attribute("id")->value() != list[i])
		{			
            xe = xe->next_sibling();
		}
        if (xe && Core::storeDir.size() == 0)
		{
            rapidxml::xml_node<> *xeBeautyList = xe->first_node("Beauties");
			if (xeBeautyList)
			{
                rapidxml::xml_node<> *xeBeauty = xeBeautyList->first_node("Beauty");
				while (xeBeauty)
				{
                    Agregator::PushFileName(xeBeauty->first_attribute("texture")->value());
                    xeBeauty = xeBeauty->next_sibling("Beauty");
				}
			}
		}
        else if (xe)
        {
            std::vector<char> buffer;
            rapidxml::xml_document<> doc;

            std::ifstream myfile(Core::Resource_MakePath((Core::storeDir + "/" + list[i] + ".xml").c_str()));
            buffer.clear();
            buffer = std::vector<char>(std::istreambuf_iterator<char>(myfile), std::istreambuf_iterator<char>());
            if (buffer.size() == 0)
            {
                LOG("Error! Level file not found! " + Core::Resource_MakePath(list[i].c_str()));
                return;
            }

            buffer.push_back('\0');
            if (!Parse(doc, buffer, list[i].c_str()))
            {
                return;
            }

            rapidxml::xml_node<> *xeBeautyList = doc.first_node()->first_node("Beauties");
            if (xeBeautyList)
            {
                rapidxml::xml_node<> *xeBeauty = xeBeautyList->first_node("Beauty");
                while (xeBeauty)
                {
                    Agregator::PushFileName(xeBeauty->first_attribute("texture")->value());
                    xeBeauty = xeBeauty->next_sibling("Beauty");
                }
            }
        }
	}
}

void TileEditor::PushFileNamesToMap(QVector<std::string> list, NameList &names)
{
	names.clear();
	for (unsigned int i = 0; i < list.size(); ++i) 
	{
        rapidxml::xml_node<> *xe = _doc.first_node()->first_node();
		std::string s;
        while (xe != NULL && xe->first_attribute("id")->value() != list[i])
		{			
            xe = xe->next_sibling();
		}
		if (xe)
		{
            rapidxml::xml_node<> *xeBeautyList = xe->first_node();
			if (xeBeautyList)
			{
                rapidxml::xml_node<> *xeBeauty = xeBeautyList->first_node();
				while (xeBeauty)
				{
                    rapidxml::xml_attribute<> *tmp = xeBeauty->first_attribute("texture");
					if (tmp)
					{
                        names[tmp->value()] = 1;
					}
                    xeBeauty = xeBeauty->next_sibling();
				}
			}
		}
	}
}

void TileEditor::RemoveSelection()
{
	for (unsigned int k = 0; k < _currents.beauty.size(); ++k) {					
		for (unsigned int i = 0; i < _level.beauties.size(); ++i) {
			if (_level.beauties[i] == _currents.beauty[k]) {
				delete _level.beauties[i];
				_level.beauties.erase(_level.beauties.begin() + i);							
				break;
			}
		}
	}
	ClearSelectionList();
	_currents.beautyUnderCursor = NULL;
    PushCopyToRedoUndoManager();
}

void TileEditor::CutSelection()
{
	CopySelection();
	RemoveSelection();
}

void TileEditor::CopySelection()
{
	DropCopyPastBuffer();
	for (unsigned int k = 0; k < _currents.beauty.size(); ++k) 
	{
		_copyPastBuffer.beauties.push_back(MakeCopy(_currents.beauty[k]));
	}
}

void TileEditor::PasteSelection()
{
	ClearSelectionList();
	FPoint center(0, 0);
	for (BeautyList::iterator i = _copyPastBuffer.beauties.begin(), e = _copyPastBuffer.beauties.end(); i != e; ++i) 
	{
		BeautyBase *b = MakeCopy(*i);
		_level.beauties.push_back(b);
		PushBeautyToSelectionList(b);
		center += (*i)->GetPos()/* - FPoint((*i)->Width() / 2, (*i)->Height() / 2)*/;
	}
	center.x /= _copyPastBuffer.beauties.size();
	center.y /= _copyPastBuffer.beauties.size();
	FPoint shift = _mouseWorld - center;
	for (unsigned int k = 0; k < _currents.beauty.size(); ++k) 
	{
		_currents.beauty[k]->ShiftTo(shift.x, shift.y);
	}
}

void TileEditor::DropCopyPastBuffer()
{
	_copyPastBuffer.Clear();
}

void TileEditor::CreateLevelList(QVector<std::string> &list)
{
	list.clear();

    rapidxml::xml_node<> *xe = _doc.first_node()->first_node();
	std::string s;
	while (xe)
	{			
        s = xe->first_attribute("id")->value();
		list.push_back(s);
        xe = xe->next_sibling();
	}
}

void TileEditor::initializeGL()
{
    glDisable(GL_DEPTH_TEST);
    glEnable ( GL_BLEND );
    glClearColor(0.50f, 0.50f, 0.50f, 1.0f);
    glBlendFunc(GL_SRC_ALPHA ,GL_ONE_MINUS_SRC_ALPHA);
}

void TileEditor::resizeGL(int width, int height)
{
    _widgetHeight = height;
    _widgetWidth = width;
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glOrtho( 0, width, 0, height, -1.0, 1.0);
    glTranslatef(0, height, 0);
    glScalef(1.f, -1.f, 1.f);
    glMatrixMode( GL_MODELVIEW );
    glViewport(0, 0, width, height);
}

void TileEditor::mouseReleaseEvent(QMouseEvent *event)
{
    FPoint mousePos(event->pos().x(), event->pos().y());
    if (event->button() == Qt::LeftButton)
    {
        if ((mousePos - _mouseDownPos).Length() < 10 && _mouseMovingMode < 0.2f)
        {
            _mouseMoveAction = mouse_none;
        }
        _selectionTool.OnMouseUp(mousePos);
        OnMouseUp();
    }
    else if (event->button() == Qt::RightButton)
    {
        _mouseMoveAction = mouse_none;
    }
}

void TileEditor::mousePressEvent(QMouseEvent *event)
{
    FPoint mousePos(event->pos().x(), event->pos().y());
    _lastMousePos = mousePos;
    if (event->button() == Qt::LeftButton)
    {
        OnMouseDown(mousePos);
    }
    else if (event->button() == Qt::RightButton)
    {
        _mouseMoveAction = mouse_dragging_world;
    }
}

void TileEditor::mouseMoveEvent(QMouseEvent *event)
{
    FPoint mousePos(event->pos().x(), event->pos().y());
    if ((QApplication::keyboardModifiers() & Qt::ControlModifier) != 0)
    {
        if (fabs(_mouseDownPos.x - mousePos.x) > fabs(_mouseDownPos.y - mousePos.y))
        {
            mousePos.y = _mouseDownPos.y;
        }
        else
        {
            mousePos.x = _mouseDownPos.x;
        }
    }
    _selectionTool.OnMouseMove(mousePos);
    OnMouseMove(mousePos);
}

void TileEditor::ResaveAllLevels()
{
    rapidxml::xml_node<> *xe = _doc.first_node()->first_node();
    while (xe != NULL) {
        _saveLevelXml = xe;
        char *name = xe->first_attribute("id")->value();
        if (Core::storeDir.size() == 0 || !_level.LoadFromFile(Core::storeDir + "/" + name + ".xml"))
        {
            _level.LoadFromXml(xe);
        }

        rapidxml::xml_node<> *elem = xe->first_node();
        while (elem && std::string(elem->name()) != "Beauties")
        {
            elem = elem->next_sibling();
        }
        if (elem && std::string(elem->name()) == "Beauties") {
            xe->remove_node(elem);
        }

        if (TileEditorInterface::Instance()->SaveCopyToFile())
        {
            SaveLevelToFile(_saveLevelXml->first_attribute("id")->value(), _level);
        }
        else
        {
            rapidxml::xml_node<> *beautyList = _doc.allocate_node(rapidxml::node_element, "Beauties");
            xe->append_node(beautyList);
            for (BeautyList::iterator i = _level.beauties.begin(), e = _level.beauties.end(); i != e; ++i)
            {
                char *name = _doc.allocate_string((*i)->Type().c_str());
                rapidxml::xml_node<> *beauty = _doc.allocate_node(rapidxml::node_element, name);
                beautyList->append_node(beauty);
                (*i)->SaveToXml(beauty);
            }
        }

        xe = xe->next_sibling();
    }
    // Save to file
    std::ofstream file_stored(Core::Resource_MakePath(Core::storeXML.c_str()));
    file_stored << _doc;
    file_stored.close();
}

void TileEditor::DrawBeautyInBuffer(BeautyBase *base, QIcon *&icon)
{
    test_beauty = base;
    updateGL();
    test_beauty = NULL;
    icon = new QIcon(QPixmap::fromImage(QImage(image_buffer, 64, 64, QImage::Format_RGB888)));
}

void TileEditor::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat("application/x-dndbeauty"))
    {
        event->acceptProposedAction();
    }
}

void TileEditor::dropEvent(QDropEvent *event)
{
    if (event->source() == this && event->possibleActions() & Qt::MoveAction)
        return;

    if (event->mimeData()->hasFormat("application/x-dndbeauty"))
    {
        QByteArray array = event->mimeData()->data("application/x-dndbeauty");
        BeautyBase *origin = NULL;
        memcpy(&origin, array.constData(), sizeof(origin));
        if (origin)
        {
            BeautyBase *b = MakeCopy(origin);
            FPoint fp = ScreenToWorld(FPoint(event->pos().x(), event->pos().y()));
            b->SetPosition(fp.x, fp.y);
            _level.beauties.push_back(b);
            ClearSelectionList();
            PushBeautyToSelectionList( b );
            event->accept();
            PushCopyToRedoUndoManager();
        }
    }
}

void TileEditor::SetButtonsState(ButtonsState state)
{
    _commandButtons.clear();
    _buttonsState = state;
    if (state == bs_nothing)
    {
    }
    else if (state == bs_standart || state == bs_beauty_text)
    {
        _commandButtons.push_back(_editBtn);
    }
    else if (state == bs_beauty)
    {
        _commandButtons.push_back(_editBtn);
    }
    else if (state == bs_spline)
    {
        _commandButtons.push_back(_editBtn);
    }
    else if (state == bs_stretch_mesh)
    {
        _commandButtons.push_back(_editBtn);
        _commandButtons.push_back(_splineBtn);
    }
    else if (state == bs_tiled_mesh)
    {
        _commandButtons.push_back(_editBtn);
        _commandButtons.push_back(_splineBtn);
    }
    else if (state == bs_tiled_line)
    {
        _commandButtons.push_back(_editBtn);
        _commandButtons.push_back(_meshBtn);
    }
    else if (state == bs_stretch_spline)
    {
        _commandButtons.push_back(_editBtn);
        _commandButtons.push_back(_meshBtn);
        _commandButtons.push_back(_stretchModeBtn);
    }
    else if (state == bs_polygon_mesh)
    {
        _commandButtons.push_back(_editBtn);
        _commandButtons.push_back(_rotRigthBtn);
        _commandButtons.push_back(_rotLeftBtn);
        _commandButtons.push_back(_scaleTexUpBtn);
        _commandButtons.push_back(_scaleTexDnBtn);
    }
    else if (state == bs_complex)
    {
        _commandButtons.push_back(_editBtn);
        _commandButtons.push_back(_ungroupBtn);
    }
    else if (state == bs_animation)
    {
        _commandButtons.push_back(_editBtn);
        _commandButtons.push_back(_playBtn);
    }
    else if (state == bs_dots_edit)
    {
        _commandButtons.push_back(_editBtn);
    }
    else
    {
        assert(false);
    }
    _ungroupBtn->Visible() = false;
    _editBtn->Visible() = false;
    _meshBtn->Visible() = false;
    _stretchModeBtn->Visible() = false;
    _splineBtn->Visible() = false;
    _playBtn->Visible() = false;
    _rotRigthBtn->Visible() = false;
    _rotLeftBtn->Visible() = false;
    _scaleTexUpBtn->Visible() = false;
    _scaleTexDnBtn->Visible() = false;

    for (int i = 0; i < _commandButtons.size(); ++i)
    {
        _commandButtons[i]->Visible() = true;
    }

}

