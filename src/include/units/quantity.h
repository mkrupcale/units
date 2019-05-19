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

#include <units/unit.h>
#include <units/bits/concepts.h>
#include <limits>
#include <gsl/gsl-lite.hpp>

namespace units {

  // is_quantity
  namespace detail {

    template<typename T>
    inline constexpr bool is_quantity = false;

  }

  template<typename T>
  concept bool Quantity = detail::is_quantity<T>;

  // Scalar

  template<typename T>
  concept bool Scalar = Number<T> && !Quantity<T>;

  template<Dimension D, Unit U, Scalar Rep>
      requires std::Same<D, typename U::dimension>
  class quantity;

  namespace detail {

    template<Dimension D, Unit U, Scalar Rep>
    inline constexpr bool is_quantity<quantity<D, U, Rep>> = true;

  }  // namespace detail

  // common_quantity
  template<Quantity Q1, Quantity Q2, Scalar Rep>
  struct common_quantity;

  template<Dimension D, Unit U, Scalar Rep1, Scalar Rep2, Scalar Rep>
  struct common_quantity<quantity<D, U, Rep1>, quantity<D, U, Rep2>, Rep> {
    using type = quantity<D, U, Rep>;
  };

  template<Dimension D, Unit U1, Scalar Rep1, Unit U2, Scalar Rep2, Scalar Rep>
  struct common_quantity<quantity<D, U1, Rep1>, quantity<D, U2, Rep2>, Rep> {
    using type = quantity<D, upcasting_traits_t<unit<D, units::common_ratio(U1::ratio, U2::ratio)>>, Rep>;
  };

  template<Quantity Q1, Quantity Q2, Scalar Rep = std::common_type_t<typename Q1::rep, typename Q2::rep>>
  using common_quantity_t = common_quantity<Q1, Q2, Rep>::type;

  // treat_as_floating_point

  template<typename Rep> // todo Conceptify that
  inline constexpr bool treat_as_floating_point = std::is_floating_point_v<Rep>;

  // quantity_cast

  namespace detail {

    template<Quantity To, Ratio CR, Scalar CRep, bool NumIsOne = false, bool DenIsOne = false>
    struct quantity_cast_impl {
      template<Quantity Q>
      static constexpr To cast(const Q& q)
      {
        return To(static_cast<To::rep>(static_cast<CRep>(q.count()) * static_cast<CRep>(CR.num) / static_cast<CRep>(CR.den)));
      }
    };

    template<Quantity To, Ratio CR, Scalar CRep>
    struct quantity_cast_impl<To, CR, CRep, true, true> {
      template<Quantity Q>
      static constexpr To cast(const Q& q)
      {
        return To(static_cast<To::rep>(q.count()));
      }
    };

    template<Quantity To, Ratio CR, Scalar CRep>
    struct quantity_cast_impl<To, CR, CRep, true, false> {
      template<Quantity Q>
      static constexpr To cast(const Q& q)
      {
        return To(static_cast<To::rep>(static_cast<CRep>(q.count()) / static_cast<CRep>(CR.den)));
      }
    };

    template<Quantity To, Ratio CR, Scalar CRep>
    struct quantity_cast_impl<To, CR, CRep, false, true> {
      template<Quantity Q>
      static constexpr To cast(const Q& q)
      {
        return To(static_cast<To::rep>(static_cast<CRep>(q.count()) * static_cast<CRep>(CR.num)));
      }
    };

  }  // namespace detail

  template<Quantity To, Dimension D, Unit U, Scalar Rep>
      requires std::Same<typename To::dimension, D>
  constexpr To quantity_cast(const quantity<D, U, Rep>& q)
  {
    constexpr auto c_ratio = U::ratio / To::unit::ratio;
    using c_rep = std::common_type_t<typename To::rep, Rep, intmax_t>;
    using cast = detail::quantity_cast_impl<To, c_ratio, c_rep, c_ratio.num == 1, c_ratio.den == 1>;
    return cast::cast(q);
  }

  // quantity_values

  template<Scalar Rep>
  struct quantity_values {
    static constexpr Rep zero() noexcept { return Rep(0); }
    static constexpr Rep one() noexcept { return Rep(1); }
    static constexpr Rep max() noexcept { return std::numeric_limits<Rep>::max(); }
    static constexpr Rep min() noexcept { return std::numeric_limits<Rep>::lowest(); }
  };

  // quantity

  template<Dimension D, Unit U, Scalar Rep>
      requires std::Same<D, typename U::dimension>
  class quantity {
    Rep value_;

  public:
    using dimension = D;
    using unit = U;
    using rep = Rep;

    static_assert(!Quantity<Rep>, "rep cannot be a quantity");

    quantity() = default;
    quantity(const quantity&) = default;

    template<std::ConvertibleTo<rep> Rep2>
        requires treat_as_floating_point<rep> || (!treat_as_floating_point<Rep2>)
    constexpr explicit quantity(const Rep2& r) : value_{static_cast<rep>(r)}
    {
    }

    template<Quantity Q2>
        requires std::Same<dimension, typename Q2::dimension> &&
                 std::ConvertibleTo<typename Q2::rep, rep> &&
                 (treat_as_floating_point<rep> ||
                   ((Q2::unit::ratio / unit::ratio).den == 1 && !treat_as_floating_point<typename Q2::rep>))
    constexpr quantity(const Q2& q) : value_{quantity_cast<quantity>(q).count()}
    {
    }

    quantity& operator=(const quantity& other) = default;

    [[nodiscard]] constexpr rep count() const noexcept { return value_; }

    [[nodiscard]] static constexpr quantity zero() noexcept { return quantity(quantity_values<Rep>::zero()); }
    [[nodiscard]] static constexpr quantity one() noexcept { return quantity(quantity_values<Rep>::one()); }
    [[nodiscard]] static constexpr quantity min() noexcept { return quantity(quantity_values<Rep>::min()); }
    [[nodiscard]] static constexpr quantity max() noexcept { return quantity(quantity_values<Rep>::max()); }

    [[nodiscard]] constexpr quantity operator+() const { return quantity(*this); }
    [[nodiscard]] constexpr quantity operator-() const { return quantity(-count()); }

    constexpr quantity& operator++()
    {
      ++value_;
      return *this;
    }
    constexpr quantity operator++(int) { return quantity(value_++); }

    constexpr quantity& operator--()
    {
      --value_;
      return *this;
    }
    constexpr quantity operator--(int) { return quantity(value_--); }

    constexpr quantity& operator+=(const quantity& q)
    {
      value_ += q.count();
      return *this;
    }

    constexpr quantity& operator-=(const quantity& q)
    {
      value_ -= q.count();
      return *this;
    }

    constexpr quantity& operator*=(const rep& rhs)
    {
      value_ *= rhs;
      return *this;
    }

    constexpr quantity& operator/=(const rep& rhs)
    {
      value_ /= rhs;
      return *this;
    }

    constexpr quantity& operator%=(const rep& rhs)
    {
      value_ %= rhs;
      return *this;
    }

    constexpr quantity& operator%=(const quantity& q)
    {
      value_ %= q.count();
      return *this;
    }
  };

  // clang-format off
  template<Dimension D, Unit U1, Scalar Rep1, Unit U2, Scalar Rep2>
  [[nodiscard]] constexpr Quantity operator+(const quantity<D, U1, Rep1>& lhs,
                                             const quantity<D, U2, Rep2>& rhs)
  {
    using common_rep = decltype(lhs.count() + rhs.count());
    using ret = common_quantity_t<quantity<D, U1, Rep1>, quantity<D, U2, Rep2>, common_rep>;
    return ret(ret(lhs).count() + ret(rhs).count());
  }

  template<Dimension D, Unit U1, Scalar Rep1, Unit U2, Scalar Rep2>
  [[nodiscard]] constexpr Quantity operator-(const quantity<D, U1, Rep1>& lhs,
                                             const quantity<D, U2, Rep2>& rhs)
  {
    using common_rep = decltype(lhs.count() - rhs.count());
    using ret = common_quantity_t<quantity<D, U1, Rep1>, quantity<D, U2, Rep2>, common_rep>;
    return ret(ret(lhs).count() - ret(rhs).count());
  }

//  template<Dimension D, Unit U, Scalar Rep1, Scalar Rep2>
  template<typename D, typename U, typename Rep1, typename Rep2>
  [[nodiscard]] constexpr Quantity operator*(const quantity<D, U, Rep1>& q,
                                             const Rep2& v)
    requires (!Quantity<Rep2>)
  {
    using common_rep = decltype(q.count()* v);
    using ret = quantity<D, U, common_rep>;
    return ret(ret(q).count() * v);
  }

  //template<Scalar Rep1, Dimension D, Unit U, Scalar Rep2>
  template<typename Rep1, typename D, typename U, typename Rep2>
  [[nodiscard]] constexpr Quantity operator*(const Rep1& v,
                                             const quantity<D, U, Rep2>& q)
    requires (!Quantity<Rep1>)
  {
    return q * v;
  }

  template<Dimension D1, Unit U1, Scalar Rep1, Dimension D2, Unit U2, Scalar Rep2>
  [[nodiscard]] constexpr Quantity operator*(const quantity<D1, U1, Rep1>& lhs,
                                             const quantity<D2, U2, Rep2>& rhs)
      requires treat_as_floating_point<decltype(lhs.count() * rhs.count())> || ((U1::ratio * U2::ratio).den == 1)
  {
    using dim = dimension_multiply_t<D1, D2>;
    using common_rep = decltype(lhs.count() * rhs.count());
    using ret = quantity<dim, upcasting_traits_t<unit<dim, U1::ratio * U2::ratio>>, common_rep>;
    return ret(lhs.count() * rhs.count());
  }

//  template<Scalar Rep1, Dimension D, Unit U, Scalar Rep2>
  template<typename Rep1, typename D, typename U, typename Rep2>
  [[nodiscard]] constexpr Quantity operator/(const Rep1& v,
                                             const quantity<D, U, Rep2>& q)
    requires (!Quantity<Rep1>)
  {
    Expects(q != std::remove_cvref_t<decltype(q)>(0));

    using dim = dim_invert_t<D>;
    using common_rep = decltype(v / q.count());
    using ret = quantity<dim, upcasting_traits_t<unit<dim, ratio(U::ratio.den, U::ratio.num)>>, common_rep>;
    using den = quantity<D, U, common_rep>;
    return ret(v / den(q).count());
  }

//  template<Dimension D, Unit U, Scalar Rep1, Scalar Rep2>
  template<typename D, typename U, typename Rep1, typename Rep2>
  [[nodiscard]] constexpr Quantity operator/(const quantity<D, U, Rep1>& q,
                                             const Rep2& v)
    requires (!Quantity<Rep2>)
  {
    Expects(v != Rep2{0});

    using common_rep = decltype(q.count() / v);
    using ret = quantity<D, U, common_rep>;
    return ret(ret(q).count() / v);
  }

  template<Dimension D, Unit U1, Scalar Rep1, Unit U2, Scalar Rep2>
  [[nodiscard]] constexpr Scalar operator/(const quantity<D, U1, Rep1>& lhs,
                                           const quantity<D, U2, Rep2>& rhs)
  {
    Expects(rhs != std::remove_cvref_t<decltype(rhs)>(0));

    using common_rep = decltype(lhs.count() / rhs.count());
    using cq = common_quantity_t<quantity<D, U1, Rep1>, quantity<D, U2, Rep2>, common_rep>;
    return cq(lhs).count() / cq(rhs).count();
  }

  template<Dimension D1, Unit U1, Scalar Rep1, Dimension D2, Unit U2, Scalar Rep2>
  [[nodiscard]] constexpr Quantity operator/(const quantity<D1, U1, Rep1>& lhs,
                                             const quantity<D2, U2, Rep2>& rhs)
    requires treat_as_floating_point<decltype(lhs.count() / rhs.count())> || ((U1::ratio / U2::ratio).den == 1)
  {
    Expects(rhs != std::remove_cvref_t<decltype(rhs)>(0));

    using common_rep = decltype(lhs.count() / rhs.count());
    using dim = dimension_divide_t<D1, D2>;
    using ret = quantity<dim, upcasting_traits_t<unit<dim, U1::ratio / U2::ratio>>, common_rep>;
    return ret(lhs.count() / rhs.count());
  }

  template<Dimension D, Unit U, Scalar Rep1, Scalar Rep2>
  [[nodiscard]] constexpr Quantity operator%(const quantity<D, U, Rep1>& q,
                                             const Rep2& v)
  {
    using common_rep = decltype(q.count() % v);
    using ret = quantity<D, U, common_rep>;
    return ret(ret(q).count() % v);
  }

  template<Dimension D, Unit U1, Scalar Rep1, Unit U2, Scalar Rep2>
  [[nodiscard]] constexpr Quantity operator%(const quantity<D, U1, Rep1>& lhs,
                                             const quantity<D, U2, Rep2>& rhs)
  {
    using common_rep = decltype(lhs.count() % rhs.count());
    using ret = common_quantity_t<quantity<D, U1, Rep1>, quantity<D, U2, Rep2>, common_rep>;
    return ret(ret(lhs).count() % ret(rhs).count());
  }

  // clang-format on

  template<Dimension D, Unit U1, Scalar Rep1, Unit U2, Scalar Rep2>
  [[nodiscard]] constexpr bool operator==(const quantity<D, U1, Rep1>& lhs, const quantity<D, U2, Rep2>& rhs)
  {
    using ct = common_quantity_t<quantity<D, U1, Rep1>, quantity<D, U2, Rep2>>;
    return ct(lhs).count() == ct(rhs).count();
  }

  template<Dimension D, Unit U1, Scalar Rep1, Unit U2, Scalar Rep2>
  [[nodiscard]] constexpr bool operator!=(const quantity<D, U1, Rep1>& lhs, const quantity<D, U2, Rep2>& rhs)
  {
    return !(lhs == rhs);
  }

  template<Dimension D, Unit U1, Scalar Rep1, Unit U2, Scalar Rep2>
  [[nodiscard]] constexpr bool operator<(const quantity<D, U1, Rep1>& lhs, const quantity<D, U2, Rep2>& rhs)
  {
    using ct = common_quantity_t<quantity<D, U1, Rep1>, quantity<D, U2, Rep2>>;
    return ct(lhs).count() < ct(rhs).count();
  }

  template<Dimension D, Unit U1, Scalar Rep1, Unit U2, Scalar Rep2>
  [[nodiscard]] constexpr bool operator<=(const quantity<D, U1, Rep1>& lhs, const quantity<D, U2, Rep2>& rhs)
  {
    return !(rhs < lhs);
  }

  template<Dimension D, Unit U1, Scalar Rep1, Unit U2, Scalar Rep2>
  [[nodiscard]] constexpr bool operator>(const quantity<D, U1, Rep1>& lhs, const quantity<D, U2, Rep2>& rhs)
  {
    return rhs < lhs;
  }

  template<Dimension D, Unit U1, Scalar Rep1, Unit U2, Scalar Rep2>
  [[nodiscard]] constexpr bool operator>=(const quantity<D, U1, Rep1>& lhs, const quantity<D, U2, Rep2>& rhs)
  {
    return !(lhs < rhs);
  }

}  // namespace units
