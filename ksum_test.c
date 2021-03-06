/*
  lsum: C library for large floating point sums.
  Test function for Kahan summation.
  It is slower than pairwise Kahan summation but uses less memory.
  Theoretically, Kahan summation could exhaust precision while pairwise
  summation would stay precise. 

  Copyright (C) 2022  Joao Luis Meloni Assirati.

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.
  
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/


#include <stdio.h>

#include "lsum.h"
/* SIZE_BENCHMARK defined here */
#include "size_bm.h"


int main(int argc, char **argv) {
  long int i;
  ksum s;

  ksum_zero(s);
  for(i = 1; i <= SIZE_BENCHMARK; i++) {
    double f = 1.0 / 3;
    /* ksum_add() is an inline function. If a compiled version is
       needed, use ksum_cadd(). */
    ksum_add(s, f);
  }

  printf("%.16g\n", ksum_value(s));

  return 0;
}
