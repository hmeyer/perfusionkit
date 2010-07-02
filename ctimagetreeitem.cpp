#include "ctimagetreeitem.h"
#include "binaryimagetreeitem.h"
#include <itkMetaDataObject.h>
#include <QPalette>
#include <QInputDialog>
#include <QApplication>
#include <boost/assign.hpp>
#include <itkCastImageFilter.h>
#include "ctimagetreemodel.h"


CTImageTreeItem::CTImageTreeItem(TreeItem * parent, DicomTagListPointer headerFields, const itk::MetaDataDictionary &_dict )
  :BaseClass(parent),HeaderFields(headerFields),dict(_dict) {
    getUIDFromDict(dict, itemUID);
}

TreeItem *CTImageTreeItem::clone(TreeItem *clonesParent) const {
  if (clonesParent==NULL) clonesParent = const_cast<TreeItem*>(parent());
  CTImageTreeItem *c = new CTImageTreeItem( clonesParent, HeaderFields, dict );
  c->fnList = fnList;
  cloneChildren(c);
  return c;
}
    
bool CTImageTreeItem::setData(int column, const QVariant& value) {
  return false;
}

QVariant CTImageTreeItem::do_getData_DisplayRole(int column) const {
  if (column < 0 || column >= int(HeaderFields->size())) return QVariant::Invalid;
  if ((*HeaderFields)[ column ].second == getNumberOfFramesTag()) return getNumberOfSlices();
  std::string val;
  itk::ExposeMetaData( dict, (*HeaderFields)[ column ].second, val );
  return QString::fromStdString( val );
}

QVariant CTImageTreeItem::do_getData_ForegroundRole(int column) const {
  if (itkImage.IsNull()) return QApplication::palette().midlight();
  return QVariant::Invalid;
}


Qt::ItemFlags CTImageTreeItem::flags(int column) const {
    if (column < 0 || column >= int(HeaderFields->size())) return Qt::NoItemFlags;
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;  
}

int CTImageTreeItem::columnCount() const {
  return HeaderFields->size();
}

int CTImageTreeItem::getNumberOfSlices() const {
  if (itkImage.IsNotNull()) {
    ImageType::RegionType imageRegion = itkImage->GetLargestPossibleRegion();
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


CTImageTreeItem::ImageType::Pointer CTImageTreeItem::getITKImage(QProgressDialog *progress, int progressScale, int progressBase) {
  if (itkImage.IsNull()) {
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
    itkImage = imageReader->GetOutput();
    itkImage->Print( std::cerr );
    model->dataChanged(model->createIndex(childNumber(),0,parent()),model->createIndex(childNumber(),columnCount()-1,parent()));
  }
  return itkImage;
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
  }
  BinaryImageTreeItem *result = new BinaryImageTreeItem(this, seg, segName);
  insertChild(result);
  return result;
}

