#ifndef VTKBINARYIMAGETOCOLOR_H
#define VTKBINARYIMAGETOCOLOR_H
#include <vtkScalarsToColors.h>
#include "imagedefinitions.h"
#include <boost/array.hpp>



class vtkBinaryImageToColor: public vtkScalarsToColors {
  public:
  vtkTypeRevisionMacro(vtkBinaryImageToColor,vtkScalarsToColors);
  static vtkBinaryImageToColor *New();
  vtkBinaryImageToColor();
  vtkBinaryImageToColor(const RGBType &color);
  virtual double *GetRange();
  virtual void SetRange(double min, double max);
  virtual unsigned char* MapValue(double value);
  virtual void GetColor(double value, double rgb[3]);
  virtual void MapScalarsThroughTable2(void *input, unsigned char *output,
    int inputDataType, int numberOfValues, int numberOfComponents, int outputFormat);
  void SetColor(const RGBType &color);
  void SetRandomColor();
  private:
    void adjustUpperLower(void);
    boost::array< double, 2> range;
    unsigned char threshold;
    double dthreshold;
    boost::array< unsigned char, 4> &upperColor, &lowerColor, blackColor, colorColor;
    boost::array< double,3 > &dUpperColor, &dLowerColor, dBlackColor, dColorColor;
};



#endif // VTKBINARYIMAGETOCOLOR_H
