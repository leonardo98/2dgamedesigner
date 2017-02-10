#include <QLabel>
#include <QPushButton>
#include <QFileDialog>
#include "CreateProject.h"

CreateProject::CreateProject(QWidget *parent)
    : QDialog(parent)
{
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setAttribute(Qt::WA_DeleteOnClose, true);
    setModal(true);
    setWindowTitle( "Create New Project" );
    setWindowFlags( Qt::Dialog | Qt::WindowTitleHint );
    int width = 500;
    int height = 230;
    int textBoxWidth = (width - 40) / 2;
    setFixedSize( width, height );

    QLabel *label = new QLabel("Choose directory with game", this);
    label->move(10, 10);
    QLineEdit *edit = _gameFolder = new QLineEdit(this);
    edit->move(10, 25);
    edit->resize(width - 100, 20);
    QPushButton *btn = new QPushButton(this);
    btn->setText("Browse..");
    btn->resize(70, 20);
    btn->move(width - 80, 25);
    connect(btn, SIGNAL(pressed()), this, SLOT(ChooseDir()));

    label = new QLabel("Project file name", this);
    label->move(10, 10 + 40);
    edit = _projectFileName = new QLineEdit(this);
    edit->move(10, 25 + 40);
    edit->resize(textBoxWidth, 20);
    edit->setText("project.xml");

    label = new QLabel("File(s) - level storage", this);
    label->move(10, 10 + 80);
    edit = _levelsFile = new QLineEdit(this);
    edit->move(10, 25 + 80);
    edit->resize(textBoxWidth, 20);
    edit->setText("levels.xml");

    label = new QLabel("Data folder", this);
    label->move(10, 10 + 120);
    edit = _dataFolder = new QLineEdit(this);
    edit->move(10, 25 + 120);
    edit->resize(textBoxWidth, 20);
    edit->setText("data");

    label = new QLabel("Default atlas", this);
    label->move(width / 2 + 10, 10 + 80);
    edit = _defaultAtlas = new QLineEdit(this);
    edit->move(width / 2 + 10, 25 + 80);
    edit->resize(textBoxWidth, 20);
    edit->setText("");

    label = new QLabel("Game run command", this);
    label->move(width / 2 + 10, 10 + 120);
    edit = _gameRunCommand = new QLineEdit(this);
    edit->move(width / 2 + 10, 25 + 120);
    edit->resize(textBoxWidth, 20);
    edit->setText("");

    btn = new QPushButton(this);
    btn->setText("Go");
    btn->resize(60, 30);
    btn->move(30, height - 45);
    connect(btn, SIGNAL(pressed()), this, SLOT(CreateProjectFileAndOpen()));

    btn = new QPushButton(this);
    btn->setText("Cancel");
    btn->resize(60, 30);
    btn->move(width - 30 - 60, height - 45);
    connect(btn, SIGNAL(pressed()), this, SLOT(close()));

    show();
}

void CreateProject::ChooseDir()
{
    QFileDialog dialog;
    dialog.setFileMode(QFileDialog::Directory);
    dialog.setOption(QFileDialog::ShowDirsOnly);
    if (dialog.exec())
    {
        QString folderName = dialog.selectedFiles().front();
        _gameFolder->setText(folderName);
    }
}

void CreateProject::CreateProjectFileAndOpen()
{

    close();
}
