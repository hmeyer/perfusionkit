#include "vtkbinaryimagetocolor.h"
#include <vtkType.h>

vtkBinaryImageToColor *vtkBinaryImageToColor::New(unsigned char r, unsigned char g, unsigned char b) {
  return new vtkBinaryImageToColor(r,g,b);
}

vtkBinaryImageToColor::vtkBinaryImageToColor(unsigned char r, unsigned char g, unsigned char b) {
  SetRange(0,255);
  SetColor(r,g,b);
  nothing[0] = 0;nothing[1] = 0;nothing[2] = 0;nothing[3] = 0;
  dnothing[0] = 0;dnothing[1] = 0;dnothing[2] = 0;
}

double *vtkBinaryImageToColor::GetRange() {
  return range;
}

void vtkBinaryImageToColor::SetRange(double min, double max) {
  range[0] = min;
  range[1] = max;
  dthreshold = (min+max)*.5;
  threshold = static_cast<unsigned char>(dthreshold);
}

unsigned char* vtkBinaryImageToColor::MapValue(double value) {
  if (value>=threshold) return color;
  else return nothing;
}

void vtkBinaryImageToColor::GetColor(double value, double rgb[3]) {
  if (value>=threshold) {
    memcpy(rgb, dcolor, sizeof(double) * 3);
  } else {
    memcpy(rgb, dnothing, sizeof(double) * 3);
  }
}

void vtkBinaryImageToColor::MapScalarsThroughTable2(void *input, unsigned char *output,
    int inputDataType, int numberOfValues, int numberOfComponents, int outputFormat) {
  if (numberOfComponents!=1 || inputDataType != VTK_UNSIGNED_CHAR) return;
  unsigned char *inputData = static_cast<unsigned char*>(input);
  unsigned char *sourceColor;
  if (outputFormat == VTK_RGBA) {
    for(int i=0;i<numberOfValues;i++) {
      sourceColor = nothing;
      if ((*inputData) >= threshold) {
	sourceColor = color;
      }
      memcpy(output, sourceColor, sizeof(unsigned char) * 4);
      output+=4;
      inputData++;
    }
  } else if (outputFormat == VTK_RGB) {
    for(int i=0;i<numberOfValues;i++) {
      sourceColor = nothing;
      if ((*inputData) >= threshold) {
	sourceColor = color;
      }
      memcpy(output, sourceColor, sizeof(unsigned char) * 3);
      output+=3;
      inputData++;
    }
  }
}

void vtkBinaryImageToColor::SetColor(unsigned int r, unsigned int g, unsigned int b) {
  color[0] = r;
  color[1] = g;
  color[2] = b;
  color[3] = 255;
  dcolor[0] = r/255.0;dcolor[1] = g/255.0;dcolor[2] = b/255.0;
}