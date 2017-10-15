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

    CollectionControl(QWidget *parent);
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
    bool FilterByNameMask(QTreeWidgetItem *item, const QString &mask);
    void FilterByNameMask(const QString &mask);

    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;

    virtual void dragEnterEvent(QDragEnterEvent* event) override;
    virtual void dragMoveEvent(QDragMoveEvent* event) override;
    virtual void dragLeaveEvent(QDragLeaveEvent* event) override;
    virtual void dropEvent(QDropEvent* event) override;

private:
    typedef std::list<QTreeWidgetItem *> ItemList;
    void CloneItems(QTreeWidgetItem *item, ItemList &list);
    QPoint dragStartPosition;
    QTreeWidgetItem *_itemUnderCursor;
    std::map<std::string, QTreeWidgetItem*> _treeMap;
};

#endif//BONESLISTCONTROL_H
