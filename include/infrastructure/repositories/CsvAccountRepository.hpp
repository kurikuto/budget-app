#pragma once

#include "domain/repositories/IAccountRepository.hpp"
#include <string>
#include <string_view>
#include <filesystem>

namespace infrastructure
{
  namespace repositories
  {

    /**
     * CSVファイルベースのリポジトリ実装（C++20版）
     * インフラストラクチャ層の実装
     */
    class CsvAccountRepository : public domain::repositories::IAccountRepository
    {
    public:
      explicit CsvAccountRepository(std::filesystem::path filePath);

      void save(const domain::aggregates::Account &account) override;
      [[nodiscard]] std::optional<domain::aggregates::Account> findById(std::string_view accountId) override;
      [[nodiscard]] bool exists(std::string_view accountId) override;

    private:
      std::filesystem::path filePath_;

      // CSVヘルパーメソッド
      void writeTransactionToCsv(
          std::ofstream &file,
          std::string_view accountId,
          const domain::entities::Transaction &transaction);

      [[nodiscard]] domain::aggregates::Account loadAccountFromCsv(std::string_view accountId);
    };

  } // namespace repositories
} // namespace infrastructure
