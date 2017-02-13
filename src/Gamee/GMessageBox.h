#ifndef GMESSAGE_BOX_H
#define GMESSAGE_BOX_H

//#include "Gwen/Controls/Button.h"
#include <QPushButton>
#include <QDialog>

void GMessageBoxShow(const char *text);

enum GMessageBoxResult
{
    gmessage_box_yes,
    gmessage_box_no
};

int GMessageBoxYesNoShow(const char *text);

#endif//GMESSAGE_BOX_H
