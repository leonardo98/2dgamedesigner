#ifndef CREATEPROJECT_H
#define CREATEPROJECT_H

#include <QDialog>
#include <QLineEdit>

class CreateProject : public QDialog
{
    Q_OBJECT
    QLineEdit *_gameFolder;
    QLineEdit *_projectFileName;
    QLineEdit *_levelsFile;
    QLineEdit *_dataFolder;
    QLineEdit *_defaultAtlas;
    QLineEdit *_gameRunCommand;

public:
    explicit CreateProject(QWidget *parent = 0);

signals:

public slots:
    void ChooseDir();
    void CreateProjectFileAndOpen();
};

#endif // CREATEPROJECT_H
