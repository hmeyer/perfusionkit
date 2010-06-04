#include "treeitem.h"

TreeItem::TreeItem(const TreeItem * parent):parentItem(parent) {
}

TreeItem::~TreeItem() {
}

void TreeItem::cloneChildren(TreeItem *dest) const {
  for(ChildListType::const_iterator it=childItems.begin(); it!=childItems.end(); it++)
    dest->insertChild( it->clone() );
}


TreeItem &TreeItem::child(unsigned int number) {
  if (number >= childItems.size()) throw TreeTrouble();
  return childItems[number];
}

const TreeItem &TreeItem::child(unsigned int number) const {
//  if (number >= childItems.size()) throw TreeTrouble();
  unsigned int size = childItems.size();
  if (number >= size) throw TreeTrouble();
  return childItems[number];
}


unsigned int TreeItem::childCount() const {
  return childItems.size();
}

bool TreeItem::insertChild(TreeItem *child) {
  childItems.push_back(child);
  return true;
}

bool TreeItem::insertChild(TreeItem *child, unsigned int position) {
  if (position > childItems.size())
    return false;
  childItems.insert(childItems.begin() + position, child);
  return true;
}

const TreeItem *TreeItem::parent() const {
  return parentItem;
}

bool TreeItem::removeChildren(unsigned int position, unsigned int count) {
  if (position + count > childItems.size())
    return false;
  for (unsigned int row = 0; row < count; ++row)
    childItems.release( childItems.begin() + position );
  return true;
}

int TreeItem::childNumber() const {
  if (parentItem != NULL) {
    const ChildListType &pList = parentItem->childItems;
    for(ChildListType::size_type i = 0; i != pList.size(); ++i)
      if (&pList[i] == this) return i;
  }
  return 0;  
}

bool TreeItem::setData(int column, const QVariant &value) {
  return false;
}
