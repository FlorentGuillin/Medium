#ifndef AUDIOMETADATA_H
#define AUDIOMETADATA_H

#include <QString>
#include <QRegExp>
#include <QMap>
#include <taglib/tlist.h>
#include <taglib/fileref.h>
#include <taglib/tfile.h>
#include <taglib/tag.h>

/*
 * recherche par cle/valeur sur un ensemble de tag dont les cles sont parmis
 * title, artist, album et genre
 *
 *
 * */
class AudioMetadata
{
public:
    AudioMetadata();
    AudioMetadata(QString path);
    ~AudioMetadata();
    bool loadData(QString path);
    bool regexSearchPerKey(QRegExp key_reg,QRegExp value_reg);
    //QMap<QString,QString>* regexSearchKey(QRegExp regex);
    //QMap<QString,QString>* regexSearchValue(QRegExp regex);
    bool isValue(QRegExp regex);
    QString getAlbum(){return m_album;}
    QString getArtist(){return m_artist;}
    QString getTitle(){return m_title;}
    QString getGenre(){return m_genre;}
    unsigned int getYear(){return m_year;}
    bool isAlbum(QRegExp reg);
    bool isArtist(QRegExp reg);
    bool isTitle(QRegExp reg);
    bool isGenre(QRegExp reg);
    bool isYear(unsigned int year);

private:
    QString m_path;
    QString m_album;
    QString m_artist;
    QString m_title;
    QString m_genre;
    unsigned int m_year;
    bool has_data;


};

#endif // AUDIOMETADATA_H
