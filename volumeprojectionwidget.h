#ifndef MAXIMUMINTENSITYVOLUMEWIDGET_H
#define MAXIMUMINTENSITYVOLUMEWIDGET_H

#include <GUISupport/Qt/QVTKWidget.h>

class vtkImageData;
class vtkVolume;
class vtkRenderer;
class vtkInteractorStyle;

class vtkVolumeMapper;
class vtkVolumeProperty;
class vtkPiecewiseFunction;

/// QT-Widget displaying a Volume in Maximum-Intensity-Projection (MIP)
class VolumeProjectionWidget : public QVTKWidget
{
  public:
  VolumeProjectionWidget();  
  ~VolumeProjectionWidget();  
  void setImage(vtkImageData *image);
  double getCameraDistance(void);
  void setCameraDistance(double dist);
  double getEyeAngle(void);
  void setEyeAngle(double ang);
  void toggleStereo(void);
  protected:
  vtkImageData *m_image; ///< volume image data to be displayed - set by setImage()
  vtkVolume *m_volume; ///< vtkVolume which actually displays the projected volume
  vtkRenderer *m_renderer; ///< the used renderer
  vtkInteractorStyle *m_interactorStyle; ///< special InteractorStyle for Projected Volumes
  vtkVolumeMapper *m_volumeMapper;
  vtkVolumeProperty *m_volumeProperty;
  vtkPiecewiseFunction *m_opacityTransferFunction;
  vtkPiecewiseFunction *m_grayTransferFunction;
};

#endif // MAXIMUMINTENSITYVOLUMEWIDGET_H
