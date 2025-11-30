#pragma once

#include "domain/aggregates/Account.hpp"
#include <memory>
#include <optional>
#include <string_view>

namespace domain
{
  namespace repositories
  {

    /**
     * リポジトリインターフェース（抽象化）
     * ドメイン層ではインフラストラクチャの実装詳細を知らない
     */
    class IAccountRepository
    {
    public:
      virtual ~IAccountRepository() = default;

      // アカウントの永続化
      virtual void save(const aggregates::Account &account) = 0;

      // アカウントの取得
      [[nodiscard]] virtual std::optional<aggregates::Account> findById(std::string_view accountId) = 0;

      // アカウントの存在チェック
      [[nodiscard]] virtual bool exists(std::string_view accountId) = 0;
    };

  } // namespace repositories
} // namespace domain
