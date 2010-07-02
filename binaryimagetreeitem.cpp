#include "binaryimagetreeitem.h"
#include <boost/random.hpp>
#include <itkImageRegionIterator.h>
#include <vector>
#include "ctimagetreeitem.h"
#include <itkBinaryThresholdImageFilter.h>


BinaryImageTreeItem::BinaryImageTreeItem(TreeItem * parent, BinaryImageType::Pointer itkImage, const QString &name)
  :BaseClass(parent, itkImage), name(name) {
    createRandomColor();
}

TreeItem *BinaryImageTreeItem::clone(TreeItem *clonesParent) const {
  BinaryImageTreeItem *c = new BinaryImageTreeItem(clonesParent, peekITKImage(), name );
  cloneChildren(c);
  return c;
}

int BinaryImageTreeItem::columnCount() const {
  return 1;
}
QVariant BinaryImageTreeItem::do_getData_DisplayRole(int c) const {
  if (c==0) return name;
  else return QVariant::Invalid;
}

QVariant BinaryImageTreeItem::do_getData_BackgroundRole(int column) const {
  QColor qcolor( color[0], color[1], color[2]);
  return QBrush( qcolor );
}


Qt::ItemFlags BinaryImageTreeItem::flags(int column) const {
    if (column != 0) return Qt::NoItemFlags;
    return Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;  
}


bool BinaryImageTreeItem::setData(int c, const QVariant &value) {
  if (c==0 && static_cast<QMetaType::Type>(value.type()) == QMetaType::QString) {
    name = value.toString();
    return true;
  } 
  return false;
}

template <typename T>
inline T clip(T min, T val, T max) {
  return std::max<T>( std::min<T>( val, max), min );
}

void BinaryImageTreeItem::drawSphere( float radius, float x, float y, float z, bool erase ) {
  itk::ContinuousIndex< double, ImageDimension > idx;
  ImageType::PointType point;
  point[0] = x;point[1] = y;point[2] = z;
  peekITKImage()->TransformPhysicalPointToContinuousIndex(point, idx);
  ImageType::SpacingType spacing = peekITKImage()->GetSpacing();
  ImageType::SizeType size = peekITKImage()->GetBufferedRegion().GetSize();
  
  long start[3],end[3];
  ImageType::SizeType itSize;
  ImageType::IndexType itIndex;
  for(int i=0; i < 3; ++i) {
    start[i]	= (int)(idx[i] - radius/spacing[i]); start[i] 	= clip<long>(0, start[i], size[i]);
    end[i]	= (int)(idx[i] + radius/spacing[i]); end[i] 	= clip<long>(0, end[i], size[i]);
    itSize[i] = end[i] - start[i];
    itIndex[i] = start[i];
  }
  
  ImageType::RegionType itRegion;
  itRegion.SetSize( itSize );
  itRegion.SetIndex( itIndex );
  typedef itk::ImageRegionIterator< ImageType > BinImageIterator;
  BinImageIterator iterator = BinImageIterator( peekITKImage(), itRegion );
  iterator.GoToBegin();
  float brushRadius2 = radius * radius;

  static std::vector< float > prodx;
  prodx.resize(end[0] - start[0]);
  for(int lx = start[0]; lx < end[0]; ++lx) {
    float t = (lx - idx[0]) * spacing[0]; t *= t;
    prodx[lx-start[0]] = t;
  }
  BinaryPixelType pixelVal = 255;
  if (erase) pixelVal = 0;
  for(int lz = start[2]; lz < end[2]; ++lz) {
	  float sumz = (lz - idx[2]) * spacing[2]; sumz *= sumz;
	  for(int ly = start[1]; ly < end[1]; ++ly) {
		  float sumy = (ly - idx[1]) * spacing[1]; sumy *= sumy; sumy += sumz;
		  for(int lx = start[0]; lx < end[0]; ++lx) {
			  if ((prodx[lx-start[0]] + sumy) < brushRadius2) {
				  iterator.Set(pixelVal);
			  }
			  ++iterator;
		  }
	  }
  }  
  getVTKImage()->Update();
}

void BinaryImageTreeItem::thresholdParent(double lower, double upper) {
  CTImageType::Pointer parentImage = dynamic_cast<CTImageTreeItem*>(parent())->getITKImage();
  typedef itk::BinaryThresholdImageFilter< CTImageType, BinaryImageType > ThresholdFilterType;
  ThresholdFilterType::Pointer thresholder = ThresholdFilterType::New();
  thresholder->SetInput( parentImage );
  thresholder->SetLowerThreshold( lower );
  thresholder->SetUpperThreshold( upper );
  thresholder->Update();
  BinaryImageType::Pointer thresholdImage = thresholder->GetOutput();
  setITKImage( thresholdImage );
  getVTKImage()->Update();
}


void BinaryImageTreeItem::createRandomColor() {
  static boost::mt19937 rng;
  static boost::uniform_int<> rainbow(0,256*6);
  static boost::variate_generator<boost::mt19937&, boost::uniform_int<> > rainbowcolor(rng, rainbow);
  int hue = rainbowcolor();     
  if (hue<256) {
    int rise = hue;
      color[0] = 255;
      color[1] = rise;
      color[2] = 0;
  } else if (hue<512) {
    int fall = 511-hue;
      color[0] = fall;
      color[1] = 255;
      color[2] = 0;
  } else if (hue<768) {
    int rise = hue-512;
      color[0] = 0;
      color[1] = 255;
      color[2] = rise;
  } else if (hue<1024) {
    int fall = 1023-hue;
      color[0] = 0;
      color[1] = fall;
      color[2] = 255;
  } else if (hue<1280) {
    int rise = hue-1024;
      color[0] = rise;
      color[1] = 0;
      color[2] = 255;
  } else { //if (color<1536) 
    int fall = 1535-hue;
      color[0] = 255;
      color[1] = 0;
      color[2] = fall;
  }
}
