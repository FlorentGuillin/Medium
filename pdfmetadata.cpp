#include "pdfmetadata.h"
#include <stack>
#include <iostream>

using namespace PoDoFo;

PdfMetadata::PdfMetadata()
{
    metadata = 0;
}
PdfMetadata::PdfMetadata(QString path)
{
    metadata = 0;
    m_path = path;
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

unsigned int PdfMetadata::searchRegText(QRegExp reg)
{
    unsigned int occ_number = 0;
    bool is_text = false;
    PdfMemDocument *pdf_doc;

    try{
        pdf_doc= new PdfMemDocument (m_path.toStdString().c_str());
    }catch(PoDoFo::PdfError err){
        std::cerr<<err.what()<<std::endl;
    }

    for (int curr_page = 0; curr_page < pdf_doc->GetPageCount(); ++curr_page) {

        PoDoFo::PdfPage* pdf_page = pdf_doc->GetPage(curr_page);

        PoDoFo::PdfContentsTokenizer tok(pdf_page);

        //ReadNext parameters
        const char* token = 0;
        PoDoFo::PdfVariant var;
        PoDoFo::EPdfContentsType type;

        std::stack<PdfVariant> stack;
        QString page_str("");

        while (tok.ReadNext(type, token, var)) {
            if (type == PoDoFo::ePdfContentsType_Keyword) {
                if(strcmp(token,"BT") == 0)
                {
                    //debut du texte
                    is_text = true;
                }else if(strcmp(token,"ET") == 0)
                {
                    //fin de texte
                    is_text = false;
                }else if( strcmp( token, "l" ) == 0 ||
                             strcmp( token, "m" ) == 0 )
                {
                     stack.pop();
                     stack.pop();
                }
                if( is_text)
                {
                    if( strcmp( token, "Tf" ) == 0 )
                    {
                        stack.pop();
                    }else if( strcmp( token, "Tj" ) == 0 ||
                              strcmp( token, "'" ) == 0 )
                     {

                        page_str = page_str+ QString(stack.top().GetString().GetString());
                         stack.pop();
                     }else if(strcmp( token, "\"" ) == 0){
                        page_str = page_str + QString(stack.top().GetString().GetString());
                        stack.pop();
                        stack.pop(); // remove char spacing from stack

                        stack.pop(); // remove word spacing from stack
                    }else if( strcmp( token, "TJ" ) == 0 )
                    {
                        PdfArray array = stack.top().GetArray();
                        stack.pop();
                        //QString word_qstr("");


                        for( int i=0; i<static_cast<int>(array.GetSize()); i++ )
                        {
                            if( array[i].IsString() || array[i].IsHexString())
                            {
                                page_str = page_str + QString(array[i].GetString().GetString());
                            }
                        }
                    }
                }
            }else if(type == PoDoFo::ePdfContentsType_Variant){
                stack.push( var );

            }

            /*if(occ_number != 0)
                std::cout<<occ_number<<std::endl;*/
        }
        int curr_index= reg.indexIn(page_str);

        while(curr_index !=-1 && curr_index != -2)
        {
            curr_index += reg.matchedLength();

            occ_number++;
            std::cerr<<occ_number<<std::endl;
            curr_index = reg.indexIn(page_str,curr_index);
        }
    }
    return occ_number;
}
