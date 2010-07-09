#include "ctimagetreeitem.h"
#include "binaryimagetreeitem.h"
#include <itkMetaDataObject.h>
#include <itkImageRegionConstIteratorWithIndex.h>
#include <QPalette>
#include <QInputDialog>
#include <QApplication>
#include <boost/assign.hpp>
#include <itkCastImageFilter.h>
#include "ctimagetreemodel.h"
#include <boost/date_time/posix_time/posix_time.hpp>
#include <sstream>
#include <set>

CTImageTreeItem::CTImageTreeItem(TreeItem * parent, DicomTagListPointer headerFields, const itk::MetaDataDictionary &_dict )
  :BaseClass(parent),HeaderFields(headerFields),dict(_dict),imageTime(-1) {
    getUIDFromDict(dict, itemUID);
}

TreeItem *CTImageTreeItem::clone(TreeItem *clonesParent) const {
  if (clonesParent==NULL) clonesParent = const_cast<TreeItem*>(parent());
  CTImageTreeItem *c = new CTImageTreeItem( clonesParent, HeaderFields, dict );
  c->fnList = fnList;
  cloneChildren(c);
  return c;
}


struct IndexCompareFunctor {
  typedef CTImageType::IndexType argT;
  bool operator()(const argT &x, const argT &y) const {
    if (x[0] < y[0]) return true;
    else if (x[0] > y[0]) return false;
    
    else if (x[1] < y[1]) return true;
    else if (x[1] > y[1]) return false;
    
    else if (x[2] < y[2]) return true;
    else /*if (x[2] > y[2])*/ return false;
  }
};
    
bool CTImageTreeItem::getSegmentationValues( const BinaryImageTreeItem *segment, double &mean, double &stddev, int &min, int &max) const {
  min = itk::NumericTraits< int >::max();
  max = itk::NumericTraits< int >::min();
  mean = 0; stddev = 0;
  std::vector<int> samples;
  BinaryImageType::Pointer itkSeg = segment->getITKImage();
  ImageType::Pointer image = getITKImage();
  typedef itk::ImageRegionConstIteratorWithIndex< BinaryImageType > BinaryIteratorType;
  BinaryIteratorType binIter( itkSeg, itkSeg->GetBufferedRegion() );
  ImageType::PointType point;
  ImageType::IndexType index;
  std::set< ImageType::IndexType, IndexCompareFunctor > indexSet;
  for(binIter.GoToBegin(); !binIter.IsAtEnd(); ++binIter) {
    if (binIter.Value() == BinaryPixelOn) {
      itkSeg->TransformIndexToPhysicalPoint(binIter.GetIndex(),point);
      image->TransformPhysicalPointToIndex(point, index);
      if (indexSet.find(index) == indexSet.end()) {
	indexSet.insert(index);
	int t = image->GetPixel(index);
	if (isRealHUvalue(t)) {
	  samples.push_back(t);
	  mean += t;
	  min = std::min( t, min );
	  max = std::max( t, max );
	}
      }
    }
  }
  if (samples.size()==0) return false;
  mean /= samples.size();
  for(unsigned i=0;i<samples.size();i++) {
    double t = samples[i] - mean;
    stddev += t * t;
  }
  stddev = std::sqrt(stddev/samples.size());
  return true;
}    
    
    
bool CTImageTreeItem::setData(int column, const QVariant& value) {
  return false;
}

QVariant CTImageTreeItem::do_getData_DisplayRole(int column) const {
  if (column < 0 || column >= int(HeaderFields->size())) return QVariant::Invalid;
  if ((*HeaderFields)[ column ].tag == getNumberOfFramesTag()) return getNumberOfSlices();
  std::string val;
  itk::ExposeMetaData( dict, (*HeaderFields)[ column ].tag, val );
  return QString::fromStdString( val );
}

QVariant CTImageTreeItem::do_getData_ForegroundRole(int column) const {
  if (peekITKImage().IsNull()) return QApplication::palette().midlight();
  return QVariant::Invalid;
}

double CTImageTreeItem::getTime() const {
  if (imageTime > 0) return imageTime;
  std::string dicomTimeString;
  itk::ExposeMetaData( dict, getAcquisitionDatetimeTag(), dicomTimeString );
  using namespace boost::posix_time;
  time_input_facet *dicomTimeFacet = new time_input_facet("%Y%m%d%H%M%S%F");
  std::stringstream dicomTimeStream;
  dicomTimeStream.imbue( std::locale(std::locale::classic(), dicomTimeFacet) );
  dicomTimeStream.str( dicomTimeString );
  ptime dicomTime;
  dicomTimeStream >> dicomTime;
  time_duration since1900 = dicomTime - ptime(boost::gregorian::date(1900,1,1));
  double secSince1900 = double(since1900.ticks()) / time_duration::ticks_per_second();
  return secSince1900;
}


Qt::ItemFlags CTImageTreeItem::flags(int column) const {
    if (column < 0 || column >= int(HeaderFields->size())) return Qt::NoItemFlags;
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;  
}

int CTImageTreeItem::columnCount() const {
  return HeaderFields->size();
}

int CTImageTreeItem::getNumberOfSlices() const {
  if (peekITKImage().IsNotNull()) {
    ImageType::RegionType imageRegion = peekITKImage()->GetLargestPossibleRegion();
    return static_cast<uint>(imageRegion.GetSize(2));
  } else {
    int num = fnList.size();
    if (num > 1) return num;
    else {
      std::string t;
      itk::ExposeMetaData( dict, getNumberOfFramesTag(), t );
      std::istringstream buffer(t);
      buffer >> num;
      return num;
    }  
  }
}

class CTImageTreeItem::ReaderProgress : public itk::Command {
  public: 
    typedef ReaderProgress Self;
    typedef itk::Command Superclass;
    typedef itk::SmartPointer<Self> Pointer;
    typedef itk::ImageSeriesReader< ImageType > ReaderType;
    itkNewMacro( Self );
    void setDialog( QProgressDialog *progress, int progressScale, int progressBase ) {
      if (progressDialog_scpd) progressDialog_scpd.reset(NULL);
      progressDialog = progress;
      this->progressScale = progressScale;
      this->progressBase = progressBase;
    }
  protected:
    const static int defaultProgressScale = 10000;
    ReaderProgress():progressScale(defaultProgressScale), progressBase(0), progressDialog(NULL) {};
    void Execute( itk::Object *caller, const itk::EventObject &event) {
      Execute((const itk::Object*)caller, event);
    }
    void Execute( const itk::Object *caller, const itk::EventObject &event) {
      const ReaderType *reader = dynamic_cast<const ReaderType *>(caller);
      getDialog()->setValue( progressBase + reader->GetProgress() * progressScale );
    }
    QProgressDialog *getDialog(void) {
      if (progressDialog == NULL) {
	progressDialog_scpd.reset(new QProgressDialog(QObject::tr("Loading Volume..."), QObject::tr("Abort"), 0, progressScale));
	progressDialog = progressDialog_scpd.get();
	progressDialog->setCancelButton(0);
	progressDialog->setMinimumDuration(1000);
	progressDialog->setWindowModality(Qt::ApplicationModal);
      }
      return progressDialog;
    }
    int progressScale;
    int progressBase;
    boost::scoped_ptr< QProgressDialog > progressDialog_scpd;
    QProgressDialog *progressDialog;
};




CTImageTreeItem::ImageType::Pointer CTImageTreeItem::getITKImage(QProgressDialog *progress, int progressScale, int progressBase) const {
  if (BaseClass::getITKImage().IsNull()) {
    typedef ReaderProgress::ReaderType ReaderType;
    ReaderType::Pointer imageReader = ReaderType::New();
    ReaderType::FileNamesContainer fc;
    fc.assign(fnList.begin(), fnList.end());

    itk::GDCMImageIO::Pointer gdcmImageIO = itk::GDCMImageIO::New();
    imageReader->SetImageIO( gdcmImageIO );
    imageReader->SetFileNames(fc);
    ReaderProgress::Pointer progressor = ReaderProgress::New();
    if (progress) progressor->setDialog( progress, progressScale, progressBase );
    imageReader->AddObserver(itk::AnyEvent(), progressor);
    try {
	    imageReader->Update();
    }catch( itk::ExceptionObject & excep ) {
	    std::cerr << "Exception caught !" << std::endl;
	    std::cerr << excep << std::endl;
    }
    CTImageType::Pointer result =  imageReader->GetOutput();
    const_cast<CTImageTreeItem*>(this)->setITKImage( result );
    model->dataChanged(model->createIndex(childNumber(),0,parent()),model->createIndex(childNumber(),columnCount()-1,parent()));
  }
  return BaseClass::getITKImage();
}

void CTImageTreeItem::getUIDFromDict(const itk::MetaDataDictionary &dict, std::string &iUID) {
  std::string nFrames;
  itk::ExposeMetaData( dict, getNumberOfFramesTag(), nFrames );
  if (nFrames.empty() || nFrames == "0") itk::ExposeMetaData( dict, getSeriesInstanceUIDTag(), iUID );
  else  itk::ExposeMetaData( dict, getSOPInstanceUIDTag(), iUID );
}

const std::string &CTImageTreeItem::getNumberOfFramesTag() {
  const static std::string NumberOfFramesTag("0028|0008");
  return NumberOfFramesTag;
}
const std::string &CTImageTreeItem::getSOPInstanceUIDTag() {
  const static std::string SOPInstanceUIDTag("0008|0018");
  return SOPInstanceUIDTag;
}
const std::string &CTImageTreeItem::getSeriesInstanceUIDTag() {
  const static std::string SeriesInstanceUIDTag("0020|000e");
  return SeriesInstanceUIDTag;
}
const std::string &CTImageTreeItem::getAcquisitionDatetimeTag() {
  const static std::string SOPInstanceUIDTag("0008|002a");
  return SOPInstanceUIDTag;
}

BinaryImageTreeItem *CTImageTreeItem::generateSegment(void) {
  typedef itk::CastImageFilter< CTImageType, BinaryImageType> CastFilterType;
  
  bool ok;
  QString segName = QInputDialog::getText(NULL, QObject::tr("Segment Name"),
				      QObject::tr("Name:"), QLineEdit::Normal,
				      QObject::tr("Unnamed Segment"), &ok);
  BinaryImageType::Pointer seg;
  if (ok && !segName.isEmpty()) {
    CastFilterType::Pointer caster = CastFilterType::New();
    caster->SetInput( getITKImage() );
    caster->Update();
    seg = caster->GetOutput();
    seg->FillBuffer(BinaryPixelOff);
  }
  BinaryImageTreeItem *result = new BinaryImageTreeItem(this, seg, segName);
  insertChild(result);
  return result;
}

