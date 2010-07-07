#ifndef VTKTREEITEM_H
#define VTKTREEITEM_H
#include <treeitem.h>

class QProgressDialog;
class vtkImageData;

class VTKTreeItem : public TreeItem
{
  public:
  VTKTreeItem(TreeItem * parent): TreeItem(parent) {}
  virtual vtkImageData *getVTKImage(QProgressDialog *progress = NULL, int progressScale=0, int progressBase=0) = 0;

  protected:
    VTKTreeItem() {}
  
  private:
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
      ar & boost::serialization::base_object<TreeItem>(*this);
    }
};


#endif // VTKTREEITEM_H
