#ifndef INTERACTORSTYLEVOLUMEVIEW_H
#define INTERACTORSTYLEVOLUMEVIEW_H

#include <vtkInteractorStyleTrackballCamera.h>

class vtkTextActor;
class vtkFixedPointVolumeRayCastMapper;


/// Interactor Style specific for Volume Views.
/** This class should be used as interaction style for Volume views.
It features mouse interaction with combination of Mouse Button presses 
as well as selection of interaction mode by pressing Space.
*/
class InteractorStyleVolumeView : public vtkInteractorStyleTrackballCamera
{
  public:
  /// Default Construction Method filled by vtkStandardNewMacro
  static InteractorStyleVolumeView *New();

  InteractorStyleVolumeView();
  ~InteractorStyleVolumeView();

  /** @name Overidden Event Handlers
      inherited from vtkInteractorStyleTrackballCamera */
  ///@{
  virtual void OnMouseMove();
  virtual void OnLeftButtonDown();
  virtual void OnLeftButtonUp();
  virtual void OnMiddleButtonDown();
  virtual void OnMiddleButtonUp();
  virtual void OnRightButtonDown();
  virtual void OnRightButtonUp();
  virtual void OnKeyDown();
//  virtual void OnMouseWheelForward();
//  virtual void OnMouseWheelBackward();
  virtual void OnTimer();
  virtual void StopState();
  ///@}

  void RotateBy( double theta, double phi  );
  virtual void Rotate();
  virtual void Zoom();
  virtual void ZoomBy( double factor );
  virtual void WindowLevel();
  virtual void StartWindowLevel();
  /// Set a RayCastMapper - in order to enable Window-Level-Modifications.
  void SetRayCastMapper( vtkFixedPointVolumeRayCastMapper *m) { m_rayCastMapper = m; }
  
/*
  void SpinBy( int alpha );
  void DollyBy( int delta );
  void PanBy( int dx, int dy, bool fromInitial=false );

  void Rotate();
  void Spin();
  void Dolly();
  void Pan();
*/
  void CycleLeftButtonAction();
 
  protected:
  void updateLMBHint();
  void SetLMBHint( float alpha, const char *text = NULL );
  void dipatchActions();
  void saveMousePosition(void);
  bool updateRestricted();


  int mousePositionBegin[2]; ///< Mouse position at the beginning of an action
  double actionDelta[2]; ///< Mouse travel since the beginning of an action
  /** @name Mouse Button Flags
      State of the Mouse Buttons (Pressed?) */
  //@{
  bool m_stateRButton; 
  bool m_stateLButton;
  bool m_stateMButton;
  //@}
  int m_LMBState;///< State of the LeftMouseButton-Action
  vtkTextActor *m_leftMBHint; ///< Hint actor for showing the Action associated with the Left Mouse Button
  float m_leftMBHintAlpha; ///< alpha value for #m_leftMBHint
  bool m_restricted; ///< flag indicating, whether or not the current Action is restrict to a major Axis
  vtkFixedPointVolumeRayCastMapper* m_rayCastMapper; ///< pointer to RayCastMapper - if set
};


#endif // INTERACTORSTYLEVOLUMEVIEW_H
