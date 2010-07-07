#ifndef CTIMAGETREEITEM_H
#define CTIMAGETREEITEM_H

#include <itkvtktreeitem.h>
#include <imagedefinitions.h>
#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/set.hpp>

class BinaryImageTreeItem;

class CTImageTreeItem : public ITKVTKTreeItem< CTImageType >
{
  public:
    typedef ITKVTKTreeItem< CTImageType > BaseClass;
//    typedef std::pair< const std::string, const std::string > DicomTagType;
    struct DicomTagType {
      std::string name;
      std::string tag;
      DicomTagType(const std::string &n, const std::string &t):name(n), tag(t) {}
      private:
	friend class boost::serialization::access;
	DicomTagType(){}
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
	  ar & name;
	  ar & tag;
	}
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
    
    void appendFileName( const std::string &fn ) { fnList.insert( fn ); }
    
    BinaryImageTreeItem *generateSegment(void);
    
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

  private:
    friend class boost::serialization::access;
    CTImageTreeItem() {}
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
      ar & boost::serialization::base_object<BaseClass>(*this);
      ar & itemUID;
      ar & fnList;
      ar & HeaderFields;
      ar & dict;
    }
};



#endif // CTIMAGETREEITEM_H
