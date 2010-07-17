#ifndef VOLUMEPROJECTIONWIDGET_H
#define VOLUMEPROJECTIONWIDGET_H

#include <QVTKWidget.h>

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
  enum {
    Anaglyph,
    Interlaced,
    Off
  } StereoMode;
  VolumeProjectionWidget(QWidget* parent = NULL, Qt::WFlags f = 0);
  ~VolumeProjectionWidget();  
  void setImage(vtkImageData *image);
  double getCameraDistance(void);
  void setCameraDistance(double dist);
  double getEyeAngle(void);
  void setEyeAngle(double ang);
  void setStereoMode(int mode);
  protected:
  vtkVolume *m_volume; ///< vtkVolume which actually displays the projected volume
  vtkRenderer *m_renderer; ///< the used renderer
  vtkInteractorStyle *m_interactorStyle; ///< special InteractorStyle for Projected Volumes
  vtkVolumeMapper *m_volumeMapper;
  vtkVolumeProperty *m_volumeProperty;
  vtkPiecewiseFunction *m_opacityTransferFunction;
  vtkPiecewiseFunction *m_grayTransferFunction;
};

#endif // VOLUMEPROJECTIONWIDGET_H
