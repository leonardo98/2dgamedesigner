/****************************************************************************
** Meta object code from reading C++ file 'TileEditorInterface.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../src/Gamee/TileEditorInterface.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'TileEditorInterface.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_CustomWidget_t {
    QByteArrayData data[1];
    char stringdata0[13];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CustomWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CustomWidget_t qt_meta_stringdata_CustomWidget = {
    {
QT_MOC_LITERAL(0, 0, 12) // "CustomWidget"

    },
    "CustomWidget"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CustomWidget[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

void CustomWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObject CustomWidget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_CustomWidget.data,
      qt_meta_data_CustomWidget,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *CustomWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CustomWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CustomWidget.stringdata0))
        return static_cast<void*>(const_cast< CustomWidget*>(this));
    return QWidget::qt_metacast(_clname);
}

int CustomWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    return _id;
}
struct qt_meta_stringdata_CustomDock_t {
    QByteArrayData data[3];
    char stringdata0[20];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CustomDock_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CustomDock_t qt_meta_stringdata_CustomDock = {
    {
QT_MOC_LITERAL(0, 0, 10), // "CustomDock"
QT_MOC_LITERAL(1, 11, 7), // "onClose"
QT_MOC_LITERAL(2, 19, 0) // ""

    },
    "CustomDock\0onClose\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CustomDock[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   19,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void,

       0        // eod
};

void CustomDock::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        CustomDock *_t = static_cast<CustomDock *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->onClose(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (CustomDock::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&CustomDock::onClose)) {
                *result = 0;
                return;
            }
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject CustomDock::staticMetaObject = {
    { &QDockWidget::staticMetaObject, qt_meta_stringdata_CustomDock.data,
      qt_meta_data_CustomDock,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *CustomDock::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CustomDock::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CustomDock.stringdata0))
        return static_cast<void*>(const_cast< CustomDock*>(this));
    return QDockWidget::qt_metacast(_clname);
}

int CustomDock::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDockWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 1)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void CustomDock::onClose()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}
struct qt_meta_stringdata_TileEditorInterface_t {
    QByteArrayData data[49];
    char stringdata0[711];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_TileEditorInterface_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_TileEditorInterface_t qt_meta_stringdata_TileEditorInterface = {
    {
QT_MOC_LITERAL(0, 0, 19), // "TileEditorInterface"
QT_MOC_LITERAL(1, 20, 5), // "about"
QT_MOC_LITERAL(2, 26, 0), // ""
QT_MOC_LITERAL(3, 27, 6), // "NewBtn"
QT_MOC_LITERAL(4, 34, 4), // "Load"
QT_MOC_LITERAL(5, 39, 8), // "FastSave"
QT_MOC_LITERAL(6, 48, 6), // "Update"
QT_MOC_LITERAL(7, 55, 12), // "ListWindowOk"
QT_MOC_LITERAL(8, 68, 14), // "OpenSupportURL"
QT_MOC_LITERAL(9, 83, 19), // "ChangeEditorOptions"
QT_MOC_LITERAL(10, 103, 4), // "Undo"
QT_MOC_LITERAL(11, 108, 4), // "Redo"
QT_MOC_LITERAL(12, 113, 17), // "RenameLevelDialog"
QT_MOC_LITERAL(13, 131, 17), // "EnterNameDialogOk"
QT_MOC_LITERAL(14, 149, 13), // "EditorOptions"
QT_MOC_LITERAL(15, 163, 9), // "DropLevel"
QT_MOC_LITERAL(16, 173, 14), // "MenuItemRemove"
QT_MOC_LITERAL(17, 188, 17), // "ChangeWalkThrough"
QT_MOC_LITERAL(18, 206, 19), // "OpenLevelChangeMask"
QT_MOC_LITERAL(19, 226, 4), // "text"
QT_MOC_LITERAL(20, 231, 19), // "ItemNameMaskChanged"
QT_MOC_LITERAL(21, 251, 19), // "LoadSelectionPreset"
QT_MOC_LITERAL(22, 271, 19), // "SaveSelectionPreset"
QT_MOC_LITERAL(23, 291, 22), // "PropertiesCheckboxSlot"
QT_MOC_LITERAL(24, 314, 21), // "ResourcesCheckboxSlot"
QT_MOC_LITERAL(25, 336, 18), // "PropertiesCheckbox"
QT_MOC_LITERAL(26, 355, 17), // "ResourcesCheckbox"
QT_MOC_LITERAL(27, 373, 16), // "UpdateCheckboxes"
QT_MOC_LITERAL(28, 390, 17), // "StartGameFunction"
QT_MOC_LITERAL(29, 408, 22), // "ShowChooseLevelsWindow"
QT_MOC_LITERAL(30, 431, 19), // "CreateAtlasForLevel"
QT_MOC_LITERAL(31, 451, 15), // "CreateProjectFn"
QT_MOC_LITERAL(32, 467, 11), // "OpenProject"
QT_MOC_LITERAL(33, 479, 9), // "AddSprite"
QT_MOC_LITERAL(34, 489, 13), // "TestAnimation"
QT_MOC_LITERAL(35, 503, 14), // "OnSliderChange"
QT_MOC_LITERAL(36, 518, 12), // "valueChanged"
QT_MOC_LITERAL(37, 531, 11), // "QtProperty*"
QT_MOC_LITERAL(38, 543, 8), // "property"
QT_MOC_LITERAL(39, 552, 5), // "value"
QT_MOC_LITERAL(40, 558, 15), // "ResaveAllLevels"
QT_MOC_LITERAL(41, 574, 18), // "ChangeViewPortSize"
QT_MOC_LITERAL(42, 593, 8), // "QAction*"
QT_MOC_LITERAL(43, 602, 15), // "ChangeAtlasSize"
QT_MOC_LITERAL(44, 618, 14), // "ChangeListTree"
QT_MOC_LITERAL(45, 633, 17), // "CreateCommonAtlas"
QT_MOC_LITERAL(46, 651, 23), // "ExportSelectUnselectAll"
QT_MOC_LITERAL(47, 675, 19), // "ChangeSelectionMode"
QT_MOC_LITERAL(48, 695, 15) // "ShowHelloWindow"

    },
    "TileEditorInterface\0about\0\0NewBtn\0"
    "Load\0FastSave\0Update\0ListWindowOk\0"
    "OpenSupportURL\0ChangeEditorOptions\0"
    "Undo\0Redo\0RenameLevelDialog\0"
    "EnterNameDialogOk\0EditorOptions\0"
    "DropLevel\0MenuItemRemove\0ChangeWalkThrough\0"
    "OpenLevelChangeMask\0text\0ItemNameMaskChanged\0"
    "LoadSelectionPreset\0SaveSelectionPreset\0"
    "PropertiesCheckboxSlot\0ResourcesCheckboxSlot\0"
    "PropertiesCheckbox\0ResourcesCheckbox\0"
    "UpdateCheckboxes\0StartGameFunction\0"
    "ShowChooseLevelsWindow\0CreateAtlasForLevel\0"
    "CreateProjectFn\0OpenProject\0AddSprite\0"
    "TestAnimation\0OnSliderChange\0valueChanged\0"
    "QtProperty*\0property\0value\0ResaveAllLevels\0"
    "ChangeViewPortSize\0QAction*\0ChangeAtlasSize\0"
    "ChangeListTree\0CreateCommonAtlas\0"
    "ExportSelectUnselectAll\0ChangeSelectionMode\0"
    "ShowHelloWindow"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_TileEditorInterface[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      45,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,  239,    2, 0x08 /* Private */,
       3,    0,  240,    2, 0x08 /* Private */,
       4,    0,  241,    2, 0x08 /* Private */,
       5,    0,  242,    2, 0x08 /* Private */,
       6,    0,  243,    2, 0x08 /* Private */,
       7,    0,  244,    2, 0x08 /* Private */,
       8,    0,  245,    2, 0x08 /* Private */,
       9,    0,  246,    2, 0x08 /* Private */,
      10,    0,  247,    2, 0x08 /* Private */,
      11,    0,  248,    2, 0x08 /* Private */,
      12,    0,  249,    2, 0x08 /* Private */,
      13,    0,  250,    2, 0x08 /* Private */,
      14,    0,  251,    2, 0x08 /* Private */,
      15,    0,  252,    2, 0x08 /* Private */,
      16,    0,  253,    2, 0x08 /* Private */,
      17,    0,  254,    2, 0x08 /* Private */,
      18,    1,  255,    2, 0x08 /* Private */,
      20,    1,  258,    2, 0x08 /* Private */,
      21,    0,  261,    2, 0x08 /* Private */,
      22,    0,  262,    2, 0x08 /* Private */,
      23,    0,  263,    2, 0x08 /* Private */,
      24,    0,  264,    2, 0x08 /* Private */,
      25,    0,  265,    2, 0x08 /* Private */,
      26,    0,  266,    2, 0x08 /* Private */,
      27,    0,  267,    2, 0x08 /* Private */,
      28,    0,  268,    2, 0x0a /* Public */,
      29,    0,  269,    2, 0x0a /* Public */,
      30,    0,  270,    2, 0x0a /* Public */,
      31,    0,  271,    2, 0x0a /* Public */,
      32,    0,  272,    2, 0x0a /* Public */,
      33,    0,  273,    2, 0x0a /* Public */,
      34,    0,  274,    2, 0x0a /* Public */,
      35,    0,  275,    2, 0x0a /* Public */,
      36,    2,  276,    2, 0x0a /* Public */,
      36,    2,  281,    2, 0x0a /* Public */,
      36,    2,  286,    2, 0x0a /* Public */,
      36,    2,  291,    2, 0x0a /* Public */,
      40,    0,  296,    2, 0x0a /* Public */,
      41,    1,  297,    2, 0x0a /* Public */,
      43,    1,  300,    2, 0x0a /* Public */,
      44,    0,  303,    2, 0x0a /* Public */,
      45,    0,  304,    2, 0x0a /* Public */,
      46,    0,  305,    2, 0x0a /* Public */,
      47,    1,  306,    2, 0x0a /* Public */,
      48,    0,  309,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   19,
    QMetaType::Void, QMetaType::QString,   19,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 37, QMetaType::QString,   38,   39,
    QMetaType::Void, 0x80000000 | 37, QMetaType::QColor,   38,   39,
    QMetaType::Void, 0x80000000 | 37, QMetaType::Double,   38,   39,
    QMetaType::Void, 0x80000000 | 37, QMetaType::Int,   38,   39,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 42,    2,
    QMetaType::Void, 0x80000000 | 42,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 42,    2,
    QMetaType::Void,

       0        // eod
};

void TileEditorInterface::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        TileEditorInterface *_t = static_cast<TileEditorInterface *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->about(); break;
        case 1: _t->NewBtn(); break;
        case 2: _t->Load(); break;
        case 3: _t->FastSave(); break;
        case 4: _t->Update(); break;
        case 5: _t->ListWindowOk(); break;
        case 6: _t->OpenSupportURL(); break;
        case 7: _t->ChangeEditorOptions(); break;
        case 8: _t->Undo(); break;
        case 9: _t->Redo(); break;
        case 10: _t->RenameLevelDialog(); break;
        case 11: _t->EnterNameDialogOk(); break;
        case 12: _t->EditorOptions(); break;
        case 13: _t->DropLevel(); break;
        case 14: _t->MenuItemRemove(); break;
        case 15: _t->ChangeWalkThrough(); break;
        case 16: _t->OpenLevelChangeMask((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 17: _t->ItemNameMaskChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 18: _t->LoadSelectionPreset(); break;
        case 19: _t->SaveSelectionPreset(); break;
        case 20: _t->PropertiesCheckboxSlot(); break;
        case 21: _t->ResourcesCheckboxSlot(); break;
        case 22: _t->PropertiesCheckbox(); break;
        case 23: _t->ResourcesCheckbox(); break;
        case 24: _t->UpdateCheckboxes(); break;
        case 25: _t->StartGameFunction(); break;
        case 26: _t->ShowChooseLevelsWindow(); break;
        case 27: _t->CreateAtlasForLevel(); break;
        case 28: _t->CreateProjectFn(); break;
        case 29: _t->OpenProject(); break;
        case 30: _t->AddSprite(); break;
        case 31: _t->TestAnimation(); break;
        case 32: _t->OnSliderChange(); break;
        case 33: _t->valueChanged((*reinterpret_cast< QtProperty*(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 34: _t->valueChanged((*reinterpret_cast< QtProperty*(*)>(_a[1])),(*reinterpret_cast< const QColor(*)>(_a[2]))); break;
        case 35: _t->valueChanged((*reinterpret_cast< QtProperty*(*)>(_a[1])),(*reinterpret_cast< double(*)>(_a[2]))); break;
        case 36: _t->valueChanged((*reinterpret_cast< QtProperty*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 37: _t->ResaveAllLevels(); break;
        case 38: _t->ChangeViewPortSize((*reinterpret_cast< QAction*(*)>(_a[1]))); break;
        case 39: _t->ChangeAtlasSize((*reinterpret_cast< QAction*(*)>(_a[1]))); break;
        case 40: _t->ChangeListTree(); break;
        case 41: _t->CreateCommonAtlas(); break;
        case 42: _t->ExportSelectUnselectAll(); break;
        case 43: _t->ChangeSelectionMode((*reinterpret_cast< QAction*(*)>(_a[1]))); break;
        case 44: _t->ShowHelloWindow(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 38:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QAction* >(); break;
            }
            break;
        case 39:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QAction* >(); break;
            }
            break;
        case 43:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QAction* >(); break;
            }
            break;
        }
    }
}

const QMetaObject TileEditorInterface::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_TileEditorInterface.data,
      qt_meta_data_TileEditorInterface,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *TileEditorInterface::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *TileEditorInterface::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_TileEditorInterface.stringdata0))
        return static_cast<void*>(const_cast< TileEditorInterface*>(this));
    if (!strcmp(_clname, "Messager"))
        return static_cast< Messager*>(const_cast< TileEditorInterface*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int TileEditorInterface::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 45)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 45;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 45)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 45;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
