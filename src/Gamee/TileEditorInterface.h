#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QSettings>
#include <QMainWindow>
#include <QCheckBox>
#include <QTreeWidget>
#include <QStatusBar>
#include <QAction>
#include <QLabel>
#include <QScrollArea>
#include <QMenu>
#include <QMenuBar>
#include <QSlider>
#include <QPushButton>
#include <QMutex>
#include <QSplitter>
#include <QListWidget>

#include "qtvariantproperty.h"

#include "../Core/Messager.h"
#include "CollectionControl.h"
#include "TileEditor.h"
#include "ColoredButton.h"
#include "qtpropertymanager.h"
#include "qteditorfactory.h"
#include "qttreepropertybrowser.h"
#include "qtbuttonpropertybrowser.h"
#include "qtgroupboxpropertybrowser.h"

enum EnterNameMode
{
    mode_save_as,
    mode_rename,
    mode_level_atlas_name,
    mode_run_command
};

//QT_BEGIN_NAMESPACE
//class QLabel;
//class QMenu;
//class QScrollArea;
//class QSlider;
//QT_END_NAMESPACE

class TileEditorInterface
    : public QMainWindow
    , public Messager
{
    Q_OBJECT

public:
    TileEditorInterface();
    bool eventFilter(QObject *obj, QEvent *event);
private slots:
    void about();
    void NewBtn();
    void Load();
    void FastSave();
    void Update();
    void ListWindowOk();
    void OpenSupportURL();
    void ChangeEditorOptions();
    void Undo();
    void Redo();
    void RenameLevelDialog();
    void EnterNameDialogOk();
    void EditorOptions();
    void DropLevel();
    void MenuItemRemove();
    void ChangeWalkThrough();
    void OpenLevelChangeMask(const QString & text);
    // для окна Create Common Atlas
    void LoadSelectionPreset();
    void SaveSelectionPreset();
public slots:
    void StartGameFunction();
    void ShowChooseLevelsWindow();
    void CreateAtlasForLevel();

    void CreateProjectFn();
    void OpenProject();
    void TestAnimation();
    void OnSliderChange() { _tileEditor->SetSliderValueForSelected(); }
    void valueChanged(QtProperty *property, const QString &value);
    void valueChanged(QtProperty *property, const QColor &value);
    void valueChanged(QtProperty *property, double value);
    void valueChanged(QtProperty *property, int value);
    void ResaveAllLevels();
    void ChangeViewPortSize(QAction*);
    void ChangeAtlasSize(QAction*);
    void ChangeListTree();
    void CreateCommonAtlas();
    void ExportSelectUnselectAll();
    void ChangeSelectionMode(QAction*);
    void ShowHelloWindow();

private:
    static TileEditorInterface *_instance;
    void createActions();
    void createMenus();
    QSize getSize();

    QMutex	m_mutex;
    QWidget *centralWidget;
    QScrollArea *pixmapLabelArea;
    QLabel *pixmapLabel;
    QDockWidget *CreatePropertyBrowser();

    QAction *grabFrameBufferAct;
    QAction *renderIntoPixmapAct;
    QAction *clearPixmapAct;
    QAction *_netViewBtn;
    QAction *_listTreeBtn;
    QAction *_filteringTextureBtn;
    QAction *_blendingTextureBtn;
    QAction *_saveCopyToFile;
    QAction *_viewportBtn;
    QAction *_walkThroughBtn;

    QAction *deleteSelection;
    //QAction *selectionToComplex;
    QAction *cutSelection;
    QAction *copySelection;
    QAction *pasteSelection;

    QLineEdit *_storeXMLText;
    QLineEdit *_storeDirText;
    QLineEdit *_atlasNameText;
    QLineEdit *_workDirText;
    QLineEdit *_pathToGameText;


private:
    QMap<QString, QtProperty *> idToProperty;
    QMap<QtProperty *, QString> propertyToId;
    void InitInterface();
    CollectionControl* _itemList;
    TileEditor *_tileEditor;
    QDialog *_listWindow;
    QLineEdit *_levelOpenName;
    QListWidget *_customListBox;
    QStatusBar* _statusBas;
    std::string _messagePrefix;
    QListWidgetItem *_selectedItem;

public:
    void closeEvent(QCloseEvent *event);
    void ItemSelected();
    void UpdateProperties();
    void SetStatusBar(const std::string &str) { _statusBas->showMessage(tr(str.c_str())); }
    QListWidgetItem *SelectedItem() { return _selectedItem; }

    bool FilteringTexture() { return _filteringTextureBtn->isChecked(); }
    bool AlphaBlend() { return _blendingTextureBtn->isChecked(); }
    bool SaveCopyToFile();
    bool ViewportVisible() { return _viewportBtn->isChecked(); }
    bool NetVisible() { return _netViewBtn->isChecked(); }
    bool ListTreeBtn() { return _listTreeBtn->isChecked(); }
    QAction *GetWalkThroughBtn();

    CollectionControl* GetCollectionControl();
    virtual ~TileEditorInterface() { _instance = NULL; _walkThroughBtn = NULL; }
    void CreateWindowList(std::string okText);
    QVector<std::string> m_AtlasLevels;
    QListWidget *m_ExportList;
    QCheckBox *_exportSelectUnselectAll;
    void CreateAtlasForLevelName(const std::string &name);
    void ShowColorPickWindow();
    QtProperty *UserString() { return m_UserString; }
    QSlider *HorizontalSlider() { return _animationSlider; }
    QtProperty *ColorPickButton() { return m_colorPickBtn; }

    bool CreateDotMode();
    static TileEditorInterface *Instance() { return _instance; }
    virtual void OnMessage(const std::string &msg);
    void Changes();
    void Saved();
    void UpdateTitle();
    void ShowPropWindow();
    void EnterNameDialog(EnterNameMode, std::string caption, std::string name);
    //void OnBoneMouseClickRight(Gwen::Event::Info info);

    void SaveAfterLevelNameChange();
    void ExitEditor();
    void OpenProjectCallback(const QString &fileName);
    const std::string &GetProjectFileName() const { return _projectFileName; }
    void InitPropertiesList();
    bool OnKeyPress(int iKey, bool bPress );
    void GenOxygineResourceFile(NameList &files);
    bool EmptyProperties() { return propertyToId.size() == 0; }
private:
    QDialog* m_CommonAtlasWindow;
    std::string _projectFileName;
    EnterNameMode _currentEnterNameMode;
    std::string _currentEnteredName;
    QDialog* _levelRenameWindow;
    QLineEdit* _levelName;
    QSlider *_animationSlider;
    bool _saved;

    QtProperty*     m_UserString;
    QtProperty*     m_BeautyText;
    QtProperty*     m_colorPickBtn;
    QtProperty*     m_textAlign;
    QtProperty*     m_UserFloat;
    QtProperty*     m_VertInterval;
    QtProperty*     m_HorSpacing;
    QtProperty*     m_UserPoint;
    QtProperty*     m_MovePos;
    QtProperty*     m_RotateAngle;
    QtProperty*     m_ScaleXY;
    QtProperty*     m_UnicOrLinked;

    QLabel *m_objectType;

    QtBoolPropertyManager *boolManager;
    QtIntPropertyManager *intManager;
    QtDoublePropertyManager *doubleManager;
    QtStringPropertyManager *stringManager;
    QtSizePropertyManager *sizeManager;
    QtRectPropertyManager *rectManager;
    QtSizePolicyPropertyManager *sizePolicyManager;
    QtEnumPropertyManager *enumManager;
    QtColorPropertyManager *colorManager;
    QtAbstractPropertyBrowser *editorProperties;

    bool _propertyUpdate;
    bool _firstUpdate;
public:
    QSettings settings;

};

#endif // MAINWINDOW_H
