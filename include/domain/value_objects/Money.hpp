#pragma once

#include <stdexcept>
#include <string>
#include <format>
#include <compare>

namespace domain
{
  namespace value_objects
  {

    /**
     * 金額を表す値オブジェクト（C++23版）
     * 不変性を保証し、ビジネスルールを持つ
     *
     * 主要なC++機能の説明：
     * - explicit: 暗黙の型変換を防ぐ（Money m = 100; をエラーに）
     * - constexpr: コンパイル時に計算可能
     * - [[nodiscard]]: 戻り値を無視すると警告
     * - noexcept: 例外を投げないことを保証
     */
    class Money
    {
    public:
      // コンストラクタ：負の金額を拒否（ビジネスルール）
      explicit constexpr Money(int amount) : amount_(amount)
      {
        if (amount < 0)
        {
          throw std::invalid_argument("金額は0以上である必要があります");
        }
      }

      // ゲッター：金額を取得
      [[nodiscard]] constexpr int getAmount() const noexcept { return amount_; }

      // 加算：不変性のため、新しいMoneyインスタンスを返す
      [[nodiscard]] constexpr Money add(const Money &other) const
      {
        return Money(amount_ + other.amount_);
      }

      // 減算：結果が負になる場合は例外を投げる
      [[nodiscard]] constexpr Money subtract(const Money &other) const
      {
        if (amount_ < other.amount_)
        {
          throw std::logic_error("減算結果が負になります");
        }
        return Money(amount_ - other.amount_);
      }

      [[nodiscard]] std::string toString() const;

      // C++20の三方比較演算子（全ての比較演算子を自動生成）
      [[nodiscard]] constexpr auto operator<=>(const Money &other) const noexcept = default;
      [[nodiscard]] constexpr bool operator==(const Money &other) const noexcept = default;

    private:
      int amount_; // 金額（円単位）
    };

  } // namespace value_objects
} // namespace domain
