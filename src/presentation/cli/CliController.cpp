#include "presentation/cli/CliController.hpp"
#include "domain/value_objects/Date.hpp"
#include "domain/value_objects/Category.hpp"
#include "domain/value_objects/Money.hpp"
#include <iostream>
#include <format>
#include <chrono>

namespace presentation
{
  namespace cli
  {

    CliController::CliController(
        std::shared_ptr<application::usecases::RegisterTransactionUseCase> registerUseCase,
        std::shared_ptr<application::usecases::ViewBalanceUseCase> viewBalanceUseCase,
        std::shared_ptr<application::usecases::ViewTransactionsByCategoryUseCase> viewByCategoryUseCase) : registerUseCase_(registerUseCase),
                                                                                                           viewBalanceUseCase_(viewBalanceUseCase),
                                                                                                           viewByCategoryUseCase_(viewByCategoryUseCase)
    {
    }

    void CliController::run()
    {
      std::cout << "=================" << std::endl;
      std::cout << "   家計簿アプリ   " << std::endl;
      std::cout << "=================" << std::endl;
      std::cout << std::endl;

      bool running = true;
      while (running)
      {
        displayMenu();

        int choice;
        std::cout << "選択してください: ";
        std::cin >> choice;
        std::cin.ignore(); // 改行を消費

        std::cout << std::endl;

        switch (choice)
        {
        case 1:
          handleRegisterTransaction();
          break;
        case 2:
          handleViewBalance();
          break;
        case 3:
          handleViewByCategory();
          break;
        case 4:
          handleExit();
          running = false;
          break;
        default:
          std::cout << "無効な選択です。もう一度選んでください。" << std::endl;
        }

        std::cout << std::endl;
      }
    }

    void CliController::displayMenu()
    {
      std::cout << "--- メニュー ---" << std::endl;
      std::cout << "1. 取引を登録" << std::endl;
      std::cout << "2. 収支を照会" << std::endl;
      std::cout << "3. カテゴリ別集計を表示" << std::endl;
      std::cout << "4. 終了" << std::endl;
      std::cout << std::endl;
    }

    void CliController::handleRegisterTransaction()
    {
      std::cout << "=== 取引登録 ===" << std::endl;

      try
      {
        // 日付入力
        std::cout << "日付（YYYY-MM-DD形式、空白で今日）: ";
        std::string dateStr;
        std::getline(std::cin, dateStr);

        domain::value_objects::Date date = dateStr.empty()
                                               ? domain::value_objects::Date::today()
                                               : domain::value_objects::Date::fromString(dateStr);

        // カテゴリ入力
        std::cout << "カテゴリ（収入/食費/住居費/交通費/娯楽/光熱費/医療費/その他）: ";
        std::string categoryStr;
        std::getline(std::cin, categoryStr);
        domain::value_objects::Category category =
            domain::value_objects::Category::fromString(categoryStr);

        // 金額入力
        std::cout << "金額（円）: ";
        int amount;
        std::cin >> amount;
        std::cin.ignore();
        domain::value_objects::Money money(amount);

        // 説明入力
        std::cout << "説明: ";
        std::string description;
        std::getline(std::cin, description);

        // ユースケース実行
        application::usecases::RegisterTransactionUseCase::Request request{
            currentAccountId_,
            generateTransactionId(),
            date,
            category,
            money,
            description};

        registerUseCase_->execute(request);

        std::cout << std::endl;
        std::cout << "✓ 取引を登録しました。" << std::endl;
      }
      catch (const std::exception &e)
      {
        std::cout << "エラー: " << e.what() << std::endl;
      }
    }

    void CliController::handleViewBalance()
    {
      std::cout << "=== 収支照会 ===" << std::endl;

      try
      {
        auto response = viewBalanceUseCase_->execute(currentAccountId_);

        if (!response.success)
        {
          std::cout << response.message << std::endl;
          std::cout << "まだ取引が登録されていません。" << std::endl;
          return;
        }

        std::cout << "収入合計:   " << response.income.toString() << std::endl;
        std::cout << "支出合計:   " << response.expense.toString() << std::endl;
        std::cout << "----------------------------" << std::endl;
        std::cout << "残高:       " << response.balance.toString() << std::endl;
      }
      catch (const std::exception &e)
      {
        std::cout << "エラー: " << e.what() << std::endl;
      }
    }

    void CliController::handleViewByCategory()
    {
      std::cout << "=== カテゴリ別集計 ===" << std::endl;

      try
      {
        auto response = viewByCategoryUseCase_->execute(currentAccountId_);

        if (!response.success)
        {
          std::cout << response.message << std::endl;
          return;
        }

        if (response.categoryTotals.empty())
        {
          std::cout << "取引がありません。" << std::endl;
          return;
        }

        std::cout << std::format("{:<15}{}", "カテゴリ", "金額") << std::endl;
        std::cout << "----------------------------" << std::endl;

        for (const auto &[type, money] : response.categoryTotals)
        {
          domain::value_objects::Category category(type);
          std::cout << std::format("{:<15}{}", category.toString(), money.toString()) << std::endl;
        }
      }
      catch (const std::exception &e)
      {
        std::cout << "エラー: " << e.what() << std::endl;
      }
    }

    void CliController::handleExit()
    {
      std::cout << "アプリケーションを終了します。" << std::endl;
    }

    std::string CliController::generateTransactionId()
    {
      using namespace std::chrono;
      auto now = system_clock::now();
      auto timestamp = duration_cast<milliseconds>(now.time_since_epoch()).count();

      return std::format("TXN{}", timestamp);
    }

  } // namespace cli
} // namespace presentation
