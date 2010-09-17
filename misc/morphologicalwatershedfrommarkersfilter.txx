#include "morphologicalwatershedfrommarkersfilter.h"



template <class TInputImage, class TLabelImage>
MorphologicalWatershedFromMarkersFilter<TInputImage, TLabelImage>
::MorphologicalWatershedFromMarkersFilter()
{
  m_filter = InternalFilterType::New();
}

template <class TInputImage, class TLabelImage>
void MorphologicalWatershedFromMarkersFilter<TInputImage, TLabelImage>
::SetInput(const TInputImage* input)
{
  m_filter->SetInput(input);
}

template <class TInputImage, class TLabelImage>
void MorphologicalWatershedFromMarkersFilter<TInputImage, TLabelImage>
::SetMarkerImage(const TLabelImage* markers)
{
  m_filter->SetMarkerImage(markers);
}

template <class TInputImage, class TLabelImage>
TLabelImage *MorphologicalWatershedFromMarkersFilter<TInputImage, TLabelImage>
::GetOutput()
{
  return m_filter->GetOutput();
}

template <class TInputImage, class TLabelImage>
void 
MorphologicalWatershedFromMarkersFilter<TInputImage, TLabelImage>
::GenerateInputRequestedRegion()
{
  Superclass::GenerateInputRequestedRegion();
  
  // get pointers to the inputs
  LabelImagePointer markerPtr = 
    const_cast< LabelImageType * >( this->GetMarkerImage() );

  InputImagePointer inputPtr = 
    const_cast< InputImageType * >( this->GetInput() );
  
  if ( !markerPtr || !inputPtr )
    { return; }

  // We need to
  // configure the inputs such that all the data is available.
  //
  markerPtr->SetRequestedRegion(markerPtr->GetLargestPossibleRegion());
  inputPtr->SetRequestedRegion(inputPtr->GetLargestPossibleRegion());
}

template <class TInputImage, class TLabelImage>
void 
MorphologicalWatershedFromMarkersFilter<TInputImage, TLabelImage>
::EnlargeOutputRequestedRegion(itk::DataObject *)
{
  this->GetOutput()->SetRequestedRegion(
    this->GetOutput()->GetLargestPossibleRegion() );
}


template<class TInputImage, class TLabelImage>
void
MorphologicalWatershedFromMarkersFilter<TInputImage, TLabelImage>
::PrintSelf(std::ostream &os, itk::Indent indent) const
{
  Superclass::PrintSelf(os, indent);
}