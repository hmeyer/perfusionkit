#ifndef VTKBINARYIMAGEOVERLAY_H
#define VTKBINARYIMAGEOVERLAY_H

#include "vtkbinaryimagetocolor.h"
#include "vtkinteractorstyleprojectionview.h"
#include <vtkSmartPointer.h>

class vtkImageData;
class vtkImageReslice;
class vtkImageMapToColors;
class vtkBinaryImageToColor;
class vtkImageActor;
class vtkMatrix4x4;



class vtkBinaryImageOverlay {
  public:
    vtkBinaryImageOverlay( vtkSmartPointer<vtkRenderer> renderer,
			   vtkSmartPointer<vtkInteractorStyleProjectionView> interactorStyle,
			  const ActionDispatch &action, vtkImageData *image, vtkSmartPointer<vtkMatrix4x4> reslicePlaneTransform,
			  const unsigned char *color, double opacity = 0.3);
    ~vtkBinaryImageOverlay();
    vtkImageData *getImage() const { return m_image; }
    void activateAction();
    vtkSmartPointer<vtkImageActor> getActor() { return m_actor; }
    bool operator<(const vtkBinaryImageOverlay &other) const;
  protected:
  vtkImageData *m_image; ///< volume image data to be displayed
  vtkSmartPointer<vtkImageReslice> m_reslice; ///< vtkImageAlgorithm to reslice the image
  vtkSmartPointer<vtkImageMapToColors> m_colormap; ///< used to apply Color
  vtkSmartPointer<vtkBinaryImageToColor> m_lookup; ///< color lookup
  vtkSmartPointer<vtkImageActor> m_actor; ///< vtkActor which actually displays the resliced volume
  vtkSmartPointer<vtkRenderer> m_renderer;
  vtkSmartPointer<vtkInteractorStyleProjectionView> m_interactorStyle;
  int actionHandle;
};

#endif // VTKBINARYIMAGEOVERLAY_H
