/*
 * PhySim library -- DEVS for physics
 * Copyright (C) 2020  Hugues Cassé <hug.casse@gmail.com>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
 *  USA
 */
#ifndef INCLUDE_PHYSIM_TYPE_H_
#define INCLUDE_PHYSIM_TYPE_H_

namespace physim {

typedef unsigned long long date_t;
typedef unsigned long long duration_t;

typedef enum {
	IN,
	OUT
} mode_t;

typedef enum {
	PORT,
	PARAM,
	STATE
} flavor_t;

class Type { };
template <class T> const Type&type_of() { static Type t; return t; }
template <class T> inline bool supports_real() { return false; }
template <> inline bool supports_real<bool>() { return true; }
template <> inline bool supports_real<int>() { return true; }
template <> inline bool supports_real<unsigned>() { return true; }
template <> inline bool supports_real<char>() { return true; }
template <> inline bool supports_real<signed char>() { return true; }
template <> inline bool supports_real<unsigned char>() { return true; }
template <> inline bool supports_real<signed short>() { return true; }
template <> inline bool supports_real<unsigned short>() { return true; }
template <> inline bool supports_real<signed long>() { return true; }
template <> inline bool supports_real<unsigned long>() { return true; }
template <> inline bool supports_real<signed long long>() { return true; }
template <> inline bool supports_real<unsigned long long>() { return true; }
template <> inline bool supports_real<float>() { return true; }
template <> inline bool supports_real<double>() { return true; }
template <> inline bool supports_real<long double>() { return true; }
template <class T> inline long double as_real(const T& x) { return 0; }
inline long double as_real(bool x) { return x; }
inline long double as_real(int x) { return x; }
inline long double as_real(unsigned int x) { return x; }
inline long double as_real(char x) { return x; }
inline long double as_real(signed char x) { return x; }
inline long double as_real(unsigned char x) { return x; }
inline long double as_real(signed short x) { return x; }
inline long double as_real(unsigned short x) { return x; }
inline long double as_real(signed long x) { return x; }
inline long double as_real(unsigned long x) { return x; }
inline long double as_real(signed long long x) { return x; }
inline long double as_real(unsigned long long x) { return x; }
inline long double as_real(float x) { return x; }
inline long double as_real(double x) { return x; }
inline long double as_real(long double x) { return x; }

}	// physim

#endif /* INCLUDE_PHYSIM_TYPE_H_ */
