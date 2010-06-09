#ifndef TREEITEM_H
#define TREEITEM_H

#include <QList>
#include <QVariant>
#include <auto_ptr.h>
#include <boost/noncopyable.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <string>

class CTImageTreeModel;

class TreeItem : boost::noncopyable {
  public:
    struct TreeTrouble : public std::exception { };
    TreeItem(TreeItem *parent);
    explicit TreeItem(CTImageTreeModel *model);
    ~TreeItem();
    TreeItem &child(unsigned int number);
    const TreeItem &child(unsigned int number) const;
    unsigned int childCount() const;
    void cloneChildren(TreeItem *dest) const;
    QVariant data(int column, int role = Qt::DisplayRole) const;
    
    virtual TreeItem *clone(TreeItem *clonesParent) const;
    virtual int columnCount() const;
    
    virtual QVariant do_getData_DisplayRole(int column) const { return QVariant::Invalid; }
    virtual QVariant do_getData_ForegroundRole(int column) const { return QVariant::Invalid; }
    virtual QVariant do_getData_FontRole(int column) const;
    
    virtual Qt::ItemFlags flags(int column) const;
    virtual const std::string &getUID(void) const { static std::string t; return t; }
    
    unsigned int depth(void) const;
    void sortChildren( int column, bool ascending=true );
    bool insertChild(TreeItem *child, unsigned int position);
    bool insertChild(TreeItem *child);
    TreeItem *parent();
    const TreeItem *parent() const;
    bool removeChildren(unsigned int position, unsigned int count=1);
    int childNumber() const;
    virtual bool setData(int column, const QVariant &value);
    CTImageTreeModel *getModel(void) { return model; }
    
  protected:
    CTImageTreeModel *model;
  private:
    typedef boost::ptr_vector<TreeItem> ChildListType;
    ChildListType childItems;
    TreeItem * parentItem;
    bool active;
};


#endif // TREEITEM_H
