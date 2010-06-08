#ifndef CTIMAGETREEITEM_H
#define CTIMAGETREEITEM_H

#include <itkvtktreeitem.h>
#include <imagedefinitions.h>
#include <string>
#include <boost/shared_ptr.hpp>


class CTImageTreeItem : public ITKVTKTreeItem< CTImageType >
{
  public:
    typedef ITKVTKTreeItem< CTImageType > BaseClass;
    typedef std::pair< const std::string, const std::string > DicomTagType;
    typedef std::vector< DicomTagType > DicomTagList;
    typedef boost::shared_ptr< const DicomTagList > DicomTagListPointer;
    CTImageTreeItem(const TreeItem * parent, DicomTagListPointer headerFields, const itk::MetaDataDictionary &_dict=itk::MetaDataDictionary());

    virtual TreeItem *clone(const TreeItem *clonesParent=NULL) const;
    virtual bool setData(int column, const QVariant& value);
    virtual QVariant data(int column, int role = Qt::DisplayRole) const;
    virtual Qt::ItemFlags flags(int column) const;
    virtual int columnCount() const;
    const std::string &getUID() { return itemUID; }
    void appendFileName( const std::string &fn ) { fnList.insert( fn ); }
    
    bool generateSegment(void);
    
    static const std::string &getNumberOfFramesTag();
    static const std::string &getSeriesInstanceUIDTag();
    static const std::string &getSOPInstanceUIDTag();
    ImageType::Pointer getITKImage(QProgressDialog *progress = NULL, int progressScale=0, int progressBase=0);
    static void getUIDFromDict(const itk::MetaDataDictionary &dict, std::string &iUID);
  protected:
    class ReaderProgress;
    typedef std::set< std::string > FileNameList;
    int getNumberOfSlices() const;
    std::string itemUID;
    FileNameList fnList;
    DicomTagListPointer HeaderFields;
    itk::MetaDataDictionary dict;

};



#endif // CTIMAGETREEITEM_H
