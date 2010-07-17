#include "segmentselectbutton.h"

SegmentSelectButton::SegmentSelectButton(QWidget *parent)
  :QPushButton(tr("not selected"), parent),
  selectedSegment(NULL) {
    connect(this, SIGNAL(clicked(bool)), this, SLOT(onClick()));
}


void SegmentSelectButton::onClick(void) {
  std::cerr << "clicked" << std::endl;
}
