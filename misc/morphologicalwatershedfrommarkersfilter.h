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

#ifndef MORPHOLOGICALWATERSHEDFROMMARKERSFILTER_H
#define MORPHOLOGICALWATERSHEDFROMMARKERSFILTER_H

#include <itkImageToImageFilter.h>
#include <../Review/itkMorphologicalWatershedFromMarkersImageFilter.h>

template<class TInputImage, class TLabelImage>
class MorphologicalWatershedFromMarkersFilter : 
    public itk::ImageToImageFilter<TInputImage, TLabelImage>
{
public:
  /** Standard class typedefs. */
  typedef MorphologicalWatershedFromMarkersFilter Self;
  typedef itk::ImageToImageFilter<TInputImage, TLabelImage> Superclass;
  typedef itk::SmartPointer<Self>                           Pointer;
  typedef itk::SmartPointer<const Self>                     ConstPointer;

  /** Some convenient typedefs. */
  typedef TInputImage                             InputImageType;
  typedef TLabelImage                             LabelImageType;
  typedef typename InputImageType::Pointer        InputImagePointer;
  typedef typename InputImageType::ConstPointer   InputImageConstPointer;
  typedef typename InputImageType::RegionType     InputImageRegionType;
  typedef typename InputImageType::PixelType      InputImagePixelType;
  typedef typename LabelImageType::Pointer        LabelImagePointer;
  typedef typename LabelImageType::ConstPointer   LabelImageConstPointer;
  typedef typename LabelImageType::RegionType     LabelImageRegionType;
  typedef typename LabelImageType::PixelType      LabelImagePixelType;
  
  typedef typename LabelImageType::IndexType      IndexType;
  
  /** ImageDimension constants */
  itkStaticConstMacro(ImageDimension, unsigned int,
                      TInputImage::ImageDimension);

  /** Standard New method. */
  itkNewMacro(Self);  

  /** Runtime information support. */
  itkTypeMacro(MorphologicalWatershedFromMarkersImageFilter, 
               ImageToImageFilter);
	       
  /** Get the marker image */
  const LabelImageType * GetMarkerImage() const
    {
      return m_filter->GetMarkerImage();
//    return static_cast<LabelImageType*>(
//      const_cast<DataObject *>(this->ProcessObject::GetInput(1)));
    }  
    
  /** Set the input image */
  void SetInput1(const TInputImage *input)
    {
    this->SetInput( input );
    }

  /** Set the marker image */
  void SetInput2(const TLabelImage *input)
    {
    this->SetMarkerImage( input );
    }


void SetInput(const TInputImage* input);
void SetMarkerImage(const TLabelImage* markers);
TLabelImage *GetOutput();



protected:
  MorphologicalWatershedFromMarkersFilter();
  ~MorphologicalWatershedFromMarkersFilter() {};
  void PrintSelf(std::ostream& os, itk::Indent indent) const;

  /** MorphologicalWatershedFromMarkersImageFilter needs to request the
   * entire input images.
   */
  void GenerateInputRequestedRegion();

  /** This filter will enlarge the output requested region to produce
   * all of the output.
   * \sa ProcessObject::EnlargeOutputRequestedRegion() */
  void EnlargeOutputRequestedRegion(itk::DataObject *itkNotUsed(output));

  /** The filter is single threaded. */
//  void GenerateData();
  
private:
  //purposely not implemented
  MorphologicalWatershedFromMarkersFilter(const Self&);
  void operator=(const Self&); //purposely not implemented
  
  typedef itk::MorphologicalWatershedFromMarkersImageFilter<TInputImage, TLabelImage> InternalFilterType;
  typename InternalFilterType::Pointer m_filter;

}; // end of class

#include "morphologicalwatershedfrommarkersfilter.txx"

#endif // MORPHOLOGICALWATERSHEDFROMMARKERSFILTER_H
