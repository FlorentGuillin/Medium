#include "pdfmetadata.h"

using namespace PoDoFo;

PdfMetadata::PdfMetadata()
{
    metadata = 0;
}
PdfMetadata::PdfMetadata(QString path)
{
    metadata = 0;

    loadData(path);
}
PdfMetadata::~PdfMetadata()
{
    if(metadata != 0)
        delete metadata;
}
bool PdfMetadata::loadData(QString path)
{
    PdfMemDocument doc(path.toStdString().c_str());

    metadata = new QMap<QString,QString>;
    QString author(doc.GetInfo()->GetAuthor().GetString());
    (*metadata)["Author"] = author;
    QString creator(doc.GetInfo()->GetCreator().GetString());
    (*metadata)["Creator"] = creator;
    QString keywords(doc.GetInfo()->GetKeywords ().GetString());
    (*metadata)["Keywords"] = keywords;
    QString subjects(doc.GetInfo()->GetSubject().GetString());
    (*metadata)["Subjects"] = subjects;
    QString title(doc.GetInfo()->GetTitle().GetString());
    (*metadata)["Title"] = title;
    PdfString d_string;
    doc.GetInfo()->GetCreationDate().ToString(d_string);
    QString date(d_string.GetString());
    (*metadata)["Date"] = date;

    return true;
}
bool PdfMetadata::regexSearchPerKey(QRegExp key_reg,QRegExp value_reg)
{
    /*for(QMap<QString,QString>::iterator it = metadata->begin();
        it != metadata->end();
        it++)
        {
            if(key_reg.indexIn(it.key()) != -1 && value_reg.indexIn(it.value()) != -1)
                return true;
        }
    return false;*/
    if(key_reg.indexIn(QString("Author")) != -1 || key_reg.indexIn(QString("author"))!= -1)
        if(value_reg.indexIn((*metadata)["Author"]) != -1)
            return true;
    if(key_reg.indexIn(QString("Creator")) != -1 || key_reg.indexIn(QString("creator"))!= -1)
        if(value_reg.indexIn((*metadata)["Creator"]) != -1)
            return true;
    if(key_reg.indexIn(QString("Keywords")) != -1 || key_reg.indexIn(QString("keywords"))!= -1)
        if(value_reg.indexIn((*metadata)["Keywords"]) != -1)
            return true;
    if(key_reg.indexIn(QString("Subjects")) != -1 || key_reg.indexIn(QString("subjects"))!= -1)
        if(value_reg.indexIn((*metadata)["Subjects"]) != -1)
            return true;
    if(key_reg.indexIn(QString("Title")) != -1 || key_reg.indexIn(QString("title"))!= -1)
        if(value_reg.indexIn((*metadata)["Title"]) != -1)
            return true;
    if(key_reg.indexIn(QString("Date")) != -1 || key_reg.indexIn(QString("date"))!= -1)
        if(value_reg.indexIn((*metadata)["Date"]) != -1)
            return true;

    return false;
}
QString PdfMetadata::getAuthor(){return (*metadata)["Author"];}
QString PdfMetadata::getCreator(){return (*metadata)["Creator"];}
QString PdfMetadata::getKeywords(){return (*metadata)["Keywords"];}
QString PdfMetadata::getSubjects(){return (*metadata)["Subjects"];}
QString PdfMetadata::getTitle(){return (*metadata)["Title"];}
QString PdfMetadata::getDate(){return (*metadata)["Date"];}

bool PdfMetadata::isAuthor(QRegExp reg)
{
    if(reg.indexIn((*metadata)["Author"]) != -1)
        return true;
    return false;
}
bool PdfMetadata::isCreator(QRegExp reg)
{
    if(reg.indexIn((*metadata)["Creator"]) != -1)
        return true;
    return false;
}
bool PdfMetadata::isKeywords(QRegExp reg)
{
    if(reg.indexIn((*metadata)["Keywords"]) != -1)
        return true;
    return false;
}
bool PdfMetadata::isSubjects(QRegExp reg)
{
    if(reg.indexIn((*metadata)["Subjects"]) != -1)
        return true;
    return false;
}
bool PdfMetadata::isTitle(QRegExp reg)
{
    if(reg.indexIn((*metadata)["Title"]) != -1)
        return true;
    return false;
}
bool PdfMetadata::isDate(QRegExp reg)
{
    if(reg.indexIn((*metadata)["Date"]) != -1)
        return true;
    return false;
}
bool PdfMetadata::regSearchVal(QRegExp reg)
{
    return (isAuthor(reg) || isCreator(reg) || isKeywords(reg) || isSubjects(reg) || isTitle(reg) || isDate(reg));
}
