#ifndef MULTIPLANARREFORMATWIDGET_H
#define MULTIPLANARREFORMATWIDGET_H

#include <QVTKWidget.h>
#include <map>
#include "vtkbinaryimageoverlay.h"
#include "vtkinteractorstyleprojectionview.h"
#include "vtkinteractoractiondispatch.h"
#include <boost/shared_ptr.hpp>
#include "imagedefinitions.h"

class vtkImageData;
class vtkImageReslice;
class vtkImageMapToWindowLevelColors;
class vtkImageMapToColors;
class vtkBinaryImageToColor;
class vtkImageActor;
class vtkRenderer;
class vtkMatrix4x4;


/// QT-Widget displaying a Volume Slice
class MultiPlanarReformatWidget : public QVTKWidget
{
  public:
  MultiPlanarReformatWidget(QWidget* parent = NULL, Qt::WFlags f = 0);
  ~MultiPlanarReformatWidget();  
  void setImage(vtkImageData *image);
  int addBinaryOverlay(vtkImageData *image, const RGBType &color, const ActionDispatch &dispatch = ActionDispatch() );
  int addAction(const ActionDispatch &dispatch);
  void removeAction(int actionHandle);
  void activateAction(int actionHandle);
  void activateOverlayAction(vtkImageData *image);
  void removeBinaryOverlay(vtkImageData *image);
  virtual void resizeEvent( QResizeEvent * event );
  void setCubicInterpolation(bool cubic=true);
  
  protected:
  typedef std::map< vtkImageData *, boost::shared_ptr< vtkBinaryImageOverlay > > OverlayMapType;
  OverlayMapType m_overlays;
  vtkImageData *m_image; ///< volume image data to be displayed - set by setImage()
  vtkImageReslice *m_reslice; ///< vtkImageAlgorithm to reslice the image
  vtkImageMapToWindowLevelColors *m_colormap; ///< used to apply Window and Level
  vtkImageActor *m_actor; ///< vtkActor which actually displays the resliced volume
  vtkRenderer *m_renderer; ///< the used renderer
  vtkMatrix4x4 *m_reslicePlaneTransform; ///< Tranformation Matrix for the Reslicing Plane
  vtkInteractorStyleProjectionView *m_interactorStyle; ///< special InteractorStyle for Projected Volumes
};

#endif // MULTIPLANARREFORMATWIDGET_H
