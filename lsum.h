/*
  lsum: C library for large floating point sums.
  Library header file.

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


#ifndef LSUM_H
#define LSUM_H

#include <math.h>

typedef struct psum {
  /* Alloc: array dynamically allocated; last: points to last element
     of array pointed by alloc */
  double *alloc, *last;
} psum;

extern int psum_minsize;
extern int psum_increase;

psum *psum_alloc(void);
void psum_free(psum *s);
void psum_zero(psum *s);
void psum_cadd(psum *s, double x);
double psum_value(psum *s);

void psum_loop_(psum *s, double *a, double x);

/* Add x to s */
static inline void psum_add(psum *s, double x) {
  double *a = s->alloc;

  /* Unroll some loops to increase performance. Do not unroll more
     than 10 loops (the minimum alloc size, see psum_alloc() at
     lsum.c) */
  if(*a) { x += *a; *(a++) = 0; } else { *a = x; return; }
  if(*a) { x += *a; *(a++) = 0; } else { *a = x; return; }
  if(*a) { x += *a; *(a++) = 0; } else { *a = x; return; }
  if(*a) { x += *a; *(a++) = 0; } else { *a = x; return; }

  psum_loop_(s, a, x);
}

typedef double ksum[2];

void ksum_zero(ksum k);
void ksum_cadd(ksum k, double x);
double ksum_value(ksum k);

/* Add x to s
   s[0]: Main value
   s[1]: Compensation */
static inline void ksum_add(ksum s, double x) {
  double s0 = s[0];
  volatile double t = s0 + x;

  /* Neumaier improvement of Kahan summation
     https://en.wikipedia.org/wiki/Kahan_summation_algorithm */
  if(fabs(s0) >= fabs(x)) {
    volatile double y = s0 - t;
    s[1] += y + x;
  } else {
    volatile double y = x - t;
    s[1] += y + s0;
  }

  s[0] = t;
}


#endif /* LSUM_H */
