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

#include "multiplanarreformatwidget.h"
#include <vtkMatrix4x4.h>
#include <vtkinteractorstyleprojectionview.h>
#include <vtkCommand.h>
#include <vtkImageReslice.h>
#include <vtkImageMapToWindowLevelColors.h>
#include <vtkImageMapToColors.h>
#include <vtkScalarsToColors.h>
#include <vtkImageActor.h>
#include <vtkRenderer.h>
#include <vtkImageData.h>
#include <vtkRenderWindow.h>
#include <vtkTransform.h>
#include <algorithm>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>


#include <vtkCamera.h>

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
  m_reslice->SetInterpolationModeToCubic();
  
  m_colormap->SetOutputFormatToRGB();

  m_colormap->SetInputConnection(m_reslice->GetOutputPort());

  m_actor->SetInput(m_colormap->GetOutput());
  m_renderer->AddActor(m_actor);

  // Set up the interaction
  vtkTransform *transform = vtkTransform::New();
  transform->SetMatrix( m_reslicePlaneTransform );
  transform->RotateX(180);
  m_reslicePlaneTransform->DeepCopy( transform->GetMatrix() );
  transform->Delete();
  
  m_interactorStyle->SetImageMapToWindowLevelColors( m_colormap );
  m_interactorStyle->SetOrientationMatrix( m_reslicePlaneTransform );

  vtkRenderWindowInteractor *interactor = this->GetInteractor();
  interactor->SetInteractorStyle(m_interactorStyle);
  m_reslice->SetResliceAxes(m_reslicePlaneTransform);
  m_reslice->SetOutputDimensionality(2);
}

/** Destructor*/
MultiPlanarReformatWidget::~MultiPlanarReformatWidget() {
  this->hide();
  if (m_renderer) m_renderer->Delete();
  m_overlays.clear();
  if (m_colormap) m_colormap->Delete();
  if (m_reslice) m_reslice->Delete();
  if (m_actor) m_actor->Delete();
  if (m_interactorStyle) m_interactorStyle->Delete();
  if (m_reslicePlaneTransform) m_reslicePlaneTransform->Delete();
}

void MultiPlanarReformatWidget::resizeEvent( QResizeEvent * event ) {
  QVTKWidget::resizeEvent(event);
  int xres = this->size().width();
  int yres = this->size().height();
  m_reslice->SetOutputExtent(0,xres,0,yres,0,0);
  m_reslice->SetOutputOrigin(-xres/2.0,-yres/2.0,0);
  BOOST_FOREACH(OverlayMapType::value_type it, m_overlays) {
    it.second->resize( xres, yres );
  }
}

void MultiPlanarReformatWidget::setCubicInterpolation(bool cubic) {
  if (cubic) m_reslice->SetInterpolationModeToCubic();
  else m_reslice->SetInterpolationModeToLinear();
}


/** Volume Setter*/
void MultiPlanarReformatWidget::setImage(vtkImageData *image/**<[in] Volume (3D) Image with one component*/) {
  if (image==NULL) {
    m_image = NULL;
    vtkRenderWindow *window = this->GetRenderWindow();
    window->RemoveRenderer( m_renderer );
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
    m_reslice->SetOutputSpacing(1,1,1);
    window->AddRenderer(m_renderer);
  }
  this->update();
}


int MultiPlanarReformatWidget::addBinaryOverlay(vtkImageData *image, const QColor &color, const ActionDispatch &dispatch) {
  if (m_overlays.find( image ) == m_overlays.end() ) {
    int actionHandle;
    RGBType rgbColor;
    rgbColor[0] = color.red();
    rgbColor[1] = color.green();
    rgbColor[2] = color.blue();
    boost::shared_ptr< vtkBinaryImageOverlay > overlay(
     new vtkBinaryImageOverlay( m_renderer, m_interactorStyle, dispatch, image, m_reslicePlaneTransform, rgbColor, actionHandle ) );
    m_overlays.insert( OverlayMapType::value_type( image, overlay ) );
    overlay->resize( this->size().width(), this->size().height() );
    this->update();
    return actionHandle;
  }
  return -1;
}

void MultiPlanarReformatWidget::removeBinaryOverlay(vtkImageData *image) {
  m_overlays.erase(image);
  this->update();
}

void MultiPlanarReformatWidget::activateOverlayAction(vtkImageData *image) {
  OverlayMapType::iterator it = m_overlays.find( image );
  if (it != m_overlays.end()) {
    it->second->activateAction();
  }
}

void MultiPlanarReformatWidget::activateAction(int actionHandle) {
  m_interactorStyle->activateAction(actionHandle);
}

int MultiPlanarReformatWidget::addAction(const ActionDispatch &dispatch) {
  return m_interactorStyle->addAction(dispatch);
}

void MultiPlanarReformatWidget::removeAction(int actionHandle) {
  m_interactorStyle->removeAction(actionHandle);
}

