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
