#ifndef VOLUMEIMAGEWIDGET_H
#define VOLUMEIMAGEWIDGET_H

#include <GUISupport/Qt/QVTKWidget.h>


class vtkImageData;
class vtkImageReslice;
class vtkLookupTable;
class vtkImageMapToColors;
class vtkImageActor;
class vtkRenderer;
class vtkMatrix4x4;
class vtkInteractorStyleImage;
class vtkImageInteractionCallback;

class VolumeImageWidget : public QVTKWidget
{
  public:
  VolumeImageWidget();  
  ~VolumeImageWidget();  
  void setImage(vtkImageData *image);
  protected:
  vtkImageData *m_image;
  vtkImageReslice *m_reslice;
  vtkLookupTable *m_table;
  vtkImageMapToColors *m_color;
  vtkImageActor *m_actor;
  vtkRenderer *m_renderer;
  vtkMatrix4x4 *m_resliceAxes;
  vtkInteractorStyleImage *m_interactorStyle;
  vtkImageInteractionCallback *m_interactionCallback;
};

#endif // VOLUMEIMAGEWIDGET_H
