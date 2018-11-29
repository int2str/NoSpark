// NoSpark - OpenEVSE charger firmware
// Copyright (C) 2015 Andre Eisenbach
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// See LICENSE for a copy of the GNU General Public License or see
// it online at <http://www.gnu.org/licenses/>.

#include <stdint.h>
#include <stdlib.h>

void *operator new(const size_t size) { return malloc(size); }

void operator delete(void *p, size_t) { free(p); }
void operator delete(void *p) { free(p); }

void *operator new[](const size_t size) { return malloc(size); }

void operator delete[](void *p) { free(p); }
void operator delete[](void *p, size_t) { free(p); }

extern "C" {
__extension__ typedef int __guard __attribute__((mode(__DI__)));

int __cxa_guard_acquire(__guard *g) { return !*reinterpret_cast<uint8_t *>(g); }

void __cxa_guard_release(__guard *g) { *reinterpret_cast<uint8_t *>(g) = 1; }

void __cxa_guard_abort(__guard *) {}

void __cxa_pure_virtual() {
  while (1) {
  }
}

int __attribute__((weak)) atexit(void (*)(void)) { return 0; }
}
