#pragma once

#include "domain/repositories/IAccountRepository.hpp"
#include "domain/value_objects/Category.hpp"
#include "domain/value_objects/Money.hpp"
#include <string>
#include <memory>
#include <map>

namespace application
{
  namespace usecases
  {

    /**
     * カテゴリ別取引照会ユースケース
     */
    class ViewTransactionsByCategoryUseCase
    {
    public:
      explicit ViewTransactionsByCategoryUseCase(
          std::shared_ptr<domain::repositories::IAccountRepository> repository);

      struct Response
      {
        bool success;
        std::string message;
        std::map<domain::value_objects::Category::Type, domain::value_objects::Money> categoryTotals;
      };

      Response execute(const std::string &accountId);

    private:
      std::shared_ptr<domain::repositories::IAccountRepository> repository_;
    };

  } // namespace usecases
} // namespace application
