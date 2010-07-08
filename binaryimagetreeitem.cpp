#include "binaryimagetreeitem.h"
#include <boost/random.hpp>
#include <itkImageRegionIterator.h>
#include <vector>
#include "ctimagetreeitem.h"
#include <itkBinaryThresholdImageFilter.h>
#include <itkBinaryDilateImageFilter.h>
#include <itkBinaryErodeImageFilter.h>
#include <itkBinaryBallStructuringElement.h>
#include <itkConnectedThresholdImageFilter.h>

#include <QMessageBox>


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
    double iradius =  std::abs(radius/spacing[i]);
    start[i]	= (int)(idx[i] - iradius); start[i] 	= clip<long>(0, start[i], size[i]);
    end[i]	= (int)(idx[i] + iradius); end[i] 	= clip<long>(0, end[i], size[i]);
    itSize[i] = end[i] - start[i];
    itIndex[i] = start[i];
  }
  
  ImageType::RegionType DrawRegion;
  DrawRegion.SetSize( itSize );
  DrawRegion.SetIndex( itIndex );
  typedef itk::ImageRegionIterator< ImageType > BinImageIterator;
  BinImageIterator iterator = BinImageIterator( peekITKImage(), DrawRegion );
  iterator.GoToBegin();
  float brushRadius2 = radius * radius;

  static std::vector< float > prodx;
  prodx.resize(end[0] - start[0]);
  for(int lx = start[0]; lx < end[0]; ++lx) {
    float t = (lx - idx[0]) * spacing[0]; t *= t;
    prodx[lx-start[0]] = t;
  }
  BinaryPixelType pixelVal = BinaryPixelOn;
  if (erase) pixelVal = BinaryPixelOff;
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
  getVTKImage()->Modified();
}

void BinaryImageTreeItem::regionGrow( float x, float y, float z, boost::function<void()> clearAction) {
  ImageType::IndexType idx;
  ImageType::PointType point;
  point[0] = x;point[1] = y;point[2] = z;
  peekITKImage()->TransformPhysicalPointToIndex(point, idx);
  ImageType::PixelType p = peekITKImage()->GetPixel(idx);
  if (p > 0) {
    typedef itk::ConnectedThresholdImageFilter< ImageType, ImageType > RegionGrowFilterType;
    RegionGrowFilterType::Pointer regionGrower = RegionGrowFilterType::New();
    regionGrower->SetInput( peekITKImage() );
    regionGrower->SetLower(BinaryPixelOn);
    regionGrower->SetUpper(BinaryPixelOn);
    regionGrower->SetSeed(idx);
    regionGrower->Update();
    BinaryImageType::Pointer result = regionGrower->GetOutput();
    setITKImage( result );
    getVTKImage()->Modified();
    clearAction();
  } else {
    QMessageBox::information(0,QObject::tr("Region Grow Error"),QObject::tr("Click on a part of the segmentation"));
  }
}


void BinaryImageTreeItem::thresholdParent(double lower, double upper) {
  CTImageType::Pointer parentImage = dynamic_cast<CTImageTreeItem*>(parent())->getITKImage();
  typedef itk::BinaryThresholdImageFilter< CTImageType, BinaryImageType > ThresholdFilterType;
  ThresholdFilterType::Pointer thresholder = ThresholdFilterType::New();
  thresholder->SetInput( parentImage );
  thresholder->SetLowerThreshold( lower );
  thresholder->SetUpperThreshold( upper );
  thresholder->SetInsideValue( BinaryPixelOn );
  thresholder->SetOutsideValue( BinaryPixelOff );
  thresholder->Update();
  BinaryImageType::Pointer thresholdImage = thresholder->GetOutput();
  setITKImage( thresholdImage );
  getVTKImage()->Modified();
}

void BinaryImageTreeItem::binaryDilate(int iterations) {
  typedef itk::BinaryBallStructuringElement< BinaryPixelType, ImageDimension > KernelType;
  typedef itk::BinaryDilateImageFilter< BinaryImageType, BinaryImageType, KernelType > FilterType;
  FilterType::Pointer filter = FilterType::New();
  filter->SetInput( getITKImage() );
  KernelType kernel;
  kernel.SetRadius( iterations );
  kernel.CreateStructuringElement();
  filter->SetBackgroundValue(BinaryPixelOff);
  filter->SetForegroundValue(BinaryPixelOn);
  filter->SetKernel( kernel );
  filter->Update();
  BinaryImageType::Pointer result = filter->GetOutput();
  setITKImage( result );
  getVTKImage()->Modified();
}

void BinaryImageTreeItem::binaryErode(int iterations) {
  typedef itk::BinaryBallStructuringElement< BinaryPixelType, ImageDimension > KernelType;
  typedef itk::BinaryErodeImageFilter< BinaryImageType, BinaryImageType, KernelType > FilterType;
  FilterType::Pointer filter = FilterType::New();
  filter->SetInput( getITKImage() );
  KernelType kernel;
  kernel.SetRadius( iterations );
  kernel.CreateStructuringElement();
  filter->SetBackgroundValue(BinaryPixelOff);
  filter->SetForegroundValue(BinaryPixelOn);
  filter->SetKernel( kernel );
  filter->Update();
  BinaryImageType::Pointer result = filter->GetOutput();
  setITKImage( result );
  getVTKImage()->Modified();
}



void BinaryImageTreeItem::createRandomColor() {
  static boost::mt19937 rng;
  static boost::uniform_int<> rainbow(0,256*6);
  static boost::variate_generator<boost::mt19937&, boost::uniform_int<> > rainbowcolor(rng, rainbow);
  int hue = rainbowcolor();     
  if (hue<256) {
    int ascend = hue;
      color[0] = 255;
      color[1] = ascend;
      color[2] = 0;
  } else if (hue<512) {
    int descend = 511-hue;
      color[0] = descend;
      color[1] = 255;
      color[2] = 0;
  } else if (hue<768) {
    int ascend = hue-512;
      color[0] = 0;
      color[1] = 255;
      color[2] = ascend;
  } else if (hue<1024) {
    int descend = 1023-hue;
      color[0] = 0;
      color[1] = descend;
      color[2] = 255;
  } else if (hue<1280) {
    int ascend = hue-1024;
      color[0] = ascend;
      color[1] = 0;
      color[2] = 255;
  } else { //if (color<1536) 
    int descend = 1535-hue;
      color[0] = 255;
      color[1] = 0;
      color[2] = descend;
  }
}
