#ifndef CTIMAGETREEMODEL_H
#define CTIMAGETREEMODEL_H

#include <QAbstractItemModel>
#include "ctimagetreeitem.h"
#include <string>
#include <vector>


class CTImageTreeModel : public QAbstractItemModel, private CTImageTreeItem {
  Q_OBJECT
  public:
    CTImageTreeModel(const DicomTagListType &header, QObject *parent = 0);
    ~CTImageTreeModel();

    QVariant data(int role) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &index) const;

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    const TreeItem &getItem(const QModelIndex &index) const;
    TreeItem &getItem(const QModelIndex &index);
    
    void appendFilename( const itk::MetaDataDictionary &dict, const std::string &fname);
    void insertItemCopy(const TreeItem& item);
    void loadAllImages(void);

  private:
    QModelIndex createIndex(int r, int c, const TreeItem*p) const;
};

#endif // CTIMAGETREEMODEL_H
