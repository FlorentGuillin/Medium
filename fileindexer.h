#ifndef FILEINDEXER_H
#define FILEINDEXER_H

#include <QDir>
#include <QDirIterator>
#include <QSqlQuery>
#include <QMimeDatabase>
#include <QDateTime>
#include "pdfmetadata.h"
#include "imagemetadata.h"
#include "audiometadata.h"

class FileIndexer
{
public:
    FileIndexer();
    ~FileIndexer();
    void buildIndex(QDir *dir);
    QDateTime buildDateTimeFromQString(QString d);
    QStringList loadTextContent(QString file_path, QStringList useless_words);
    QStringList loadAudioContent(QString file_path, QStringList useless_words);
    QStringList loadImageContent(QString file_path, QStringList useless_words);
    QStringList loadPDFContent(QString file_path, QStringList useless_words);
    QString removeAccentsAndToLower(QString string);
    QString removePunctuation(QString string);
};

#endif // FILEINDEXER_H
