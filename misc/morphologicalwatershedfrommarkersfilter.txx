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

#include "morphologicalwatershedfrommarkersfilter.h"
#include <memory>
#include <boost/foreach.hpp>
#include <map>



template <class TInputImage, class TLabelImage>
MorphologicalWatershedFromMarkersFilter<TInputImage, TLabelImage>
::MorphologicalWatershedFromMarkersFilter()
{
  m_internalWatershed = InternalWatershedImageFilterType::New();
  m_internalWatershed->SetThreshold(0.0);
  m_internalWatershed->SetLevel(0.2);
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
  m_internalWatershed->Print(os, indent.GetNextIndent());
}

template<class TInputImage, class TLabelImage>
class MorphologicalWatershedFromMarkersFilter<TInputImage, TLabelImage>::InternalWatershedProgress : public itk::Command {
  public: 
    typedef InternalWatershedProgress Self;
    typedef itk::Command Superclass;
    typedef itk::SmartPointer<Self> Pointer;
    itkNewMacro( Self );
    typedef typename MorphologicalWatershedFromMarkersFilter<TInputImage, TLabelImage>::InternalWatershedImageFilterType InternalWatershedImageFilterType;
  protected:
    InternalWatershedProgress() {};
    void Execute( itk::Object *caller, const itk::EventObject &event) {
      Execute((const itk::Object*)caller, event);
    }
    void Execute( const itk::Object *caller, const itk::EventObject &event) {
      const InternalWatershedImageFilterType *filter = dynamic_cast<const InternalWatershedImageFilterType *>(caller);
      std::cerr << "InternalWatershedProgress:" << filter->GetProgress() << std::endl;
    }
};


#include "../../myLibs/itkbasics.h"


class MergeTreeNode {
  public:
  typedef std::auto_ptr<MergeTreeNode> AutoPointer;
  MergeTreeNode(unsigned long l1, unsigned long l2)
    :parent(NULL), label1(l1), label2(l2) {
  }
  MergeTreeNode(AutoPointer c1, unsigned long l2)
    :parent(NULL), child1(c1), label2(l2) {
      c1->setParent(this);
  }
  MergeTreeNode(std::auto_ptr<MergeTreeNode> c1, std::auto_ptr<MergeTreeNode> c2)
    :parent(NULL), child1(c1), child2(c2) {
      c1->setParent(this);
      c2->setParent(this);
  }
  void setParent( MergeTreeNode *parent_ ) { parent = parent_; }
  virtual ~MergeTreeNode() {}
  protected:
    MergeTreeNode *parent;
    AutoPointer child1;
    AutoPointer child2;
    unsigned long label1;
    unsigned long label2;
};

template< class ScalarType >
MergeTreeNode::AutoPointer buildMergeTree(const itk::watershed::SegmentTree< ScalarType > *segmentTree) {
  typedef itk::watershed::SegmentTree< ScalarType > SegmentTreeType;
  std::cerr << "SegmentTree:" << segmentTree->Size() << " nodes" << std::endl;
  typedef std::map< unsigned long, MergeTreeNode::AutoPointer> LabelTreeMapType;
  
  LabelTreeMapType labelMap;
  
  for(typename SegmentTreeType::ConstIterator treeIter = segmentTree->Begin(); treeIter != segmentTree->End(); ++treeIter) { 
    unsigned long from = treeIter->from;
    unsigned long to = treeIter->to;
    std::cerr << __FUNCTION__ << " saliency:" << treeIter->saliency << " from:" << from << " to:" << to << std::endl;
    LabelTreeMapType::const_iterator nfrom = labelMap.find( from );
    LabelTreeMapType::const_iterator nto = labelMap.find( to );
    MergeTreeNode::AutoPointer node;
    if (nfrom != labelMap.end()) {
      if (nto != labelMap.end()) {
	node.reset( new MergeTreeNode(nfrom->second, nto->second ) );
      } else {
	node.reset( new MergeTreeNode(nfrom->second, to ) );
      }
    } else {
      if (nto != labelMap.end()) {
	node.reset( new MergeTreeNode(nto->second, from  ) );
      } else {
	node.reset( new MergeTreeNode(from, to ) );
      }
    }
    labelMap.erase( from );
    labelMap[ to ] = node;
  }
  std::cerr << "map size:" << labelMap.size() << std::endl;
  return MergeTreeNode::AutoPointer();
}


template<class TInputImage, class TLabelImage>
void
MorphologicalWatershedFromMarkersFilter<TInputImage, TLabelImage>
::GenerateData()
{
  typename InternalWatershedProgress::Pointer progressor = InternalWatershedProgress::New();

  m_internalWatershed->SetInput( this->GetInput() );
  m_internalWatershed->AddObserver(itk::AnyEvent(), progressor);
std::cerr << "before internalWatershedUpdate" << std::endl;  
  m_internalWatershed->Update();
std::cerr << "after internalWatershedUpdate" << std::endl;  
  m_internalWatershed->RemoveAllObservers();

  typename InternalWatershedImageFilterType::OutputImagePointer basicSeg = m_internalWatershed->GetBasicSegmentation();
  itkBasic::ImageSave( basicSeg, "/home/hmeyer/Desktop/perf-data/out%04d.png");
  
  
  MergeTreeNode::AutoPointer mergeTree = buildMergeTree( m_internalWatershed->GetSegmentTree() );

  static const LabelImagePixelType bgLabel
    = itk::NumericTraits< LabelImagePixelType >::Zero;
  // the label used to mark the watershed line in the output image
  static const LabelImagePixelType wsLabel
    = itk::NumericTraits< LabelImagePixelType >::Zero;

  this->AllocateOutputs();

  LabelImageConstPointer markerImage = this->GetMarkerImage();
  InputImageConstPointer inputImage = this->GetInput();
  LabelImagePointer outputImage = this->GetOutput();

  // Set up the progress reporter
  // we can't found the exact number of pixel to process in the 2nd pass, so we
  // use the maximum number possible.
  itk::ProgressReporter
    progress(this, 0, markerImage->GetRequestedRegion().GetNumberOfPixels()*2);
  //progress.CompletedPixel();
  
  // mask and marker must have the same size
  if( markerImage->GetRequestedRegion().GetSize() != inputImage->GetRequestedRegion().GetSize() )
    itkExceptionMacro( << "Marker and input must have the same size." );
  
}
