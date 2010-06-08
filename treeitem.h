#ifndef TREEITEM_H
#define TREEITEM_H

#include <QList>
#include <QVariant>
#include <auto_ptr.h>
#include <boost/noncopyable.hpp>
#include <boost/ptr_container/ptr_vector.hpp>



class TreeItem : boost::noncopyable {
  public:
    struct TreeTrouble : public std::exception { };
    TreeItem(const TreeItem *parent = NULL);
    ~TreeItem();
    TreeItem &child(unsigned int number);
    const TreeItem &child(unsigned int number) const;
    unsigned int childCount() const;
    void cloneChildren(TreeItem *dest) const;
    virtual TreeItem *clone(const TreeItem *clonesParent=NULL) const = 0;
    virtual int columnCount() const = 0;
    virtual QVariant data(int column, int role = Qt::DisplayRole) const = 0;
    virtual Qt::ItemFlags flags(int column) const;
    void sortChildren( int column, bool ascending=true );
    bool insertChild(TreeItem *child, unsigned int position);
    bool insertChild(TreeItem *child);
    const TreeItem *parent() const;
    bool removeChildren(unsigned int position, unsigned int count);
    int childNumber() const;
    virtual bool setData(int column, const QVariant &value);
  protected:
  private:
    typedef boost::ptr_vector<TreeItem> ChildListType;
    ChildListType childItems;
    const TreeItem * parentItem;
};


#endif // TREEITEM_H
