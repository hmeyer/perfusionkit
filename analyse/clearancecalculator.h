#ifndef CLEARANCECALCULATOR_H
#define CLEARANCECALCULATOR_H

#include "analysedialog.h"

class ClearanceCalculator {
  public:
  ClearanceCalculator(AnalyseDialog *parent_):parent(parent_) {};
  double getLeftCTClearance();
  double getRightCTClearance();
  private:
    double getCTClearance(SegmentSelectButton *cortex, SegmentSelectButton *artery, SegmentSelectButton *vein);
    bool checkSegment(SegmentSelectButton *seg);
    AnalyseDialog *parent;
};

#endif // CLEARANCECALCULATOR_H
