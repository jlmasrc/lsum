/*
  lsum: C library for large floating point sums.
  Test function for pairwise summation.
  It is faster that Kahan summation but uses more memory.
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
  psum *s = psum_alloc();

  for(i = 1; i <= SIZE_BENCHMARK; i++) {
    double f = 1.0 / 3;
    psum_add(s, f);
  }

  printf("%.16g\n", psum_value(s));

  psum_free(s);

  return 0;
}
