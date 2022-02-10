/*
  lsum: C library for large sums of floating point numbers with controlled round-off error.
  Features:
  * Online, non-recursive pairwise summation (cascade summation)
  * Kahan summation
  See psum_test.c and ksum_test.c for pairwise and Kahan summation examples.

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
#include <stdio.h>

#include "lsum.h"

/** Pairwise summation ********************************************************/

/* The initial allocation for psum. A value of 20 allows over a million
   summands before a realloc(). */
int psum_minsize = 20;
/* At each realloc(), the psum will increase by this amount. A value of
   10 allows 1024 times more summands than the previous allocation */
int psum_increase = 10;

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

/* Realloc s->alloc expanding it by increasing elements. When summing
   n numbers, ->alloc will have around log2(n) elements, so the
   allocation size is small. */
static void psum_expand(psum *s, int increase) {
  /* a->alloc == NULL indicates zero elements */
  int old_size = s->alloc ? (s->last - s->alloc + 1) : 0;
  int new_size = old_size + increase;
  if(!(s->alloc = realloc(s->alloc, new_size * sizeof(*(s->alloc))))) {
    /* Very improbable, as the allocation size is small */
    fprintf(stderr, "psum: out of memory\n");
    exit(1);
  }
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
  /* If psum_minsize was redefined to a value too small, ignore it */
  psum_expand(s, psum_minsize < 10 ? 10 : psum_minsize);
  return s;
}

/* free s->alloc and s */
void psum_free(psum *s) {
  free(s->alloc);
  free(s);
}

/* Continue psum_add()'s job. If s->alloc[] is exhausted, realloc it
   with psum_expand() */
void psum_loop_(psum *s, double *a, double x) {
  while(*a) {
    x += *a;
    *(a++) = 0;
  }
  *a = x;

  /* Boundary check. If psum_increase was redefined to a value too
     small, ignore it */
  if(a == s->last) psum_expand(s, psum_increase < 1 ? 1 : psum_increase);
}

/* Compiled version of psum_add() */
void psum_cadd(psum *s, double x) {
  psum_add(s, x);
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

/* Compiled version of ksum_add() */
void ksum_cadd(ksum s, double x) {
  ksum_add(s, x);
}

/* Returns the value stored in s */
double ksum_value(ksum s) {
  return s[0] + s[1];
}

/*********************************************************** Kahan summation **/
