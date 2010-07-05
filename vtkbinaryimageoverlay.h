#ifndef VTKBINARYIMAGEOVERLAY_H
#define VTKBINARYIMAGEOVERLAY_H

#include "vtkinteractoractiondispatch.h"

class vtkImageData;
class vtkImageReslice;
class vtkImageMapToColors;
class vtkBinaryImageToColor;
class vtkImageActor;
class vtkMatrix4x4;
class vtkBinaryImageToColor;
class vtkInteractorStyleProjectionView;
class vtkRenderer;



class vtkBinaryImageOverlay {
  public:
    vtkBinaryImageOverlay( vtkRenderer *renderer,
			   vtkInteractorStyleProjectionView *interactorStyle,
			  const ActionDispatch &action, vtkImageData *image, vtkMatrix4x4 *reslicePlaneTransform,
			  const unsigned char *color, double opacity = 0.3);
    ~vtkBinaryImageOverlay();
    vtkImageData *getImage() const { return m_image; }
    void activateAction();
    vtkImageActor *getActor() { return m_actor; }
    bool operator<(const vtkBinaryImageOverlay &other) const;
  protected:
  vtkImageData *m_image; ///< volume image data to be displayed
  vtkImageReslice *m_reslice; ///< vtkImageAlgorithm to reslice the image
  vtkImageMapToColors *m_colormap; ///< used to apply Color
  vtkBinaryImageToColor *m_lookup; ///< color lookup
  vtkImageActor *m_actor; ///< vtkActor which actually displays the resliced volume
  vtkRenderer *m_renderer;
  vtkInteractorStyleProjectionView *m_interactorStyle;
  int actionHandle;
};

#endif // VTKBINARYIMAGEOVERLAY_H
