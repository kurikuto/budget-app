#include "presentation/cli/CliController.hpp"
#include "infrastructure/repositories/CsvAccountRepository.hpp"
#include "application/usecases/RegisterTransactionUseCase.hpp"
#include "application/usecases/ViewBalanceUseCase.hpp"
#include "application/usecases/ViewTransactionsByCategoryUseCase.hpp"
#include <memory>   // std::make_shared用
#include <iostream> // std::cout, std::cerr用

#ifdef _WIN32
#include <windows.h> // Windows API用
#endif

/**
 * エントリーポイント（プログラムの開始地点）
 *
 * ここでは依存性注入（DI: Dependency Injection）を使用して
 * 各層（Domain, Application, Infrastructure, Presentation）を組み立てます。
 *
 * メモリ管理：std::shared_ptrを使用することで、
 * 自動的にメモリが解放されます（RAIIパターン）。
 */
int main()
{
#ifdef _WIN32
  // Windowsコンソールの文字コードをUTF-8に設定
  SetConsoleOutputCP(CP_UTF8);
  SetConsoleCP(CP_UTF8);
#endif

  try
  {
    // 【ステップ1: インフラストラクチャ層】
    // CSVファイルでデータを永続化するリポジトリを作成
    // std::make_shared: スマートポインタを作成（自動メモリ管理）
    auto repository = std::make_shared<infrastructure::repositories::CsvAccountRepository>(
        "household_data.csv" // データファイル名
    );

    // 【ステップ2: アプリケーション層】
    // ビジネスロジックを実行するユースケースを作成
    // 各ユースケースにリポジトリを渡す（DI）
    auto registerUseCase = std::make_shared<application::usecases::RegisterTransactionUseCase>(
        repository);
    auto viewBalanceUseCase = std::make_shared<application::usecases::ViewBalanceUseCase>(
        repository);
    auto viewByCategoryUseCase = std::make_shared<application::usecases::ViewTransactionsByCategoryUseCase>(
        repository);

    // 【ステップ3: プレゼンテーション層】
    // CLIインターフェースを作成
    // ユースケースを渡す（DI）
    presentation::cli::CliController controller(
        registerUseCase,
        viewBalanceUseCase,
        viewByCategoryUseCase);

    // 【ステップ4: 実行】
    // メインループを開始し、ユーザー入力を待ち受ける
    controller.run();
  }
  catch (const std::exception &e)
  {
    std::cerr << "致命的エラー: " << e.what() << std::endl;
    return 1;
  }

  return 0;
}
