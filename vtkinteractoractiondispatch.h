#ifndef VTKINTERACTORACTIONDISPATCH_H
#define VTKINTERACTORACTIONDISPATCH_H

#include "signalslib.hpp"
#include <boost/shared_ptr.hpp>

typedef boost::signal<void (int, int, float , float, float)> ActionSignal; // dx, dy, posx, posy, posz
typedef ActionSignal::slot_type ActionSlotType;
struct ActionDispatch {
  boost::shared_ptr< ActionSignal > sig;
  std::string label;
  bool restricted;
  bool valid;
  ActionDispatch(const std::string &label_, const ActionSignal::slot_type &slot, bool restricted_ )
    :sig( new ActionSignal ), label(label_), restricted(restricted_), valid( true ) { sig->connect(slot); }
  ActionDispatch():valid(false) {}
};


#endif // VTKINTERACTORACTIONDISPATCH_H