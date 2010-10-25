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

#ifndef VOLUMEPROJECTIONWIDGET_H
#define VOLUMEPROJECTIONWIDGET_H

#include <QVTKWidget.h>

class vtkImageData;
class vtkVolume;
class vtkRenderer;
class vtkInteractorStyle;

class vtkVolumeMapper;
class vtkVolumeProperty;
class vtkPiecewiseFunction;

/// QT-Widget displaying a Volume in Maximum-Intensity-Projection (MIP)
class VolumeProjectionWidget : public QVTKWidget
{
  public:
  enum {
    Anaglyph,
    Interlaced,
    Off
  } StereoMode;
  VolumeProjectionWidget(QWidget* parent = NULL, Qt::WFlags f = 0);
  ~VolumeProjectionWidget();  
  void setImage(vtkImageData *image);
  double getCameraDistance(void);
  void setCameraDistance(double dist);
  double getEyeAngle(void);
  void setEyeAngle(double ang);
  void setStereoMode(int mode);
  protected:
  vtkVolume *m_volume; ///< vtkVolume which actually displays the projected volume
  vtkRenderer *m_renderer; ///< the used renderer
  vtkInteractorStyle *m_interactorStyle; ///< special InteractorStyle for Projected Volumes
  vtkVolumeMapper *m_volumeMapper;
  vtkVolumeProperty *m_volumeProperty;
  vtkPiecewiseFunction *m_opacityTransferFunction;
  vtkPiecewiseFunction *m_grayTransferFunction;
};

#endif // VOLUMEPROJECTIONWIDGET_H
