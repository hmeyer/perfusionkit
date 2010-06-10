#ifndef VTKBINARYIMAGETOCOLOR_H
#define VTKBINARYIMAGETOCOLOR_H
#include <vtkScalarsToColors.h>

class vtkBinaryImageToColor: public vtkScalarsToColors {
  public:
  static vtkBinaryImageToColor *New(unsigned char r, unsigned char g, unsigned char b);
  vtkBinaryImageToColor(unsigned char r, unsigned char g, unsigned char b);
  virtual double *GetRange();
  virtual void SetRange(double min, double max);
  virtual unsigned char* MapValue(double value);
  virtual void GetColor(double value, double rgb[3]);
  virtual void MapScalarsThroughTable2(void *input, unsigned char *output,
    int inputDataType, int numberOfValues, int numberOfComponents, int outputFormat);
  void SetColor(unsigned int r, unsigned int g, unsigned int b);
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
