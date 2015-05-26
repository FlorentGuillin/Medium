#ifndef IMAGEMETADATA_H
#define IMAGEMETADATA_H

#include <QString>
#include <QRegExp>
#include <QMap>
#include <exiv2/exiv2.hpp>


class ImageMetadata
{
public:
    ImageMetadata();
    ImageMetadata(QString path);
    ~ImageMetadata();
    bool loadData(QString path);
    bool hasExifTags()const;
    bool hasIptcTags()const;
    bool hasXmpTags() const;
    bool regexSearchPerKey(QRegExp key_reg,QRegExp value_reg);
    bool regexHasKey(QRegExp regex);
    bool regexHasValue(QRegExp regex);
    QMap<QString,QString>* regexSearchKey(QRegExp regex);
    QMap<QString,QString>* regexSearchValue(QRegExp regex);
    QStringList getMetadata();

private:
    QString m_path;
    bool has_edata;
    bool has_idata;
    bool has_xdata;
    QMap<QString,QString> *ex_mdata_map;
    QMap<QString,QString> *ip_mdata_map;
    QMap<QString,QString> *xm_mdata_map;
    void loadExifTags(Exiv2::ExifData e_data);
    void loadIptcTags(Exiv2::IptcData i_data);
    void loadXmpData(Exiv2::XmpData x_data);

};

#endif // IMAGEMETADATA_H
