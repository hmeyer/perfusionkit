#ifndef VTKBINARYIMAGEOVERLAY_H
#define VTKBINARYIMAGEOVERLAY_H

#include "vtkbinaryimagetocolor.h"

class vtkImageData;
class vtkImageReslice;
class vtkImageMapToColors;
class vtkBinaryImageToColor;
class vtkImageActor;
class vtkMatrix4x4;



class vtkBinaryImageOverlay {
  public:
    vtkBinaryImageOverlay(vtkImageData *image, vtkMatrix4x4 *reslicePlaneTransform, const unsigned char *color, double opacity = 0.3);
    ~vtkBinaryImageOverlay();
    vtkImageData *getImage() const { return m_image; }
    vtkImageActor *getActor() const { return m_actor; }
    bool operator<(const vtkBinaryImageOverlay &other) const;
  protected:
  vtkImageData *m_image; ///< volume image data to be displayed
  vtkImageReslice *m_reslice; ///< vtkImageAlgorithm to reslice the image
  vtkImageMapToColors *m_colormap; ///< used to apply Color
  vtkBinaryImageToColor *m_lookup; ///< color lookup
  vtkImageActor *m_actor; ///< vtkActor which actually displays the resliced volume
};

#endif // VTKBINARYIMAGEOVERLAY_H
