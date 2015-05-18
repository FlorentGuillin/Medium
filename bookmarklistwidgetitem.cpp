#include "bookmarklistwidgetitem.h"

BookmarkListWidgetItem::BookmarkListWidgetItem(QListWidget *parent):QListWidgetItem(parent)
{

}

BookmarkListWidgetItem::~BookmarkListWidgetItem()
{

}

void BookmarkListWidgetItem::setBookmarkId(int bi) {
    bookmark_id = bi;
}

void BookmarkListWidgetItem::setFilter(QString f) {
    filter = f;
}

void BookmarkListWidgetItem::setSearchDirectory(QString sd) {
    search_directory = sd;
}

void BookmarkListWidgetItem::setSearchName(QString sn) {
    search_name = sn;
}

int BookmarkListWidgetItem::getBookmarkId() {
    return bookmark_id;
}

QString BookmarkListWidgetItem::getFilter() {
    return filter;
}

QString BookmarkListWidgetItem::getSearchDirectory() {
    return search_directory;
}

QString BookmarkListWidgetItem::getSearchName() {
    return search_name;
}
