#pragma once

#include <string>
#include <string_view>
#include <stdexcept>
#include <compare>
#include <chrono>

namespace domain
{
  namespace value_objects
  {

    /**
     * 日付を表す値オブジェクト（C++20版）
     * chrono::year_month_dayを活用
     */
    class Date
    {
    public:
      constexpr Date(int year, int month, int day);

      [[nodiscard]] constexpr int getYear() const noexcept { return year_; }
      [[nodiscard]] constexpr int getMonth() const noexcept { return month_; }
      [[nodiscard]] constexpr int getDay() const noexcept { return day_; }

      [[nodiscard]] std::string toString() const;

      [[nodiscard]] constexpr bool isBefore(const Date &other) const noexcept;
      [[nodiscard]] constexpr bool isAfter(const Date &other) const noexcept;

      // C++20の三方比較演算子
      [[nodiscard]] constexpr auto operator<=>(const Date &other) const noexcept = default;
      [[nodiscard]] constexpr bool operator==(const Date &other) const noexcept = default;

      static Date today();
      static Date fromString(std::string_view dateStr); // YYYY-MM-DD形式

    private:
      int year_;
      int month_;
      int day_;

      constexpr void validate() const;
    };

  } // namespace value_objects
} // namespace domain
