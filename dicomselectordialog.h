#ifndef DICOMSELECTORDIALOG_H
#define DICOMSELECTORDIALOG_H

#include <QDialog>
#include "ui_DicomSelectDialog.h"
#include <boost/shared_ptr.hpp>
#include <boost/assign.hpp>
#include "ctimagetreemodel.h"

class DicomSelectorDialog : public QDialog, private Ui_DicomSelectDialog
{
  public:
  DicomSelectorDialog(QWidget * parent = 0, Qt::WindowFlags f = 0);
  void setFileOrDirectory( const QString &name ) { fileNames.clear(); fileNames.push_back( name ); }
  void setFilesOrDirectories( const QStringList &names ) { fileNames = names; }
  void getSelectedImageDataList(CTImageTreeModel &other) const;
  
  virtual void exec();
  virtual void reject();
  
  private:
  CTImageTreeModel ctImageModel;
  QStringList fileNames;
  static const CTImageTreeItem::DicomTagList HeaderFields;
};




#endif // DICOMSELECTORDIALOG_H
