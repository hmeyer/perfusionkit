#include "vtkinteractorstyleprojectionview.h"

#include <vtkObjectFactory.h>
#include <vtkImageMapToWindowLevelColors.h>
#include <vtkMatrix4x4.h>
#include <vtkTransform.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>
#include <vtkImageReslice.h>
#include <vtkImageData.h>
#include "vtkRenderWindowInteractor.h"
#include <vtkRenderer.h>

#include <string>
#include <iostream>

using namespace std;

const string keySymLeft = "Left";
const string keySymRight = "Right";
const string keySymUp = "Up";
const string keySymDown = "Down";
const string keySymSpace = "space";

vtkStandardNewMacro(vtkInteractorStyleProjectionView);

/// default Constructor
vtkInteractorStyleProjectionView::vtkInteractorStyleProjectionView():
  m_interAction(ActionNone),
  m_leftButtonAction(ActionSlice),
  m_stateRButton(false),
  m_stateLButton(false),
  m_stateMButton(false),
  m_sliceIncrement(1.0),
  m_leftMBHint( NULL ),
  m_imageMapToWindowLevelColors(NULL),
  m_orientation(NULL)
{
  State = VTKIS_NONE;
  UseTimers = 1;
  SetLMBHint(0);
  m_initialState.orientation = vtkMatrix4x4::New();
}

/// Destructor
vtkInteractorStyleProjectionView::~vtkInteractorStyleProjectionView() {
  if (m_leftMBHint) m_leftMBHint->Delete();
  m_initialState.orientation->Delete();
}

/** Get Position of Current Event 
  @return true, if successful, false otherwise
*/
bool vtkInteractorStyleProjectionView::GetEventPosition( int &x/**<[out]*/, int &y/**<[out]*/, bool last/**<[in]GetLastEventPosition instead*/ ) {
  vtkRenderWindowInteractor *rwi = this->Interactor;
  if (rwi) {
    const int *pos;
    if (!last)
      pos = rwi->GetEventPosition();
    else
      pos = rwi->GetLastEventPosition();
    if (pos) {
      x = pos[0];
      y = pos[1];
      return true;
    }
  }
  return false;
}

/** Save the state of the Display in order to enable Restricted Actions and ActionCancles*/
void vtkInteractorStyleProjectionView::saveDisplayState(void) {
  if (m_imageMapToWindowLevelColors) {
    m_initialState.window = m_imageMapToWindowLevelColors->GetWindow();
    m_initialState.level = m_imageMapToWindowLevelColors->GetLevel();
  }
  GetEventPosition( m_initialState.mousePosition[0], m_initialState.mousePosition[1]);
  if (m_orientation) m_initialState.orientation->DeepCopy( m_orientation );
}


/** dispatch Actions according to the State of the Mouse Buttons */
void vtkInteractorStyleProjectionView::dipatchActions() {
  saveDisplayState();
  if ( m_stateLButton &&  m_stateMButton &&  m_stateRButton) { m_interAction = ActionSpin; return; }
  if ( m_stateLButton &&  m_stateMButton && !m_stateRButton) { m_interAction = ActionRotate; return; }
  if ( m_stateLButton && !m_stateMButton &&  m_stateRButton) { m_interAction = ActionZoom; return; }
  if ( m_stateLButton && !m_stateMButton && !m_stateRButton) { m_interAction = m_leftButtonAction; return; }
  if (!m_stateLButton &&  m_stateMButton &&  m_stateRButton) { m_interAction = ActionPan; return; }
  if (!m_stateLButton &&  m_stateMButton && !m_stateRButton) { m_interAction = ActionWindowLevel; return; }
  if (!m_stateLButton && !m_stateMButton &&  m_stateRButton) { m_interAction = ActionSlice; return; }
  if (!m_stateLButton && !m_stateMButton && !m_stateRButton) { m_interAction = ActionNone; return; }
}

/** Cycles through all vtkInteractorStyleProjectionView::InterAction. */
void vtkInteractorStyleProjectionView::CycleLeftButtonAction() {
  m_leftButtonAction = static_cast<InterAction>(m_leftButtonAction + 1);
  if (m_leftButtonAction >= ActionLast)
    m_leftButtonAction = ActionFirst;
  updateLMBHint();
}

/** Updated the Hint for the Left Mouse Button and starts Animation to fade the hint. */
void vtkInteractorStyleProjectionView::updateLMBHint() {
  switch( m_leftButtonAction ) {
    case ActionNone:		SetLMBHint(1, "None"); break;
    case ActionSlice:		SetLMBHint(1, "Slice"); break;
    case ActionRotate:		SetLMBHint(1, "Rotate"); break;
    case ActionSpin:		SetLMBHint(1, "Spin"); break;
    case ActionZoom:		SetLMBHint(1, "Zoom"); break;
    case ActionPan:		SetLMBHint(1, "Pan"); break;
    case ActionWindowLevel:	SetLMBHint(1, "Window/Level"); break;
    default:;
  } 
  m_leftMBHintAlpha = 2;
  if (AnimState != VTKIS_ANIM_ON) StartAnimate();
  updateDisplay();
}

/** Update Renderer information, so that vtkInteractorStyleProjectionView::GetCurrentRenderer() actually works*/
void vtkInteractorStyleProjectionView::updateRenderer() {
  int x,y;
  if (GetEventPosition(x,y)) 
    FindPokedRenderer(x,y);
}

/** Sets Left Mouse Button Hint Text and Visibility. 
  @param [in] alpha alpha value (opacity) 0 means invisible, alpha > 1 is clipped to 1
  @param [in] text optional, set new text
  */
void vtkInteractorStyleProjectionView::SetLMBHint( float alpha, const char *text ) {
  if ( !m_leftMBHint ) {
    updateRenderer();
    vtkRenderer *ren = GetCurrentRenderer();
    if (ren) {
      m_leftMBHint = vtkTextActor::New();
      ren->AddActor( m_leftMBHint );
    }
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

/** Restrict the Action to major Axis?*/
bool vtkInteractorStyleProjectionView::restrictAction() { 
  switch( m_interAction ) {
    case ActionSpin:
    case ActionZoom: return false;
    default:
        if (this->Interactor)
	  return this->Interactor->GetControlKey();
	else 
	  return false;
  }
}


/** Gathers Event information and calls the appropiate Action */
void vtkInteractorStyleProjectionView::OnMouseMove()
{
  vtkInteractorStyle::OnMouseMove();
  if (m_interAction != ActionNone) {
    int x,y,ox,oy;
    if (!GetEventPosition(x,y) || !GetEventPosition(ox,oy,true)) return;
    int dx = x - ox;
    int dy = y - oy;
    bool restrict = restrictAction();
    if (restrict) {
      dx = x - m_initialState.mousePosition[0];
      dy = y - m_initialState.mousePosition[0];
      if (abs(dx) > abs(dy)) dy = 0; else dx = 0;
    }
   
    switch( m_interAction ) {
      case ActionWindowLevel: WindowLevelDelta( dx, dy, restrict ); return;
      case ActionSlice: Slice( dx, dy, restrict ); return;
      case ActionRotate: Rotate( -dx, dy, restrict ); return;
      case ActionSpin: Spin( dx ); return;
      case ActionZoom: Zoom( dy ); return;
      case ActionPan: Pan( dx, dy, restrict ); return;
      default:     cerr << __FILE__ << "[" << __LINE__ << "]:" << __FUNCTION__ << ": Action=" << m_interAction << endl; return;
    }
  }
}

/** Slice forward */
void vtkInteractorStyleProjectionView::OnMouseWheelForward() {
  vtkInteractorStyle::OnMouseWheelForward();
  Slice(0, 1);
}

/** Slice backward */
void vtkInteractorStyleProjectionView::OnMouseWheelBackward() {
  vtkInteractorStyle::OnMouseWheelBackward();
  Slice(0, -1);
}

/** Pan the viewed Object */
void vtkInteractorStyleProjectionView::Pan( int dx/**<[in]*/, int dy/**<[in]*/, bool fromInitial/**<[in] do it from initial State?*/) {
  if (m_orientation) {
    updateRenderer();
    vtkRenderer *ren = this->GetCurrentRenderer();
    if (ren) {
      double zero[3], d[3];
      zero[0] = 0; zero[1] = 0; zero[2] = 0;
      d[0] = dx; d[1] = dy; d[2] = 0;
      
      ren->SetDisplayPoint( zero );
      ren->DisplayToView();
      ren->GetViewPoint( zero );
      ren->ViewToWorld( zero[0], zero[1], zero[2] );
      
      ren->SetDisplayPoint( d );
      ren->DisplayToView();
      ren->GetViewPoint( d );
      ren->ViewToWorld( d[0], d[1], d[2] );

      vtkTransform *newTransform = vtkTransform::New();
      newTransform->PreMultiply();
      if (fromInitial)
	newTransform->SetMatrix( m_initialState.orientation );
      else
	newTransform->SetMatrix( m_orientation );
      newTransform->Translate( zero[0] - d[0], zero[1] - d[1], 0 );
      newTransform->GetMatrix( m_orientation );
      newTransform->Delete();
      updateDisplay();
    }
  }
}

/** update the Display */
void vtkInteractorStyleProjectionView::updateDisplay(void) {
/*
  if (m_orientation->GetMTime() > this->GetInteractor()->GetMTime()) {
    updateRenderExtent();
  }
*/  
  this->GetInteractor()->Render();
}


/** zoom the viewed object*/
void vtkInteractorStyleProjectionView::Zoom( int delta/**<[in] positive numbers mean positive zoom, negative....*/) {
  if (delta && m_orientation) {
    double zoom = 1.0 - delta / 100.0;
    vtkTransform *newTransform = vtkTransform::New();
    newTransform->PreMultiply();
    newTransform->SetMatrix( m_orientation );
    newTransform->Scale( zoom, zoom, zoom );
    newTransform->GetMatrix( m_orientation );
    newTransform->Delete();
    updateDisplay();
  }
}

/** spin the viewed object around the viewing direction */
void vtkInteractorStyleProjectionView::Spin( int angle/**<[in] in degrees*/) {
  if (m_orientation) {
    vtkTransform *newTransform = vtkTransform::New();
    newTransform->PreMultiply();
    newTransform->SetMatrix( m_orientation );
    newTransform->RotateZ( angle );
    newTransform->GetMatrix( m_orientation );
    newTransform->Delete();
    updateDisplay();
  }
}

/** rotate the viewed object around axes perpendicular to the viewing direction*/
void vtkInteractorStyleProjectionView::Rotate( int theta/**<[in] angle around vertical axis*/, int phi/**<[in] angle around horizontal axis*/, bool fromInitial/**<[in] do it from initial State?*/) {
  if (m_orientation) {
    vtkTransform *newTransform = vtkTransform::New();
    newTransform->PreMultiply();
    if (fromInitial)
      newTransform->SetMatrix( m_initialState.orientation );
    else
      newTransform->SetMatrix( m_orientation );
    newTransform->RotateX( phi );
    newTransform->RotateY( theta );
    newTransform->GetMatrix( m_orientation );
    newTransform->Delete();
    updateDisplay();
  }
}

/** slice the viewed object */
void vtkInteractorStyleProjectionView::Slice( int dthickness/**<unused*/, int dpos/**<[in] delta in position perpendicular to the viewing direction*/, bool fromInitial/**<[in] do it from initial State?*/ ) {
  if (m_orientation) {
    vtkTransform *newTransform = vtkTransform::New();
    newTransform->PreMultiply();
    if (fromInitial)
      newTransform->SetMatrix( m_initialState.orientation );
    else
      newTransform->SetMatrix( m_orientation );
    newTransform->Translate(0, 0, dpos* m_sliceIncrement);
    newTransform->GetMatrix( m_orientation );
    newTransform->Delete();
    updateDisplay();
  }
}

/** change Window and Level */
void vtkInteractorStyleProjectionView::WindowLevelDelta( int dw/**<[in] delta window*/, int dl/**<[in] delta level*/, bool fromInitial/**<[in] do it from initial State?*/  ) {
  if (m_imageMapToWindowLevelColors  && (dw || dl)) {
    if (fromInitial) {
      dw += m_initialState.window;
      dl += m_initialState.level;
    } else {
      dw += m_imageMapToWindowLevelColors->GetWindow();
      dl += m_imageMapToWindowLevelColors->GetLevel();
    }
    if (dw) m_imageMapToWindowLevelColors->SetWindow(dw);
    if (dl) m_imageMapToWindowLevelColors->SetLevel(dl);
    updateDisplay();
  }
}


void vtkInteractorStyleProjectionView::OnLeftButtonDown()
{
  vtkInteractorStyle::OnLeftButtonDown();
  m_stateLButton = true;
  dipatchActions();
}

void vtkInteractorStyleProjectionView::OnLeftButtonUp()
{
  vtkInteractorStyle::OnLeftButtonUp();
  m_stateLButton = false;
  dipatchActions();
}

void vtkInteractorStyleProjectionView::OnMiddleButtonDown()
{
  vtkInteractorStyle::OnMiddleButtonDown();
  m_stateMButton = true;
  dipatchActions();
}

void vtkInteractorStyleProjectionView::OnMiddleButtonUp()
{
  vtkInteractorStyle::OnMiddleButtonUp();
  m_stateMButton = false;
  dipatchActions();
}

void vtkInteractorStyleProjectionView::OnRightButtonDown()
{
  vtkInteractorStyle::OnRightButtonDown();
  m_stateRButton = true;
  dipatchActions();
}

void vtkInteractorStyleProjectionView::OnRightButtonUp()
{
  vtkInteractorStyle::OnRightButtonUp();
  m_stateRButton = false;
  dipatchActions();
}

/** 
  - change Left Interaction Mode on Space.
  - rotate 90 degress when [CTRL] + [Cursor Key] is pressed
  */
void vtkInteractorStyleProjectionView::OnKeyDown()
{
  vtkInteractorStyle::OnKeyDown();
  const char *keySym = this->GetInteractor()->GetKeySym();
  if (this->GetInteractor()->GetControlKey()) {
    if (keySymLeft == keySym) { Rotate(90,0); return; }
    if (keySymRight == keySym) { Rotate(-90,0); return; }
    if (keySymUp == keySym) { Rotate(0,90); return; }
    if (keySymDown == keySym) { Rotate(0,-90); return; }
  }
  if (keySymSpace == keySym) { CycleLeftButtonAction(); return; }
  cerr << __FILE__ << "[" << __LINE__ << "]:" << __FUNCTION__ << " Code:" << (int)this->GetInteractor()->GetKeyCode() << " Sym:" << this->GetInteractor()->GetKeySym() << endl;
}

/** animate the Left Mouse Button hint*/
void vtkInteractorStyleProjectionView::OnTimer() 
{
  vtkInteractorStyle::OnTimer();
  if (m_leftMBHintAlpha > 0) {
    m_leftMBHintAlpha -= 0.5 / this->GetInteractor()->GetDesiredUpdateRate();
    if (m_leftMBHintAlpha >= 1) return;
    if (m_leftMBHintAlpha <= 0) 
      SetLMBHint( 0 );
    else {
      SetLMBHint( m_leftMBHintAlpha );
    }
    updateDisplay();
  } else {
    StopAnimate();
  }
}
