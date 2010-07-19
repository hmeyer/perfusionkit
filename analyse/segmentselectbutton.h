#ifndef SEGMENTSELECTBUTTON_H
#define SEGMENTSELECTBUTTON_H

#include <QPushButton>
#include "segmentlistmodel.h"


class SegmentSelectButton : public QPushButton
{
  Q_OBJECT
  public:
  SegmentSelectButton(QWidget *parent = 0);
  const SegmentListModel::SegmentInfo * getSelectedSegment(void) const { return selectedSegment; }
  void setSegmentListModel(const SegmentListModel *model) { segmentList = model; }
  void setSelection(const SegmentListModel::SegmentInfo *segment);
  
  signals:
  void selected(const SegmentListModel::SegmentInfo *segment);
  
  private slots:
  void onClick();
  private:
  void updateText();
  const SegmentListModel::SegmentInfo *selectedSegment;
  const SegmentListModel *segmentList;
};

#endif // SEGMENTSELECTBUTTON_H
