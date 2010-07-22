#ifndef CTIMAGETREEITEM_H
#define CTIMAGETREEITEM_H

#include <itkvtktreeitem.h>
#include <imagedefinitions.h>
#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/serialization/access.hpp>
#include "segmentationvalues.h"

class BinaryImageTreeItem;

class CTImageTreeItem : public ITKVTKTreeItem< CTImageType >
{
  public:
    typedef ITKVTKTreeItem< CTImageType > BaseClass;
    struct DicomTagType {
      std::string name;
      std::string tag;
      DicomTagType(const std::string &n, const std::string &t):name(n), tag(t) {}
      private:
	DicomTagType(){}
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version);
    };
    typedef std::vector< DicomTagType > DicomTagList;
    typedef boost::shared_ptr< DicomTagList > DicomTagListPointer;
    CTImageTreeItem(TreeItem * parent, DicomTagListPointer headerFields, const itk::MetaDataDictionary &_dict=itk::MetaDataDictionary());

    virtual TreeItem *clone(TreeItem *clonesParent=NULL) const;
    virtual bool setData(int column, const QVariant& value);
    virtual QVariant do_getData_DisplayRole(int column) const;
    virtual QVariant do_getData_ForegroundRole(int column) const;
    virtual Qt::ItemFlags flags(int column) const;
    virtual int columnCount() const;
    virtual const std::string &getUID() const { return itemUID; }
    double getTime() const;

    bool getSegmentationValues( SegmentationValues &values) const;
    
    
    void appendFileName( const std::string &fn ) { fnList.insert( fn ); }
    
    BinaryImageTreeItem *generateSegment(void);
    
    static const std::string &getNumberOfFramesTag();
    static const std::string &getSeriesInstanceUIDTag();
    static const std::string &getSOPInstanceUIDTag();
    static const std::string &getAcquisitionDatetimeTag();
    ImageType::Pointer getITKImage(QProgressDialog *progress = NULL, int progressScale=0, int progressBase=0) const;
    static void getUIDFromDict(const itk::MetaDataDictionary &dict, std::string &iUID);
    static inline bool isRealHUvalue(CTPixelType value) { return (value!=-2048)?true:false; }
    
    typedef std::map< const ITKVTKTreeItem<BinaryImageType> *, SegmentationValues > SegmentationValueMap;
    
  protected:
    SegmentationValueMap segmentationValueCache;
    bool internalGetSegmentationValues( SegmentationValues &values) const;
    class ReaderProgress;
    typedef std::set< std::string > FileNameList;
    int getNumberOfSlices() const;
    std::string itemUID;
    FileNameList fnList;
    DicomTagListPointer HeaderFields;
    itk::MetaDataDictionary dict;
    double imageTime;

  private:
    friend class boost::serialization::access;
    CTImageTreeItem():imageTime(-1) {}
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version);
};






#endif // CTIMAGETREEITEM_H
