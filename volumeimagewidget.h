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

class VolumeImageWidget : public QVTKWidget
{
  public:
  VolumeImageWidget();  
  ~VolumeImageWidget();  
  void setImage(vtkImageData *image);
  protected:
  vtkImageData *m_image;
  vtkImageReslice *m_reslice;
  vtkImageMapToWindowLevelColors *m_colormap;
  vtkImageActor *m_actor;
  vtkRenderer *m_renderer;
  vtkMatrix4x4 *m_resliceAxes;
  vtkInteractorStyleProjectionView *m_interactorStyle;
};

#endif // VOLUMEIMAGEWIDGET_H
