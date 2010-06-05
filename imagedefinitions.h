#ifndef IMAGEDEFINITIONS_H
#define IMAGEDEFINITIONS_H

#include <itkImage.h>

const unsigned int ImageDimension = 3;

//typedef bool BinaryPixelType;
typedef unsigned char BinaryPixelType;
typedef itk::Image< BinaryPixelType, ImageDimension> BinaryImageType;

typedef signed short CTPixelType;
typedef itk::Image< CTPixelType, ImageDimension> CTImageType;

#endif // IMAGEDEFINITIONS_H