#ifndef FILETREEITEM_H
#define FILETREEITEM_H

#include <QList>
#include <QVariant>
#include <QStandardItem>

class FileTreeItem : public QStandardItem
{
public:
    explicit FileTreeItem(const QList<QVariant> &data, FileTreeItem *parentItem = 0);
    explicit FileTreeItem(const QList<QVariant> &data, FileTreeItem *parent, QString file_id, QString bookmark_id, QString file_path, QString file_type);
    ~FileTreeItem();

    void appendChild(FileTreeItem *child);

    FileTreeItem *child(int row);
    int childCount() const;
    int columnCount() const;
    QVariant data(int column) const;
    int row() const;
    FileTreeItem *parentItem();
    void setFileId(QString fi);
    void setBookMarkIdFk(QString bi);
    void setFilePath(QString fp);
    void setFileType(QString ft);
    QString getFileId();
    QString getBookMarkIdFk();
    QString getFilePath();
    QString getFileType();

private:
    QList<FileTreeItem*> m_childItems;
    QList<QVariant> m_itemData;
    FileTreeItem *m_parentItem;
    QString file_id;
    QString bookmark_id_fk;
    QString file_path;
    QString file_type;
};

#endif // FILETREEITEM_H
