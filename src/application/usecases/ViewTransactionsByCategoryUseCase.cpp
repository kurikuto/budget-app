#include "application/usecases/ViewTransactionsByCategoryUseCase.hpp"

namespace application
{
  namespace usecases
  {

    ViewTransactionsByCategoryUseCase::ViewTransactionsByCategoryUseCase(
        std::shared_ptr<domain::repositories::IAccountRepository> repository) : repository_(repository)
    {
    }

    ViewTransactionsByCategoryUseCase::Response
    ViewTransactionsByCategoryUseCase::execute(const std::string &accountId)
    {
      auto accountOpt = repository_->findById(accountId);

      if (!accountOpt.has_value())
      {
        return Response{
            false,
            "アカウントが見つかりません",
            {}};
      }

      auto account = accountOpt.value();
      auto categoryTotals = account.calculateByCategory();

      return Response{
          true,
          "成功",
          categoryTotals};
    }

  } // namespace usecases
} // namespace application
