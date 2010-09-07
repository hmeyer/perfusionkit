#ifndef IMAGEDEFINITIONS_H
#define IMAGEDEFINITIONS_H

#include <itkImage.h>
#include <boost/array.hpp>

const unsigned int ImageDimension = 3;

typedef float RealPixelType;
typedef itk::Image< RealPixelType, ImageDimension> RealImageType;

typedef unsigned char LabelPixelType;
typedef itk::Image< LabelPixelType, ImageDimension> LabelImageType;
//typedef bool BinaryPixelType;
typedef unsigned char BinaryPixelType;
const BinaryPixelType BinaryPixelOff = 0;
const BinaryPixelType BinaryPixelOn = 255;
typedef itk::Image< BinaryPixelType, ImageDimension> BinaryImageType;

typedef signed short CTPixelType;
typedef itk::Image< CTPixelType, ImageDimension> CTImageType;

typedef boost::array< unsigned char, 3 > RGBType;


#endif // IMAGEDEFINITIONS_H