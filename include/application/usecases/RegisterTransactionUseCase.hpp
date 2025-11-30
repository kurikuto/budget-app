#pragma once

#include "domain/repositories/IAccountRepository.hpp"
#include "domain/value_objects/Date.hpp"
#include "domain/value_objects/Category.hpp"
#include "domain/value_objects/Money.hpp"
#include <string>
#include <string_view>
#include <memory>

namespace application
{
  namespace usecases
  {

    /**
     * 取引登録ユースケース（C++20版）
     * アプリケーション層のビジネスロジック
     */
    class RegisterTransactionUseCase
    {
    public:
      explicit RegisterTransactionUseCase(
          std::shared_ptr<domain::repositories::IAccountRepository> repository);

      struct Request
      {
        std::string accountId;
        std::string transactionId;
        domain::value_objects::Date date;
        domain::value_objects::Category category;
        domain::value_objects::Money amount;
        std::string description;
      };

      void execute(const Request &request);

    private:
      std::shared_ptr<domain::repositories::IAccountRepository> repository_;
    };

  } // namespace usecases
} // namespace application
