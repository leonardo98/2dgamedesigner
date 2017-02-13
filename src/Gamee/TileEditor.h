#ifndef MYENGINE_TILEEDITOR_H
#define MYENGINE_TILEEDITOR_H
#include <QGLWidget>
#include <QLineEdit>
#include <QGLPixelBuffer>
#include <QTreeWidgetItem>
#include <QGLFramebufferObject>

#include "../Core/Messager.h"
#include "../Core/ogl/render.h"
#include <cstdlib>
//#include "Box2D/Box2D.h"
#include "../2dga_api/Animation.h"
#include "LevelSet.h"
#include "Beauty.h"
#include "RedoUndoManager.h"
#include "SelectionTool.h"
#include "../Core/WalkThrough.h"
#include "../Atlas/DataContainer.h"

#define NOT_DEFINED_NAME "#not defined yet#"

class TileEditor;
struct Settings;

enum MouseActionState
{
    mouse_dragging_world,
    mouse_moving_beauty,
    mouse_moving_beauty_control,
    mouse_select_region,
    mouse_drop_or_select_region,
    mouse_none
};


//       
class TileEditor 
    : public QGLWidget
    , public Messager
{
    Q_OBJECT

public slots:
    void Save();
    void SaveAsNew();
    void CutSelection();
    void CopySelection();
    void PasteSelection();
    void RemoveSelection();
    void SaveAsComplex();
    void ItemDublicate();
    void DeletePressed();
    void CreateFolder();
    void BackspaceSwitch();

private:
    void UngroupComplex();
    bool _currentSelectionMode;
    unsigned int _clearColor;
    WalkThrough _walkThrough;
    int _widgetWidth;
    int _widgetHeight;
    int Width() { return _widgetWidth; }
    int Height() { return _widgetHeight; }
public:
    QGLFramebufferObject *fbo;
    TileEditor(QWidget *parent = 0);
    QList<QPair<BeautyBase *, QTreeWidgetItem *> > _needIcons;
    void NeedBeautyIcon(BeautyBase *b, QTreeWidgetItem *i)
    {
        _needIcons.push_back(qMakePair(b, i));
    }
    void DrawBeautyInBuffer(BeautyBase *, QIcon *&);
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

    void wheelEvent(QWheelEvent* event) ;

    virtual ~TileEditor();

    virtual void OnMouseDown(const FPoint &mousePos);
    virtual void OnMouseUp();
    void OnMouseMove(const FPoint &mousePos);

    virtual bool IsMouseOver(const FPoint &mousePos);
    virtual bool OnMouseWheel(float value);
    virtual void Draw();
    virtual void Update(float deltaTime);

    void LoadTemplates(const rapidxml::xml_document<> &doc);
    bool SaveTemplates(const std::string &filename);
    void ReadSelectedInGameType();
    void SetInGameTypeForSelected();
    void SetColorForSelected();
    void SetSliderValueForSelected();
    bool IsSelection() { return _currents.beauty.size() > 0; }
    void PushCopyToRedoUndoManager();

    void ItemRemove(BeautyBase *base);
    void ItemProperties();
    bool CreateBeautyWithPng(std::string filePng);
    bool IsLevelExist(const std::string &name);
    void DeleteLevel(const std::string &name);
    std::string GetCurrentLevel();
    void PushBeautyToSelectionList(BeautyBase *);
    void ClearSelectionList();
    bool TryLoadWalkThrough();
    void TurnOffWalkThrough();

    typedef QVector<BeautyBase *> Collection;
    Collection _collection;

    RedoUndoManager redoUndoManager;
    bool _pushCopyOnMouseUp;

    void Rename(const std::string &name);
    void ProcessSelectionArea(Rect rect);
    void SetSelectionMode(bool value) { _currentSelectionMode = value; }
    static TileEditor *Instance() { return instance;}
    BeautyList &Selection() { return _currents.beauty; }
    void PushTexturesToAtlas();
    void PushLevelsTexturesToAtlas(QVector<std::string> list);
    void PushFileNamesToMap(QVector<std::string> list, NameList &names);
    void LoadLevelSet();
    void Clear();
    void SetCurrentBeautyGeometry(bool move, float x, float y, bool rotate, float a, bool scale, float sx, float sy);
    void SetCurrentBeautyUserData(bool move, float x, float y, bool scalar, float a, const char *id);
    void DropCopyPastBuffer();
    void CreateLevelList(QVector<std::string> &list);
    void ResaveAllLevels();
    bool LoadLevel(const std::string &id, LevelSet &level);
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);
    void SetFrame(int w, int h) { _viewportWidth = w; _viewportHeight = h; }
protected:
    BeautyBase *test_beauty;
    unsigned char image_buffer[64 * 64 * 3];
    static TileEditor *instance;

    MouseActionState _mouseMoveAction;
    SelectionTool _selectionTool;
    FPoint _mouseWorld;
    int _viewportWidth, _viewportHeight;
    bool _mouseDown;
    int SCREEN_WIDTH;
    int SCREEN_HEIGHT;
    const float SLIDER_SCALE;
    const float SLIDER_MIN;

    std::string _preSaveLevelName;
    std::string _saveLevelName;
    rapidxml::xml_node<> *_preSaveLevelXml;
    rapidxml::xml_node<> *_saveLevelXml;
    void SaveLevel(const std::string &levelName, LevelSet &level);
    void SaveLevelToFile(const std::string &levelName, LevelSet &level);

    FPoint _lastMousePos;
    FPoint _mouseDownPos;
    float _mouseMovingMode;

    float _viewScale; //   
    
    FPoint _worldOffset; //         
    FPoint _screenOffset; //        (0, 0) (       )

    virtual void OnMessage(const std::string &message);

    std::vector<char> _bufferForDoc;
    rapidxml::xml_document<> _doc;
    std::string _currentLevel;
    void SaveLevelToXml(rapidxml::xml_node<> *xe, LevelSet &level);

    int round(float a);

    LevelSet _level;
    CurrentBlock _currents;
    void ClearLevel();
    rapidxml::xml_node<> * LoadLevel(const std::string &msg);
    //typedef QList<b2Body *> LandBodies;
    //LandBodies _landBodies;
    FPoint _endPoint;
    FPoint _startPoint;
    void NewLevelYes();
    void OverwriteYes();
    void AddBackImage(const std::string &msg);
    void AddCover(const std::string &msg);
    void PreSaveLevel(const std::string &msg);
    //    
    void CreateLinkToComplex();
    FPoint ScreenToWorld(const FPoint &screenPos);
    FPoint WorldToScreen(const FPoint &worldPos);

    virtual void  keyPressEvent ( QKeyEvent * event );

    void SelectionToComplex();
    friend class TileEditorInterface;
    BeautyBase *BeautyAtPoint(const FPoint &worldPoint);
    BeautyBase *ControlsAtPoint(const FPoint &screenPos);

    LevelSet _copyPastBuffer;

    Beauty *_ungroupBtn;
    Beauty *_editBtn;
    Beauty *_meshBtn;
    Beauty *_stretchModeBtn;
    Beauty *_splineBtn;
    Beauty *_playBtn;
    Beauty *_rotRigthBtn;
    Beauty *_rotLeftBtn;
    Beauty *_scaleTexUpBtn;
    Beauty *_scaleTexDnBtn;

    BeautyList _commandButtons;
    BeautyList _oldCommandButtons;

    enum ButtonsState
    {
        bs_nothing,
        bs_standart,
        bs_beauty,
        bs_beauty_text,
        bs_spline,
        bs_stretch_mesh,
        bs_stretch_spline,
        bs_dots_edit,
        bs_polygon_mesh,
        bs_complex,
        bs_tiled_mesh,
        bs_tiled_line,
        bs_animation
    };

    ButtonsState _buttonsState;

//    struct ButtonState
//    {
//    protected:
//        bool pressed;
//    public:
//        bool visible;
//        ButtonState(bool pressed, bool visible)
//        {
//            this->pressed = pressed;
//            this->visible = visible;
//        }
//        void SetPressed(bool value)
//        {
//            pressed = value;
//        }
//        bool IsPressed()
//        {
//            return pressed;
//        }
//    };
//    ButtonState _meshBtnState;

    void SwitchToCustomEditMode();
public:
    //ButtonState &GetEditBtnState() { return _meshBtnState; }
//    Beauty *GetPlayBtn() { return _playBtn; }
//    Beauty *GetUngroupBtn() { return _ungroupBtn; }

    void SetButtonsState(ButtonsState state);
    ButtonsState GetButtonsState() { return _buttonsState; }
};

#endif//MYENGINE_TILEEDITOR_H
