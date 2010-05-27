#include "interactorstylevolumeview.h"

#include <vtkObjectFactory.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkCamera.h>
#include <vtkCommand.h>

#include <vtkFixedPointVolumeRayCastMapper.h>

#include <string>

using namespace std;

const string keySymLeft = "Left";
const string keySymRight = "Right";
const string keySymUp = "Up";
const string keySymDown = "Down";
const string keySymSpace = "space";

#define VTKIS_WINDOWLEVEL   20


vtkStandardNewMacro(InteractorStyleVolumeView);


/// default Constructor
InteractorStyleVolumeView::InteractorStyleVolumeView():
  m_stateRButton(false),
  m_stateLButton(false),
  m_stateMButton(false),
  m_leftMBHint( NULL ),
  m_restricted( false ),
  m_rayCastMapper( NULL )
{
  SetLMBHint(0);
  m_LMBState=VTKIS_ROTATE;
  UseTimers = 1;
}

/// Destructor
InteractorStyleVolumeView::~InteractorStyleVolumeView() {
  if (m_leftMBHint) m_leftMBHint->Delete();
}

/** dispatch Actions according to the State of the Mouse Buttons */
void InteractorStyleVolumeView::dipatchActions() {
  saveMousePosition();
  if ( m_stateLButton &&  m_stateMButton &&  m_stateRButton) { StopState(); StartSpin(); return; }
  if ( m_stateLButton &&  m_stateMButton && !m_stateRButton) { StopState(); StartRotate(); return; }
  if ( m_stateLButton && !m_stateMButton &&  m_stateRButton) { StopState(); StartZoom(); return; }
  if ( m_stateLButton && !m_stateMButton && !m_stateRButton) { StopState(); StartState( m_LMBState ); return; }
  if (!m_stateLButton &&  m_stateMButton &&  m_stateRButton) { StopState(); StartPan(); return; }
  if (!m_stateLButton &&  m_stateMButton && !m_stateRButton) { StopState(); StartWindowLevel(); return; }
  if (!m_stateLButton && !m_stateMButton &&  m_stateRButton) { StopState(); StartDolly(); return; }
  if (!m_stateLButton && !m_stateMButton && !m_stateRButton) { StopState(); return; }
}

void InteractorStyleVolumeView::OnLeftButtonDown()
{
  m_stateLButton = true;
  dipatchActions();
}

void InteractorStyleVolumeView::OnLeftButtonUp()
{
  m_stateLButton = false;
  dipatchActions();
}

void InteractorStyleVolumeView::OnMiddleButtonDown()
{
  m_stateMButton = true;
  dipatchActions();
}

void InteractorStyleVolumeView::OnMiddleButtonUp()
{
  m_stateMButton = false;
  dipatchActions();
}

void InteractorStyleVolumeView::OnRightButtonDown()
{
  m_stateRButton = true;
  dipatchActions();
}

void InteractorStyleVolumeView::OnRightButtonUp()
{
  m_stateRButton = false;
  dipatchActions();
}

/** 
  - change Left Interaction Mode on Space.
  - rotate 90 degress when [CTRL] + [Cursor Key] is pressed
  */
void InteractorStyleVolumeView::OnKeyDown()
{
  vtkInteractorStyle::OnKeyDown();
  const char *keySym = this->GetInteractor()->GetKeySym();
  if (this->GetInteractor()->GetControlKey()) {
    int x,y;
    this->Interactor->GetEventPosition(x,y);
    this->FindPokedRenderer(x, y);
    if (keySymLeft == keySym) { RotateBy(90,0); return; }
    if (keySymRight == keySym) { RotateBy(-90,0); return; }
    if (keySymUp == keySym) { RotateBy(0,90); return; }
    if (keySymDown == keySym) { RotateBy(0,-90); return; }
  }
  if (keySymSpace == keySym) { CycleLeftButtonAction(); return; }
  cerr << __FILE__ << "[" << __LINE__ << "]:" << __FUNCTION__ << " Code:" << (int)this->GetInteractor()->GetKeyCode() << " Sym:" << this->GetInteractor()->GetKeySym() << endl;
}

/** animate the Left Mouse Button hint*/
void InteractorStyleVolumeView::OnTimer() 
{
//  vtkInteractorStyle::OnTimer();
  if (m_leftMBHintAlpha > 0) {
    m_leftMBHintAlpha -= 0.5 / this->GetInteractor()->GetDesiredUpdateRate();
    if (m_leftMBHintAlpha >= 1) return;
    if (m_leftMBHintAlpha <= 0) 
      SetLMBHint( 0 );
    else {
      SetLMBHint( m_leftMBHintAlpha );
    }
    this->Interactor->Render();
  } else {
    StopAnimate();
    vtkRenderWindowInteractor *rwi = this->Interactor;
    rwi->Render();
  }
}

/** Cycles through all vtkInteractorStyleProjectionView::InterAction. */
void InteractorStyleVolumeView::CycleLeftButtonAction() {
  switch( m_LMBState ) {
    case VTKIS_NONE: m_LMBState = VTKIS_ROTATE;break;
    case VTKIS_ROTATE: m_LMBState = VTKIS_PAN;break;
    case VTKIS_PAN: m_LMBState = VTKIS_SPIN;break;
    case VTKIS_SPIN: m_LMBState = VTKIS_DOLLY;break;
    case VTKIS_DOLLY: m_LMBState = VTKIS_WINDOWLEVEL;break;
    case VTKIS_WINDOWLEVEL: m_LMBState = VTKIS_NONE;break;
    default: m_LMBState = VTKIS_NONE; break;
  };
  updateLMBHint();
}

/** Sets Left Mouse Button Hint Text and Visibility. 
  @param [in] alpha alpha value (opacity) 0 means invisible, alpha > 1 is clipped to 1
  @param [in] text optional, set new text
  */
void InteractorStyleVolumeView::SetLMBHint( float alpha, const char *text ) {
  if ( !m_leftMBHint ) {
    if ( this->CurrentRenderer ==  NULL ) return;
    m_leftMBHint = vtkTextActor::New();
    this->CurrentRenderer->AddActor( m_leftMBHint );
  }
  if (m_leftMBHint) {
    if (text) m_leftMBHint->SetInput(text);
    if (alpha <= 0) m_leftMBHint->SetVisibility(false);
    else {
      if (alpha > 1) alpha = 1;
      vtkTextProperty *tp = m_leftMBHint->GetTextProperty();
      tp->SetOpacity( alpha );
      m_leftMBHint->SetTextProperty( tp );
      m_leftMBHint->SetVisibility(true);
    }
  }
}

/** Save the state of the Mouse in order to enable Restricted Actions and ActionCancles*/
void InteractorStyleVolumeView::saveMousePosition(void) {
  Interactor->GetEventPosition(mousePositionBegin[0], mousePositionBegin[1]);
  actionDelta[0] = 0;
  actionDelta[1] = 0;
}

/** Updated the Hint for the Left Mouse Button and starts Animation to fade the hint. */
void InteractorStyleVolumeView::updateLMBHint() {
  switch( m_LMBState ) {
    case VTKIS_NONE:		SetLMBHint(1, "None"); break;
//    case ActionSlice:		SetLMBHint(1, "Slice"); break;
    case VTKIS_ROTATE:		SetLMBHint(1, "Rotate"); break;
    case VTKIS_SPIN:		SetLMBHint(1, "Spin"); break;
    case VTKIS_DOLLY:		SetLMBHint(1, "Dolly"); break;
    case VTKIS_PAN:		SetLMBHint(1, "Pan"); break;
    case VTKIS_WINDOWLEVEL:	SetLMBHint(1, "Window/Level"); break;
    default:;
  } 
  m_leftMBHintAlpha = 2;
  if (AnimState != VTKIS_ANIM_ON) StartAnimate();
  this->Interactor->Render();
}


/** Gathers Event information and calls the appropiate Action */
void InteractorStyleVolumeView::OnMouseMove()
{
  int x,y;
  Interactor->GetEventPosition(x,y);
  int lx,ly;
  Interactor->GetLastEventPosition(lx,ly);
  int dx = x - mousePositionBegin[0];
  int dy = y - mousePositionBegin[1];
  if (updateRestricted()) {
    if (abs(dx) > abs(dy)) {
      Interactor->SetLastEventPosition( lx, y );
    } else {
      Interactor->SetLastEventPosition( x, ly );
    }
  }
  
  switch (this->State) 
    {
    case VTKIS_WINDOWLEVEL:
      this->FindPokedRenderer(x, y);
      this->WindowLevel();
      this->InvokeEvent(vtkCommand::InteractionEvent, NULL);
      break;

    case VTKIS_ZOOM:
      this->FindPokedRenderer(x, y);
      this->Zoom();
      this->InvokeEvent(vtkCommand::InteractionEvent, NULL);
      break;
    default:
    vtkInteractorStyleTrackballCamera::OnMouseMove();
    }
}

void InteractorStyleVolumeView::RotateBy( double theta, double phi) {
  if (this->CurrentRenderer == NULL)
    {
    return;
    }
  vtkRenderWindowInteractor *rwi = this->Interactor;
  
  vtkCamera *camera = this->CurrentRenderer->GetActiveCamera();
  
  if (m_restricted) {
    if (abs(theta) > abs(phi)) {
      phi = -actionDelta[1];
    } else {
      theta = -actionDelta[0];
    }
    actionDelta[0] += theta;
    actionDelta[1] += phi;
  }
  camera->Azimuth(theta);
  camera->Elevation(phi);
  camera->OrthogonalizeViewUp();
  
  if (this->AutoAdjustCameraClippingRange)
    {
    this->CurrentRenderer->ResetCameraClippingRange();
    }

  if (rwi->GetLightFollowCamera()) 
    {
    this->CurrentRenderer->UpdateLightsGeometryToFollowCamera();
    }

  rwi->Render();
}

void InteractorStyleVolumeView::StopState() 
{
  if (this->State != VTKIS_NONE)
    vtkInteractorStyle::StopState();
}

/** Restrict the Action to major Axis?*/
bool InteractorStyleVolumeView::updateRestricted() {
  switch( State ) {
    case VTKIS_ROTATE:
      m_restricted = this->Interactor->GetControlKey();
	return m_restricted;
    default:
      m_restricted = false; return false;
  }
}


void InteractorStyleVolumeView::Rotate()
{
  if (this->CurrentRenderer == NULL)
    {
    return;
    }
  vtkRenderWindowInteractor *rwi = this->Interactor;

  int dx = rwi->GetEventPosition()[0] - rwi->GetLastEventPosition()[0];
  int dy = rwi->GetEventPosition()[1] - rwi->GetLastEventPosition()[1];
  
  int *size = this->CurrentRenderer->GetRenderWindow()->GetSize();

  double delta_elevation = -20.0 / size[1];
  double delta_azimuth = -20.0 / size[0];
  
  double rxf = dx * delta_azimuth * this->MotionFactor;
  double ryf = dy * delta_elevation * this->MotionFactor;
  
  RotateBy( rxf, ryf );
}

void InteractorStyleVolumeView::StartWindowLevel() 
{
  if (this->State != VTKIS_NONE) 
    {
    return;
    }
  this->StartState(VTKIS_WINDOWLEVEL);
}

void InteractorStyleVolumeView::WindowLevel() {
  if (this->CurrentRenderer == NULL)
    {
    return;
    }
  if (!m_rayCastMapper) return;
  vtkRenderWindowInteractor *rwi = this->Interactor;

  int dw = rwi->GetEventPosition()[0] - rwi->GetLastEventPosition()[0];
  int dl = rwi->GetEventPosition()[1] - rwi->GetLastEventPosition()[1];
  
  if (dw || dl) {
    float newW = (1.0 - dw / 100.0) * m_rayCastMapper->GetFinalColorWindow();
    float newL = (1.0 - dl / 100.0) * m_rayCastMapper->GetFinalColorLevel();
    
    m_rayCastMapper->SetFinalColorWindow( newW );
    m_rayCastMapper->SetFinalColorLevel( newL );
    this->Interactor->Render();
  }
}

void InteractorStyleVolumeView::Zoom()
{
  if (this->CurrentRenderer == NULL)
    {
    return;
    }
  vtkRenderWindowInteractor *rwi = this->Interactor;
  double *center = this->CurrentRenderer->GetCenter();
  int dy = rwi->GetEventPosition()[1] - rwi->GetLastEventPosition()[1];
  double dyf = this->MotionFactor * dy / center[1];
  this->ZoomBy(pow(1.1, dyf));
}

//----------------------------------------------------------------------------
void InteractorStyleVolumeView::ZoomBy(double factor)
{
  if (this->CurrentRenderer == NULL)
    {
    return;
    }
  vtkCamera *camera = this->CurrentRenderer->GetActiveCamera();
  if (camera->GetParallelProjection())
    {
    camera->SetParallelScale(camera->GetParallelScale() / factor);
    }
  else
    {
    camera->Zoom(factor);
    if (this->AutoAdjustCameraClippingRange)
      {
      this->CurrentRenderer->ResetCameraClippingRange();
      }
    }
  this->Interactor->Render();
}
