#ifndef BONESLISTCONTROL_H
#define BONESLISTCONTROL_H

#include <QTreeWidget>
#include "../Core/ogl/render.h"
#include "BeautyBase.h"
#include <QMouseEvent>
#include <QDrag>


class CustomDataClass
{
public:
    BeautyBase *beauty;
};

Q_DECLARE_METATYPE(CustomDataClass)


class CollectionControl : public QTreeWidget
{
    Q_OBJECT

public slots:
    void customMenuRequested(QPoint);

public:

    CollectionControl();
	~CollectionControl();

    void RemoveChildren();

    enum PaletteViewState
    {
        pvs_list,
        pvs_tree
    };
    void SwitchTo(PaletteViewState state);

    void AddItem(BeautyBase *base, const std::string &text, int insertPosition = -1, QTreeWidgetItem *parent = NULL);
    void AddRecursive(QTreeWidgetItem *item, BeautyBase *base);
    void AddRecursive(QTreeWidgetItem *item, const std::string &path);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

    void dragEnterEvent(QDragEnterEvent* event);
    void dragMoveEvent(QDragMoveEvent* event);
    void dragLeaveEvent(QDragLeaveEvent* event);
    void dropEvent(QDropEvent* event);

private:
    typedef std::list<QTreeWidgetItem *> ItemList;
    void CloneItems(QTreeWidgetItem *item, ItemList &list);
    QPoint dragStartPosition;
    QTreeWidgetItem *_itemUnderCursor;
    std::map<std::string, QTreeWidgetItem*> _treeMap;
};

#endif//BONESLISTCONTROL_H
