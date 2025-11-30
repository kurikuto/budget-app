#include "application/usecases/RegisterTransactionUseCase.hpp"
#include "domain/entities/Transaction.hpp"

namespace application
{
  namespace usecases
  {

    RegisterTransactionUseCase::RegisterTransactionUseCase(
        std::shared_ptr<domain::repositories::IAccountRepository> repository) : repository_(repository)
    {
    }

    void RegisterTransactionUseCase::execute(const Request &request)
    {
      // アカウントを取得または新規作成
      auto accountOpt = repository_->findById(request.accountId);

      domain::aggregates::Account account = accountOpt.has_value()
                                                ? accountOpt.value()
                                                : domain::aggregates::Account(request.accountId);

      // トランザクションを作成
      domain::entities::Transaction transaction(
          request.transactionId,
          request.date,
          request.category,
          request.amount,
          request.description);

      // アカウントにトランザクションを追加（集約のビジネスルール適用）
      account.addTransaction(transaction);

      // 永続化
      repository_->save(account);
    }

  } // namespace usecases
} // namespace application
