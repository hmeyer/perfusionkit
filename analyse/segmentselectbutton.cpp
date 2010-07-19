#include "segmentselectbutton.h"
#include <QInputDialog>
#include <boost/foreach.hpp>

SegmentSelectButton::SegmentSelectButton(QWidget *parent)
  :QPushButton(0, parent),
  selectedSegment(NULL),
  segmentList(NULL) {
    updateText();
    connect(this, SIGNAL(clicked(bool)), this, SLOT(onClick()));
}


void SegmentSelectButton::onClick() {
  if (segmentList && segmentList->rowCount()>0) {
    QStringList nameList;
    BOOST_FOREACH(const SegmentListModel::SegmentInfo &seg, *segmentList) {
      nameList << seg.segment->getName();
    }
    bool ok;
    QString selectedName = QInputDialog::getItem(this, tr("Select Segment"), tr("Choose segment"), nameList, 0, false, &ok);
    if (!ok) return;
    BOOST_FOREACH(const SegmentListModel::SegmentInfo &seg, *segmentList) {
      if (seg.segment->getName() == selectedName) {
	selectedSegment = &seg;
	updateText();
	emit selected(selectedSegment);
	break;
      }
    }
  }
}

void SegmentSelectButton::setSelection(const SegmentListModel::SegmentInfo *segment) {
  selectedSegment = segment;
  updateText();
}

void SegmentSelectButton::updateText() {
  if (selectedSegment!=NULL) {
    setText( selectedSegment->segment->getName() );
  } else {
    setText( tr("not selected") );
  }
  repaint();
}
