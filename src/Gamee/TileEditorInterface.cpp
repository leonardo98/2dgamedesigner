#include <QApplication>
#include <QWindow>
#include <QTimer>
#include <QGridLayout>
#include <QFileDialog>
#include <QInputDialog>
#include <QDir>
#include <QDialog>
#include "TileEditorInterface.h"
#include "../Core/Math.h"
#include "../Core/Core.h"
#include "BeautyText.h"
#include "AnimationArea.h"
#include "GMessageBox.h"
#include "../Core/rapidxml/rapidxml_print.hpp"
#include "CreateProject.h"
#include <set>
#include <QDesktopServices>
#include <QTextEdit>

TileEditorInterface *TileEditorInterface::_instance = NULL;

TileEditorInterface::TileEditorInterface()
    : Messager("Interface")
    , _propertyUpdate(false)
    , settings("PakholkovLeonid", "GameDesigner")
    , _saveCopyToFile(NULL)
{

    Render::InitApplication();
    assert(_instance == NULL);
    _instance = this;
    _itemList = NULL;

    mainWindow = this;

    _tileEditor = new TileEditor( this );

    createMenus();
    _itemList = new CollectionControl;

    _propertiesDock = CreatePropertyBrowser();

    addDockWidget(Qt::RightDockWidgetArea, _propertiesDock);
 
    _listDock = new CustomDock();
    _listDock->setWidget(_itemList);
    _listDock->setObjectName("items");
    _listDock->setAllowedAreas(Qt::AllDockWidgetAreas);
    _listDock->setFeatures(CustomDock::DockWidgetMovable | CustomDock::DockWidgetFloatable
        | CustomDock::DockWidgetClosable);
    addDockWidget(Qt::RightDockWidgetArea, _listDock);

    setCentralWidget(_tileEditor);

    setWindowTitle(tr("2D Game Designer"));

    _statusBas = statusBar();

    UpdateProperties();

    restoreGeometry(settings.value("mainwindow_geometry").toByteArray());
    restoreState(settings.value("mainwindow_state").toByteArray());

    UpdateCheckboxes();
    connect(_propertiesDock, SIGNAL(onClose()), this, SLOT(PropertiesCheckbox()));
    connect(_listDock, SIGNAL(onClose()), this, SLOT(ResourcesCheckbox()));

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(Update()));
    timer->start(20);
    _firstUpdate = true;
}

void TileEditorInterface::Update()
{
    float dt = 0.02f;
    Core::Update(dt);
    if (_firstUpdate)
    {
        _firstUpdate = false;
        QString path = settings.value("lastproject", "").toString();
        if (path.size())
        {
            settings.setValue("lastproject", "");
            OpenProjectCallback(path);
        }
        ShowHelloWindow();
    }
    _tileEditor->Update(dt);
    _tileEditor->updateGL();
}

bool TileEditorInterface::CreateDotMode() {
    return _tileEditor->GetButtonsState() != TileEditor::bs_nothing
            && _tileEditor->GetButtonsState() != TileEditor::bs_standart;
}

void TileEditorInterface::Changes() {
    bool old = _saved;
    _saved = false;
    if (old != _saved)
    {
        UpdateTitle();
    }
}

void TileEditorInterface::Saved() {
    _saved = true;
    UpdateTitle();
}

void TileEditorInterface::UpdateTitle()
{
    std::string title;
    if (_saved) {
        title = "2D Game Designer by Pakholkov - " + _projectFileName + " : " + TileEditor::Instance()->GetCurrentLevel() + " - *saved*";
    } else {
        title = "2D Game Designer by Pakholkov - " + _projectFileName + " : " + TileEditor::Instance()->GetCurrentLevel() + " - *UNSAVED*";
    }
    setWindowTitle(tr(title.c_str()));
}

void TileEditorInterface::FastSave()
{
    if (TileEditor::Instance()->GetCurrentLevel() != "#not defined yet#")
    {
        TileEditor::Instance()->PreSaveLevel(TileEditor::Instance()->GetCurrentLevel());
    }
}

void TileEditorInterface::about()
{
    QMessageBox::about(this, tr("About 2D Game Designer"),
            tr("The <b>2D Game Designer</b> version 3.0a<br>by Pakholkov Leonid<br>Feel free to contact: am98pln@gmail.com"));
}

void TileEditorInterface::createMenus()
{
    {
        QMenu *workMenu = menuBar()->addMenu(tr("&Work"));

        QAction *aboutAct = new QAction(tr("&About"), this);
        connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));
        workMenu->addAction(aboutAct);

        workMenu->addSeparator();

        QAction *createProj = new QAction(tr("&Create project..."), this);
        connect(createProj, SIGNAL(triggered()), this, SLOT(CreateProjectFn()));
        workMenu->addAction(createProj);

        QAction *openProj = new QAction(tr("&Open project..."), this);
        connect(openProj, SIGNAL(triggered()), this, SLOT(OpenProject()));
        workMenu->addAction(openProj);

        workMenu->addSeparator();

        QAction *exitAct = new QAction(tr("E&xit"), this);
        exitAct->setShortcuts(QKeySequence::Quit);
        connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));
        workMenu->addAction(exitAct);
    }
    QAction *saveAsNewMenuItem;
    {
        QMenu *pRoot = menuBar()->addMenu(tr("&Edit"));

        QAction *undo = new QAction(tr("&Undo"), this);
        connect(undo, SIGNAL(triggered()), this, SLOT(Undo()));//Undo
        undo->setShortcuts(QKeySequence::Undo);
        pRoot->addAction(undo);


        QAction *redo = new QAction(tr("&Redo"), this);
        connect(redo, SIGNAL(triggered()), this, SLOT(Redo()));//Redo
        redo->setShortcuts(QKeySequence::Redo);
        pRoot->addAction(redo);

        pRoot->addSeparator();

        cutSelection = new QAction(tr("C&ut"), this);
        cutSelection->setShortcuts(QKeySequence::Cut);
        pRoot->addAction(cutSelection);

        copySelection = new QAction(tr("&Copy"), this);
        copySelection->setShortcuts(QKeySequence::Copy);
        pRoot->addAction(copySelection);

        pasteSelection = new QAction(tr("&Paste"), this);
        pasteSelection->setShortcuts(QKeySequence::Paste);
        pRoot->addAction(pasteSelection);

        pRoot->addSeparator();

        deleteSelection = new QAction(tr("&Delete"), this);
        deleteSelection->setShortcuts(QKeySequence::Delete);
        pRoot->addAction(deleteSelection);

    }

    {
        QMenu *pRoot = menuBar()->addMenu(tr("&View"));

        _propertiesCheckbox = new QAction(tr("&Properties"), this);
        connect(_propertiesCheckbox, SIGNAL(triggered()), this, SLOT(PropertiesCheckboxSlot()));
        _propertiesCheckbox->setCheckable(true);
        pRoot->addAction(_propertiesCheckbox);


        _resourcesCheckbox = new QAction(tr("&Resources"), this);
        connect(_resourcesCheckbox, SIGNAL(triggered()), this, SLOT(ResourcesCheckboxSlot()));
        _resourcesCheckbox->setCheckable(true);
        pRoot->addAction(_resourcesCheckbox);
    }

    {
        QMenu *levelMenu = menuBar()->addMenu(tr("&Level"));

        QAction *newtAct = new QAction(tr("&New"), this);
        connect(newtAct, SIGNAL(triggered()), this, SLOT(NewBtn()));//NewBtn
        newtAct->setShortcuts(QKeySequence::New);
        levelMenu->addAction(newtAct);

        QAction *openAct = new QAction(tr("&Open..."), this);
        openAct->setShortcuts(QKeySequence::Open);
        connect(openAct, SIGNAL(triggered()), this, SLOT(Load()));//Load
        levelMenu->addAction(openAct);

        QAction *saveAct = new QAction(tr("&Save..."), this);
        connect(saveAct, SIGNAL(triggered()), _tileEditor, SLOT(Save()));
        saveAct->setShortcuts(QKeySequence::Save);
        levelMenu->addAction(saveAct);

        levelMenu->addSeparator();

        saveAsNewMenuItem = new QAction(tr("Save &as..."), this);
        connect(saveAsNewMenuItem, SIGNAL(triggered()), _tileEditor, SLOT(SaveAsNew()));
        saveAsNewMenuItem->setShortcuts(QKeySequence::SaveAs);
        levelMenu->addAction(saveAsNewMenuItem);

        levelMenu->addSeparator();

        levelMenu->addAction(tr("Create link"), _tileEditor, SLOT(SaveAsComplex())
            , QKeySequence(tr("Ctrl+Shift+S")));

        levelMenu->addSeparator();

        QAction *renameAct = new QAction(tr("&Rename"), this);
        connect(renameAct, SIGNAL(triggered()), this, SLOT(RenameLevelDialog()));
        levelMenu->addAction(renameAct);

        levelMenu->addSeparator();

        QAction *dropAct = new QAction(tr("&Drop level"), this);
        connect(dropAct, SIGNAL(triggered()), this, SLOT(DropLevel()));
        levelMenu->addAction(dropAct);
    }

    {
        QMenu *pRoot = menuBar()->addMenu( tr("&Option") );
        pRoot->addAction(_netViewBtn = new QAction(tr("&Draw net"), this));
        _netViewBtn->setCheckable(true);
        _netViewBtn->setChecked((settings.value("netview", 1).toBool()));

        pRoot->addAction(_listTreeBtn = new QAction(tr("&Tree view"), this));
        _listTreeBtn->setCheckable(true);
        _listTreeBtn->setChecked((settings.value("listtree", 1).toBool()));
        connect(_listTreeBtn, SIGNAL(changed()), this, SLOT(ChangeListTree()));

        pRoot->addAction(_filteringTextureBtn = new QAction(tr("&Filtering"), this));
        _filteringTextureBtn->setCheckable(true);
        _filteringTextureBtn->setChecked((settings.value("filtering", 1).toBool()));

        pRoot->addAction(_viewportBtn = new QAction(tr("&Show frame"), this));
        _viewportBtn->setCheckable(true);
        _viewportBtn->setChecked((settings.value("viewport", 1).toBool()));

        pRoot->addSeparator();

        pRoot->addAction(_walkThroughBtn = new QAction(tr("&Load and show walkthrough"), this));
        _walkThroughBtn->setCheckable(true);
        _walkThroughBtn->setChecked(false);
        connect(_walkThroughBtn, SIGNAL(changed()), this, SLOT(ChangeWalkThrough()));

        {
            bool curValue = settings.value("selection_mode", false).toBool();
            TileEditor::Instance()->SetSelectionMode(curValue);

            QMenu *selection = new QMenu( tr("Selection mode"));
            pRoot->addMenu( selection );

            QActionGroup* group = new QActionGroup( this );

            QAction *item;

            selection->addAction(item = new QAction(tr("Intersection"), this));
            item->setCheckable(true);
            item->setChecked(!curValue);
            item->setActionGroup(group);
            item->setProperty("value", false);

            selection->addAction(item = new QAction(tr("Full object inside"), this));
            item->setCheckable(true);
            item->setChecked(curValue);
            item->setActionGroup(group);
            item->setProperty("value", true);

            connect(group, SIGNAL(triggered(QAction*)), this, SLOT(ChangeSelectionMode(QAction*)));

        }
        {
            QString s = settings.value("atlassize", 1024).toString()
                        + "x"
                        + settings.value("atlassize", 1024).toString();

            QMenu *atlas = new QMenu( tr("Set &atlas size"));
            pRoot->addMenu( atlas );

            QActionGroup* group = new QActionGroup( this );

            QAction *item;

            atlas->addAction(item = new QAction(tr("1024x1024"), this));
            item->setCheckable(true);
            item->setChecked(item->text() == s);
            item->setActionGroup(group);
            if (item->text() == s)
            {
                ChangeAtlasSize(item);
            }

            atlas->addAction(item = new QAction(tr("2048x2048"), this));
            item->setCheckable(true);
            item->setChecked(item->text() == s);
            item->setActionGroup(group);
            if (item->text() == s)
            {
                ChangeAtlasSize(item);
            }

            connect(group, SIGNAL(triggered(QAction*)), this, SLOT(ChangeAtlasSize(QAction*)));

        }
        {
            QString s = settings.value("viewportwidth", 1024).toString()
                        + "x"
                        + settings.value("viewportheight", 768).toString();


            QMenu *atlas = new QMenu( tr("Frame size"));
            pRoot->addMenu( atlas );

            QActionGroup* group = new QActionGroup( this );

            bool standart = false;
            QAction *item;
            atlas->addAction(item = new QAction(tr("800x600"), this));
            item->setCheckable(true);
            item->setChecked(item->text() == s);
            standart |= (item->text() == s);
            item->setActionGroup(group);

            atlas->addAction(item = new QAction(tr("960x640"), this));
            item->setCheckable(true);
            item->setChecked(item->text() == s);
            standart |= (item->text() == s);
            item->setActionGroup(group);

            atlas->addAction(item = new QAction(tr("1024x768"), this));
            item->setCheckable(true);
            item->setChecked(item->text() == s);
            standart |= (item->text() == s);
            item->setActionGroup(group);

            atlas->addAction(item = new QAction(tr("1136x640"), this));
            item->setCheckable(true);
            item->setChecked(item->text() == s);
            standart |= (item->text() == s);
            item->setActionGroup(group);

            atlas->addAction(item = new QAction(tr("custom size ..."), this));
            item->setCheckable(true);
            item->setActionGroup(group);
            item->setChecked(!standart);

            connect(group, SIGNAL(triggered(QAction*)), this, SLOT(ChangeViewPortSize(QAction*)));

       }
        pRoot->addSeparator();

        QAction *tmp;
        pRoot->addAction(tmp = new QAction(tr("&Project options..."), this));
        connect(tmp, SIGNAL(triggered()), this, SLOT(EditorOptions()));

        pRoot->addSeparator();

        pRoot->addAction(_blendingTextureBtn = new QAction(tr("&Blending add"), this));
        _blendingTextureBtn->setCheckable(true);
        _blendingTextureBtn->setChecked(false);
        _blendingTextureBtn->setShortcut(Qt::CTRL + Qt::Key_B);

        _saveCopyToFile = NULL;
//        pRoot->addAction(_saveCopyToFile = new QAction(tr("&Save level copy to file"), this));
//        _saveCopyToFile->setCheckable(true);
//        _saveCopyToFile->setChecked(settings.value("save_copy_to_file", 0).toBool());

        QAction *dropAct = new QAction(tr("Resave all levels(Debug)"), this);
        connect(dropAct, SIGNAL(triggered()), this, SLOT(ResaveAllLevels()));
        pRoot->addAction(dropAct);

    }

    {
        QMenu *pRoot = menuBar()->addMenu( tr("&Build") );

        QAction *tmp;

        tmp = new QAction(tr("&Run game"), this);
        tmp->setShortcut(Qt::CTRL + Qt::Key_W);
        pRoot->addAction(tmp);
        connect(tmp, SIGNAL(triggered()), this, SLOT(StartGameFunction()));

        pRoot->addSeparator();

        pRoot->addAction(tmp = new QAction(tr("&Create common atlas..."), this));
        connect(tmp, SIGNAL(triggered()), this, SLOT(ShowChooseLevelsWindow()));

//        pRoot->addSeparator();

        pRoot->addAction(tmp = new QAction(tr("&Create atlas for level..."), this));
        connect(tmp, SIGNAL(triggered()), this, SLOT(CreateAtlasForLevel()));
    }

    connect(deleteSelection, SIGNAL(triggered()), TileEditor::Instance(), SLOT(DeletePressed()));
    connect(cutSelection, SIGNAL(triggered()), TileEditor::Instance(), SLOT(CutSelection()));
    connect(copySelection, SIGNAL(triggered()), TileEditor::Instance(), SLOT(CopySelection()));
    connect(pasteSelection, SIGNAL(triggered()), TileEditor::Instance(), SLOT(PasteSelection()));
    //dublicate->SetAction( TileEditor::Instance(), &TileEditor::BeautyDublicate);
    //selectionToComplex->SetAction( TileEditor::Instance(), &TileEditor::SelectionToComplex);
    //cutSelection->SetAction( TileEditor::Instance(), &TileEditor::CutSelection );
    //copySelection->SetAction( TileEditor::Instance(), &TileEditor::CopySelection );
    //pasteSelection->SetAction( TileEditor::Instance(), &TileEditor::PasteSelection );
    qApp->installEventFilter(this);
}

void TileEditorInterface::ChangeSelectionMode(QAction *action)
{
    bool value = action->property("value").toBool();
    TileEditor::Instance()->SetSelectionMode(value);
    settings.setValue("selection_mode", value);
}

bool TileEditorInterface::eventFilter(QObject *, QEvent *event)
{
    if (event->type() == QEvent::MouseMove)
    {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        if (TileEditor::Instance())
        {
            TileEditor::Instance()->mouseMoveEvent(mouseEvent);
        }
    }
    return false;
}

QSize TileEditorInterface::getSize()
{
    bool ok;
    QString text = QInputDialog::getText(this, tr("Grabber"),
                                         tr("Enter pixmap size:"),
                                         QLineEdit::Normal,
                                         tr("%1 x %2").arg(_tileEditor->width())
                                                      .arg(_tileEditor->height()),
                                         &ok);
    if (!ok)
        return QSize();

    QRegExp regExp(tr("([0-9]+) *x *([0-9]+)"));
    if (regExp.exactMatch(text)) {
        int width = regExp.cap(1).toInt();
        int height = regExp.cap(2).toInt();
        if (width > 0 && width < 2048 && height > 0 && height < 2048)
            return QSize(width, height);
    }

    return _tileEditor->size();
}

void TileEditorInterface::CreateProjectFn()
{
    CreateProject *win = new CreateProject(this);
}

void TileEditorInterface::OpenProject()
{
//#ifdef WIN32
//    QString fileName = "C:/Dropbox/Projects/DreamGame/Game/mainlevels.xml";
//#else
    // на win8 вызов QFileDialog::getOpenFileName часто приводит к крешу
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("Files (*.*)"));
//#endif

    if (fileName.size())
    {
        OpenProjectCallback(fileName);
    }
}

CollectionControl *TileEditorInterface::GetCollectionControl() {
    return  _itemList;
}

QAction *TileEditorInterface::GetWalkThroughBtn()
{
    return _walkThroughBtn;
}

bool TileEditorInterface::SaveCopyToFile()
{
    return (_saveCopyToFile && _saveCopyToFile->isChecked()) || !Core::storeDir.empty();
}

void TileEditorInterface::OnMessage(const std::string &message) {
    std::string msg;
    if (CanCut(message, "add ", msg)) {
        _customListBox->addItem( msg.c_str() );
    } else if (CanCut(message, "select ", msg)) {
//        for (Gwen::ControlList::List::iterator i = _customListBox->GetChildren().begin(), e = _customListBox->GetChildren().end(); i != e; ++i) {
//            if ((*i)->GetName() == msg) {
//                Gwen::Controls::Layout::TableRow *row = gwen_cast<Gwen::Controls::Layout::TableRow>(*i);
//                if (row) {
//                    _customListBox->SetSelectedRow(row);
//                }
//            }
//        }
    } else if (CanCut(message, "prefix ", msg)) {
        _messagePrefix = msg;
    } else {
        assert(false);
    }
}

void TileEditorInterface::OpenProjectCallback(const QString &fileNameInput)
{
    TileEditor::Instance()->DropCopyPastBuffer();
    TileEditor::Instance()->Clear();
    _projectFileName = fileNameInput.toStdString();
    std::string path = Math::CutFileName(_projectFileName);
    std::string fileName = _projectFileName.substr(path.size() + 1);
    QDir::setCurrent(path.c_str());
    if (Core::InitWithXml(fileName.c_str()))
    {
        settings.setValue("lastproject", _projectFileName.c_str());
    }
//    TileEditor::Instance()->LoadLevelSet();
//    Messager::SendMsg("tile_editor", "load templates");
    UpdateTitle();
}

void TileEditorInterface::Load() {
    CreateWindowList("load");
    _tileEditor->OnMessage("open");
    _customListBox->sortItems();
}

void TileEditorInterface::CreateWindowList(std::string okText)
{
    QDialog* pWindow = _listWindow = new QDialog( this );
    pWindow->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    pWindow->setAttribute(Qt::WA_DeleteOnClose, true);
    pWindow->setModal(true);
    pWindow->setWindowTitle( okText.c_str() );
    pWindow->setWindowFlags( Qt::Dialog | Qt::WindowTitleHint );
    int width = 300;
    int height = this->height() / 3 * 2;
    pWindow->setFixedSize( width, height );

    QLineEdit *line = _levelOpenName = new QLineEdit( pWindow );
    line->move(0, 0);
    line->resize(width, 20);
    connect(line, SIGNAL(textChanged(const QString &)), this, SLOT(OpenLevelChangeMask(const QString &)));

    QListWidget *list = _customListBox = new QListWidget( pWindow );
    list->setSelectionMode( QAbstractItemView::SingleSelection );
    list->move(0, 20);
    list->resize(width, height - 50 - 20);

    QPushButton *button = new QPushButton( pWindow );
    button->setText( okText.c_str() );
    button->resize( 60, 30 );
    button->move( width / 2 - 30 - 100, height - 40 );
    connect(button, SIGNAL(pressed()), this, SLOT(ListWindowOk()));

    button = new QPushButton( pWindow );
    button->setText( "cancel" );
    button->resize( 60, 30 );
    button->move( width / 2 - 30 + 100, height - 40 );
    connect(button, SIGNAL(pressed()), pWindow, SLOT(close()));

    pWindow->show();
}

void TileEditorInterface::OpenLevelChangeMask(const QString & text)
{
    for (unsigned int i = 0; i < _customListBox->count(); ++i)
    {
        QListWidgetItem *row = _customListBox->item(i);
        row->setHidden(!row->text().contains(text, Qt::CaseInsensitive));
        if (row->isSelected() && row->isHidden())
        {
            row->setSelected(false);
            _customListBox->setCurrentRow(-1);
        }
    }
}

void TileEditorInterface::ListWindowOk()
{
    QListWidgetItem *row = _customListBox->currentItem();
    if (row) {
        SendMsg("tile_editor", _messagePrefix + row->text().toStdString());

        _listWindow->hide();
        _listWindow->deleteLater();
        _listWindow = NULL;
        _customListBox = NULL;
        _levelOpenName = NULL;
    } else {
        LOG("Select level id!");
    }
}

void TileEditorInterface::NewBtn() {
    Messager::SendMsg("tile_editor", "new");
}

void TileEditorInterface::DropLevel()
{
    std::string name = TileEditor::Instance()->GetCurrentLevel();
    if (GMessageBoxYesNoShow(("Remove level " + name + " forever?").c_str()) == QMessageBox::Yes)
    {
        TileEditor::Instance()->NewLevelYes();
        TileEditor::Instance()->DeleteLevel(name);
    }
}

void TileEditorInterface::Undo()
{
    TileEditor::ButtonsState bs = _tileEditor->GetButtonsState();
    QVector<int> selected;
    _tileEditor->_level = *_tileEditor->redoUndoManager.Undo(selected);
    _tileEditor->ClearSelectionList();
    _tileEditor->_currents.beautyUnderCursor = NULL;

    for (QVector<int>::size_type i = 0; i < selected.size(); ++i) {
        _tileEditor->PushBeautyToSelectionList(_tileEditor->_level.beauties[selected[i]]);
    }
    if (TileEditor::Instance()->Selection().size() == 1)
    {
        _tileEditor->SetButtonsState(bs);
    }
    else
    {
        _tileEditor->SetButtonsState(TileEditor::bs_nothing);
    }
}

void TileEditorInterface::Redo()
{
    TileEditor::ButtonsState bs = _tileEditor->GetButtonsState();
    QVector<int> selected;
    _tileEditor->_level = *(_tileEditor->redoUndoManager.Redo(selected));
    _tileEditor->ClearSelectionList();
    _tileEditor->_currents.beautyUnderCursor = NULL;

    for (QVector<int>::size_type i = 0; i < selected.size(); ++i) {
        _tileEditor->PushBeautyToSelectionList(_tileEditor->_level.beauties[selected[i]]);
    }
    if (TileEditor::Instance()->Selection().size() == 1)
    {
        _tileEditor->SetButtonsState(bs);
    }
    else
    {
        _tileEditor->SetButtonsState(TileEditor::bs_nothing);
    }
}

void TileEditorInterface::TestAnimation() {
    Messager::SendMsg("tile_editor", "test");
}

void TileEditorInterface::EnterNameDialog(EnterNameMode mode, std::string caption, std::string name)
{
    _currentEnterNameMode = mode;

    QDialog* pWindow = _levelRenameWindow = new QDialog( this );
    pWindow->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    pWindow->setAttribute(Qt::WA_DeleteOnClose, true);
    pWindow->setModal(true);
    if (mode == mode_run_command)
    {
        pWindow->setWindowTitle( "Run game command" );
    }
    else
    {
        pWindow->setWindowTitle( "Level name" );
    }
    pWindow->setWindowFlags( Qt::Dialog | Qt::WindowTitleHint );
    int width = 250;
    pWindow->setFixedSize( width, 110 );

    QLabel* text = new QLabel( caption.c_str(), pWindow );
    text->resize(width, 20);
    text->move( 0, 5 );
    text->setAlignment(Qt::AlignCenter | Qt::AlignHCenter);

    QLineEdit *textBox = _levelName = new QLineEdit( pWindow );
    textBox->move((width - 150) / 2, 30);
    textBox->resize(150, 20);
    _levelName->setText( name.c_str() );

    QPushButton *button = new QPushButton( pWindow );
    button->setText( "ok" );
    button->resize( 60, 30 );
    button->move(  width / 2 - 60 - 40, 65 );
    connect(button, SIGNAL(pressed()), this, SLOT(EnterNameDialogOk()));

    button = new QPushButton( pWindow );
    button->setText( "cancel" );
    button->resize( 60, 30 );
    button->move( width / 2 + 40, 65 );
    connect(button, SIGNAL(pressed()), pWindow, SLOT(close()));

    pWindow->show();
}

void TileEditorInterface::EnterNameDialogOk() {
    if (_levelName->text() == NOT_DEFINED_NAME)
    {
        return;
    }
    std::string enteredText = _levelName->text().toStdString();
    _levelRenameWindow->hide();
    _levelRenameWindow->deleteLater();
    _levelRenameWindow = NULL;

    if (_currentEnterNameMode == mode_save_as
            || _currentEnterNameMode == mode_rename)
    {
        _currentEnteredName = enteredText;
    }

    if (_currentEnterNameMode == mode_rename || _currentEnterNameMode == mode_save_as)
    {
        if (TileEditor::Instance()->IsLevelExist(_currentEnteredName ))
        {
            if (GMessageBoxYesNoShow((std::string("Level ") + _currentEnteredName + " already exist. Overwrite?").c_str()) == QMessageBox::Yes)
            {
                SaveAfterLevelNameChange();
            }
        }
        else
        {
            SaveAfterLevelNameChange();
        }
    }
    else if (_currentEnterNameMode == mode_level_atlas_name)
    {
        CreateAtlasForLevelName(enteredText);
    }
    else if (_currentEnterNameMode == mode_run_command)
    {
        Core::pathToGame = enteredText;
    }
}

void TileEditorInterface::RenameLevelDialog()
{
    if (TileEditor::Instance()->GetCurrentLevel() == NOT_DEFINED_NAME)
    {
        GMessageBoxShow("Save level first");
        return;
    }
    EnterNameDialog(mode_rename, "Enter name of level", TileEditor::Instance()->GetCurrentLevel());
}

void TileEditorInterface::SaveAfterLevelNameChange()
{
    if (_currentEnterNameMode == mode_rename)
    {
        TileEditor::Instance()->DeleteLevel(_currentEnteredName);
        TileEditor::Instance()->Rename(_currentEnteredName);
        UpdateTitle();
    }
    else if (_currentEnterNameMode == mode_save_as)
    {
        TileEditor::Instance()->DeleteLevel(_currentEnteredName);
        TileEditor::Instance()->PreSaveLevel(_currentEnteredName);
        UpdateTitle();
    }
    else
    {
        assert(false);
    }
}

void TileEditorInterface::CreateAtlasForLevel()
{
    TileEditorInterface::Instance()->EnterNameDialog(mode_level_atlas_name, "Enter name of atlas", TileEditor::Instance()->GetCurrentLevel());
}

void TileEditorInterface::EditorOptions()
{
    QDialog* pWindow = new QDialog( this );

    pWindow->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    pWindow->setAttribute(Qt::WA_DeleteOnClose, true);
    pWindow->setModal(true);
    pWindow->setWindowTitle( "Project options" );
    pWindow->setWindowFlags( Qt::Dialog | Qt::WindowTitleHint );
    int width = 300;
    int height = 220;
    pWindow->setFixedSize( width, height );

    int textWidth = 110;
    QLabel *label = new QLabel( "File for levels", pWindow );
    label->move(10, 14);
    QLineEdit *text = _storeXMLText = new QLineEdit( pWindow );
    text->move(textWidth, 10);
    text->resize(width - textWidth - 10, 20);
    text->setText(Core::storeXML.c_str());

    label = new QLabel( "Folder for levels", pWindow );
    label->move(10, 44);
    text = _storeDirText = new QLineEdit( pWindow );
    text->move(textWidth, 40);
    text->resize(width - textWidth - 10, 20);
    text->setText(Core::storeDir.c_str());

    label = new QLabel( "Directory for levels", pWindow );
    label->move(10, 74);
    text = _atlasNameText = new QLineEdit( pWindow );
    text->move(textWidth, 70);
    text->resize(width - textWidth - 10, 20);
    text->setText(Core::atlasName.c_str());

    label = new QLabel( "Working directory", pWindow );
    label->move(10, 104);
    text = _workDirText = new QLineEdit( pWindow );
    text->move(textWidth, 100);
    text->resize(width - textWidth - 10, 20);
    text->setText(Core::workDir.c_str());

    label = new QLabel( "Run command", pWindow );
    label->move(10, 134);
    text = _pathToGameText = new QLineEdit( pWindow );
    text->move(textWidth, 130);
    text->resize(width - textWidth - 10, 20);
    text->setText(Core::pathToGame.c_str());

    QPushButton *button = new QPushButton( pWindow );
    button->setText( "ok" );
    button->resize( 80, 30 );
    button->move( width / 2 - 40 - 100, height - 40 );
    connect(button, SIGNAL(pressed()), this, SLOT(ChangeEditorOptions()));
    connect(button, SIGNAL(pressed()), pWindow, SLOT(close()));

    button = new QPushButton( pWindow );
    button->setText( "cancel" );
    button->resize( 80, 30 );
    button->move( width / 2 - 40 + 100, height - 40 );
    connect(button, SIGNAL(pressed()), pWindow, SLOT(close()));

    pWindow->show();
}

void TileEditorInterface::CreateAtlasForLevelName(const std::string &name)
{
    Agregator::Reset();
    TileEditor::Instance()->PushTexturesToAtlas();
    Agregator::Process(Core::workDir + name);
}

void TileEditorInterface::ChangeAtlasSize(QAction *a)
{
    int w, h;
    if (sscanf(a->text().toStdString().c_str(), "%ix%i", &w, &h) == 2 && w == h)
    {
        settings.setValue("atlassize", w);
        Agregator::SetMaxAtlasSize(w);
    }
}

void TileEditorInterface::ChangeWalkThrough()
{
    if (!_walkThroughBtn->isChecked())
    {
        TileEditor::Instance()->TurnOffWalkThrough();
    }
    else
    {
        if (!TileEditor::Instance()->TryLoadWalkThrough())
        {
            _walkThroughBtn->setChecked(false);
        }
    }
}

void TileEditorInterface::StartGameFunction()
{
    std::string command = Core::pathToGame;// Resource_MakePath( IniFile::GetString("system", "gameStartCommand", "") );
    if (command.size() && TileEditor::Instance()->GetCurrentLevel() != "#not defined yet#")
    {
        char buff[1000];
        TileEditor::Instance()->PreSaveLevel(TileEditor::Instance()->GetCurrentLevel());
        sprintf(buff, command.c_str(), TileEditor::Instance()->GetCurrentLevel().c_str());
        int result = system(buff);
        if (result)
        {
            LOG(Math::IntToStr(result))
        }
    }
    else
    {
        GMessageBoxShow("Load any level to start!");
    }
}

void TileEditorInterface::ShowChooseLevelsWindow()
{
    QDialog* pWindow = m_CommonAtlasWindow = new QDialog( this );
    pWindow->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    pWindow->setAttribute(Qt::WA_DeleteOnClose, true);
    pWindow->setModal(true);
    pWindow->setWindowTitle( "Choose levels to pack" );
    pWindow->setWindowFlags( Qt::Dialog | Qt::WindowTitleHint );
    int width = 300;
    int height = 400;//GetCanvas()getSize()->Height() / 3 * 2;
    pWindow->setFixedSize( width, height );

    m_ExportList = new QListWidget( pWindow );
    m_ExportList->setSelectionMode( QAbstractItemView::SingleSelection );
    m_ExportList->move(0, 0);
    m_ExportList->resize(width, height - 80);//-100

    QCheckBox *checkBox = _exportSelectUnselectAll = new QCheckBox( pWindow );
    connect(_exportSelectUnselectAll, SIGNAL(clicked()), this, SLOT(ExportSelectUnselectAll()));
    checkBox->move( 0, height - 70 );//-90
    checkBox->setText("Select/unselect all");

    QPushButton *button = new QPushButton( pWindow );
    button->setText( "export" );
    button->resize( 50, 30 );
    button->move( width / 2 - 30 + 70, height - 40 );
    connect(button, SIGNAL(pressed()), this, SLOT(CreateCommonAtlas()));

    button = new QPushButton( pWindow );
    button->setText( "cancel" );
    button->resize( 50, 30 );
    button->move( width / 2 - 30 + 130, height - 40 );
    connect(button, SIGNAL(pressed()), pWindow, SLOT(close()));

    button = new QPushButton( pWindow );
    button->setText( "load preset" );
    button->resize( 70, 30 );
    button->move( width / 2 - 30 - 120, height - 40 );
    connect(button, SIGNAL(pressed()), this, SLOT(LoadSelectionPreset()));

    button = new QPushButton( pWindow );
    button->setText( "save preset" );
    button->resize( 70, 30 );
    button->move( width / 2 - 30 - 40, height - 40 );
    connect(button, SIGNAL(pressed()), this, SLOT(SaveSelectionPreset()));

    pWindow->show();

    m_ExportList->clear();

	TileEditor::Instance()->CreateLevelList(m_AtlasLevels);

    std::sort(m_AtlasLevels.begin(), m_AtlasLevels.end());

	for (unsigned int i = 0; i < m_AtlasLevels.size(); ++i) 
	{
        QListWidgetItem* item = new QListWidgetItem(m_AtlasLevels[i].c_str(), m_ExportList);
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable); // set checkable flag
        item->setCheckState(Qt::Unchecked); // AND initialize check state
	}
}

void TileEditorInterface::ExportSelectUnselectAll()
{
    Qt::CheckState value = _exportSelectUnselectAll->isChecked() ? Qt::Checked : Qt::Unchecked;
    for(int i = 0; i < m_ExportList->count(); ++i)
    {
        m_ExportList->item(i)->setCheckState(value);
	}
}

void TileEditorInterface::CreateCommonAtlas()
{
	unsigned int counter = 0;
    for(int i = 0; i < m_ExportList->count(); ++i)
    {
        bool skip = m_ExportList->item(i)->checkState() == Qt::Unchecked;
        if (skip)
        {
            m_AtlasLevels.erase(m_AtlasLevels.begin() + counter);
        }
        else
        {
            counter++;
        }
    }
    m_CommonAtlasWindow->close();

	Agregator::Reset();
	TileEditor::Instance()->PushLevelsTexturesToAtlas(m_AtlasLevels);

//    Agregator::Process(Core::Resource_MakePath(Core::atlasName.c_str()));

    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Atlas"), "", tr("Files (*.*)"));
    if (fileName.size())
    {
        Agregator::Process(Core::Resource_MakePath(fileName.toStdString().c_str()));
    }
    else
    {
        Agregator::Reset();
    }
}

void TileEditorInterface::GenOxygineResourceFile(NameList &files)
{
    QList<std::string> atlas;
    QList<std::string> other;
    for (NameList::iterator i = files.begin(), e = files.end(); i != e; ++i) {
        DataContainer *dc = new DataContainer(i->first.c_str());
        if (dc->failed == false && dc->textureWidth < 1024
            && ((dc->fileWidth | (dc->fileWidth - 1)) != (dc->fileWidth * 2 - 1)
                || (dc->fileHeight | (dc->fileHeight - 1)) != (dc->fileHeight * 2 - 1)
                )//
            )
        {
            atlas.push_back(i->first);
        }
        else
        {
            other.push_back(i->first);
        }
        delete dc;
    }

    rapidxml::xml_document<> doc;

    {
        rapidxml::xml_node<>* decl = doc.allocate_node(rapidxml::node_declaration);
        decl->append_attribute(doc.allocate_attribute("version", "1.0"));
        decl->append_attribute(doc.allocate_attribute("encoding", "utf-8"));
        decl->append_attribute(doc.allocate_attribute("standalone", "no"));
        doc.append_node(decl);
    }
    rapidxml::xml_node<> *root = doc.allocate_node(rapidxml::node_element, "resources");
    doc.append_node(root);

    for (QList<std::string>::iterator i = other.begin(), e = other.end(); i != e; ++i)
    {
        rapidxml::xml_node<> *atlasXml = doc.allocate_node(rapidxml::node_element, "atlas");
        root->append_node(atlasXml);
        rapidxml::xml_node<> *sprite = doc.allocate_node(rapidxml::node_element, "image");
        atlasXml->append_node(sprite);
        char *copyName = doc.allocate_string((*i).c_str());
        sprite->append_attribute(doc.allocate_attribute("id", copyName));
        sprite->append_attribute(doc.allocate_attribute("file", copyName));
    }

    // Save to file
    std::ofstream file_stored(Core::Resource_MakePath((Core::atlasName + ".xml").c_str()));
    file_stored << doc;
    file_stored.close();
    doc.clear();
}

void TileEditorInterface::ResaveAllLevels()
{
    if (GMessageBoxYesNoShow("It will resave all levels! Are you sure?") == QMessageBox::Yes)
    {
        TileEditor::Instance()->ResaveAllLevels();
    }
}

CustomDock *TileEditorInterface::CreatePropertyBrowser()
{
    QWidget *w = new QWidget();
    w->setWindowTitle("properties");

    boolManager = new QtBoolPropertyManager(w);
    intManager = new QtIntPropertyManager(w);
    doubleManager = new QtDoublePropertyManager(w);
    stringManager = new QtStringPropertyManager(w);
    sizeManager = new QtSizePropertyManager(w);
    rectManager = new QtRectPropertyManager(w);
    sizePolicyManager = new QtSizePolicyPropertyManager(w);
    enumManager = new QtEnumPropertyManager(w);
    colorManager = new QtColorPropertyManager(w);

    connect(stringManager, SIGNAL(valueChanged(QtProperty *, const QString &)),
                this, SLOT(valueChanged(QtProperty *, const QString &)));
    connect(colorManager, SIGNAL(valueChanged(QtProperty *, const QColor &)),
                this, SLOT(valueChanged(QtProperty *, const QColor &)));
    connect(doubleManager, SIGNAL(valueChanged(QtProperty *, double)),
                this, SLOT(valueChanged(QtProperty *, double)));

    connect(enumManager, SIGNAL(valueChanged(QtProperty *, int)),
                this, SLOT(valueChanged(QtProperty *, int)));

    //QtGroupPropertyManager *groupManager = new QtGroupPropertyManager(w);
    //QtVariantPropertyManager *variantManager = new QtVariantPropertyManager(w);

//    QtProperty *item7 = boolManager->addProperty("mouseTracking");
//    item0->addSubProperty(item7);

//    QtProperty *item8 = enumManager->addProperty("direction");
//    QStringList enumNames;
//    enumNames << "Up" << "Right" << "Down" << "Left";
//    enumManager->setEnumNames(item8, enumNames);
//    QMap<int, QIcon> enumIcons;
//    enumIcons[0] = QIcon(":/demo/images/up.png");
//    enumIcons[1] = QIcon(":/demo/images/right.png");
//    enumIcons[2] = QIcon(":/demo/images/down.png");
//    enumIcons[3] = QIcon(":/demo/images/left.png");
//    enumManager->setEnumIcons(item8, enumIcons);
//    item0->addSubProperty(item8);

//    QtProperty *item9 = intManager->addProperty("value");
//    intManager->setRange(item9, -100, 100);
//    item0->addSubProperty(item9);

    QtCheckBoxFactory *checkBoxFactory = new QtCheckBoxFactory(w);
    QtSpinBoxFactory *spinBoxFactory = new QtSpinBoxFactory(w);
    QtDoubleSpinBoxFactory *doubleSpinBoxFactory = new QtDoubleSpinBoxFactory(this);
    QtLineEditFactory *lineEditFactory = new QtLineEditFactory(w);
    QtEnumEditorFactory *comboBoxFactory = new QtEnumEditorFactory(w);
    //QtVariantEditorFactory *variantFactory = new QtVariantEditorFactory(w);
    QtColorEditorFactory *colorFactory = new QtColorEditorFactory(w);

    editorProperties = new QtTreePropertyBrowser();
    //editorProperties->setFactoryForManager(variantManager, variantFactory);
    editorProperties->setFactoryForManager(boolManager, checkBoxFactory);
    editorProperties->setFactoryForManager(colorManager, colorFactory);
    editorProperties->setFactoryForManager(doubleManager, doubleSpinBoxFactory);
    editorProperties->setFactoryForManager(intManager, spinBoxFactory);
    editorProperties->setFactoryForManager(stringManager, lineEditFactory);
    editorProperties->setFactoryForManager(sizeManager->subIntPropertyManager(), spinBoxFactory);
    editorProperties->setFactoryForManager(rectManager->subIntPropertyManager(), spinBoxFactory);
    editorProperties->setFactoryForManager(sizePolicyManager->subIntPropertyManager(), spinBoxFactory);
    editorProperties->setFactoryForManager(sizePolicyManager->subEnumPropertyManager(), comboBoxFactory);
    editorProperties->setFactoryForManager(enumManager, comboBoxFactory);

    QGridLayout *layout = new QGridLayout(w);
    m_objectType = new QLabel("Select an object to see it's properies");
    m_objectType->setAlignment(Qt::AlignCenter);
    m_objectType->setMargin(4);
    m_objectType->setFrameShadow(QFrame::Sunken);
    m_objectType->setFrameShape(QFrame::Panel);

    _animationSlider = new QSlider(Qt::Horizontal, this);
    _animationSlider->setRange(0, 100);
    connect(_animationSlider, SIGNAL(valueChanged(int)), this, SLOT(OnSliderChange()));

    layout->addWidget(m_objectType,     0, 0, 1, 3);
    layout->addWidget(_animationSlider, 1, 2);
    layout->addWidget(editorProperties, 2, 0, 1, 3);

    layout->setMargin(0);

    m_objectType->setText("Select an object to see it's properies");
    _animationSlider->hide();
//    _tileEditor->GetPlayBtn()->Visible() = false;
//    _tileEditor->GetUngroupBtn()->Visible() = false;

    CustomDock *dw = new CustomDock();
    dw->setWidget(w);
    dw->setObjectName("properties");
    dw->setAllowedAreas(Qt::AllDockWidgetAreas);
    dw->setFeatures(CustomDock::DockWidgetMovable | CustomDock::DockWidgetFloatable
        | CustomDock::DockWidgetClosable);

    return dw;
}

void TileEditorInterface::valueChanged(QtProperty *property, const QString &value)
{
    if (_propertyUpdate
            || !propertyToId.contains(property)
            || !TileEditor::Instance()->IsSelection())
    {
        return;
    }

    QString id = propertyToId[property];
    if (id == QLatin1String("UserString"))
    {
        for (BeautyList::iterator i = TileEditor::Instance()->Selection().begin()
             ; i != TileEditor::Instance()->Selection().end()
             ; ++i)
        {
            (*i)->UserString() = value.toStdString();
        }
    }
    else if (id == QLatin1String("BeautyText"))
    {
        for (BeautyList::iterator i = TileEditor::Instance()->Selection().begin()
             ; i != TileEditor::Instance()->Selection().end()
             ; ++i)
        {
            static_cast<BeautyText *>(*i)->SetText(value.toStdString());
        }
    }
    else if (id == QLatin1String("UserPoint"))
    {
        float x, y;
        bool pos = sscanf(value.toStdString().c_str(), "%f; %f", &x, &y) == 2;
        if(!pos)
        {
            return;
        }
        for (BeautyList::iterator i = TileEditor::Instance()->Selection().begin()
             ; i != TileEditor::Instance()->Selection().end()
             ; ++i)
        {
            (*i)->PointData().x = x;
            (*i)->PointData().y = y;
        }
    }
    else if (id == QLatin1String("Pos"))
    {
        float x, y;
        bool pos = sscanf(value.toStdString().c_str(), "%f; %f", &x, &y) == 2;
        if(!pos)
        {
            return;
        }
        for (BeautyList::iterator i = TileEditor::Instance()->Selection().begin()
             ; i != TileEditor::Instance()->Selection().end()
             ; ++i)
        {
            (*i)->SetPosition(x, y);
        }
    }
    else if (id == QLatin1String("Scale"))
    {
        float x, y;
        bool pos = sscanf(value.toStdString().c_str(), "%f; %f", &x, &y) == 2;
        if(!pos)
        {
            return;
        }
        for (BeautyList::iterator i = TileEditor::Instance()->Selection().begin()
             ; i != TileEditor::Instance()->Selection().end()
             ; ++i)
        {
            (*i)->SetScale(x, y);
        }
    }
    TileEditor::Instance()->PushCopyToRedoUndoManager();
}

void TileEditorInterface::valueChanged(QtProperty *property, const QColor &value)
{
    if (_propertyUpdate
            || !propertyToId.contains(property)
            || !TileEditor::Instance()->IsSelection())
    {
        return;
    }

    QString id = propertyToId[property];
    if (id == QLatin1String("Color"))
    {
        DWORD a = value.alpha();
        DWORD r = value.red();
        DWORD g = value.green();
        DWORD b = value.blue();

        DWORD color = a << 24 | r << 16 | g << 8 | b;
        for (BeautyList::iterator i = TileEditor::Instance()->Selection().begin()
             ; i != TileEditor::Instance()->Selection().end()
             ; ++i)
        {
            (*i)->SetColor(color);
        }
    }
    TileEditor::Instance()->PushCopyToRedoUndoManager();

}

void TileEditorInterface::valueChanged(QtProperty *property, int value)
{
    if (_propertyUpdate
            || !propertyToId.contains(property)
            || !TileEditor::Instance()->IsSelection())
    {
        return;
    }

    QString id = propertyToId[property];
    if (id == QLatin1String("TextAlign"))
    {
        for (BeautyList::iterator i = TileEditor::Instance()->Selection().begin()
             ; i != TileEditor::Instance()->Selection().end()
             ; ++i)
        {
            if (value == 0)
                (*i)->Command("align:0");
            else if (value == 1)
                (*i)->Command("align:-1");
            else if (value == 2)
                (*i)->Command("align:1");
        }
    }
    TileEditor::Instance()->PushCopyToRedoUndoManager();
}

void TileEditorInterface::valueChanged(QtProperty *property, double value)
{
    if (_propertyUpdate
            || !propertyToId.contains(property)
            || !TileEditor::Instance()->IsSelection())
    {
        return;
    }

    QString id = propertyToId[property];
    if (id == QLatin1String("UserFloat"))
    {
        for (BeautyList::iterator i = TileEditor::Instance()->Selection().begin()
             ; i != TileEditor::Instance()->Selection().end()
             ; ++i)
        {
            (*i)->FloatData() = (float)value;
        }
    }
    else if (id == QLatin1String("VertInterval"))
    {
        for (BeautyList::iterator i = TileEditor::Instance()->Selection().begin()
             ; i != TileEditor::Instance()->Selection().end()
             ; ++i)
        {
            static_cast<BeautyText *>(*i)->SetVertInterval(value);
        }
    }
    else if (id == QLatin1String("HorSpacing"))
    {
        for (BeautyList::iterator i = TileEditor::Instance()->Selection().begin()
             ; i != TileEditor::Instance()->Selection().end()
             ; ++i)
        {
            static_cast<BeautyText *>(*i)->SetHorSpacing(value);
        }
    }
    else if (id == QLatin1String("Angle"))
    {
        for (BeautyList::iterator i = TileEditor::Instance()->Selection().begin()
             ; i != TileEditor::Instance()->Selection().end()
             ; ++i)
        {
            (*i)->SetAngle((float)value);
        }
    }
    TileEditor::Instance()->PushCopyToRedoUndoManager();
}

void TileEditorInterface::UpdateProperties()
{
    if (!TileEditor::Instance()->IsSelection())
    {
        return;
    }

    _propertyUpdate = true;

    BeautyBase *beauty = TileEditor::Instance()->Selection()[0];

    stringManager->setValue(m_UserString, beauty->UserString().c_str());
    char buff[100];
    sprintf(buff, "%0.1f; %0.1f", beauty->GetPos().x, beauty->GetPos().y);
    stringManager->setValue(m_MovePos, buff);
    sprintf(buff, "%0.2f", beauty->GetAngle());
    doubleManager->setValue(m_RotateAngle, beauty->GetAngle());
    sprintf(buff, "%0.2f; %0.2f", beauty->GetScale().x, beauty->GetScale().y);
    stringManager->setValue(m_ScaleXY, buff);
    sprintf(buff, "%0.2f; %0.2f", beauty->PointData().x, beauty->PointData().y);
    stringManager->setValue(m_UserPoint, buff);
    sprintf(buff, "%0.2f", beauty->FloatData());
    doubleManager->setValue(m_UserFloat, beauty->FloatData());
    colorManager->setValue(m_colorPickBtn, QColor(
                               (beauty->GetColor() & 0xFF0000) >> 16
                               , (beauty->GetColor() & 0xFF00) >> 8
                               , (beauty->GetColor() & 0xFF)
                               , (beauty->GetColor() & 0xFF000000) >> 24));

    if (beauty->Type() == "BeautyText")
    {
        BeautyText *txt = static_cast<BeautyText *>(beauty);
        int align = txt->Align();
        if (align == 0)
            enumManager->setValue(m_textAlign, 0);
        else if (align == -1)
            enumManager->setValue(m_textAlign, 1);
        else if (align == 1)
            enumManager->setValue(m_textAlign, 2);

        doubleManager->setValue(m_VertInterval, txt->VertInterval());
        doubleManager->setValue(m_HorSpacing, txt->HorSpacing());

        stringManager->setValue(m_BeautyText, txt->SaveString().c_str());

    }
    //m_UnicOrLinked->SetPropertyValue(  beauty->GetUnicOrLinkedId().c_str(), false );

    //_tileEditor->GetEditBtnState().visible = true;
    m_objectType->setText( beauty->Type().c_str() );
    if (beauty->Type() == "Animation")
    {
         //_tileEditor->GetPlayBtn()->Visible() = true;
        _animationSlider->show();
    }
    else
    {
        // _tileEditor->GetPlayBtn()->Visible() = false;
        _animationSlider->hide();
    }
//    if (beauty->Type() == "LinkToComplex")
//    {
//        _tileEditor->GetUngroupBtn()->Visible() = true;
//    }
//    else
//    {
//        _tileEditor->GetUngroupBtn()->Visible() = false;
//    }

    _propertyUpdate = false;
}

void TileEditorInterface::ItemSelected()
{
    QMap<QtProperty *, QString>::ConstIterator itProp = propertyToId.constBegin();
    while (itProp != propertyToId.constEnd()) {
        delete itProp.key();
        itProp++;
    }
    idToProperty.clear();
    propertyToId.clear();

    if (!TileEditor::Instance()->IsSelection())
    {
        m_objectType->setText("Select an object to see it's properies");
        _animationSlider->hide();
//        _tileEditor->GetPlayBtn()->Visible() = false;
//        _tileEditor->GetUngroupBtn()->Visible() = false;
//        _tileEditor->GetEditBtnState().visible = false;
//        _tileEditor->GetEditBtnState().SetPressed(false);
        _tileEditor->SetButtonsState(TileEditor::bs_nothing);
        deleteSelection->setEnabled(false);
        cutSelection->setEnabled(false);
        copySelection->setEnabled(false);
        return;
    }

    editorProperties->clear();

    idToProperty["UserString"] = m_UserString = stringManager->addProperty(tr("UserString"));
    editorProperties->addProperty(m_UserString);
    idToProperty["UserFloat"] = m_UserFloat = doubleManager->addProperty(tr("UserFloat"));
    doubleManager->setSingleStep(m_UserFloat, 0.1f);
    editorProperties->addProperty(m_UserFloat);
    idToProperty["UserPoint"] = m_UserPoint = stringManager->addProperty(tr("UserPoint"));
    editorProperties->addProperty(m_UserPoint);
    idToProperty["Color"] = m_colorPickBtn = colorManager->addProperty(tr("Color"));
    editorProperties->addProperty(m_colorPickBtn);
    idToProperty["Pos"] = m_MovePos = stringManager->addProperty(tr("Pos"));
    editorProperties->addProperty(m_MovePos);

    if (TileEditor::Instance()->Selection()[0]->Type() == "Animation"
            || TileEditor::Instance()->Selection()[0]->Type() == "Beauty")
    {
        idToProperty["Angle"] = m_RotateAngle = doubleManager->addProperty(tr("Angle"));
        doubleManager->setSingleStep(m_RotateAngle, 0.02f);
        editorProperties->addProperty(m_RotateAngle);
        idToProperty["Scale"] = m_ScaleXY = stringManager->addProperty(tr("Scale"));
        editorProperties->addProperty(m_ScaleXY);
    }
    else if (TileEditor::Instance()->Selection()[0]->Type() == "BeautyText")
    {
        idToProperty["Angle"] = m_RotateAngle = doubleManager->addProperty(tr("Angle"));
        doubleManager->setSingleStep(m_RotateAngle, 0.02f);
        editorProperties->addProperty(m_RotateAngle);
        idToProperty["Scale"] = m_ScaleXY = stringManager->addProperty(tr("Scale"));
        editorProperties->addProperty(m_ScaleXY);

        //todo: количество строк, выравнивание, тень(?), другие атрибуты текста(?)
        idToProperty["TextAlign"] = m_textAlign = enumManager->addProperty(tr("TextAlign"));
        editorProperties->addProperty(m_textAlign);
        {
            QStringList enumNames;
            enumNames << "Center" << "Left" << "Rigth";
            enumManager->setEnumNames(m_textAlign, enumNames);
        }

        idToProperty["VertInterval"] = m_VertInterval = doubleManager->addProperty(tr("Line interval"));
        doubleManager->setSingleStep(m_VertInterval, 0.1f);
        doubleManager->setMinimum(m_VertInterval, 0.1f);
        doubleManager->setMaximum(m_VertInterval, 2.f);
        editorProperties->addProperty(m_VertInterval);

        idToProperty["HorSpacing"] = m_HorSpacing = doubleManager->addProperty(tr("Symbol space"));
        doubleManager->setSingleStep(m_HorSpacing, 1.f);
        doubleManager->setMinimum(m_HorSpacing, -20.f);
        doubleManager->setMaximum(m_HorSpacing, 20.f);
        editorProperties->addProperty(m_HorSpacing);

        idToProperty["BeautyText"] = m_BeautyText = stringManager->addProperty(tr("Text"));
        editorProperties->addProperty(m_BeautyText);
    }

    QMap<QString, QtProperty *>::ConstIterator valProp = idToProperty.constBegin();
    while (valProp != idToProperty.constEnd())
    {
        propertyToId[valProp.value()] = valProp.key();
        valProp++;
    }

    deleteSelection->setEnabled(true);
    cutSelection->setEnabled(true);
    copySelection->setEnabled(true);

    UpdateProperties();
}

void TileEditorInterface::closeEvent(QCloseEvent *event)
{
    settings.setValue("mainwindow_geometry", saveGeometry());
    settings.setValue("mainwindow_state", saveState());

    settings.setValue("netview", _netViewBtn->isChecked());
    settings.setValue("listtree", _listTreeBtn->isChecked());
    settings.setValue("filtering", _filteringTextureBtn->isChecked());
    settings.setValue("viewport", _viewportBtn->isChecked());

    settings.setValue("save_copy_to_file", _saveCopyToFile && _saveCopyToFile->isChecked());

    QWidget::closeEvent(event);

}

void TileEditorInterface::ChangeViewPortSize(QAction *a)
{
    if (a->text().compare("custom size ...") == 0)
    {
        about();
    }
    else
    {
        int w, h;
        if (sscanf(a->text().toStdString().c_str(), "%ix%i", &w, &h) == 2)
        {
            TileEditor::Instance()->SetFrame(w, h);
            settings.setValue("viewportwidth", w);
            settings.setValue("viewportheight", h);
        }
        if (!_viewportBtn->isChecked())
        {
            _viewportBtn->setChecked(true);
        }
    }
}

void TileEditorInterface::MenuItemRemove()
{
    QTreeWidgetItem *item = _itemList->currentItem();
    if (item == NULL)
    {
       return;
    }
    CustomDataClass data = item->data(0, Qt::UserRole).value<CustomDataClass>();
    BeautyBase *b = data.beauty;
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
        caption = "\"" + a->GetTexturePath() + "\"";
    } else if (typeName == "BeautyText") {
        BeautyText *a = static_cast<BeautyText *>(b);
        caption = "\"" + a->GetTexturePath() + "\"";
    } else if (typeName == "Animation") {
        AnimationArea *a = static_cast<AnimationArea *>(b);
        caption = "Animation \"" + a->Name() + "\"";
    } else if (typeName == "GroundSpline") {
        caption = "Spline Way";
    } else if (typeName == "LinkToComplex") {
        caption = "Complex";
    } else if (typeName == "SolidGroundLine") {
        caption = "Spline Way";
    } else {
        assert(false);
    }
    caption = (b->UserString() != "" ? ("\"" + b->UserString() + "\" ") : "") + caption;
    if (GMessageBoxYesNoShow(std::string("Do you want to delete\n" + caption + "\nforever?").c_str()) == QMessageBox::Yes)
    {
        delete item;
//        _itemList->removeItemWidget(item);
//        _itemList->update();
        TileEditor::Instance()->ItemRemove(b);
    }
}

void TileEditorInterface::LoadSelectionPreset()
{
    // на win8 вызов QFileDialog::getOpenFileName часто приводит к крешу
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("Files (*.*)"));

    if (fileName.size())
    {
        // загрузить список имен, которые надо выделить
        FILE *file = fopen(fileName.toStdString().c_str(), "rt");
        if (!file)
        {
            GMessageBoxShow("Can't open preset file!");
            return;
        }
        std::set<std::string> preset;
        char buff[1000];
        while (!feof(file))
        {
            fscanf(file, "%s", buff);
            preset.insert(buff);
        }
        Qt::CheckState value;
        for(int i = 0; i < m_ExportList->count(); ++i)
        {
            value = (preset.find(m_ExportList->item(i)->text().toStdString()) != preset.end()) ? Qt::Checked : Qt::Unchecked;
            m_ExportList->item(i)->setCheckState(value);
        }
        fclose(file);
    }
}

void TileEditorInterface::SaveSelectionPreset()
{
    // на win8 вызов QFileDialog::getSaveFileName?? часто приводит к крешу
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"), "", tr("Files (*.*)"));

    if (fileName.size())
    {
        // сохранить список имен, которые выделены
        FILE *file = fopen(fileName.toStdString().c_str(), "wt");
        if (!file)
        {
            GMessageBoxShow("Can't create preset file!");
            return;
        }
        for(int i = 0; i < m_ExportList->count(); ++i)
        {
            if (m_ExportList->item(i)->checkState() != Qt::Unchecked)
            {
                fprintf(file, "%s\n", m_ExportList->item(i)->text().toStdString().c_str());
            }
        }
        fclose(file);
    }
}

void TileEditorInterface::OpenSupportURL()
{
    QDesktopServices::openUrl ( QUrl("http://yasobe.ru/na/gamedesigner") );
}

void TileEditorInterface::ChangeEditorOptions()
{
    Core::storeXML = _storeXMLText->text().toStdString();
    Core::storeDir = _storeDirText->text().toStdString();
    Core::atlasName = _atlasNameText->text().toStdString();
    Core::workDir = _workDirText->text().toStdString();
    Core::pathToGame = _pathToGameText->text().toStdString();
}

void TileEditorInterface::ShowHelloWindow()
{
    QDialog* pWindow = new QDialog( this );

    pWindow->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    pWindow->setAttribute(Qt::WA_DeleteOnClose, true);
    pWindow->setModal(true);
    pWindow->setWindowTitle( "Hello!" );
    pWindow->setWindowFlags( Qt::Dialog | Qt::WindowTitleHint );
    int width = 300;
    int height = 260;
    pWindow->setFixedSize( width, height );

    QTextEdit *text = new QTextEdit( pWindow );
    text->setReadOnly( true );
    text->move(0, 0);
    text->resize(width, height - 80);
    text->setHtml("<center><p>Please<p>support me at<p>\
                        http://yasobe.ru/na/gamedesigner<p>\
                        It will help to create new awesome feautures!<p>\
                        Press &quot;Ok&quot; to open support page<p>\
                        at your browser.");

    QPushButton *button = new QPushButton( pWindow );
    button->setText( "ok" );
    button->resize( 80, 30 );
    button->move( width / 2 - 40 - 100, height - 40 );
    connect(button, SIGNAL(pressed()), this, SLOT(OpenSupportURL()));
    connect(button, SIGNAL(pressed()), pWindow, SLOT(close()));

    button = new QPushButton( pWindow );
    button->setText( "cancel" );
    button->resize( 80, 30 );
    button->move( width / 2 - 40 + 100, height - 40 );
    connect(button, SIGNAL(pressed()), pWindow, SLOT(close()));

    pWindow->show();
}

void TileEditorInterface::ChangeListTree()
{
    if (ListTreeBtn())
        _itemList->SwitchTo(CollectionControl::pvs_tree);
    else
        _itemList->SwitchTo(CollectionControl::pvs_list);
}

void TileEditorInterface::PropertiesCheckboxSlot()
{
    _propertiesDock->setVisible(_propertiesCheckbox->isChecked());
}

void TileEditorInterface::ResourcesCheckboxSlot()
{
    _listDock->setVisible(_resourcesCheckbox->isChecked());
}

void TileEditorInterface::UpdateCheckboxes()
{
    _resourcesCheckbox->setChecked(_listDock->isVisible());
    _propertiesCheckbox->setChecked(_propertiesDock->isVisible());
}

void TileEditorInterface::PropertiesCheckbox()
{
    _propertiesCheckbox->setChecked(false);
}

void TileEditorInterface::ResourcesCheckbox()
{
    _resourcesCheckbox->setChecked(false);
}
