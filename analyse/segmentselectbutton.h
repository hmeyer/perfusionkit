#ifndef SEGMENTSELECTBUTTON_H
#define SEGMENTSELECTBUTTON_H

#include <QPushButton>
#include "segmentlistmodel.h"


class SegmentSelectButton : public QPushButton
{
  Q_OBJECT
  public:
  SegmentSelectButton(QWidget *parent = 0);
  BinaryImageTreeItem *const getSelectedSegment(void) const { return selectedSegment; }
  private slots:
  void onClick(void);
  private:
  BinaryImageTreeItem * const selectedSegment;
};

#endif // SEGMENTSELECTBUTTON_H
