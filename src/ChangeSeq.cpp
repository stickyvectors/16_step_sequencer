#include "Arduino.h"
#include "ChangeSeq.h"
#include "Display.h"

ChangeSeq::ChangeSeq(int seq)

{
  Display Display;
  Display.seq(seq);
}
