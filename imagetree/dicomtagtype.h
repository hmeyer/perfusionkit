#ifndef DICOMTAGTYPE_H
#define DICOMTAGTYPE_H

#include <string>
#include <vector>
#include <boost/serialization/access.hpp>
#include <boost/shared_ptr.hpp>

struct DicomTagType {
  std::string name;
  std::string tag;
  DicomTagType(const std::string &n, const std::string &t):name(n), tag(t) {}
  private:
    DicomTagType(){}
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version);
};
typedef std::vector< DicomTagType > DicomTagList;
typedef boost::shared_ptr< DicomTagList > DicomTagListPointer;

#endif // DICOMTAGTYPE_H
