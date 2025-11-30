#pragma once

#include <string>
#include <string_view>
#include <stdexcept>
#include <compare>

namespace domain
{
  namespace value_objects
  {

    /**
     * カテゴリを表す値オブジェクト（C++20版）
     */
    class Category
    {
    public:
      enum class Type
      {
        INCOME,        // 収入
        FOOD,          // 食費
        HOUSING,       // 住居費
        TRANSPORT,     // 交通費
        ENTERTAINMENT, // 娯楽
        UTILITIES,     // 光熱費
        HEALTHCARE,    // 医療費
        OTHER          // その他
      };

      explicit constexpr Category(Type type) noexcept : type_(type) {}

      [[nodiscard]] constexpr Type getType() const noexcept { return type_; }
      [[nodiscard]] std::string toString() const;

      [[nodiscard]] constexpr bool isIncome() const noexcept { return type_ == Type::INCOME; }
      [[nodiscard]] constexpr bool isExpense() const noexcept { return !isIncome(); }

      // C++20の三方比較演算子
      [[nodiscard]] constexpr auto operator<=>(const Category &other) const noexcept = default;
      [[nodiscard]] constexpr bool operator==(const Category &other) const noexcept = default;

      static Category fromString(std::string_view str);

    private:
      Type type_;
    };

  } // namespace value_objects
} // namespace domain
