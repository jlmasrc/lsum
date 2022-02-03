/*
  lsum: C library for large floating point sums.
  Features:
  * Online, non-recursive pairwise summation (cascade summation)
  * Kahan summation

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

#include <stdlib.h>
#include <math.h>

#ifdef DEBUG_ALLOC
#include <stdio.h>
#endif

#include "lsum.h"

/** Pairwise summation ********************************************************/

/* The initial allocation for psum. A value of 20 allows over a million
   summands before a realloc(). */
int psum_minsize = 20;
/* At each realloc(), the psum will increase by this amount. A value of
   10 allows 1024 times more summands than the previous allocation */
int psum_increase = 10;

typedef struct psum {
  /* Alloc: array dynamically allocated; last: points to last element
     of array pointed by alloc */
  double *alloc, *last;
} psum;

/* Zero all elements of s->alloc starting from s->alloc[tail] */
static void psum_zero_tail(psum *s, int tail) {
  double *a = s->alloc + tail;
  while(a <= s->last) *(a++) = 0;
}

/* Zero all elements of s->alloc. This function has to used only if
   reusing s, as psum_alloc() already delivers a zeroed psum. */
void psum_zero(psum *s) {
  psum_zero_tail(s, 0);
}

/* Realloc s->alloc expanding it by increasing elements */
static void psum_expand(psum *s, int increase) {
  /* a->alloc == NULL indicates zero elements */
  int old_size = s->alloc ? (s->last - s->alloc + 1) : 0;
  int new_size = old_size + increase;
  s->alloc = realloc(s->alloc, new_size * sizeof(*(s->alloc)));
  s->last = s->alloc + new_size - 1;
  psum_zero_tail(s, old_size); /* Initialize new memory */
#ifdef DEBUG_ALLOC
  printf("DEBUG old_size = %d, new_size = %d\n", old_size, new_size);
#endif
}

/* Return a newly allocated psum with psum_minsize elements */
psum *psum_alloc(void) {
  psum *s = malloc(sizeof(*s));
  /* Indicates zero elements to psum_expand; s->last is undefined. */
  s->alloc = NULL;
  psum_expand(s, psum_minsize);
  return s;
}

/* free s->alloc and s */
void psum_free(psum *s) {
  free(s->alloc);
  free(s);
}

/* Add x to s. If s->alloc[] is exhausted, realloc it with psum_expand() */
void psum_add(psum *s, double x) {
  double *a = s->alloc;

  /* Unroll some loops to avoid frequent boundary checks */
  if(*a) { x += *a; *(a++) = 0; } else { *a = x; return; }
  if(*a) { x += *a; *(a++) = 0; } else { *a = x; return; }
  if(*a) { x += *a; *(a++) = 0; } else { *a = x; return; }

  while(*a) {
    x += *a;
    *(a++) = 0;
  }
  *a = x;

  /* Boundary check */
  if(a == s->last) psum_expand(s, psum_increase);
}

/* Return the value stored in s. */
double psum_value(psum *s) {
  double t = 0, *a = s->alloc;
  while(a <= s->last) t += *(a++);
  return t;
}

/******************************************************** Pairwise summation **/
 
 
/** Kahan summation ***********************************************************/

/* Zero s */
void ksum_zero(ksum s) {
  s[0] = 0;
  s[1] = 0;
}

/* Add x to s
   s[0]: Main value
   s[1]: Compensation */
void ksum_add(ksum s, double x) {
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

/* Returns the value stored in s */
double ksum_value(ksum s) {
  return s[0] + s[1];
}

/*********************************************************** Kahan summation **/
