#ifndef BOOKMARKLISTWIDGETITEM_H
#define BOOKMARKLISTWIDGETITEM_H

#include <QString>
#include <QListWidgetItem>


class BookmarkListWidgetItem : public QListWidgetItem
{
public:
    explicit BookmarkListWidgetItem(QListWidget *parent);
    ~BookmarkListWidgetItem();
    void setBookmarkId(int bi);
    void setFilter(QString f);
    void setSearchDirectory(QString sd);
    void setSearchName(QString sn);
    int getBookmarkId();
    QString getFilter();
    QString getSearchDirectory();
    QString getSearchName();

private:
    int bookmark_id;
    QString filter;
    QString search_directory;
    QString search_name;
};

#endif // BOOKMARKLISTWIDGETITEM_H
