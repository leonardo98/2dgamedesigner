#include "CollectionControl.h"
#include "../Core/types.h"
#include "../Core/Core.h"
#include "../Core/Math.h"
#include "TileEditorInterface.h"
#include <QPixmap>
#include <QMimeData>
#include <QApplication>
#include <QToolTip>

CollectionControl::~CollectionControl()
{
}

void CollectionControl::mouseMoveEvent(QMouseEvent *event)
{
    QTreeWidgetItem *underCursor = itemAt(event->pos());
    if (_itemUnderCursor != underCursor)
    {
        _itemUnderCursor = underCursor;
        QToolTip::hideText();// event->pos() , "some text HERE" );
    }
    if (!(event->buttons() & Qt::LeftButton))
    {
        return;
    }
    if ((event->pos() - dragStartPosition).manhattanLength()
         < QApplication::startDragDistance())
    {
        return;
    }

    QTreeWidgetItem *item = itemAt(event->pos());
    if (item == NULL)
    {
        return;
    }

    //! [1]
    QByteArray array;
    CustomDataClass data = item->data(0, Qt::UserRole).value<CustomDataClass>();
    BeautyBase *beauty = data.beauty;
    array.resize(sizeof(beauty));
    memcpy(array.data(), &beauty, sizeof(beauty));

    //! [2]
    QMimeData *mimeData = new QMimeData;
    mimeData->setData("application/x-dndbeauty", array);
    //! [2]

    //! [3]
    QDrag *drag = new QDrag(this);
    drag->setMimeData(mimeData);
    drag->setPixmap(item->icon(0).pixmap(64, 64));
    drag->setHotSpot(QPoint(32, 32));

    //! [3]
    drag->exec();
}

void CollectionControl::mousePressEvent(QMouseEvent *event)
{
    QTreeWidget::mousePressEvent(event);
    if (event->button() == Qt::LeftButton)
    {
         dragStartPosition = event->pos();
    }
}

CollectionControl::CollectionControl( )
{
    _itemUnderCursor = NULL;
    setDragEnabled(true);
    setAcceptDrops(true);
    setIconSize(QSize(32, 32));
    setSelectionMode(QAbstractItemView::SingleSelection);
    setHeaderLabel("assets");

    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(customMenuRequested(QPoint)));
}

void CollectionControl::AddItem(BeautyBase *base, const std::string &text, int insertPosition, QTreeWidgetItem *parent)
{
    QTreeWidgetItem *item = new QTreeWidgetItem();
    QVariant a;
    CustomDataClass data;
    data.beauty = base;
    a.setValue(data);
    item->setData(0, Qt::UserRole, a);
    item->setText(0,  text.c_str() );

    {
        QString s("Object: ");
        s += base->Type().c_str();
        if (base->UserString().size())
        {
            s += QString("\nUserString: ") + base->UserString().c_str();
        }
        s += QString("\n") + text.c_str();
        item->setToolTip(0, s);
    }

    const char *spr = base->GetIconTexture();

    if (spr && strcmp(spr, "") != 0)
    {
        item->setIcon(0, QIcon(spr));
    }
    else
    {
        TileEditor::Instance()->NeedBeautyIcon(base, item);
    }

    if (parent)
    {
        assert(insertPosition >= 0);
        parent->insertChild(insertPosition, item);
    }
    else if (TileEditorInterface::Instance()->ListTreeBtn())
    {
        AddRecursive(item, base);
    }
    else
    {
        if (insertPosition == -1)
        {
            addTopLevelItem( item );
        }
        else
        {
            insertTopLevelItem( insertPosition, item );
        }
    }

    if (topLevelItemCount() % 2 == 1)
        item->setBackground(0, QColor(240, 240, 255));
    else
        item->setBackground(0, QColor(230, 230, 255));

}

void CollectionControl::AddRecursive(QTreeWidgetItem *item, BeautyBase *base)
{
    if (base == NULL || base->Type() == "LinkToComplex")
    {
        addTopLevelItem( item );
    }
    else if (!base->GetTexturePath().empty())
    {
        std::string::size_type splash = base->GetTexturePath().rfind("/");
        if (splash != std::string::npos)
        {
            std::string parentFolder = base->GetTexturePath().substr(0, splash);
            if (_treeMap.find(parentFolder) != _treeMap.end())
            {
                _treeMap[parentFolder]->addChild(item);
            }
            else
            {
                QTreeWidgetItem *parent = new QTreeWidgetItem();
                parent->addChild(item);
                parent->setToolTip(0, parentFolder.c_str());

                std::string::size_type splash = parentFolder.rfind("/");
                if (splash != std::string::npos)
                    parent->setText(0, parentFolder.substr(splash + 1).c_str());
                else
                    parent->setText(0, parentFolder.c_str());

                parent->setIcon(0, QIcon((Core::programPath + "gfx/folder.png").c_str()));
                AddRecursive(parent, parentFolder);
                _treeMap[parentFolder] = parent;
                parent->setExpanded(true);
            }
        }
        else
            assert(false);
    }
    else
    {
        addTopLevelItem( item );
    }
}


void CollectionControl::AddRecursive(QTreeWidgetItem *item, const std::string &path)
{
    std::string::size_type splash = path.rfind("/");
    if (splash != std::string::npos)
    {
        std::string parentFolder = path.substr(0, splash);
        if (_treeMap.find(parentFolder) != _treeMap.end())
        {
            _treeMap[parentFolder]->addChild(item);
        }
        else
        {
            QTreeWidgetItem *parent = new QTreeWidgetItem();
            parent->addChild(item);
            parent->setToolTip(0, parentFolder.c_str());

            std::string::size_type splash = parentFolder.rfind("/");
            if (splash != std::string::npos)
                parent->setText(0, parentFolder.substr(splash + 1).c_str());
            else
                parent->setText(0,  parentFolder.c_str());

            parent->setIcon(0, QIcon((Core::programPath + "gfx/folder.png").c_str()));
            AddRecursive(parent, parentFolder);
            _treeMap[parentFolder] = parent;
            parent->setExpanded(true);
        }
    }
    else
        addTopLevelItem( item );
}

void CollectionControl::customMenuRequested(QPoint pos)
{
//    QTreeWidgetItem *item = itemAt(pos);
    QMenu *menu = new QMenu(this);
    {

//        CustomDataClass data = item->data(0, Qt::UserRole).value<CustomDataClass>();
//        BeautyBase *base = data.beauty;

        QWidget *main = TileEditorInterface::Instance();
//        menu->addAction(new QAction( QString("Object: ") + base->Type().c_str(), main ) );
//        if (base->UserString().size())
//        {
//            menu->addAction(new QAction( QString("UserString: ") + base->UserString().c_str(), main) );
//        }

//        menu->addSeparator();

        QAction *a = new QAction( "Create folder", main );
        menu->addAction( a );
        connect(a, SIGNAL(triggered()), TileEditor::Instance(), SLOT(CreateFolder()));

        a = new QAction( "Duplicate", main );
        menu->addAction( a );
        connect(a, SIGNAL(triggered()), TileEditor::Instance(), SLOT(ItemDublicate()));

        a = new QAction( "Remove", main );
        menu->addAction( a );
        connect(a, SIGNAL(triggered()), main, SLOT(MenuItemRemove()));

        menu->addSeparator();

        menu->addAction(new QAction( "Properties...", main ) );
    }
    menu->popup(viewport()->mapToGlobal(pos));
}

void CollectionControl::dragEnterEvent(QDragEnterEvent* event)
{
    if (event->mimeData()->hasUrls())
    {
        event->acceptProposedAction();
    }
}

void CollectionControl::dragMoveEvent(QDragMoveEvent* event)
{
    if (event->mimeData()->hasUrls())
    {
        event->acceptProposedAction();
    }
}

void CollectionControl::dragLeaveEvent(QDragLeaveEvent* event)
{
    event->accept();
}

void CollectionControl::dropEvent(QDropEvent* event)
{
    const QMimeData* mimeData = event->mimeData();
    if (mimeData->hasUrls())
    {
        QList<QUrl> urlList = mimeData->urls();
        bool success = false;
        for (int i = 0; i < urlList.size(); ++i)
        {
            if (TileEditor::Instance()->CreateBeautyWithPng(urlList.at(i).toLocalFile().toStdString()))
            {
                success = true;
            }
        }
        if (success)
        {
            TileEditor::Instance()->SaveTemplates(Core::projectXML);
        }
    }
}

void CollectionControl::RemoveChildren()
{
    QTreeWidget::clear();
    _treeMap.clear();
}

void CollectionControl::SwitchTo(PaletteViewState state)
{
    ItemList list;
    CloneItems(NULL, list);
    RemoveChildren();
    if (state == pvs_list)
    {
        for (ItemList::iterator i = list.begin(), e = list.end(); i != e; ++i)
        {
            addTopLevelItem( *i );
            if (topLevelItemCount() % 2 == 1)
                (*i)->setBackground(0, QColor(240, 240, 255));
            else
                (*i)->setBackground(0, QColor(230, 230, 255));
        }
    }
    else
    {
        for (ItemList::iterator i = list.begin(), e = list.end(); i != e; ++i)
        {
            CustomDataClass data = (*i)->data(0, Qt::UserRole).value<CustomDataClass>();
            BeautyBase *base = data.beauty;
            AddRecursive( *i , base);
        }
    }
}

void CollectionControl::CloneItems(QTreeWidgetItem *item, ItemList &list)
{
    if (item == NULL)
    {
        for (int i = 0; i < topLevelItemCount(); ++i)
        {
            QTreeWidgetItem *it = topLevelItem(i);
            if (it->childCount() == 0)
                list.push_back(it->clone());
            else
                CloneItems(it, list);
        }
    }
    else
    {
        for (int i = 0; i < item->childCount(); ++i)
        {
            QTreeWidgetItem *it = item->child(i);
            if (it->childCount() == 0)
                list.push_back(it->clone());
            else
                CloneItems(it, list);
        }
    }
}

