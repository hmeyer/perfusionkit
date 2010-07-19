#ifndef CTIMAGETREEMODEL_H
#define CTIMAGETREEMODEL_H

#include <QAbstractItemModel>
#include "ctimagetreeitem.h"
#include <string>
#include <vector>
#include <boost/serialization/access.hpp>


class CTImageTreeModel : public QAbstractItemModel {
  Q_OBJECT
  public:
    typedef CTImageTreeItem::DicomTagListPointer DicomTagListPointer;
    typedef CTImageTreeItem::DicomTagList DicomTagList;
    CTImageTreeModel(const DicomTagList &header, QObject *parent = 0);
    ~CTImageTreeModel();

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
    
    void openModelFromFile(const std::string &fname);
    void saveModelToFile(const std::string &fname) const;
  
    friend class TreeItem;
    friend class CTImageTreeItem;
    
  public slots:
    bool removeCTImage(int srow);
    bool createSegment(int srow);
    
  private:
    friend class boost::serialization::access;
    
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version);
    
    CTImageTreeModel(QObject *parent = 0):QAbstractItemModel(parent),rootItem(this) {};
    void emitLayoutAboutToBeChanged() { emit layoutAboutToBeChanged(); }
    void emitLayoutChanged() { emit layoutChanged(); }
    QModelIndex createIndex(int r, int c, const TreeItem*p) const;
    DicomTagListPointer HeaderFields;
    TreeItem rootItem;
};


#endif // CTIMAGETREEMODEL_H
