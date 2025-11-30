#pragma once

#include "domain/repositories/IAccountRepository.hpp"
#include "domain/value_objects/Money.hpp"
#include <string>
#include <memory>

namespace application
{
  namespace usecases
  {

    /**
     * 収支照会ユースケース
     */
    class ViewBalanceUseCase
    {
    public:
      explicit ViewBalanceUseCase(
          std::shared_ptr<domain::repositories::IAccountRepository> repository);

      struct Response
      {
        bool success;
        std::string message;
        domain::value_objects::Money balance;
        domain::value_objects::Money income;
        domain::value_objects::Money expense;
      };

      Response execute(const std::string &accountId);

    private:
      std::shared_ptr<domain::repositories::IAccountRepository> repository_;
    };

  } // namespace usecases
} // namespace application
