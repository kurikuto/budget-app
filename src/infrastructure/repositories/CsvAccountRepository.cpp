#include "infrastructure/repositories/CsvAccountRepository.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <ranges>
#include <format>

namespace infrastructure
{
  namespace repositories
  {

    CsvAccountRepository::CsvAccountRepository(std::filesystem::path filePath)
        : filePath_(std::move(filePath))
    {
    }

    void CsvAccountRepository::save(const domain::aggregates::Account &account)
    {
      // 既存データを読み込み、該当アカウントのトランザクションを削除
      std::vector<std::string> lines;
      std::ifstream inFile(filePath_);

      if (inFile.is_open())
      {
        std::string line;
        bool isFirstLine = true;
        while (std::getline(inFile, line))
        {
          if (isFirstLine)
          {
            // ヘッダー行は保持
            lines.push_back(line);
            isFirstLine = false;
            continue;
          }

          // AccountIDが一致しない行のみ保持
          std::istringstream iss(line);
          std::string accountId;
          std::getline(iss, accountId, ',');

          if (accountId != account.getAccountId())
          {
            lines.push_back(line);
          }
        }
        inFile.close();
      }
      else
      {
        // ファイルが存在しない場合はヘッダーを追加
        lines.push_back("AccountID,TransactionID,Date,Category,Amount,Description");
      }

      // ファイルに書き込み
      std::ofstream outFile(filePath_);
      if (!outFile.is_open())
      {
        throw std::runtime_error(std::format("CSVファイルを開けません: {}", filePath_.string()));
      }

      // 既存データを書き込み
      for (const auto &line : lines)
      {
        outFile << line << "\n";
      }

      // 新しいトランザクションを書き込み
      for (const auto &transaction : account.getTransactions())
      {
        writeTransactionToCsv(outFile, account.getAccountId(), transaction);
      }

      outFile.close();
    }

    std::optional<domain::aggregates::Account>
    CsvAccountRepository::findById(std::string_view accountId)
    {
      std::ifstream file(filePath_);
      if (!file.is_open())
      {
        return std::nullopt;
      }

      bool found = false;
      std::string line;
      bool isFirstLine = true;

      while (std::getline(file, line))
      {
        if (isFirstLine)
        {
          isFirstLine = false;
          continue; // ヘッダーをスキップ
        }

        std::istringstream iss(line);
        std::string csvAccountId;
        std::getline(iss, csvAccountId, ',');

        if (csvAccountId == accountId)
        {
          found = true;
          break;
        }
      }

      file.close();

      if (!found)
      {
        return std::nullopt;
      }

      return loadAccountFromCsv(accountId);
    }

    bool CsvAccountRepository::exists(std::string_view accountId)
    {
      return findById(accountId).has_value();
    }

    void CsvAccountRepository::writeTransactionToCsv(
        std::ofstream &file,
        std::string_view accountId,
        const domain::entities::Transaction &transaction)
    {
      file << accountId << ","
           << transaction.getId() << ","
           << transaction.getDate().toString() << ","
           << transaction.getCategory().toString() << ","
           << transaction.getAmount().getAmount() << ","
           << transaction.getDescription() << "\n";
    }

    domain::aggregates::Account
    CsvAccountRepository::loadAccountFromCsv(std::string_view accountId)
    {
      domain::aggregates::Account account(accountId);

      std::ifstream file(filePath_);
      if (!file.is_open())
      {
        throw std::runtime_error(std::format("CSVファイルを開けません: {}", filePath_.string()));
      }

      std::string line;
      bool isFirstLine = true;

      while (std::getline(file, line))
      {
        if (isFirstLine)
        {
          isFirstLine = false;
          continue; // ヘッダーをスキップ
        }

        std::istringstream iss(line);
        std::string csvAccountId, transactionId, dateStr, categoryStr;
        int amount;
        std::string description;

        std::getline(iss, csvAccountId, ',');

        if (csvAccountId != accountId)
        {
          continue;
        }

        std::getline(iss, transactionId, ',');
        std::getline(iss, dateStr, ',');
        std::getline(iss, categoryStr, ',');
        iss >> amount;
        iss.ignore(); // カンマをスキップ
        std::getline(iss, description);

        // トランザクションを復元
        auto date = domain::value_objects::Date::fromString(dateStr);
        auto category = domain::value_objects::Category::fromString(categoryStr);
        auto money = domain::value_objects::Money(amount);

        domain::entities::Transaction transaction(
            transactionId, date, category, money, description);

        account.addTransaction(transaction);
      }

      file.close();
      return account;
    }

  } // namespace repositories
} // namespace infrastructure
