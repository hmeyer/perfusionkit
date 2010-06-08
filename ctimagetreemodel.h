#ifndef CTIMAGETREEMODEL_H
#define CTIMAGETREEMODEL_H

#include <QAbstractItemModel>
#include "ctimagetreeitem.h"
#include <string>
#include <vector>


class CTImageTreeModel : public QAbstractItemModel, private TreeItem {
  Q_OBJECT
  public:
    typedef CTImageTreeItem::DicomTagListPointer DicomTagListPointer;
    typedef CTImageTreeItem::DicomTagList DicomTagList;
    CTImageTreeModel(const DicomTagList &header, QObject *parent = 0);
    ~CTImageTreeModel();

    virtual QVariant data(int column, int role) const;
    virtual TreeItem *clone(const TreeItem*) const {throw TreeTrouble();return NULL;}
    virtual int columnCount() const {return HeaderFields->size();}
    
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    void sort ( int column, Qt::SortOrder order = Qt::AscendingOrder );


    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &index) const;
    bool hasChildren ( const QModelIndex & parent = QModelIndex() ) const;

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    
    void appendFilename( const itk::MetaDataDictionary &dict, const std::string &fname);
    void insertItemCopy(const TreeItem& item);
    void loadAllImages(void);
    const TreeItem &getItem(const QModelIndex &index) const;
    TreeItem &getItem(const QModelIndex &index);
    
  public slots:
    bool removeCTImage(int srow);
    bool createSegment(int srow);

  private:
    QModelIndex createIndex(int r, int c, const TreeItem*p) const;
    DicomTagListPointer HeaderFields;
};

#endif // CTIMAGETREEMODEL_H
