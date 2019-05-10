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

#include <units/dimension.h>
#include <units/ratio.h>

namespace units {

  template<Dimension D, /* Ratio */ auto R = ratio(1)>
  struct unit : upcast_base<unit<D, R>> {
    using dimension = D;
    static constexpr auto ratio = R;
  };

  // is_unit

  namespace detail {

    template<typename T>
    inline constexpr bool is_unit = false;

    template<Dimension D, Ratio R>
    inline constexpr bool is_unit<unit<D, R>> = true;

  }

  template<typename T>
  concept bool Unit =
      std::is_empty_v<T>;// &&  // todo gcc bug https://gcc.gnu.org/bugzilla/show_bug.cgi?id=90100
      //detail::is_unit<upcast_from<T>>;


  // derived_unit

  namespace detail {

    template<Dimension D>
    struct get_unit_base_dim;

    template<Exponent E, Exponent... Rest>
    struct get_unit_base_dim<dimension<E, Rest...>> {
      static_assert(sizeof...(Rest) == 0, "Base unit expected");
      using dimension = E::dimension;
    };

    template<Unit U, Unit... Us>
    [[nodiscard]] consteval auto get_ratio(int base_dimension)
    {
      using unit_base_dim = get_unit_base_dim<typename U::dimension::base_type>::dimension;
      if(unit_base_dim::value == base_dimension)
        return U::ratio;
      if constexpr(!sizeof...(Us))
        throw std::runtime_error("base_dimension not found");
      else
        return get_ratio<Us...>(base_dimension);
    }


    template<Unit... Us>
    [[nodiscard]] consteval auto derived_ratio(dimension<>) noexcept { return ratio(1); }

    template<Unit... Us, Exponent E, Exponent... Rest>
    [[nodiscard]] consteval auto derived_ratio(dimension<E, Rest...>) noexcept
    {
      auto conv = derived_ratio<Us...>(dimension<Rest...>());
      const auto e_ratio = get_ratio<Us...>(E::dimension::value);
      auto exp = E::value;
      for(; exp > 0; --exp) conv = conv * e_ratio;
      for(; exp < 0; ++exp) conv = conv / e_ratio;
      return conv;
    }

  }


  template<Dimension D, Unit... Us>
  using derived_unit = unit<D, detail::derived_ratio<Us...>(typename D::base_type())>;

  // prefixes
  template<Unit U> using atto = unit<typename U::dimension, U::ratio * std::atto()>;
  template<Unit U> using femto = unit<typename U::dimension, U::ratio * std::femto()>;
  template<Unit U> using pico = unit<typename U::dimension, U::ratio * std::pico()>;
  template<Unit U> using nano = unit<typename U::dimension, U::ratio * std::nano()>;
  template<Unit U> using micro = unit<typename U::dimension, U::ratio * std::micro()>;
  template<Unit U> using milli = unit<typename U::dimension, U::ratio * std::milli()>;
  template<Unit U> using centi = unit<typename U::dimension, U::ratio * std::centi()>;
  template<Unit U> using deca = unit<typename U::dimension, U::ratio * std::deca()>;
  template<Unit U> using hecto = unit<typename U::dimension, U::ratio * std::hecto()>;
  template<Unit U> using kilo = unit<typename U::dimension, U::ratio * std::kilo()>;
  template<Unit U> using mega = unit<typename U::dimension, U::ratio * std::mega()>;
  template<Unit U> using giga = unit<typename U::dimension, U::ratio * std::giga()>;
  template<Unit U> using tera = unit<typename U::dimension, U::ratio * std::tera()>;
  template<Unit U> using peta = unit<typename U::dimension, U::ratio * std::peta()>;
  template<Unit U> using exa = unit<typename U::dimension, U::ratio * std::exa()>;

}  // namespace units
