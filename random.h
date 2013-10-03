/*-
 * Copyright (c) 2013 Zhihao Yuan.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef _RANDOM_H
#define _RANDOM_H 1

#include <random>
#include <type_traits>

namespace stdex {

namespace detail {

inline auto global_rng(bool seeded)
	-> std::default_random_engine&
{
	// can be seeded with rdtsc
	thread_local std::default_random_engine e{
	    seeded ? std::random_device{}()
	           : std::default_random_engine::default_seed
	};
	return e;
}

}

template <bool seeded = true, typename IntType>
inline IntType randint(IntType a, IntType b)
{
	// does not satisfy 26.5.1.1/1(e).
	static_assert(std::is_integral<IntType>(), "not an integral");

	using distribution_type = std::uniform_int_distribution<IntType>;
	using param_type = typename distribution_type::param_type;

	thread_local distribution_type d;
	return d(detail::global_rng(seeded), param_type(a, b));
}

}

#endif
