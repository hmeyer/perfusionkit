#include "ctimagetreeitem.h"
#include "binaryimagetreeitem.h"
#include <itkMetaDataObject.h>
#include <itkImageRegionConstIteratorWithIndex.h>
#include <itkLinearInterpolateImageFunction.h>
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
    
bool CTImageTreeItem::getSegmentationValues( SegmentationValues &values) const {
  SegmentationValueMap::const_iterator it = segmentationValueCache.find( values.segment );
  if (it != segmentationValueCache.end() 
    && it->second.mtime == values.segment->getITKMTime()
    && it->second.accuracy == values.accuracy) {
    values = it->second;
    return values.sampleCount > 0;
  }
  return internalGetSegmentationValues( values );
}

bool CTImageTreeItem::internalGetSegmentationValues( SegmentationValues &values) const {
  values.min = itk::NumericTraits< int >::max();
  values.max = itk::NumericTraits< int >::min();
  values.mean = 0; values.stddev = 0; values.sampleCount = 0;
  std::vector<int> samples;
  ImageType::Pointer image = getITKImage();
  ImageType::RegionType ctregion = image->GetBufferedRegion();
  typedef itk::ImageRegionConstIteratorWithIndex< BinaryImageType > BinaryIteratorType;
  BinaryImageType::Pointer segment = values.segment->getITKImage();
  BinaryIteratorType binIter( segment, segment->GetBufferedRegion() );
  ImageType::PointType point;
  if (values.accuracy == SegmentationValues::SimpleAccuracy) {
    ImageType::IndexType index;
    for(binIter.GoToBegin(); !binIter.IsAtEnd(); ++binIter) {
      if (binIter.Value() == BinaryPixelOn) {
	segment->TransformIndexToPhysicalPoint(binIter.GetIndex(),point);
	image->TransformPhysicalPointToIndex(point, index);
	if (ctregion.IsInside(index)) {
	  int t = image->GetPixel(index);
	  if (isRealHUvalue(t)) {
	    samples.push_back(t);
	    values.mean += t;
	    values.min = std::min( t, values.min );
	    values.max = std::max( t, values.max );
	  }
	}
      }
    }
  } else if (values.accuracy == SegmentationValues::NonMultiSamplingAccuracy) {
    ImageType::IndexType index;
    typedef std::set< ImageType::IndexType, IndexCompareFunctor > IndexSetType;
    IndexSetType indexSet;
    for(binIter.GoToBegin(); !binIter.IsAtEnd(); ++binIter) {
      if (binIter.Value() == BinaryPixelOn) {
	segment->TransformIndexToPhysicalPoint(binIter.GetIndex(),point);
	image->TransformPhysicalPointToIndex(point, index);
	if (ctregion.IsInside(index)) {
	  std::pair<IndexSetType::iterator,IndexSetType::iterator> ret;
	  ret = indexSet.equal_range(index);
	  if (ret.first == ret.second) {
	    indexSet.insert(ret.first, index);
	    int t = image->GetPixel(index);
	    if (isRealHUvalue(t)) {
	      samples.push_back(t);
	      values.mean += t;
	      values.min = std::min( t, values.min );
	      values.max = std::max( t, values.max );
	    }
	  }
	}
      }
    }
  } else if (values.accuracy == SegmentationValues::InterpolatedAccuray) {
    typedef  itk::LinearInterpolateImageFunction< CTImageType > InterpolatorType;
    InterpolatorType::Pointer interpolator = InterpolatorType::New();
    interpolator->SetInputImage( image );
    InterpolatorType::ContinuousIndexType index;
    for(binIter.GoToBegin(); !binIter.IsAtEnd(); ++binIter) {
      if (binIter.Value() == BinaryPixelOn) {
	segment->TransformIndexToPhysicalPoint(binIter.GetIndex(),point);
	image->TransformPhysicalPointToContinuousIndex(point, index);
	if (interpolator->IsInsideBuffer(index)) {
	  int t = interpolator->EvaluateAtContinuousIndex(index);
	  if (isRealHUvalue(t)) {
	    samples.push_back(t);
	    values.mean += t;
	    values.min = std::min( t, values.min );
	    values.max = std::max( t, values.max );
	  }
	}
      }
    }
    
  }
  if (samples.size() > 0) {
    values.mean /= samples.size();
    for(unsigned i=0;i<samples.size();i++) {
      double t = samples[i] - values.mean;
      values.stddev += t * t;
    }
    values.stddev = std::sqrt(values.stddev/samples.size());
  }
  values.sampleCount = samples.size();
  values.mtime = segment->GetMTime();
  const_cast<CTImageTreeItem*>(this)->segmentationValueCache[ values.segment ] = values;
  return values.sampleCount > 0;
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

