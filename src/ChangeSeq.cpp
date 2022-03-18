#include "Arduino.h"
#include "ChangeSeq.h"
#include "UpdateDisplay.h"

ChangeSeq::ChangeSeq(int seq)

{
  UpdateDisplay updateDisplay;
  updateDisplay.seq(seq);
}
