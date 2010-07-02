#ifndef MULTIPLANARREFORMATWIDGET_H
#define MULTIPLANARREFORMATWIDGET_H

#include <QVTKWidget.h>
#include <map>
#include "vtkbinaryimageoverlay.h"
#include "vtkinteractorstyleprojectionview.h"
#include "vtkinteractoractiondispatch.h"
#include <vtkSmartPointer.h>
#include <boost/shared_ptr.hpp>

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
  void addBinaryOverlay(vtkImageData *image, const unsigned char *color, const ActionDispatch &dispatch = ActionDispatch() );
  void activateOverlayAction(vtkImageData *image);
  void removeBinaryOverlay(vtkImageData *image);
  
  protected:
  typedef std::map< vtkImageData *, boost::shared_ptr< vtkBinaryImageOverlay > > OverlayMapType;
  OverlayMapType m_overlays;
  vtkImageData *m_image; ///< volume image data to be displayed - set by setImage()
  vtkSmartPointer<vtkImageReslice> m_reslice; ///< vtkImageAlgorithm to reslice the image
  vtkSmartPointer<vtkImageMapToWindowLevelColors> m_colormap; ///< used to apply Window and Level
  vtkSmartPointer<vtkImageActor> m_actor; ///< vtkActor which actually displays the resliced volume
  vtkSmartPointer<vtkRenderer> m_renderer; ///< the used renderer
  vtkSmartPointer<vtkMatrix4x4> m_reslicePlaneTransform; ///< Tranformation Matrix for the Reslicing Plane
  vtkSmartPointer<vtkInteractorStyleProjectionView> m_interactorStyle; ///< special InteractorStyle for Projected Volumes
};

#endif // MULTIPLANARREFORMATWIDGET_H
