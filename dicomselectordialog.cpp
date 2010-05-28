#include "dicomselectordialog.h"
#include <QFileSystemModel>
#include <QProgressDialog>
#include <boost/filesystem.hpp>
#include <vector>
#include <utility>
#include <list>
#include <itkImageFileReader.h>


DicomSelectorDialog::DicomSelectorDialog(QWidget * parent, Qt::WindowFlags f):
  QDialog( parent, f ) {
  setupUi( this );
}

void DicomSelectorDialog::reject() {
  treeView->clearSelection();
  QDialog::reject();
}


void DicomSelectorDialog::exec() {
  typedef itk::ImageFileReader< DicomImageListModel::ImageType >  ReaderType;
  int index = 0;
  {
    QProgressDialog indexProgress(tr("Indexing Files..."), tr("Abort"), 0, fileNames.size(), this);
    indexProgress.setMinimumDuration(1000);
    indexProgress.setWindowModality(Qt::WindowModal);
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
	  boost::filesystem::path currentPath = pathList.takeFirst();
	  for ( boost::filesystem::directory_iterator itr( currentPath );
	    itr != end_itr; ++itr ) {
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
  }
  fileNames.removeDuplicates();
  {
    QProgressDialog metaReadProgress(tr("Reading MetaData..."), tr("Abort"), 0, fileNames.size(), this);
    metaReadProgress.setMinimumDuration(1000);
    metaReadProgress.setWindowModality(Qt::WindowModal);
    for(int i = 0; i < fileNames.size(); i++) {
      metaReadProgress.setValue(i);
      if (metaReadProgress.wasCanceled()) break;
      boost::filesystem::path fpath( fileNames[i].toStdString() );
      if ( boost::filesystem::is_regular_file( fpath ) ) {
	try {
	  ReaderType::Pointer reader = ReaderType::New();
	  reader->SetFileName( fpath.string() );
	  reader->GenerateOutputInformation();
	  dicomModel.appendFilename( reader->GetMetaDataDictionary(), fpath.string() );
	} catch (itk::ImageFileReaderException &ifrExep) {
	} catch (itk::ExceptionObject & excep) {
	  std::cerr << "Exception caught !" << std::endl;
	  std::cerr << excep << std::endl;
	}
      }
    }
  }
  if (!dicomModel.rowCount(QModelIndex())) return;
  treeView->setModel( &dicomModel );
  treeView->selectAll();
  for(unsigned int t=0; t < DicomImageListModel::dicomSelectorFields.size(); t++) treeView->resizeColumnToContents(t);
  QDialog::exec();
}

DicomImageListModel::Pointer DicomSelectorDialog::getSelectedImageDataList(void) {
  DicomImageListModel::Pointer result( new DicomImageListModel );
  QItemSelectionModel *selectionModel = treeView->selectionModel();
  if (selectionModel == NULL) return result;
  QModelIndexList selectedIndexes = selectionModel->selectedRows();
  for(QModelIndexList::const_iterator it = selectedIndexes.begin(); it != selectedIndexes.end(); it++) {
    result->appendItem( dicomModel.getItem(*it) );
  }
  return result;
}