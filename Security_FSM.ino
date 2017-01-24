
#include "Security.h"

//                                                 Yellow
//               |---- Energia Pin Numbers ----|   timeout
//               alarm  reset  green  yellow red   (mSec)
Security zone_1 (2,     3,     4,     5,     6,    4000); 
Security zone_2 (7,     8,     9,     10,    11,   2000);
Security zone_3 (12,    13,    14,    15,    19,   3000);

void setup() {
  // Nothing needed here.
}

void loop() {
  zone_1.Update();
  zone_2.Update();
  zone_3.Update();
}
