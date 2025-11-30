#include "domain/value_objects/Date.hpp"
#include <format>
#include <chrono>
#include <ctime>

namespace domain
{
  namespace value_objects
  {

    constexpr Date::Date(int year, int month, int day)
        : year_(year), month_(month), day_(day)
    {
      validate();
    }

    constexpr void Date::validate() const
    {
      if (year_ < 1900 || year_ > 2100)
      {
        throw std::invalid_argument("年は1900～2100の範囲である必要があります");
      }
      if (month_ < 1 || month_ > 12)
      {
        throw std::invalid_argument("月は1～12の範囲である必要があります");
      }
      if (day_ < 1 || day_ > 31)
      {
        throw std::invalid_argument("日は1～31の範囲である必要があります");
      }

      // 簡易的な日付チェック
      if (month_ == 2 && day_ > 29)
      {
        throw std::invalid_argument("2月の日付が不正です");
      }
      if ((month_ == 4 || month_ == 6 || month_ == 9 || month_ == 11) && day_ > 30)
      {
        throw std::invalid_argument("その月には31日は存在しません");
      }
    }

    std::string Date::toString() const
    {
      return std::format("{:04d}-{:02d}-{:02d}", year_, month_, day_);
    }

    constexpr bool Date::isBefore(const Date &other) const noexcept
    {
      if (year_ != other.year_)
        return year_ < other.year_;
      if (month_ != other.month_)
        return month_ < other.month_;
      return day_ < other.day_;
    }

    constexpr bool Date::isAfter(const Date &other) const noexcept
    {
      return other.isBefore(*this);
    }

    Date Date::today()
    {
      using namespace std::chrono;
      auto now = system_clock::now();
      auto days = floor<std::chrono::days>(now);
      year_month_day ymd{days};

      return Date(
          static_cast<int>(ymd.year()),
          static_cast<unsigned>(ymd.month()),
          static_cast<unsigned>(ymd.day()));
    }

    Date Date::fromString(std::string_view dateStr)
    {
      // YYYY-MM-DD形式をパース
      if (dateStr.length() != 10 || dateStr[4] != '-' || dateStr[7] != '-')
      {
        throw std::invalid_argument("日付形式はYYYY-MM-DDである必要があります");
      }

      int year = std::stoi(std::string(dateStr.substr(0, 4)));
      int month = std::stoi(std::string(dateStr.substr(5, 2)));
      int day = std::stoi(std::string(dateStr.substr(8, 2)));

      return Date(year, month, day);
    }

  } // namespace value_objects
} // namespace domain
