#include "filetreeitem.h"
#include "filetreemodel.h"

#include <QStringList>
#include <QFileInfo>
#include <QDir>
#include <QDebug>
#include <QMimeDatabase>

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

    /*if(role != Qt::DecorationRole && index.column() == 0)
        return QVariant();*/

    if(role != Qt::DisplayRole)
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
        //Si le fichier courant est un dossier, on crée un sous-arbre intégrant son contenu
        if(file[3].contains("inode/directory")) {
            rootItem->appendChild(createDirectorySubTree(file, parents.last()));
        } else {
            //On ajoute un fichier à la racine
            QList<QVariant> columnData;
            columnData << QFileInfo(file[2]).baseName();
            columnData << file[3];
            FileTreeItem *fti = new FileTreeItem(columnData, parents.last(), file[0], file[1], file[2], file[3]);
            fti->setIcon(QIcon(":/icons/icons/bookmark.png"));
            rootItem->appendChild(fti);
        }
    }
}


FileTreeItem* FileTreeModel::createDirectorySubTree(QStringList file, FileTreeItem *parent) {
    QList<QVariant> columnData;
    columnData << QFileInfo(file[2]).baseName();
    columnData << file[3];
    FileTreeItem* subTreeRoot = new FileTreeItem(columnData, parent, file[0], file[1], file[2], file[3]);
    QDir *dir = new QDir(file[2]);
    QStringList name_filter = QStringList("*");
    QFileInfoList files = dir->entryInfoList(name_filter, QDir::NoDotAndDotDot | QDir::Dirs | QDir::Files);
    QMimeDatabase qmd;
    foreach(QFileInfo file_from_dir, files) {
        QString mimetype = qmd.mimeTypeForFile(file_from_dir).name();
        if (mimetype.contains("inode/directory")) {
            QStringList sub_dir;
            //Pas d'id de fichier
            sub_dir.append("");
            //Pas d'id de bookmark
            sub_dir.append("");
            sub_dir.append(file_from_dir.absoluteFilePath());
            sub_dir.append(mimetype);
            subTreeRoot->appendChild(createDirectorySubTree(sub_dir, subTreeRoot));
        } else if(!mimetype.contains("x-trash")) {
            columnData.clear();
            columnData << file_from_dir.baseName();
            columnData << mimetype;
            FileTreeItem* node = new FileTreeItem(columnData, subTreeRoot, "", "", file_from_dir.baseName(), mimetype);
            subTreeRoot->appendChild(node);
        }
    }
    return subTreeRoot;
}
