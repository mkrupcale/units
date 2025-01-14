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

#include <units/length.h>
#include <units/time.h>

namespace units {

  struct dimension_velocity : make_dimension_t<exp<base_dim_length, 1>, exp<base_dim_time, -1>> {};
  template<> struct upcasting_traits<upcast_from<dimension_velocity>> : upcast_to<dimension_velocity> {};

  template<typename T>
  concept bool Velocity = Quantity<T> && std::Same<typename T::dimension, dimension_velocity>;

  template<Unit U = struct meter_per_second, Number Rep = double>
  using velocity = quantity<dimension_velocity, U, Rep>;

  struct meter_per_second : derived_unit<dimension_velocity, meter, second> {};
  template<> struct upcasting_traits<upcast_from<meter_per_second>> : upcast_to<meter_per_second> {};

  struct kilometer_per_hour : derived_unit<dimension_velocity, kilometer, hour> {};
  template<> struct upcasting_traits<upcast_from<kilometer_per_hour>> : upcast_to<kilometer_per_hour> {};

  struct mile_per_hour : derived_unit<dimension_velocity, mile, hour> {};
  template<> struct upcasting_traits<upcast_from<mile_per_hour>> : upcast_to<mile_per_hour> {};

  inline namespace literals {

    // mps
    constexpr auto operator""_mps(unsigned long long l) { return velocity<meter_per_second, std::int64_t>(l); }
    constexpr auto operator""_mps(long double l) { return velocity<meter_per_second, long double>(l); }

    // kmph
    constexpr auto operator""_kmph(unsigned long long l) { return velocity<kilometer_per_hour, std::int64_t>(l); }
    constexpr auto operator""_kmph(long double l) { return velocity<kilometer_per_hour, long double>(l); }

    // mph
    constexpr auto operator""_mph(unsigned long long l) { return velocity<mile_per_hour, std::int64_t>(l); }
    constexpr auto operator""_mph(long double l) { return velocity<mile_per_hour, long double>(l); }

  }  // namespace literals

}  // namespace units
