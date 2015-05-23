#include "audiometadata.h"

AudioMetadata::AudioMetadata()
{
}
AudioMetadata::AudioMetadata(QString path)
{
    m_path = path;
    has_data = false;

    loadData(path);

}
AudioMetadata::~AudioMetadata()
{}
bool AudioMetadata::loadData(QString path)
{
    TagLib::FileRef f(path.toStdString().c_str());
    TagLib::Tag *t = f.tag();

    has_data = false;
    if(t != 0 && !t->isEmpty())
    {
        //tags_key_value = new QMap<QString,QString>;
        has_data = true;

        //get title
        QString title(t->title().toCString());
        m_title = title;

        //get artist
        QString artist(t->artist().toCString());
        m_artist = artist;

        //get album
        QString album(t->album().toCString());
        m_album = album;

        //get genre
        QString genre(t->genre().toCString());
        m_genre = genre;

        //get year
        m_year = t->year();
    }

    return has_data;
}
bool AudioMetadata::regexSearchPerKey(QRegExp key_reg, QRegExp value_reg)
{
    if(has_data)
    {
        if(key_reg.indexIn(QString("Artist")) != -1 ||
            key_reg.indexIn(QString("artist")) != -1)
        {
            if(value_reg.indexIn(m_artist) != -1 )
                return true;
        }
        if(key_reg.indexIn(QString("Album")) != -1 ||
                key_reg.indexIn(QString("album")) != -1)
        {
            if(value_reg.indexIn(m_album) != -1 )
                    return true;
        }
        if(key_reg.indexIn(QString("Title")) != -1 ||
            key_reg.indexIn(QString("title")) != -1)
        {
            if(value_reg.indexIn(m_title) != -1 )
                return true;
        }
        if(key_reg.indexIn(QString("Genre")) != -1 ||
            key_reg.indexIn(QString("genre")) != -1)
        {
            if(value_reg.indexIn(m_genre) != -1 )
                return true;
        }
        if(key_reg.indexIn(QString("Year")) != -1 ||
                key_reg.indexIn(QString("year")) != -1)
        {
                if(value_reg.indexIn(QString(QString::number(m_year))) != -1 )
                    return true;
        }

    }

    return false;
}
//QMap<QString,QString>* AudioMetadata::regexSearchKey(QRegExp regex);

bool AudioMetadata::isValue(QRegExp regex)
{
    if(has_data)
    {
        if(regex.indexIn(m_artist) != -1 )
            return true;
        if(regex.indexIn(m_artist) != -1 )
                return true;
        if(regex.indexIn(m_title) != -1 )
            return true;
        if(regex.indexIn(m_genre) != -1 )
            return true;
        if(regex.indexIn(QString(QString::number(m_year))) != -1 )
            return true;
    }
    return false;
}
bool AudioMetadata::isAlbum(QRegExp reg)
{
    if(reg.indexIn(m_album) != -1 )
        return true;
    return false;
}
bool AudioMetadata::isArtist(QRegExp reg)
{
    if(reg.indexIn(m_artist) != -1 )
        return true;
    return false;
}
bool AudioMetadata::isTitle(QRegExp reg)
{
    if(reg.indexIn(m_title) != -1 )
        return true;
    return false;
}
bool AudioMetadata::isGenre(QRegExp reg)
{
    if(reg.indexIn(m_genre) != -1 )
        return true;
    return false;
}
bool AudioMetadata::isYear(unsigned int year)
{
    return (year == m_year);
}
