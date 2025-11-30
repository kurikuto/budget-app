# 家計簿アプリ（ドメイン駆動設計 × C++20/23）

このプロジェクトは、**ドメイン駆動設計（DDD）** の主要概念を学習するための教育的な家計簿アプリケーションです。**C++20/23**の最新機能を活用し、CLI（コマンドラインインターフェース）で動作します。

## 🎯 プロジェクトの目的

- **DDD の実践**: 値オブジェクト、エンティティ、集約、リポジトリなどの主要パターンを実装
- **モダン C++の学習**: C++20/23 の新機能（Ranges、format、constexpr、三方比較演算子など）
- **レイヤードアーキテクチャ**: Domain、Application、Infrastructure、Presentation の 4 層構造

## ✨ 使用している C++20/23 機能

- 🔹 **三方比較演算子 (`<=>`)**: 比較演算子の自動生成
- 🔹 **Ranges**: 宣言的なコレクション操作
- 🔹 **std::format**: タイプセーフな文字列フォーマット
- 🔹 **std::span**: 軽量な配列ビュー
- 🔹 **constexpr 拡張**: コンパイル時計算の強化
- 🔹 **[[nodiscard]]**: 戻り値無視の警告
- 🔹 **using enum**: enum の簡潔な記述
- 🔹 **std::chrono 拡張**: カレンダー機能
- 🔹 **std::filesystem**: パス操作の標準化

詳細は [`docs/CPP23_FEATURES.md`](docs/CPP23_FEATURES.md) を参照してください。

## 📁 プロジェクト構造

```
C++/
├── CMakeLists.txt              # ビルド設定
├── README.md                   # このファイル
├── include/                    # ヘッダーファイル
│   ├── domain/                 # ドメイン層
│   │   ├── value_objects/      # 値オブジェクト
│   │   │   ├── Money.hpp       # 金額
│   │   │   ├── Date.hpp        # 日付
│   │   │   └── Category.hpp    # カテゴリ
│   │   ├── entities/           # エンティティ
│   │   │   └── Transaction.hpp # 取引
│   │   ├── aggregates/         # 集約
│   │   │   └── Account.hpp     # アカウント（集約ルート）
│   │   └── repositories/       # リポジトリインターフェース
│   │       └── IAccountRepository.hpp
│   ├── application/            # アプリケーション層
│   │   └── usecases/           # ユースケース
│   │       ├── RegisterTransactionUseCase.hpp
│   │       ├── ViewBalanceUseCase.hpp
│   │       └── ViewTransactionsByCategoryUseCase.hpp
│   ├── infrastructure/         # インフラストラクチャ層
│   │   └── repositories/
│   │       └── CsvAccountRepository.hpp
│   └── presentation/           # プレゼンテーション層
│       └── cli/
│           └── CliController.hpp
└── src/                        # 実装ファイル
    ├── domain/
    │   ├── value_objects/
    │   ├── entities/
    │   └── aggregates/
    ├── application/
    │   └── usecases/
    ├── infrastructure/
    │   └── repositories/
    ├── presentation/
    │   └── cli/
    └── main.cpp                # エントリーポイント
```

## 🧩 DDD 概念の実装

### 1. 値オブジェクト（Value Objects）

**特徴**: 不変性、同一性の欠如、値による等価性

- **Money**: 金額を表現。負の値を許さない
- **Date**: 日付を表現。バリデーション機能付き
- **Category**: カテゴリ（収入、食費、交通費など）

```cpp
// 使用例
Money amount(5000);  // ¥5000
Date today = Date::today();
Category category(Category::Type::FOOD);
```

### 2. エンティティ（Entities）

**特徴**: 一意な ID、ライフサイクルを通じた同一性

- **Transaction**: 取引を表現。ID、日付、カテゴリ、金額、説明を持つ

```cpp
Transaction tx("TXN001", date, category, amount, "ランチ代");
```

### 3. 集約（Aggregates）

**特徴**: 整合性境界、集約ルート

- **Account**: 複数の Transaction を管理。ビジネスルール（重複チェック、残高計算など）を保証

```cpp
Account account("my_account");
account.addTransaction(transaction);
Money balance = account.calculateBalance();
```

### 4. リポジトリ（Repositories）

**特徴**: 永続化の抽象化、ドメインとインフラの分離

- **IAccountRepository**: インターフェース（ドメイン層）
- **CsvAccountRepository**: CSV 実装（インフラ層）

```cpp
auto repository = std::make_shared<CsvAccountRepository>("data.csv");
repository->save(account);
auto loadedAccount = repository->findById("my_account");
```

### 5. ユースケース（Use Cases）

**特徴**: アプリケーションのビジネスロジック、ユーザーストーリーの実装

- **RegisterTransactionUseCase**: 取引登録
- **ViewBalanceUseCase**: 収支照会
- **ViewTransactionsByCategoryUseCase**: カテゴリ別集計

## 🔧 ビルド方法

### 必要環境

- CMake 3.20 以上
- C++20/23 対応コンパイラ
  - GCC 10 以上
  - Clang 13 以上
  - MSVC 19.29 以上（Visual Studio 2019 16.11 以降）

**注意**: `std::format`は GCC 13 以上、Clang 14 以上で完全サポート。それ以前のバージョンでは一部機能が動作しない可能性があります。

### ビルド手順

```powershell
# プロジェクトディレクトリに移動
cd "c:\Users\project\C++"

# ビルドディレクトリを作成
mkdir build
cd build

# CMakeでプロジェクトを生成
cmake ..

# ビルド実行
cmake --build .
```

**注意**:

- MSYS2 の GCC など特定のコンパイラを使う場合は、CMake のオプションでコンパイラを指定してください
- コンパイラを切り替える場合は、`build`ディレクトリを削除してから再度 CMake を実行してください
- 詳細は [`docs/BUILD_GUIDE.md`](docs/BUILD_GUIDE.md) を参照してください

## 🚀 実行方法

```powershell
# ビルドディレクトリから実行
cd build
.\budget_app.exe

# または、コンパイラによってはDebug/Releaseサブディレクトリに生成される場合
.\Debug\budget_app.exe
```

## 💡 使い方

アプリケーションを起動すると、以下のメニューが表示されます：

```
--- メニュー ---
1. 取引を登録
2. 収支を照会
3. カテゴリ別集計を表示
4. 終了
```

### 取引の登録例

```
選択してください: 1
=== 取引登録 ===
日付（YYYY-MM-DD形式、空白で今日）: 2025-11-29
カテゴリ（収入/食費/住居費/交通費/娯楽/光熱費/医療費/その他）: 食費
金額（円）: 1200
説明: ランチ代

✓ 取引を登録しました。
```

### 収支照会例

```
選択してください: 2
=== 収支照会 ===
収入合計:   ¥300000
支出合計:   ¥150000
----------------------------
残高:       ¥150000
```

## 📚 ドキュメント

- **[README.md](README.md)** - プロジェクト概要とクイックスタート
- **[DDD 学習ガイド](docs/DDD_LEARNING_GUIDE.md)** - ドメイン駆動設計の詳細解説
- **[C++20/23 機能ガイド](docs/CPP23_FEATURES.md)** - 使用している最新 C++機能の解説
- **[C++初心者ガイド](docs/CPP_BEGINNERS_GUIDE.md)** - C++の基礎とこのプロジェクトの読み方
- **[ビルドガイド](docs/BUILD_GUIDE.md)** - ビルド方法とトラブルシューティング
- **[コンパイラ互換性](docs/COMPILER_COMPATIBILITY.md)** - 環境別の対応方法
- **[サンプルデータ](docs/SAMPLE_DATA.md)** - データファイル形式の説明

### DDD パターン

1. **値オブジェクト**: 不変性と値による等価性の実装
2. **エンティティ**: ID による同一性の管理
3. **集約**: トランザクション境界とビジネスルールの保護
4. **リポジトリ**: ドメインロジックと永続化の分離
5. **レイヤードアーキテクチャ**: 依存性の方向制御

### C++の特徴

1. **C++20 Ranges**: 宣言的なコレクション操作
2. **三方比較演算子**: 簡潔な比較演算子定義
3. **std::format**: タイプセーフな文字列フォーマット
4. **constexpr 拡張**: コンパイル時計算の強化
5. **std::span**: 軽量な配列ビュー

## 🎨 発展的な機能アイデア

### 機能拡張

- [ ] 期間指定での照会（月次、年次）
- [ ] 予算管理機能
- [ ] タグ付け機能
- [ ] エクスポート機能（JSON、Excel）
- [ ] マルチアカウント対応

### 技術的改善

- [ ] SQLite リポジトリの実装
- [ ] ユニットテストの追加（Google Test など）
- [ ] ドメインイベントの実装
- [ ] CQRS（コマンドとクエリの分離）
- [ ] GUI 版の作成（Qt、ImGui など）

## 📖 参考資料

### DDD 関連

- 『エリック・エヴァンスのドメイン駆動設計』
- 『実践ドメイン駆動設計』
- 『ドメイン駆動設計入門』（成瀬允宣）

### C++関連

- 『Effective Modern C++』
- 『C++のための API デザイン』
- C++17/20 リファレンス

## 🤝 貢献

学習目的のプロジェクトですが、改善案や追加機能のアイデアは歓迎します！

## 📝 ライセンス

このプロジェクトは教育目的で作成されています。自由に使用・改変してください。

---

**Happy Learning! 🚀**
