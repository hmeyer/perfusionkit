#ifndef DICOMSELECTORDIALOG_H
#define DICOMSELECTORDIALOG_H

#include <QDialog>
#include "ui_DicomSelectDialog.h"
#include <boost/shared_ptr.hpp>
#include <boost/assign.hpp>
#include "dicomimagelistmodel.h"


class DicomSelectorDialog : public QDialog, private Ui_DicomSelectDialog
{
  public:
  DicomSelectorDialog(QWidget * parent = 0, Qt::WindowFlags f = 0);
  void setFileOrDirectory( const QString &name ) { fileNames.clear(); fileNames.push_back( name ); }
  void setFilesOrDirectories( const QStringList &names ) { fileNames = names; }
  DicomImageListModel::Pointer getSelectedImageDataList(void);
  
  virtual void exec();
  virtual void reject();
  
  private:
  DicomImageListModel dicomModel;
  QStringList fileNames;
};




#endif // DICOMSELECTORDIALOG_H
