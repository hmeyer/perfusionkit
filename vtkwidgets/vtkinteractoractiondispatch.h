#ifndef VTKINTERACTORACTIONDISPATCH_H
#define VTKINTERACTORACTIONDISPATCH_H

#include "signalslib.hpp"
#include <boost/shared_ptr.hpp>

typedef boost::signal<void (int, int, float , float, float)> ActionSignal; // dx, dy, posx, posy, posz
typedef ActionSignal::slot_type ActionSlotType;
struct ActionDispatch {
  enum ActionType {
    MovingAction,
    ClickingAction
  };
  enum RestrictionType {
    Restricted,
    UnRestricted
  };
  boost::shared_ptr< ActionSignal > sig;
  std::string label;
  ActionType atype;
  RestrictionType restrict;
  bool valid;
  ActionDispatch(const std::string &label_, const ActionSignal::slot_type &slot, ActionType atype_, RestrictionType restrict_ )
    :sig( new ActionSignal ), label(label_), atype(atype_), restrict(restrict_), valid( true ) { sig->connect(slot); }
  ActionDispatch():valid(false) {}
};


#endif // VTKINTERACTORACTIONDISPATCH_H