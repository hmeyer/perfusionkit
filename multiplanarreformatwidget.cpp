#include "multiplanarreformatwidget.h"
#include <vtkMatrix4x4.h>
#include <vtkinteractorstyleprojectionview.h>
#include <vtkCommand.h>
#include <vtkImageReslice.h>
#include <vtkImageMapToWindowLevelColors.h>
#include <vtkImageMapToColors.h>
#include <vtkScalarsToColors.h>
#include <vtkImageActor.h>
#include <vtkRenderer.h>
#include <vtkImageData.h>
#include <vtkRenderWindow.h>
#include <algorithm>
#include <boost/bind.hpp>


#include <vtkCamera.h>

/** Default Constructor.
Nothing fancy - just basic setup */
MultiPlanarReformatWidget::MultiPlanarReformatWidget(QWidget* parent, Qt::WFlags f):
  QVTKWidget(parent, f),
  m_reslice(vtkImageReslice::New()),
  m_colormap(vtkImageMapToWindowLevelColors::New()),
  m_actor(vtkImageActor::New()),
  m_renderer(vtkRenderer::New()),
  m_reslicePlaneTransform(vtkMatrix4x4::New()),
  m_interactorStyle(vtkInteractorStyleProjectionView::New())
{
  m_reslice->SetOutputDimensionality(2);
  m_reslice->SetBackgroundLevel(-1000);
  m_reslice->SetInterpolationModeToLinear();
  
  m_colormap->SetOutputFormatToRGB();

  m_colormap->SetInputConnection(m_reslice->GetOutputPort());

  m_actor->SetInput(m_colormap->GetOutput());
  m_renderer->AddActor(m_actor);

  // Set up the interaction
  m_interactorStyle->SetImageMapToWindowLevelColors( m_colormap );
  m_interactorStyle->SetOrientationMatrix( m_reslicePlaneTransform );

  vtkRenderWindowInteractor *interactor = this->GetInteractor();
  interactor->SetInteractorStyle(m_interactorStyle);
  m_reslice->SetResliceAxes(m_reslicePlaneTransform);
  m_reslice->SetOutputDimensionality(2);
}

/** Destructor*/
MultiPlanarReformatWidget::~MultiPlanarReformatWidget() {
  this->hide();
  if (m_renderer) m_renderer->Delete();
  m_overlays.clear();
  if (m_colormap) m_colormap->Delete();
  if (m_reslice) m_reslice->Delete();
  if (m_actor) m_actor->Delete();
  if (m_interactorStyle) m_interactorStyle->Delete();
  if (m_reslicePlaneTransform) m_reslicePlaneTransform->Delete();
}

/** Volume Setter*/
void MultiPlanarReformatWidget::setImage(vtkImageData *image/**<[in] Volume (3D) Image with one component*/) {
  if (image==NULL) {
    m_image = NULL;
    vtkRenderWindow *window = this->GetRenderWindow();
    window->RemoveRenderer( m_renderer );
    this->update();
  } else {
    vtkRenderWindow *window = this->GetRenderWindow();
    window->RemoveRenderer( m_renderer );
    m_image = image;
    m_image->UpdateInformation();
    int extent[6];
    for(int i = 0; i < 3; i++)
      m_image->GetAxisUpdateExtent(i, extent[i*2], extent[i*2+1]);

    double spacing[3];
    double origin[3];
    m_image->GetSpacing(spacing);
    m_image->GetOrigin(origin);

    double center[3];
    center[0] = origin[0] + spacing[0] * 0.5 * (extent[0] + extent[1]); 
    center[1] = origin[1] + spacing[1] * 0.5 * (extent[2] + extent[3]); 
    center[2] = origin[2] + spacing[2] * 0.5 * (extent[4] + extent[5]); 
    
    // Set the point through which to slice
    m_reslicePlaneTransform->SetElement(0, 3, center[0]);
    m_reslicePlaneTransform->SetElement(1, 3, center[1]);
    m_reslicePlaneTransform->SetElement(2, 3, center[2]);

    m_reslice->SetInput( m_image );
    window->AddRenderer(m_renderer);
    this->update();
    
    int ex[6];
    m_reslice->GetOutputExtent(ex);
    std::cerr << "outputExtent:" << ex[0] << " ," << ex[1] << " ," << ex[2] << " ," << ex[3] << " ," << ex[4] << " ," << ex[5] << std::endl;
    int xres = 1000;
    int yres = 1000;
    m_reslice->SetOutputExtent(0,xres,0,yres,0,0);
    m_reslice->SetOutputOrigin(-xres/2.0,-yres/2.0,0);
    m_reslice->SetOutputSpacing(1,1,1);
    m_reslice->GetOutputExtent(ex);
    std::cerr << "outputExtent:" << ex[0] << " ," << ex[1] << " ," << ex[2] << " ," << ex[3] << " ," << ex[4] << " ," << ex[5] << std::endl;
  }
}


void MultiPlanarReformatWidget::addBinaryOverlay(vtkImageData *image, const unsigned char *color, const ActionDispatch &dispatch) {
  if (m_overlays.find( image ) == m_overlays.end() ) {
    boost::shared_ptr< vtkBinaryImageOverlay > overlay(
     new vtkBinaryImageOverlay( m_renderer, m_interactorStyle, dispatch, image, m_reslicePlaneTransform, color) );
    m_overlays.insert( OverlayMapType::value_type( image, overlay ) );
    this->update();
  }
}

void MultiPlanarReformatWidget::removeBinaryOverlay(vtkImageData *image) {
  m_overlays.erase(image);
  this->update();
}

void MultiPlanarReformatWidget::activateOverlayAction(vtkImageData *image) {
  OverlayMapType::iterator it = m_overlays.find( image );
  if (it != m_overlays.end()) {
    it->second->activateAction();
  }
}

