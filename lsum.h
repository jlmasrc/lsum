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

typedef struct psum psum;

extern int psum_minsize;
extern int psum_increase;

psum *psum_alloc(void);
void psum_free(psum *s);
void psum_zero(psum *s);
void psum_add(psum *s, double x);
double psum_value(psum *s);

typedef double ksum[2];

void ksum_zero(ksum k);
void ksum_add(ksum k, double x);
double ksum_value(ksum k);

#endif /* LSUM_H */
