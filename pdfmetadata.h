#ifndef PDFMETADATA_H
#define PDFMETADATA_H

#include <QString>
#include <QRegExp>
#include <QMap>


#include <podofo/podofo.h>

class PdfMetadata
{
public:
    PdfMetadata();
    PdfMetadata(QString path);
    bool loadData(QString path);
    ~PdfMetadata();
    bool regexSearchPerKey(QRegExp key_reg,QRegExp value_reg);
    QString getAuthor();
    QString getCreator();
    QString getKeywords();
    QString getSubjects();
    QString getTitle();
    QString getDate();
    bool isAuthor(QRegExp reg);
    bool isCreator(QRegExp reg);
    bool isKeywords(QRegExp reg);
    bool isSubjects(QRegExp reg);
    bool isTitle(QRegExp reg);
    bool isDate(QRegExp reg);
    bool regSearchVal(QRegExp reg);

    unsigned int searchRegText(QRegExp reg);
    QStringList getCutText();

private:
    QString m_path;
    QMap<QString,QString> *metadata;
};

#endif // PDFMETADATA_H
