#ifndef CTIMAGETREEMODEL_SERIALIZER_H
#define CTIMAGETREEMODEL_SERIALIZER_H

#include <string>
#include "ctimagetreemodel.h"

void deserializeCTImageTreeModelFromFile(CTImageTreeModel &model, const std::string &fname);
void serializeCTImageTreeModelToFile(CTImageTreeModel &model, const std::string &fname);





#endif //CTIMAGETREEMODEL_SERIALIZER_H