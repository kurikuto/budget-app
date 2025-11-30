#include "application/usecases/ViewBalanceUseCase.hpp"

namespace application
{
  namespace usecases
  {

    ViewBalanceUseCase::ViewBalanceUseCase(
        std::shared_ptr<domain::repositories::IAccountRepository> repository) : repository_(repository)
    {
    }

    ViewBalanceUseCase::Response ViewBalanceUseCase::execute(const std::string &accountId)
    {
      auto accountOpt = repository_->findById(accountId);

      if (!accountOpt.has_value())
      {
        return Response{
            false,
            "アカウントが見つかりません",
            domain::value_objects::Money(0),
            domain::value_objects::Money(0),
            domain::value_objects::Money(0)};
      }

      auto account = accountOpt.value();

      return Response{
          true,
          "成功",
          account.calculateBalance(),
          account.calculateIncomeTotal(),
          account.calculateExpenseTotal()};
    }

  } // namespace usecases
} // namespace application
