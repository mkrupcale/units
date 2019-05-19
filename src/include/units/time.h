// The MIT License (MIT)
//
// Copyright (c) 2018 Mateusz Pusz
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include <units/base_dimensions.h>
#include <units/quantity.h>

namespace units {

  struct dimension_time : make_dimension_t<exp<base_dim_time, 1>> {};
  template<> struct upcasting_traits<upcast_from<dimension_time>> : upcast_to<dimension_time> {};

  template<typename T>
  concept bool Time = Quantity<T> && std::Same<typename T::dimension, dimension_time>;

  template<Unit U = struct second, Number Rep = double>
  using time = quantity<dimension_time, U, Rep>;

  struct second : unit<dimension_time> {};
  template<> struct upcasting_traits<upcast_from<second>> : upcast_to<second> {};

  struct nanosecond : nano<second> {};
  template<> struct upcasting_traits<upcast_from<nanosecond>> : upcast_to<nanosecond> {};

  struct microsecond : micro<second> {};
  template<> struct upcasting_traits<upcast_from<microsecond>> : upcast_to<microsecond> {};

  struct millisecond : milli<second> {};
  template<> struct upcasting_traits<upcast_from<millisecond>> : upcast_to<millisecond> {};

  struct minute : unit<dimension_time, 60 * second::ratio> {};
  template<> struct upcasting_traits<upcast_from<minute>> : upcast_to<minute> {};

  struct hour : unit<dimension_time, 3600 * second::ratio> {};
  template<> struct upcasting_traits<upcast_from<hour>> : upcast_to<hour> {};

  inline namespace literals {

    // ns
    constexpr auto operator""_ns(unsigned long long l) { return time<nanosecond, std::int64_t>(l); }
    constexpr auto operator""_ns(long double l) { return time<nanosecond, long double>(l); }

    // us
    constexpr auto operator""_us(unsigned long long l) { return time<microsecond, std::int64_t>(l); }
    constexpr auto operator""_us(long double l) { return time<microsecond, long double>(l); }

    // ms
    constexpr auto operator""_ms(unsigned long long l) { return time<millisecond, std::int64_t>(l); }
    constexpr auto operator""_ms(long double l) { return time<millisecond, long double>(l); }

    // s
    constexpr auto operator""_s(unsigned long long l) { return time<second, std::int64_t>(l); }
    constexpr auto operator""_s(long double l) { return time<second, long double>(l); }

    // min
    constexpr auto operator""_min(unsigned long long l) { return time<minute, std::int64_t>(l); }
    constexpr auto operator""_min(long double l) { return time<minute, long double>(l); }

    // h
    constexpr auto operator""_h(unsigned long long l) { return time<hour, std::int64_t>(l); }
    constexpr auto operator""_h(long double l) { return time<hour, long double>(l); }

  }  // namespace literals

}  // namespace units
