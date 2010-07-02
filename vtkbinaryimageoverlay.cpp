#include "vtkbinaryimageoverlay.h"
#include <vtkMatrix4x4.h>
#include <vtkImageReslice.h>
#include <vtkImageMapToColors.h>
#include <vtkScalarsToColors.h>
#include <vtkImageActor.h>
#include <vtkImageData.h>
#include <vtkRenderer.h>


vtkBinaryImageOverlay::vtkBinaryImageOverlay( vtkSmartPointer<vtkRenderer> renderer,
			  vtkSmartPointer<vtkInteractorStyleProjectionView> interactorStyle,
			  const ActionDispatch &action, vtkImageData *image, vtkSmartPointer<vtkMatrix4x4> reslicePlaneTransform,
			  const unsigned char *color, double opacity)
  :m_image(image),
  m_reslice(vtkImageReslice::New()),
  m_colormap(vtkImageMapToColors::New()),
  m_lookup(vtkBinaryImageToColor::New(color[0],color[1],color[2])),
  m_actor(vtkImageActor::New()),
  m_renderer( renderer ),
  m_interactorStyle(interactorStyle), actionHandle(-1) {
    
  m_actor->SetOpacity(opacity);
  
  m_reslice->SetOutputDimensionality(2);
  m_reslice->SetBackgroundLevel(-1000);
  m_colormap->SetLookupTable(m_lookup);
  m_colormap->SetInputConnection(m_reslice->GetOutputPort());

  m_actor->SetInput(m_colormap->GetOutput());
  m_reslice->SetResliceAxes(reslicePlaneTransform);
  m_image->UpdateInformation();
  m_reslice->SetInput( m_image );
  
  if (m_renderer)
    m_renderer->AddActor(m_actor);
  
  if (action.valid) {
    actionHandle = interactorStyle->addAction(action);
  }
}

vtkBinaryImageOverlay::~vtkBinaryImageOverlay() {
  if (m_renderer) {
    m_renderer->RemoveActor( m_actor );
  }
  if (actionHandle!=-1) 
    m_interactorStyle->removeAction( actionHandle );
}

void vtkBinaryImageOverlay::activateAction() {
  m_interactorStyle->activateAction(actionHandle);
}

bool vtkBinaryImageOverlay::operator<(const vtkBinaryImageOverlay &other) const {
  return this->m_image < other.m_image;
}
