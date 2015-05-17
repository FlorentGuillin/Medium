#include "filetreeitem.h"
#include "filetreemodel.h"

#include <QStringList>
#include <QFileInfo>

FileTreeModel::FileTreeModel(const QList<QStringList> &data, QObject *parent)
    : QAbstractItemModel(parent)
{
    QList<QVariant> rootData;
    rootData << "Nom du fichier" << "Type du fichier";
    rootItem = new FileTreeItem(rootData);
    setupModelData(data, rootItem);
}

FileTreeModel::~FileTreeModel()
{
    delete rootItem;
}

int FileTreeModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return static_cast<FileTreeItem*>(parent.internalPointer())->columnCount();
    else
        return rootItem->columnCount();
}

QVariant FileTreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole)
        return QVariant();

    FileTreeItem *item = static_cast<FileTreeItem*>(index.internalPointer());

    return item->data(index.column());
}

Qt::ItemFlags FileTreeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    return QAbstractItemModel::flags(index);
}

QVariant FileTreeModel::headerData(int section, Qt::Orientation orientation,
                               int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return rootItem->data(section);

    return QVariant();
}

QModelIndex FileTreeModel::index(int row, int column, const QModelIndex &parent)
            const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    FileTreeItem *parentItem;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<FileTreeItem*>(parent.internalPointer());

    FileTreeItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}


QModelIndex FileTreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    FileTreeItem *childItem = static_cast<FileTreeItem*>(index.internalPointer());
    FileTreeItem *parentItem = childItem->parentItem();

    if (parentItem == rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int FileTreeModel::rowCount(const QModelIndex &parent) const
{
    FileTreeItem *parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<FileTreeItem*>(parent.internalPointer());

    return parentItem->childCount();
}


void FileTreeModel::setupModelData(const QList<QStringList> &files, FileTreeItem *parent)
{
    QList<FileTreeItem*> parents;
    parents << parent;
    foreach(QStringList file, files) {
        QList<QVariant> columnData;
        columnData << QFileInfo(file[2]).baseName();
        columnData << file[3];
        FileTreeItem *fti = new FileTreeItem(columnData, parents.last());
        fti->setFileId(file[0]);
        fti->setBookMarkIdFk(file[1]);
        fti->setFilePath(file[2]);
        fti->setFileType(file[3]);
        rootItem->appendChild(fti);
    }
}
