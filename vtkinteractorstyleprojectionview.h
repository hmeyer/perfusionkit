#ifndef VTKINTERACTORSTYLEPROJECTIONVIEW_H
#define VTKINTERACTORSTYLEPROJECTIONVIEW_H

#include <vtkInteractorStyle.h>

class vtkImageMapToWindowLevelColors;
class vtkMatrix4x4;
class vtkTextActor;


/// Interactor Style specific for Projection Views.
/** This class should be used as interaction style for projection views.
It features mouse interaction with combination of Mouse Button presses 
as well as selection of interaction mode by pressing Space.
*/

class vtkInteractorStyleProjectionView : public vtkInteractorStyle
{
  public:
  /// Default Construction Method filled by vtkStandardNewMacro
  static vtkInteractorStyleProjectionView *New();

  vtkInteractorStyleProjectionView();
  ~vtkInteractorStyleProjectionView();
  
  /** @name Overidden Event Handlers
      inherited from vtkInteractorStyle */
  ///@{
  virtual void OnMouseMove();
  virtual void OnLeftButtonDown();
  virtual void OnLeftButtonUp();
  virtual void OnMiddleButtonDown();
  virtual void OnMiddleButtonUp();
  virtual void OnRightButtonDown();
  virtual void OnRightButtonUp();
  virtual void OnKeyDown();
  virtual void OnMouseWheelForward();
  virtual void OnMouseWheelBackward();
  virtual void OnTimer();
  ///@}
  
  
  /** @name Attribute Setters
      in order to give vtkInteractorStyleProjectionView needed access to the displayed entities */
  ///@{
  void SetImageMapToWindowLevelColors(vtkImageMapToWindowLevelColors *map/**<[in]*/) { m_imageMapToWindowLevelColors = map; }
  void SetOrientationMatrix(vtkMatrix4x4 *orientation/**<[in]*/) { m_orientation = orientation; }
  ///@}
  void CycleLeftButtonAction();
  void WindowLevelDelta( int dw, int dl, bool fromInitial=false  );
  void Slice( int dthickness, int dpos, bool fromInitial=false );
  void Rotate( int theta, int phi, bool fromInitial=false  );
  void Spin( int alpha );
  void Zoom( int delta );
  void Pan( int dx, int dy, bool fromInitial=false );
  protected:
  void updateRenderer();
  void dipatchActions();
  void updateLMBHint();
  void SetLMBHint( float alpha, const char *text = NULL );
  bool GetEventPosition( int &x, int &y, bool last=false );
  bool restrictAction();
  void saveDisplayState(void);
  void updateDisplay(void);
  
  /** Definition of different Interaction modes. */
  enum InterAction {
    ActionFirst, ///< first mode = ActionSlice
    ActionSlice = ActionFirst,
    ActionRotate,
    ActionSpin,
    ActionZoom,
    ActionPan,
    ActionWindowLevel,
    ActionNone,
    ActionLast = ActionNone, ///< last mode = ActionNone
  };
  struct DisplayState {
    int window;
    int level;
    int mousePosition[2];
    vtkMatrix4x4 *orientation;
  };
  InterAction m_interAction; ///< selected interaction due to mouse button presses - determined by dipatchActions()
  InterAction m_leftButtonAction; ///< selected interaction for the left mouse button - changed by pressing Space in CycleLeftButtonAction()
  
  /** @name Mouse Button Flags
      State of the Mouse Buttons (Pressed?) */
  //@{
  bool m_stateRButton; 
  bool m_stateLButton;
  bool m_stateMButton;
  //@}
  float m_sliceIncrement; ///< Value to Increment the Viewers Z-Position when slicing
  vtkTextActor *m_leftMBHint; ///< Hint actor for showing the Action associated with the Left Mouse Button
  float m_leftMBHintAlpha; ///< alpha value for #m_leftMBHint
  vtkImageMapToWindowLevelColors *m_imageMapToWindowLevelColors; ///< Mapper (set external via SetImageMapToWindowLevelColors()) for Window and Level (changed via vtkInteractorStyleProjectionView::ActionWindowLevel)
  vtkMatrix4x4 *m_orientation; ///< the Transformation Matrix of the displayed Data
  DisplayState m_initialState; ///< Display state at the beginning of an action
};

#endif // VTKINTERACTORSTYLEPROJECTIONVIEW_H
