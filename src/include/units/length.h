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

  struct dimension_length : make_dimension_t<exp<base_dim_length, 1>> {};
  template<> struct upcasting_traits<upcast_from<dimension_length>> : upcast_to<dimension_length> {};

  template<typename T>
  concept bool Length = Quantity<T> && std::Same<typename T::dimension, dimension_length>;

  template<Unit U = struct meter, Number Rep = double>
  using length = quantity<dimension_length, U, Rep>;

  // SI units
  struct meter : unit<dimension_length> {};
  template<> struct upcasting_traits<upcast_from<meter>> : upcast_to<meter> {};

  struct millimeter : milli<meter> {};
  template<> struct upcasting_traits<upcast_from<millimeter>> : upcast_to<millimeter> {};

  struct centimeter : centi<meter> {};
  template<> struct upcasting_traits<upcast_from<centimeter>> : upcast_to<centimeter> {};

  struct kilometer : kilo<meter> {};
  template<> struct upcasting_traits<upcast_from<kilometer>> : upcast_to<kilometer> {};

  inline namespace literals {

    // mm
    constexpr auto operator""_mm(unsigned long long l) { return length<millimeter, std::int64_t>(l); }
    constexpr auto operator""_mm(long double l) { return length<millimeter, long double>(l); }

    // cm
    constexpr auto operator""_cm(unsigned long long l) { return length<centimeter, std::int64_t>(l); }
    constexpr auto operator""_cm(long double l) { return length<centimeter, long double>(l); }

    // m
    constexpr auto operator""_m(unsigned long long l) { return length<meter, std::int64_t>(l); }
    constexpr auto operator""_m(long double l) { return length<meter, long double>(l); }

    // km
    constexpr auto operator""_km(unsigned long long l) { return length<kilometer, std::int64_t>(l); }
    constexpr auto operator""_km(long double l) { return length<kilometer, long double>(l); }

  } // namespace literals

  // US customary units
  struct yard : unit<dimension_length, ratio(9'144, 10'000)> {};
  template<> struct upcasting_traits<upcast_from<yard>> : upcast_to<yard> {};

  struct foot : unit<dimension_length, yard::ratio / 3> {};
  template<> struct upcasting_traits<upcast_from<foot>> : upcast_to<foot> {};

  struct inch : unit<dimension_length, foot::ratio / 12> {};
  template<> struct upcasting_traits<upcast_from<inch>> : upcast_to<inch> {};

  struct mile : unit<dimension_length, 1'760 * yard::ratio> {};
  template<> struct upcasting_traits<upcast_from<mile>> : upcast_to<mile> {};

  inline namespace literals {

    // yd
    constexpr auto operator""_yd(unsigned long long l) { return length<yard, std::int64_t>(l); }
    constexpr auto operator""_yd(long double l) { return length<yard, long double>(l); }

    // ft
    constexpr auto operator""_ft(unsigned long long l) { return length<foot, std::int64_t>(l); }
    constexpr auto operator""_ft(long double l) { return length<foot, long double>(l); }

    // in
    constexpr auto operator""_in(unsigned long long l) { return length<inch, std::int64_t>(l); }
    constexpr auto operator""_in(long double l) { return length<inch, long double>(l); }

    // mi
    constexpr auto operator""_mi(unsigned long long l) { return length<mile, std::int64_t>(l); }
    constexpr auto operator""_mi(long double l) { return length<mile, long double>(l); }

  }  // namespace literals

}  // namespace units
