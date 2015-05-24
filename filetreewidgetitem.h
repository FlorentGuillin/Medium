#ifndef FILETREEWIDGETITEM_H
#define FILETREEWIDGETITEM_H

#include <QTreeWidget>
#include <QFileInfo>
#include <QDir>
#include <QDebug>
#include <QMimeDatabase>

class FileTreeWidgetItem : public QTreeWidgetItem
{
public:
    explicit FileTreeWidgetItem(QTreeWidget * parent);
    explicit FileTreeWidgetItem(QTreeWidgetItem * parent);
    ~FileTreeWidgetItem();
    void createDirectorySubTree(FileTreeWidgetItem *folder);
    void setFileId(QString fi);
    void setBookMarkIdFk(QString bi);
    void setFilePath(QString fp);
    void setFileType(QString ft);
    QString getFileId();
    QString getBookMarkIdFk();
    QString getFilePath();
    QString getFileType();
private:
    QString file_id;
    QString bookmark_id_fk;
    QString file_path;
    QString file_type;
};

#endif // FILETREEWIDGETITEM_H
