#include "filetreewidgetitem.h"

FileTreeWidgetItem::FileTreeWidgetItem(QTreeWidget * parent) : QTreeWidgetItem(parent)
{

}

FileTreeWidgetItem::FileTreeWidgetItem(QTreeWidgetItem * parent) : QTreeWidgetItem(parent) {

}

FileTreeWidgetItem::~FileTreeWidgetItem()
{

}

void FileTreeWidgetItem::setFileId(QString fi) {
    file_id = fi;
}

void FileTreeWidgetItem::setBookMarkIdFk(QString bi) {
    bookmark_id_fk = bi;
}

void FileTreeWidgetItem::setFilePath(QString fp) {
    file_path = fp;
}

void FileTreeWidgetItem::setFileType(QString ft) {
    file_type = ft;
}

QString FileTreeWidgetItem::getFileId() {
    return file_id;
}

QString FileTreeWidgetItem::getBookMarkIdFk() {
    return bookmark_id_fk;
}

QString FileTreeWidgetItem::getFilePath() {
    return file_path;
}

QString FileTreeWidgetItem::getFileType() {
    return file_type;
}

void FileTreeWidgetItem::createDirectorySubTree(FileTreeWidgetItem* folder) {
    QDir *dir = new QDir(folder->getFilePath());
    QStringList name_filter = QStringList("*");
    QFileInfoList files = dir->entryInfoList(name_filter, QDir::NoDotAndDotDot | QDir::Dirs | QDir::Files);
    QMimeDatabase qmd;
    QIcon text_icon = QIcon(":/icons/icons/text.png");
    QIcon picture_icon = QIcon(":/icons/icons/picture.png");
    QIcon music_icon = QIcon(":/icons/icons/music.png");
    QIcon pdf_icon = QIcon(":/icons/icons/pdf.png");
    QIcon folder_icon = QIcon(":/icons/icons/folder.png");
    foreach(QFileInfo file_from_dir, files) {
        QString mimetype = qmd.mimeTypeForFile(file_from_dir).name();
        if(!mimetype.contains("x-trash")) {
            FileTreeWidgetItem *ftwi = new FileTreeWidgetItem(folder);
            //Pas d'id de fichier
            ftwi->setFileId(QString(""));
            //Pas d'id de bookmark
            ftwi->setBookMarkIdFk(QString(""));
            ftwi->setFilePath(file_from_dir.absoluteFilePath());
            ftwi->setFileType(mimetype);
            ftwi->setText(0, QFileInfo(ftwi->getFilePath()).baseName());
            ftwi->setText(1, ftwi->getFileType());
            ftwi->setToolTip(0, ftwi->getFilePath());
            if(ftwi->getFileType().contains(QRegExp("text/"))) {
                ftwi->setIcon(1, text_icon);
            } else if (ftwi->getFileType().contains(QRegExp("image/"))) {
                ftwi->setIcon(1, picture_icon);
            } else if (ftwi->getFileType().contains(QRegExp("audio/"))) {
                ftwi->setIcon(1, music_icon);
            } else if (ftwi->getFileType().contains(QRegExp("inode/directory"))) {
                ftwi->setIcon(1, folder_icon);
                ftwi->createDirectorySubTree(ftwi);
            } else if (ftwi->getFileType().contains(QRegExp("/pdf"))) {
                ftwi->setIcon(1, pdf_icon);
            }
        }
    }
}
