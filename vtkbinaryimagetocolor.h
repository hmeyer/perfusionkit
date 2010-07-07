#ifndef VTKBINARYIMAGETOCOLOR_H
#define VTKBINARYIMAGETOCOLOR_H
#include <vtkScalarsToColors.h>
#include "imagedefinitions.h"

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
    double range[2];
    unsigned char threshold;
    double dthreshold;
    unsigned char color[4];
    double dcolor[3];
    unsigned char nothing[4];
    double dnothing[3];
};



#endif // VTKBINARYIMAGETOCOLOR_H
