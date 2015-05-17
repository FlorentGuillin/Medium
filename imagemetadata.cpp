#include "imagemetadata.h"
#include <cassert>

using namespace Exiv2;
// todo class wqui cherche le type d'image et si c'est une image (à faire pas ici faire nouvelle classe)
ImageMetadata::ImageMetadata()
{
    has_edata = false;
    ex_mdata_map = 0;
    has_idata = false;
    ip_mdata_map = 0;
    has_xdata = false;
    xm_mdata_map = 0;
}
ImageMetadata::ImageMetadata(QString path)
{
    m_path = path;
    has_edata = false;
    has_idata = false;
    has_xdata = false;
    loadData(path);
}
ImageMetadata::~ImageMetadata()
{
    if(has_edata && ex_mdata_map != 0)
        delete ex_mdata_map;

    if(has_idata && ip_mdata_map != 0)
        delete ip_mdata_map;

    if(has_xdata && xm_mdata_map != 0)
        delete xm_mdata_map;
}

bool ImageMetadata::loadData(QString path)
{
    Image::AutoPtr image = ImageFactory::open(path.toStdString());
    image->readMetadata();
    bool metadata_found = false;

    ExifData exifData = image->exifData();
    if (!exifData.empty())
    {
        std::cerr<<"has e data"<<std::endl;
        has_edata = true;
        loadExifTags(exifData);
        metadata_found = true;
    }else{
        has_edata = false;
    }

    IptcData idata = image->iptcData ();
    if(!idata.empty())
    {
        has_idata =true;
        loadIptcTags(idata);
        metadata_found = true;
    }else{
        has_idata = false;
    }

    XmpData xdata = image->xmpData();
    if(!xdata.empty())
    {
        has_xdata = true;
        loadXmpData(xdata);
        metadata_found = true;
    }else{
        has_xdata = false;
    }
    if(!metadata_found)
    {
        //error handling
        std::cerr<<"no metadata found !"<<std::endl;
    }
    image.release();

    return metadata_found;
}
void ImageMetadata::loadExifTags(ExifData e_data)
{
    ex_mdata_map = new QMap<QString,QString>;

    if(has_edata)
    {
        for(ExifData::iterator it = e_data.begin(); it != e_data.end(); ++it)
        {
            QString key_qstr(it->key().c_str());
            QString val_qstr(it->value().toString().c_str());
            (*ex_mdata_map)[key_qstr] = val_qstr;
        }
    }
}
void ImageMetadata::loadIptcTags(IptcData i_data)
{
    ip_mdata_map = new QMap<QString,QString>;

    if(has_idata)
    {
        for(IptcData::iterator it = i_data.begin(); it != i_data.end(); ++it)
        {
            QString key_qstr(it->key().c_str());
            QString val_qstr(it->value().toString().c_str());
            (*ip_mdata_map)[key_qstr] = val_qstr;
        }
    }
}
void ImageMetadata::loadXmpData(XmpData x_data)
{
    xm_mdata_map = new QMap<QString,QString>;

    if(has_xdata)
    {
        for(XmpData::iterator it = x_data.begin(); it != x_data.end(); ++it)
        {
            QString key_qstr(it->key().c_str());
            QString val_qstr(it->value().toString().c_str());
            (*xm_mdata_map)[key_qstr] = val_qstr;
        }
    }

}

QMap<QString,QString>* ImageMetadata::regexSearchKey(QRegExp regex)
{
    QMap<QString,QString> *ret_value = new QMap<QString,QString>;

    if(has_edata)
    {
        for(QMap<QString,QString>::iterator it = ex_mdata_map->begin();
            it != ex_mdata_map->end();
            it++)
        {
            if(regex.indexIn(it.key()) !=-1)
            {
                (*ret_value)[it.key()] = it.value();
            }
        }
    }
    if(has_idata)
    {        
        for(QMap<QString,QString>::iterator it = ip_mdata_map->begin();
            it != ip_mdata_map->end();
            it++)
        {
            if(regex.indexIn(it.key()) !=-1)
            {
                (*ret_value)[it.key()] = it.value();
            }
        }
    }
    if(has_xdata)
    {
        for(QMap<QString,QString>::iterator it = xm_mdata_map->begin();
            it != xm_mdata_map->end();
            it++)
        {
            if(regex.indexIn(it.key()) !=-1)
            {
                (*ret_value)[it.key()] = it.value();
            }
        }
    }

    return ret_value;
}

QMap<QString,QString>* ImageMetadata::regexSearchValue(QRegExp regex)
{
    QMap<QString,QString> *ret_value = new QMap<QString,QString>;
    std::cerr<<has_edata<<std::endl;

    if(has_edata)
    {
        for(QMap<QString,QString>::iterator it = ex_mdata_map->begin();
            it != ex_mdata_map->end();
            it++)
        {
            if(regex.indexIn(it.value()) !=-1)
            {
                (*ret_value)[it.key()] = it.value();
            }
        }
    }
    if(has_idata)
    {
        for(QMap<QString,QString>::iterator it = ip_mdata_map->begin();
            it != ip_mdata_map->end();
            it++)
        {
            if(regex.indexIn(it.value()) !=-1)
            {
                (*ret_value)[it.key()] = it.value();
            }
        }
    }
    if(has_xdata)
    {
        for(QMap<QString,QString>::iterator it = xm_mdata_map->begin();
            it != xm_mdata_map->end();
            it++)
        {
            if(regex.indexIn(it.value()) !=-1)
            {
                (*ret_value)[it.key()] = it.value();
            }
        }
    }

    return ret_value;
}
bool ImageMetadata::hasExifTags() const
{
    return has_edata;
}
bool ImageMetadata::hasIptcTags() const
{
    return has_idata;
}
bool ImageMetadata::hasXmpTags() const
{
    return has_xdata;
}
