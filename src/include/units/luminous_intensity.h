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

  struct dimension_luminous_intensity : make_dimension_t<exp<base_dim_luminous_intensity, 1>> {};
  template<> struct upcasting_traits<upcast_from<dimension_luminous_intensity>> : upcast_to<dimension_luminous_intensity> {};

  template<typename T>
  concept bool LuminousIntensity = Quantity<T> && std::Same<typename T::dimension, dimension_luminous_intensity>;

  template<Unit U = struct candela, Number Rep = double>
  using luminous_intensity = quantity<dimension_luminous_intensity, U, Rep>;

  struct candela : unit<dimension_luminous_intensity> {};
  template<> struct upcasting_traits<upcast_from<candela>> : upcast_to<candela> {};

  inline namespace literals {

    // cd
    constexpr auto operator""_cd(unsigned long long l) { return luminous_intensity<candela, std::int64_t>(l); }
    constexpr auto operator""_cd(long double l) { return luminous_intensity<candela, long double>(l); }

  }  // namespace literals

}  // namespace units
