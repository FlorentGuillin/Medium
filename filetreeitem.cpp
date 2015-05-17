#include <QStringList>
#include "filetreeitem.h"

FileTreeItem::FileTreeItem(const QList<QVariant> &data, FileTreeItem *parent)
{
    m_parentItem = parent;
    m_itemData = data;
}

FileTreeItem::~FileTreeItem()
{
    qDeleteAll(m_childItems);
}

void FileTreeItem::appendChild(FileTreeItem *item)
{
    m_childItems.append(item);
}

FileTreeItem *FileTreeItem::child(int row)
{
    return m_childItems.value(row);
}

int FileTreeItem::childCount() const
{
    return m_childItems.count();
}

int FileTreeItem::columnCount() const
{
    return m_itemData.count();
}

QVariant FileTreeItem::data(int column) const
{
    return m_itemData.value(column);
}

FileTreeItem *FileTreeItem::parentItem()
{
    return m_parentItem;
}


int FileTreeItem::row() const
{
    if (m_parentItem)
        return m_parentItem->m_childItems.indexOf(const_cast<FileTreeItem*>(this));

    return 0;
}

void FileTreeItem::setFileId(QString fi) {
    file_id = fi;
}

void FileTreeItem::setBookMarkIdFk(QString bi) {
    bookmark_id_fk = bi;
}

void FileTreeItem::setFilePath(QString fp) {
    file_path = fp;
}

void FileTreeItem::setFileType(QString ft) {
    file_type = ft;
}

QString FileTreeItem::getFileId() {
    return file_id;
}

QString FileTreeItem::getBookMarkIdFk() {
    return bookmark_id_fk;
}

QString FileTreeItem::getFilePath() {
    return file_path;
}

QString FileTreeItem::getFileType() {
    return file_type;
}
