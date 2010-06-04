#ifndef CTIMAGETREEITEM_H
#define CTIMAGETREEITEM_H

#include <itkvtktreeitem.h>
#include <imagedefinitions.h>

#include <string>


class CTImageTreeItem : public ITKVTKTreeItem< CTImageType >
{
  public:
    typedef ITKVTKTreeItem< CTImageType > BaseClass;
    typedef std::pair< const std::string, const std::string > DicomTagType;
    typedef std::vector< DicomTagType > DicomTagListType;
    CTImageTreeItem(const TreeItem * parent, const DicomTagListType &headerFields, const itk::MetaDataDictionary &_dict=itk::MetaDataDictionary());

    virtual TreeItem *clone(const TreeItem *clonesParent=NULL) const;
    virtual bool setData(int column, const QVariant& value);
    virtual QVariant data(int column, int role = Qt::DisplayRole) const;
    virtual int columnCount() const;
    const std::string &getUID() { return itemUID; }
    void appendFileName( const std::string &fn ) { fnList.insert( fn ); }
    
    static const std::string &getNumberOfFramesTag();
    static const std::string &getSeriesInstanceUIDTag();
    static const std::string &getSOPInstanceUIDTag();
    const DicomTagListType &HeaderFields;
    ImageType::Pointer getITKImage(QProgressDialog *progress = NULL, int progressScale=0, int progressBase=0);
    static void getUIDFromDict(const itk::MetaDataDictionary &dict, std::string &iUID);
  protected:
    class ReaderProgress;
    typedef std::set< std::string > FileNameList;
    int getNumberOfSlices() const;
    std::string itemUID;
    itk::MetaDataDictionary dict;
    FileNameList fnList;

};



#endif // CTIMAGETREEITEM_H
