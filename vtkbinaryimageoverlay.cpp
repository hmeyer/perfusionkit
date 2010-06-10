#include "vtkbinaryimageoverlay.h"
#include <vtkMatrix4x4.h>
#include <vtkImageReslice.h>
#include <vtkImageMapToColors.h>
#include <vtkScalarsToColors.h>
#include <vtkImageActor.h>
#include <vtkImageData.h>


vtkBinaryImageOverlay::vtkBinaryImageOverlay(vtkImageData *image, vtkMatrix4x4 *reslicePlaneTransform, const unsigned char *color, double opacity)
  :m_image(image),
  m_reslice(vtkImageReslice::New()),
  m_colormap(vtkImageMapToColors::New()),
  m_lookup(vtkBinaryImageToColor::New(color[0],color[1],color[2])),
  m_actor(vtkImageActor::New()) {
    
  m_actor->SetOpacity(opacity);
  
  m_reslice->SetOutputDimensionality(2);
  m_reslice->SetBackgroundLevel(-1000);
  m_colormap->SetLookupTable(m_lookup);
  m_colormap->SetInputConnection(m_reslice->GetOutputPort());

  m_actor->SetInput(m_colormap->GetOutput());
  m_reslice->SetResliceAxes(reslicePlaneTransform);
  m_image->UpdateInformation();
  m_reslice->SetInput( m_image );
}

vtkBinaryImageOverlay::~vtkBinaryImageOverlay() {
  if (m_actor) m_actor->Delete();
  if (m_colormap) m_colormap->Delete();
  if (m_reslice) m_reslice->Delete();
  if (m_lookup) m_lookup->Delete();
}


bool vtkBinaryImageOverlay::operator<(const vtkBinaryImageOverlay &other) const {
  return this->m_image < other.m_image;
}
