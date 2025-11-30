#pragma once

#include "domain/value_objects/Money.hpp"
#include "domain/value_objects/Date.hpp"
#include "domain/value_objects/Category.hpp"
#include <string>
#include <string_view>
#include <compare>

namespace domain
{
  namespace entities
  {

    /**
     * 取引を表すエンティティ（C++20版）
     * 一意なIDを持ち、ライフサイクルを通じて同一性が保たれる
     */
    class Transaction
    {
    public:
      Transaction(
          std::string_view id,
          const value_objects::Date &date,
          const value_objects::Category &category,
          const value_objects::Money &amount,
          std::string_view description);

      // ゲッター
      [[nodiscard]] const std::string &getId() const noexcept { return id_; }
      [[nodiscard]] const value_objects::Date &getDate() const noexcept { return date_; }
      [[nodiscard]] const value_objects::Category &getCategory() const noexcept { return category_; }
      [[nodiscard]] const value_objects::Money &getAmount() const noexcept { return amount_; }
      [[nodiscard]] const std::string &getDescription() const noexcept { return description_; }

      // エンティティの同一性は IDで判定
      [[nodiscard]] bool operator==(const Transaction &other) const noexcept
      {
        return id_ == other.id_;
      }

    private:
      std::string id_;
      value_objects::Date date_;
      value_objects::Category category_;
      value_objects::Money amount_;
      std::string description_;
    };

  } // namespace entities
} // namespace domain
