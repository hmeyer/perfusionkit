#ifndef VTKTREEITEM_H
#define VTKTREEITEM_H
#include <treeitem.h>

class QProgressDialog;
class vtkImageData;

class VTKTreeItem : public TreeItem
{
  public:
  VTKTreeItem(const TreeItem * parent=NULL): TreeItem(parent) {}
  virtual vtkImageData *getVTKImage(QProgressDialog *progress = NULL, int progressScale=0, int progressBase=0) = 0;
};


#endif // VTKTREEITEM_H
