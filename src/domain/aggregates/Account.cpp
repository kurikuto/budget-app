#include "domain/aggregates/Account.hpp"
#include <algorithm>
#include <ranges>
#include <numeric>
#include <stdexcept>

namespace domain
{
  namespace aggregates
  {

    Account::Account(std::string_view accountId)
        : accountId_(accountId)
    {
      if (accountId.empty())
      {
        throw std::invalid_argument("アカウントIDは空にできません");
      }
    }

    void Account::addTransaction(const entities::Transaction &transaction)
    {
      // 重複チェック（ビジネスルール）- C++20 rangesを使用
      namespace ranges = std::ranges;

      auto isDuplicate = ranges::any_of(transactions_, [&transaction](const auto &t)
                                        { return t.getId() == transaction.getId(); });

      if (isDuplicate)
      {
        throw std::logic_error("同じIDのトランザクションが既に存在します");
      }

      transactions_.push_back(transaction);
    }

    std::span<const entities::Transaction> Account::getTransactions() const noexcept
    {
      return transactions_;
    }

    value_objects::Money Account::calculateBalance() const
    {
      auto income = calculateIncomeTotal();
      auto expense = calculateExpenseTotal();
      return income.subtract(expense);
    }

    value_objects::Money Account::calculateIncomeTotal() const
    {
      // 名前空間エイリアス: 長い名前を短くする
      namespace ranges = std::ranges;
      namespace views = std::views;

      // C++20 ranges: パイプライン処理
      // 1. filter: 収入だけを抽出
      // 2. transform: Moneyオブジェクトから金額（int）を取り出す
      // ラムダ式: [](引数) { 処理 } の形式
      auto incomes = transactions_ | views::filter([](const auto &t)
                                                   { return t.getCategory().isIncome(); }) |
                     views::transform([](const auto &t)
                                      { return t.getAmount().getAmount(); });

      // fold_left: 全要素を合計（初期値0から開始）
      int total = ranges::fold_left(incomes, 0, std::plus<>{});
      return value_objects::Money(total);
    }

    value_objects::Money Account::calculateExpenseTotal() const
    {
      namespace ranges = std::ranges;
      namespace views = std::views;

      // C++20 ranges: 支出のみフィルタリングして集計
      auto expenses = transactions_ | views::filter([](const auto &t)
                                                    { return t.getCategory().isExpense(); }) |
                      views::transform([](const auto &t)
                                       { return t.getAmount().getAmount(); });

      int total = ranges::fold_left(expenses, 0, std::plus<>{});
      return value_objects::Money(total);
    }

    std::map<value_objects::Category::Type, value_objects::Money>
    Account::calculateByCategory() const
    {
      std::map<value_objects::Category::Type, int> totals;

      // C++20 rangesを使用した集計
      for (const auto &t : transactions_)
      {
        totals[t.getCategory().getType()] += t.getAmount().getAmount();
      }

      std::map<value_objects::Category::Type, value_objects::Money> result;
      for (const auto &[type, amount] : totals)
      {
        result.emplace(type, value_objects::Money(amount));
      }

      return result;
    }

    std::vector<entities::Transaction> Account::getTransactionsByPeriod(
        const value_objects::Date &startDate,
        const value_objects::Date &endDate) const
    {
      namespace ranges = std::ranges;
      namespace views = std::views;

      // C++20 ranges: 期間フィルタリング
      auto filtered = transactions_ | views::filter([&](const auto &t)
                                                    {
        const auto& date = t.getDate();
        return (date == startDate || date > startDate) &&
               (date == endDate || date < endDate); });

      return std::vector<entities::Transaction>(
          ranges::begin(filtered),
          ranges::end(filtered));
    }

    std::vector<entities::Transaction> Account::getTransactionsByCategory(
        const value_objects::Category &category) const
    {
      namespace ranges = std::ranges;
      namespace views = std::views;

      // C++20 ranges: カテゴリフィルタリング
      auto filtered = transactions_ | views::filter([&category](const auto &t)
                                                    { return t.getCategory() == category; });

      return std::vector<entities::Transaction>(
          ranges::begin(filtered),
          ranges::end(filtered));
    }

  } // namespace aggregates
} // namespace domain
