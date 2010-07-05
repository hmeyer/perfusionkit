#include "dicomselectordialog.h"
#include <QFileSystemModel>
#include <QProgressDialog>
#include <QPushButton>
#include <boost/filesystem.hpp>
#include <boost/assign.hpp>
#include <vector>
#include <utility>
#include <list>
#include <itkImageFileReader.h>
#include "imagedefinitions.h"
#include "ctimagetreeitem.h"

const CTImageTreeItem::DicomTagList DicomSelectorDialog::HeaderFields = boost::assign::list_of
  (CTImageTreeItem::DicomTagType("Patient Name", "0010|0010"))
  (CTImageTreeItem::DicomTagType("#Slices",CTImageTreeItem::getNumberOfFramesTag()))
  (CTImageTreeItem::DicomTagType("AcquisitionDatetime","0008|002a"))
  (CTImageTreeItem::DicomTagType("SeriesDate","0008|0021"))
  (CTImageTreeItem::DicomTagType("SeriesTime","0008|0031"))
  (CTImageTreeItem::DicomTagType("Date of Birth","0010|0030"))
  (CTImageTreeItem::DicomTagType("Series Description","0008|103e"));
  
DicomSelectorDialog::DicomSelectorDialog(QWidget * parent, Qt::WindowFlags f):
  QDialog( parent, f ), ctImageModel(HeaderFields) {
  setupUi( this );
  buttonBox->button( QDialogButtonBox::Open )->setAutoDefault(true);
  buttonBox->button( QDialogButtonBox::Cancel)->setAutoDefault(false);
}

void DicomSelectorDialog::reject() {
  treeView->clearSelection();
  QDialog::reject();
}


void DicomSelectorDialog::exec() {
  typedef itk::ImageFileReader< CTImageType >  ReaderType;
  int index = 0; bool canceled = false;
  {
    QProgressDialog indexProgress(tr("Indexing Files..."), tr("Abort"), 0, fileNames.size(), this);
    indexProgress.setMinimumDuration(1000);
    indexProgress.setWindowModality(Qt::ApplicationModal);
    while( index < fileNames.size() ) {
      indexProgress.setValue(index);
      if (indexProgress.wasCanceled()) break;
      if ( boost::filesystem::is_directory( fileNames[index].toStdString() ) ) {
	boost::filesystem::path fpath( fileNames.takeAt(index).toStdString() );
	QList< boost::filesystem::path > pathList;
	boost::filesystem::directory_iterator end_itr; // default construction yields past-the-end
	pathList.push_back( fpath );
	indexProgress.setMaximum(fileNames.size() + pathList.size());
	while( !pathList.isEmpty() ) {
	  if (indexProgress.wasCanceled()) break;
	  boost::filesystem::path currentPath = pathList.takeFirst();
	  for ( boost::filesystem::directory_iterator itr( currentPath );
	    itr != end_itr; ++itr ) {
	      if (indexProgress.wasCanceled()) break;
	      if ( boost::filesystem::is_directory(itr->status()) ) {
		pathList.push_back( itr->path() );
		indexProgress.setMaximum(fileNames.size() + pathList.size());
		indexProgress.setValue(index);
	      }
	      else if ( boost::filesystem::is_regular_file( itr->status() )) {
		fileNames.push_back( itr->path().directory_string().c_str() );
	      }
	  }
	}
      } else {
	index++;
      }
    }
    canceled = indexProgress.wasCanceled();
  }
  fileNames.removeDuplicates();
  if (!canceled ) {
    QProgressDialog metaReadProgress(tr("Reading MetaData..."), tr("Abort"), 0, fileNames.size(), this);
    metaReadProgress.setMinimumDuration(1000);
    metaReadProgress.setWindowModality(Qt::ApplicationModal);
    for(int i = 0; i < fileNames.size(); i++) {
      metaReadProgress.setValue(i);
      if (metaReadProgress.wasCanceled()) break;
      boost::filesystem::path fpath( fileNames[i].toStdString() );
      if ( boost::filesystem::is_regular_file( fpath ) ) {
	try {
	  ReaderType::Pointer reader = ReaderType::New();
	  reader->SetFileName( fpath.string() );
	  reader->GenerateOutputInformation();
	  ctImageModel.appendFilename( reader->GetMetaDataDictionary(), fpath.string() );
	} catch (itk::ImageFileReaderException &ifrExep) {
	} catch (itk::ExceptionObject & excep) {
	  std::cerr << "Exception caught !" << std::endl;
	  std::cerr << excep << std::endl;
	}
      }
    }
  }
  if (ctImageModel.rowCount(QModelIndex())==0) return;
std::cerr << __FILE__ << "[" << __LINE__ << "]:" << __FUNCTION__ << std::endl;
  treeView->setModel( &ctImageModel );
std::cerr << __FILE__ << "[" << __LINE__ << "]:" << __FUNCTION__ << std::endl;
  treeView->selectAll();
  for(unsigned int t=0; t < HeaderFields.size(); t++) treeView->resizeColumnToContents(t);
  QDialog::exec();
}

void DicomSelectorDialog::getSelectedImageDataList(CTImageTreeModel &other) const {
  QItemSelectionModel *selectionModel = treeView->selectionModel();
  if (selectionModel == NULL) return;
  QModelIndexList selectedIndexes = selectionModel->selectedRows();
  for(QModelIndexList::const_iterator it = selectedIndexes.begin(); it != selectedIndexes.end(); it++) {
    other.insertItemCopy( ctImageModel.getItem(*it) );
  }
}