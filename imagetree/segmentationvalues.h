#ifndef SEGMENTATIONVALUES_H
#define SEGMENTATIONVALUES_H

#include <boost/serialization/access.hpp>
#include "itkvtktreeitem.h"
#include "imagedefinitions.h"



struct SegmentationValues {
  enum Accuracy {
    SimpleAccuracy,
    NonMultiSamplingAccuracy,
    InterpolatedAccuray
  };
  const ITKVTKTreeItem<BinaryImageType> *segment;
  long unsigned mtime;
  double mean;
  double stddev;
  int min;
  int max;
  int sampleCount;
  Accuracy accuracy;
  private:
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version);
};

#endif // SEGMENTATIONVALUES_H
