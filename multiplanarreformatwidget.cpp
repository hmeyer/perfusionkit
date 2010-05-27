#include "multiplanarreformatwidget.h"
#include "vtkMatrix4x4.h"
#include "vtkinteractorstyleprojectionview.h"
#include "vtkCommand.h"
#include "vtkImageReslice.h"
#include "vtkImageMapToWindowLevelColors.h"
#include "vtkImageActor.h"
#include "vtkRenderer.h"
#include "vtkImageData.h"
#include "vtkRenderWindow.h"

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

  m_colormap->SetWindow(400);
  m_colormap->SetLevel(50);

  m_colormap->SetInputConnection(m_reslice->GetOutputPort());

  m_actor->SetInput(m_colormap->GetOutput());
  m_renderer->AddActor(m_actor);

  // Set up the interaction
  m_interactorStyle->SetImageMapToWindowLevelColors( m_colormap );
  m_interactorStyle->SetOrientationMatrix( m_reslicePlaneTransform );

  vtkRenderWindowInteractor *interactor = this->GetInteractor();
  interactor->SetInteractorStyle(m_interactorStyle);
  m_reslice->SetResliceAxes(m_reslicePlaneTransform);
}

/** Destructor*/
MultiPlanarReformatWidget::~MultiPlanarReformatWidget() {
  this->hide();
  m_renderer->Delete();
  m_actor->Delete();
  m_colormap->Delete();
  m_reslice->Delete();
  m_reslicePlaneTransform->Delete();
  m_interactorStyle->Delete();
}

/** Volume Setter*/
void MultiPlanarReformatWidget::setImage(vtkImageData *image/**<[in] Volume (3D) Image with one component*/) {
  if (!image) {
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
  }
}