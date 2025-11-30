#pragma once

#include "application/usecases/RegisterTransactionUseCase.hpp"
#include "application/usecases/ViewBalanceUseCase.hpp"
#include "application/usecases/ViewTransactionsByCategoryUseCase.hpp"
#include <memory>
#include <string>

namespace presentation
{
  namespace cli
  {

    /**
     * CLIコントローラー（プレゼンテーション層）C++20版
     * ユーザー入力を受け取り、ユースケースを呼び出す
     */
    class CliController
    {
    public:
      CliController(
          std::shared_ptr<application::usecases::RegisterTransactionUseCase> registerUseCase,
          std::shared_ptr<application::usecases::ViewBalanceUseCase> viewBalanceUseCase,
          std::shared_ptr<application::usecases::ViewTransactionsByCategoryUseCase> viewByCategoryUseCase);

      void run();

    private:
      std::shared_ptr<application::usecases::RegisterTransactionUseCase> registerUseCase_;
      std::shared_ptr<application::usecases::ViewBalanceUseCase> viewBalanceUseCase_;
      std::shared_ptr<application::usecases::ViewTransactionsByCategoryUseCase> viewByCategoryUseCase_;

      void displayMenu();
      void handleRegisterTransaction();
      void handleViewBalance();
      void handleViewByCategory();
      void handleExit();

      // ヘルパーメソッド
      [[nodiscard]] std::string generateTransactionId();
      std::string currentAccountId_ = "default_account";
    };

  } // namespace cli
} // namespace presentation
