#include "ctimagetreemodel_serializer.h"

#include <fstream>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filter/zlib.hpp>

#include "ctimagetreemodel.h"
#include "ctimagetreeitem.h"
#include "binaryimagetreeitem.h"
#include "serialization_helper.h"

#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT_GUID(CTImageTreeItem, "CTImageTreeItem");
BOOST_CLASS_EXPORT_GUID(BinaryImageTreeItem, "BinaryImageTreeItem");





void deserializeCTImageTreeModelFromFile(CTImageTreeModel &model, const std::string &fname) {
  using namespace std;
  using namespace boost::iostreams;
  std::ifstream inFileStream( fname.c_str(), ios_base::in | ios_base::binary  );
  filtering_stream<input> inStreamFilter;
  inStreamFilter.push(zlib_decompressor());
  inStreamFilter.push(inFileStream);
  boost::archive::binary_iarchive ia( inStreamFilter );
  model.setSerializationPath( fname );
  ia >> model;
}

void serializeCTImageTreeModelToFile(CTImageTreeModel &model, const std::string &fname) {
  using namespace std;
  using namespace boost::iostreams;
  ofstream outFileStream( fname.c_str(), ios_base::out | ios_base::binary );
  filtering_stream<output> outStreamFilter;
  outStreamFilter.push(zlib_compressor());
  outStreamFilter.push(outFileStream);
  boost::archive::binary_oarchive oa( outStreamFilter );
  model.setSerializationPath( fname );
  oa << model;
}

