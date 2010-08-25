#include "volumeprojectionwidget.h"

#include <vtkOpenGLRenderer.h>
#include <vtkImageData.h>
#include <vtkRenderWindow.h>

#include <vtkFixedPointVolumeRayCastMapper.h>
#include <vtkVolumeProperty.h>
#include <vtkPiecewiseFunction.h>

#include "interactorstylevolumeview.h"

#include <vtkCamera.h>

/** Default Constructor.
Nothing fancy - just basic setup */
VolumeProjectionWidget::VolumeProjectionWidget(QWidget* parent, Qt::WFlags f):
  QVTKWidget( parent, f),
  m_volume(vtkVolume::New()),
  m_renderer(vtkOpenGLRenderer::New()),
  m_interactorStyle(InteractorStyleVolumeView::New()),
  m_volumeMapper(vtkFixedPointVolumeRayCastMapper::New()),
  m_volumeProperty(vtkVolumeProperty::New()),
  m_opacityTransferFunction(vtkPiecewiseFunction::New()),
  m_grayTransferFunction(vtkPiecewiseFunction::New())
{
  m_opacityTransferFunction->AddPoint( -1000 , 0.0 );
//  m_opacityTransferFunction->AddPoint( -50 , 0.0 );
//  m_opacityTransferFunction->AddPoint( 350 , 0.1 );
  m_opacityTransferFunction->AddPoint( 10000 , 0.001 );
  
//  m_grayTransferFunction->AddSegment( -50 , 0.0 , 350 , 1.0 );
  m_grayTransferFunction->AddSegment( -4000 , 1.0 , 4000 , 1.0 );

  m_volumeProperty->SetScalarOpacity( m_opacityTransferFunction );
  m_volumeProperty->SetColor( m_grayTransferFunction );
  m_volumeProperty->SetInterpolationTypeToLinear();
  
//  m_volume->SetUserMatrix( m_projectionTransform );
  
//  m_volumeMapper->SetBlendModeToMaximumIntensity();
  dynamic_cast<vtkFixedPointVolumeRayCastMapper*>(m_volumeMapper)->SetFinalColorWindow(0.002);
  dynamic_cast<vtkFixedPointVolumeRayCastMapper*>(m_volumeMapper)->SetFinalColorLevel(0.925);
//  dynamic_cast<vtkOpenGLVolumeTextureMapper3D*>(m_volumeMapper)->SetBlendModeToComposite();
//  dynamic_cast<vtkVolumeTextureMapper3D*>(m_volumeMapper)->SetSampleDistance(.5);
//  dynamic_cast<vtkVolumeTextureMapper3D*>(m_volumeMapper)->SetBlendModeToMaximumIntensity();
  dynamic_cast<InteractorStyleVolumeView*>(m_interactorStyle)->SetRayCastMapper( dynamic_cast<vtkFixedPointVolumeRayCastMapper*>(m_volumeMapper) );
  
  m_volume->SetMapper( m_volumeMapper );
  m_volume->SetProperty( m_volumeProperty );
  m_renderer->AddViewProp( m_volume );
  
//  m_renderer->GetActiveCamera()->SetEyeAngle(m_renderer->GetActiveCamera()->GetEyeAngle()*2);
  
  // Set up the interaction
//  m_interactorStyle->SetOrientationMatrix( m_projectionTransform );

  vtkRenderWindowInteractor *interactor = this->GetInteractor();
  interactor->SetInteractorStyle( m_interactorStyle );
  
  GetRenderWindow()->SetStereoTypeToAnaglyph();
  GetRenderWindow()->SetAnaglyphColorSaturation(0.0);
  int anaglyphColorMask[] = {2,5}; 
  GetRenderWindow()->SetAnaglyphColorMask( anaglyphColorMask );
}

/** Destructor*/
VolumeProjectionWidget::~VolumeProjectionWidget() {
  this->hide();
  if (m_volume) m_volume->Delete();
  if (m_renderer) m_renderer->Delete();
  if (m_interactorStyle) m_interactorStyle->Delete();
  if (m_volumeMapper) m_volumeMapper->Delete();
  if (m_volumeProperty) m_volumeProperty->Delete();
  if (m_opacityTransferFunction) m_opacityTransferFunction->Delete();
  if (m_grayTransferFunction) m_grayTransferFunction->Delete();
}

/** Volume Setter*/
void VolumeProjectionWidget::setImage(vtkImageData *image/**<[in] Volume (3D) Image with one component*/) {
  vtkRenderWindow *window = this->GetRenderWindow();
  if (image != NULL) {
    image->UpdateInformation();
    window->RemoveRenderer( m_renderer );
  }
  m_volumeMapper->SetInput(image);
  if (image) {
    window->AddRenderer( m_renderer );
  }
  this->update();
}


/** Get the distance between Camera and Focal Point*/
double VolumeProjectionWidget::getCameraDistance(void) {
  return m_renderer->GetActiveCamera()->GetDistance();
}

/** Set the distance between Camera and Focal Point*/
void VolumeProjectionWidget::setCameraDistance(double dist) {
  double oldDist = m_renderer->GetActiveCamera()->GetDistance();
  double f = dist / oldDist;
  double *fp = m_renderer->GetActiveCamera()->GetFocalPoint();
  double *cp = m_renderer->GetActiveCamera()->GetPosition();
  m_renderer->GetActiveCamera()->SetPosition( 
    fp[0] + (cp[0] - fp[0]) * f,
    fp[1] + (cp[1] - fp[1]) * f,
    fp[2] + (cp[2] - fp[2]) * f);
    
  GetRenderWindow()->Render();
}

/** Get the Eye Angle for Stereo-Projections*/
double VolumeProjectionWidget::getEyeAngle(void) {
  return m_renderer->GetActiveCamera()->GetEyeAngle();
}

/** Set the Eye Angle for Stereo-Projections*/
void VolumeProjectionWidget::setEyeAngle(double ang) {
  m_renderer->GetActiveCamera()->SetEyeAngle( ang );
  if (GetRenderWindow()->GetStereoRender()) 
    GetRenderWindow()->Render();
}


void VolumeProjectionWidget::setStereoMode(int mode) {
  int anaglyphColorMask[] = {2,5}; 
  switch(mode) {
    case int(Anaglyph):	GetRenderWindow()->SetStereoTypeToAnaglyph();
			GetRenderWindow()->SetAnaglyphColorSaturation(0.0);
			GetRenderWindow()->SetAnaglyphColorMask( anaglyphColorMask );
			GetRenderWindow()->StereoRenderOn();
			break;
    case int(Interlaced):	GetRenderWindow()->SetStereoTypeToInterlaced();
			GetRenderWindow()->StereoRenderOn();
			break;
    default: GetRenderWindow()->StereoRenderOff(); break;
  }
  GetRenderWindow()->Render();
}

