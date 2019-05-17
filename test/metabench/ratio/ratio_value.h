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

#include <numeric>
#include <gsl/gsl-lite.hpp>


namespace units {

  namespace detail {

    template<typename T>
    [[nodiscard]] constexpr T abs(T v) noexcept { return v < 0 ? -v : v; }

  }

  struct ratio {
    std::intmax_t num;
    std::intmax_t den;

    explicit constexpr ratio(std::intmax_t n, std::intmax_t d = 1):
      // Note: sign(N) * abs(N) == N
      num(n * (d < 0 ? -1 : 1) / std::gcd(n, d)),
      den(detail::abs(d) / std::gcd(n, d))
    {
      Expects(d != 0); // denominator cannot be zero
      Expects(n >= -std::numeric_limits<std::intmax_t>::max() &&
              d >= -std::numeric_limits<std::intmax_t>::max()); // out of range
    }

    // [[nodiscard]] constexpr bool operator==(const ratio &) noexcept = default;
    [[nodiscard]] friend constexpr bool operator==(const ratio& lhs, const ratio& rhs) noexcept { return lhs.num == rhs.num && lhs.den == rhs.den; }

    [[nodiscard]] friend constexpr ratio operator*(const ratio& lhs, const ratio& rhs)
    {
      const auto gcd1 = std::gcd(lhs.num, rhs.den);
      const auto gcd2 = std::gcd(rhs.num, lhs.den);
      return ratio((lhs.num / gcd1) * (rhs.num / gcd2), (lhs.den / gcd2) * (rhs.den / gcd1));
    }

    [[nodiscard]] friend constexpr ratio operator*(std::intmax_t n, const ratio& rhs)
    {
      return ratio(n) * rhs;
    }

    [[nodiscard]] friend constexpr ratio operator*(const ratio& lhs, std::intmax_t n)
    {
      return lhs * ratio(n);
    }

    [[nodiscard]] friend constexpr ratio operator/(const ratio& lhs, const ratio& rhs)
    {
      Expects(rhs.num != 0);
      return lhs * ratio(rhs.den, rhs.num);
    }

    [[nodiscard]] friend constexpr ratio operator/(std::intmax_t n, const ratio& rhs)
    {
      return ratio(n) / rhs;
    }

    [[nodiscard]] friend constexpr ratio operator/(const ratio& lhs, std::intmax_t n)
    {
      return lhs / ratio(n);
    }
  };

  // common_ratio

  // todo: simplified
  [[nodiscard]] constexpr ratio common_ratio(ratio f1, ratio f2) noexcept
  {
    const auto gcd_num = std::gcd(f1.num, f2.num);
    const auto gcd_den = std::gcd(f1.den, f2.den);
    return ratio(gcd_num, f1.den / gcd_den * f2.den);
  }

}
