#ifndef VOLUMEIMAGEWIDGET_H
#define VOLUMEIMAGEWIDGET_H

#include <GUISupport/Qt/QVTKWidget.h>


class vtkImageData;
class vtkImageReslice;
class vtkImageMapToWindowLevelColors;
class vtkImageActor;
class vtkRenderer;
class vtkMatrix4x4;
class vtkInteractorStyleProjectionView;

/// QT-Widget displaying a Volume Slice
class MultiPlanarReformatWidget : public QVTKWidget
{
  public:
  MultiPlanarReformatWidget();  
  ~MultiPlanarReformatWidget();  
  void setImage(vtkImageData *image);
  protected:
  vtkImageData *m_image; ///< volume image data to be displayed - set by setImage()
  vtkImageReslice *m_reslice; ///< vtkImageAlgorithm to reslice the image
  vtkImageMapToWindowLevelColors *m_colormap; ///< used to apply Window and Level
  vtkImageActor *m_actor; ///< vtkActor which actually displays the resliced volume
  vtkRenderer *m_renderer; ///< the used renderer
  vtkMatrix4x4 *m_reslicePlaneTransform; ///< Tranformation Matrix for the Reslicing Plane
  vtkInteractorStyleProjectionView *m_interactorStyle; ///< special InteractorStyle for Projected Volumes
};

#endif // VOLUMEIMAGEWIDGET_H
