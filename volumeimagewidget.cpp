#include "volumeimagewidget.h"


#include "vtkMatrix4x4.h"
#include "vtkInteractorStyleImage.h"
#include "vtkCommand.h"

#include "vtkImageReslice.h"
#include "vtkLookupTable.h"
#include "vtkImageMapToColors.h"
#include "vtkImageActor.h"
#include "vtkRenderer.h"
#include "vtkImageData.h"
#include "vtkRenderWindow.h"


// The mouse motion callback, to turn "Slicing" on and off
class vtkImageInteractionCallback : public vtkCommand
{
public:

  static vtkImageInteractionCallback *New() {
    return new vtkImageInteractionCallback; };
 
  vtkImageInteractionCallback() {
    this->Slicing = 0; 
    this->ImageReslice = 0;
    this->Interactor = 0; };

  void SetImageReslice(vtkImageReslice *reslice) {
    this->ImageReslice = reslice; };

  vtkImageReslice *GetImageReslice() {
    return this->ImageReslice; };

  void SetInteractor(vtkRenderWindowInteractor *interactor) {
    this->Interactor = interactor; };

  vtkRenderWindowInteractor *GetInteractor() {
    return this->Interactor; };

  virtual void Execute(vtkObject *, unsigned long event, void *)
    {
    vtkRenderWindowInteractor *interactor = this->GetInteractor();

    int lastPos[2];
    interactor->GetLastEventPosition(lastPos);
    int currPos[2];
    interactor->GetEventPosition(currPos);
    
    if (event == vtkCommand::LeftButtonPressEvent)
      {
      this->Slicing = 1;
      }
    else if (event == vtkCommand::LeftButtonReleaseEvent)
      {
      this->Slicing = 0;
      }
    else if (event == vtkCommand::MouseMoveEvent)
      {
      if (this->Slicing)
        {
        vtkImageReslice *reslice = this->ImageReslice;

        // Increment slice position by deltaY of mouse
        int deltaY = lastPos[1] - currPos[1];

        reslice->GetOutput()->UpdateInformation();
        double sliceSpacing = reslice->GetOutput()->GetSpacing()[2];
        vtkMatrix4x4 *matrix = reslice->GetResliceAxes();
        // move the center point that we are slicing through
        double point[4];
        double center[4];
        point[0] = 0.0;
        point[1] = 0.0;
        point[2] = sliceSpacing * deltaY;
        point[3] = 1.0;
        matrix->MultiplyPoint(point, center);
        matrix->SetElement(0, 3, center[0]);
        matrix->SetElement(1, 3, center[1]);
        matrix->SetElement(2, 3, center[2]);
        interactor->Render();
        }
      else
        {
        vtkInteractorStyle *style = vtkInteractorStyle::SafeDownCast(
          interactor->GetInteractorStyle());
        if (style)
          {
          style->OnMouseMove();
          }
        }
      }
    };
 
private: 
  
  // Actions (slicing only, for now)
  int Slicing;

  // Pointer to vtkImageReslice
  vtkImageReslice *ImageReslice;

  // Pointer to the interactor
  vtkRenderWindowInteractor *Interactor;
};



VolumeImageWidget::VolumeImageWidget():
  m_reslice(vtkImageReslice::New()),
  m_table(vtkLookupTable::New()),
  m_color(vtkImageMapToColors::New()),
  m_actor(vtkImageActor::New()),
  m_renderer(vtkRenderer::New()),
  m_resliceAxes(vtkMatrix4x4::New()),
  m_interactorStyle(vtkInteractorStyleImage::New()),
  m_interactionCallback(vtkImageInteractionCallback::New())
{
  m_reslice->SetOutputDimensionality(2);
  m_reslice->SetInterpolationModeToLinear();

  m_table->SetRange(-50, 350); // image intensity range
  m_table->SetValueRange(0.0, 1.0); // from black to white
  m_table->SetSaturationRange(0.0, 0.0); // no color saturation
  m_table->SetRampToLinear();
  m_table->Build();

  m_color->SetLookupTable(m_table);
  m_color->SetInputConnection(m_reslice->GetOutputPort());

  m_actor->SetInput(m_color->GetOutput());
  m_renderer->AddActor(m_actor);
  
  
  // Set up the interaction
  vtkRenderWindowInteractor *interactor = this->GetInteractor();
  interactor->SetInteractorStyle(m_interactorStyle);

  m_interactionCallback->SetImageReslice(m_reslice);
  m_interactionCallback->SetInteractor(interactor);

  m_interactorStyle->AddObserver(vtkCommand::MouseMoveEvent, m_interactionCallback);
  m_interactorStyle->AddObserver(vtkCommand::LeftButtonPressEvent, m_interactionCallback);
  m_interactorStyle->AddObserver(vtkCommand::LeftButtonReleaseEvent, m_interactionCallback);
  
  
    // Matrices for axial, coronal, sagittal, oblique view orientations
    //static double axialElements[16] = {
    //         1, 0, 0, 0,
    //         0, 1, 0, 0,
    //         0, 0, 1, 0,
    //         0, 0, 0, 1 };

    static double coronalElements[16] = {
	    1, 0, 0, 0,
	    0, 0, 1, 0,
	    0, 1, 0, 0,
	    0, 0, 0, 1 };

    // static double sagittalElements[16] = {
    // 	0, 0,-1, 0,
    // 	1, 0, 0, 0,
    // 	0,-1, 0, 0,
    // 	0, 0, 0, 1 };

    //static double obliqueElements[16] = {
    //         1, 0, 0, 0,
    //         0, 0.866025, -0.5, 0,
    //         0, 0.5, 0.866025, 0,
    //         0, 0, 0, 1 };  
    
    // Set the slice orientation
    m_resliceAxes->DeepCopy(coronalElements);
    m_reslice->SetResliceAxes(m_resliceAxes);
}

VolumeImageWidget::~VolumeImageWidget() {
  this->hide();
  m_renderer->Delete();
  m_actor->Delete();
  m_color->Delete();
  m_interactionCallback->Delete();
  m_interactorStyle->Delete();
  m_reslice->Delete();
  m_table->Delete();
  m_resliceAxes->Delete();
}


void VolumeImageWidget::setImage(vtkImageData *image) {
  if (!image) {
    m_image = NULL;
    vtkRenderWindow *window = this->GetRenderWindow();
    window->RemoveRenderer( m_renderer );
  } else {
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
    m_resliceAxes->SetElement(0, 3, center[0]);
    m_resliceAxes->SetElement(1, 3, center[1]);
    m_resliceAxes->SetElement(2, 3, center[2]);

    m_reslice->SetInput( m_image );
  //  m_reslice->Update();

    vtkRenderWindow *window = this->GetRenderWindow();
    window->AddRenderer(m_renderer);
  }
}