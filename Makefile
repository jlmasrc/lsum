#
#  lsum: C library for large floating point sums.
#  Makefile
#  To run all benchmarks (requires GNU time program):
#  make benchmark
#  To make test programs:
#  make tests
#
#  Copyright (C) 2022  Joao Luis Meloni Assirati.
#
#  This program is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program.  If not, see <https://www.gnu.org/licenses/>.
#


CC = gcc
CFLAGS = -Wall -O3
.DEFAULT_GOAL = all_tests

tests := $(patsubst %_test.c,%_test,$(wildcard *_test.c))
bmtargets := $(tests:%_test=%_time)

# This makefile is a prerequisit for all targets
.EXTRA_PREREQS := $(firstword $(MAKEFILE_LIST))

all_tests: $(tests)

# Multiple targets in one rule
# Syntax: <targets> : <pattern> : <dependencies generated according to pattern> 
$(tests): %: %.o lsum.o
	$(CC) $(CFLAGS) -o $@ $^ -lm

## Rules for header file dependency generation #################################
# (As a side effect, .d files are created even with make clean)

include lsum.d $(tests:%=%.d)

%.d: %.c
	$(CC) -MM $< -MT '$@ $*.o' -MF $@

################################# Rules for header file dependency generation ##

# General compilation rule
%.o: %.c
	$(CC) $(CFLAGS) -c $<

# Hack to allow bmtargets to be phony.
# If the rule is defined as
# %_time: %_test
# it will not work as a phony target.
$(bmtargets): %_time: %_test
	@which time >/dev/null || (echo "I need the 'time' program (eg GNU time), please install it."; exit 1)
	@echo
	@echo ---------------------------
	@echo ./$<
	@/usr/bin/time --format 'cpu time = %Us' ./$<
	@echo ---------------------------

benchmark: all_tests $(bmtargets)

clean:
	rm -f *.o *.d $(tests)

.PHONY: all_tests benchmark $(bmtargets) clean
