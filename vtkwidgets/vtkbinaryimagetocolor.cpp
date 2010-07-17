#include "vtkbinaryimagetocolor.h"
#include <vtkObjectFactory.h>
#include "imagedefinitions.h"

vtkStandardNewMacro(vtkBinaryImageToColor);
vtkCxxRevisionMacro(vtkBinaryImageToColor, "$Revision: 0.1 $");

vtkBinaryImageToColor::vtkBinaryImageToColor()
  :upperColor(colorColor), lowerColor(blackColor), 
  dUpperColor(dColorColor), dLowerColor(dBlackColor) {
  SetRange(BinaryPixelOff, BinaryPixelOn);
  RGBType black;
  black[0] = 0;black[1] = 0;black[2] = 0;
  SetColor(black);
  blackColor[0] = 0;blackColor[1] = 0;blackColor[2] = 0;blackColor[3] = 0;
  dBlackColor[0] = 0;dBlackColor[1] = 0;dBlackColor[2] = 0;
}

vtkBinaryImageToColor::vtkBinaryImageToColor(const RGBType &color)
  :upperColor(colorColor), lowerColor(blackColor), 
  dUpperColor(dColorColor), dLowerColor(dBlackColor) {
  SetRange(BinaryPixelOff, BinaryPixelOn);
  SetColor(color);
  blackColor[0] = 0;blackColor[1] = 0;blackColor[2] = 0;blackColor[3] = 0;
  dBlackColor[0] = 0;dBlackColor[1] = 0;dBlackColor[2] = 0;
}

double *vtkBinaryImageToColor::GetRange() {
  return range.c_array();
}

void vtkBinaryImageToColor::SetRange(double min, double max) {
  range[0] = min;
  range[1] = max;
  dthreshold = (min+max)*.5;
  threshold = static_cast<unsigned char>(dthreshold+.5);
  adjustUpperLower();
}

unsigned char* vtkBinaryImageToColor::MapValue(double value) {
  if (value>=threshold) return upperColor.c_array();
  else return lowerColor.c_array();
}

void vtkBinaryImageToColor::GetColor(double value, double rgb[3]) {
  if (value>=threshold) {
    memcpy(rgb, dUpperColor.c_array(), sizeof(double) * 3);
  } else {
    memcpy(rgb, dLowerColor.c_array(), sizeof(double) * 3);
  }
}

void vtkBinaryImageToColor::MapScalarsThroughTable2(void *input, unsigned char *output,
    int inputDataType, int numberOfValues, int numberOfComponents, int outputFormat) {
  if (numberOfComponents!=1 || inputDataType != VTK_UNSIGNED_CHAR) return;
  unsigned char *inputData = static_cast<unsigned char*>(input);
  unsigned char *sourceColor;
  if (outputFormat == VTK_RGBA) {
    for(int i=0;i<numberOfValues;i++) {
      sourceColor = lowerColor.c_array();
      if ((*inputData) >= threshold) {
	sourceColor = upperColor.c_array();
      }
      memcpy(output, sourceColor, sizeof(unsigned char) * 4);
      output+=4;
      inputData++;
    }
  } else if (outputFormat == VTK_RGB) {
    for(int i=0;i<numberOfValues;i++) {
      sourceColor = lowerColor.c_array();
      if ((*inputData) >= threshold) {
	sourceColor = upperColor.c_array();
      }
      memcpy(output, sourceColor, sizeof(unsigned char) * 3);
      output+=3;
      inputData++;
    }
  }
}

void vtkBinaryImageToColor::SetColor(const RGBType &c) {
  upperColor[0] = c[0];
  upperColor[1] = c[1];
  upperColor[2] = c[2];
  upperColor[3] = 255;
  dUpperColor[0] = c[0]/255.0;dUpperColor[1] = c[1]/255.0;dUpperColor[2] = c[2]/255.0;
  adjustUpperLower();
}

void vtkBinaryImageToColor::adjustUpperLower(void) {
  upperColor = colorColor; dUpperColor = dColorColor;
  lowerColor = blackColor; dLowerColor = dBlackColor;
  if (range[0] > range[1]) {
    std::swap(upperColor, lowerColor);
    std::swap(dUpperColor, dLowerColor);
  }
}
