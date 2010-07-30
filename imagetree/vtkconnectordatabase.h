#ifndef VTKCONNECTORDATABASE_H
#define VTKCONNECTORDATABASE_H

#include <boost/shared_ptr.hpp>
#include "treeitem.h"

class vtkImageData;

class VTKConnectorDataBase {
  public:
  virtual vtkImageData *getVTKImageData() const = 0;
  virtual size_t getSize() const = 0;
  const TreeItem *getBaseItem() const { return baseItem; }
  TreeItem *getBaseItem() { return baseItem; }
  VTKConnectorDataBase(
    TreeItem *baseItem_): baseItem(baseItem_) {}
  private:
  TreeItem *baseItem;
};

typedef boost::shared_ptr< VTKConnectorDataBase > VTKConnectorDataBasePtr;
    
    
#endif // VTKCONNECTORDATABASE_H
