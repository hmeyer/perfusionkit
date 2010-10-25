/*
    This file is part of perfusionkit.
    Copyright 2010 Henning Meyer

    perfusionkit is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    perfusionkit is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with perfusionkit.  If not, see <http://www.gnu.org/licenses/>.
*/

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
  vtkTypeRevisionMacro(InteractorStyleVolumeView,vtkInteractorStyleTrackballCamera);
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
  void SetRayCastMapper( vtkFixedPointVolumeRayCastMapper *m);
  
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
  vtkFixedPointVolumeRayCastMapper *RayCastMapper; ///< pointer to RayCastMapper - if set
};


#endif // INTERACTORSTYLEVOLUMEVIEW_H
