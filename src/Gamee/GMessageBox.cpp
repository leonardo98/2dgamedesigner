#include "../Core/types.h"
#include "GMessageBox.h"
#include "TileEditorInterface.h"

void GMessageBoxShow(const char *text)
{
    QMessageBox::about(mainWindow, mainWindow->tr("Warning : "), mainWindow->tr(text));
}

int GMessageBoxYesNoShow(const char *text)
{
    QMessageBox msgBox;
    //msgBox.setText("The document has been modified.");
    msgBox.setInformativeText(text);
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Yes);
    return msgBox.exec();
}
