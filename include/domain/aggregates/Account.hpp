#pragma once

#include "domain/entities/Transaction.hpp"
#include "domain/value_objects/Money.hpp"
#include "domain/value_objects/Category.hpp"
#include <vector>
#include <string>
#include <string_view>
#include <map>
#include <span>   // C++20: 軽量な配列ビュー
#include <ranges> // C++20: Rangesライブラリ

namespace domain
{
  namespace aggregates
  {

    /**
     * アカウント集約ルート（C++20版）
     * トランザクションを管理し、整合性を保証する
     */
    class Account
    {
    public:
      explicit Account(std::string_view accountId);

      // 集約ルートのID
      [[nodiscard]] const std::string &getAccountId() const noexcept { return accountId_; }

      // トランザクション操作
      // const参照(&): コピーを避けて効率的に渡す
      void addTransaction(const entities::Transaction &transaction);

      // std::span: ベクターへの軽量なビュー（コピーなし）
      // const: 読み取り専用
      // C++20のstd::spanが使えない場合は const std::vector<Transaction>& を返す方法もあります
      [[nodiscard]] std::span<const entities::Transaction> getTransactions() const noexcept;

      // ビジネスロジック
      [[nodiscard]] value_objects::Money calculateBalance() const;
      [[nodiscard]] value_objects::Money calculateIncomeTotal() const;
      [[nodiscard]] value_objects::Money calculateExpenseTotal() const;

      // カテゴリ別集計
      [[nodiscard]] std::map<value_objects::Category::Type, value_objects::Money>
      calculateByCategory() const;

      // 期間フィルタリング
      [[nodiscard]] std::vector<entities::Transaction> getTransactionsByPeriod(
          const value_objects::Date &startDate,
          const value_objects::Date &endDate) const;

      // カテゴリフィルタリング
      [[nodiscard]] std::vector<entities::Transaction> getTransactionsByCategory(
          const value_objects::Category &category) const;

    private:
      std::string accountId_;
      std::vector<entities::Transaction> transactions_;
    };

  } // namespace aggregates
} // namespace domain
