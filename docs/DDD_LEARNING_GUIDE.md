# DDD 学習ガイド

## ドメイン駆動設計（DDD）とは

ドメイン駆動設計は、複雑なソフトウェアシステムを構築するための設計手法です。ビジネスの核心（ドメイン）に焦点を当て、ドメインの専門家と開発者が協力して、ビジネスロジックを明確にモデル化します。

## このプロジェクトで学べる DDD の概念

### 1. 戦術的設計パターン

#### 値オブジェクト（Value Objects）

**目的**: 値そのものに意味がある概念を表現

**特徴**:

- 不変性（イミュータブル）
- 値による等価性（内容が同じなら等しい）
- 副作用のない操作

**実装例**:

```cpp
// Money.hpp - 金額を表す値オブジェクト
class Money {
public:
    explicit Money(int amount);  // 負の値を許さない

    // 不変性を保つ：新しいインスタンスを返す
    Money add(const Money& other) const;
    Money subtract(const Money& other) const;

    // 値による等価性
    bool equals(const Money& other) const;
private:
    int amount_;  // privateで不変性を保証
};
```

**学習ポイント**:

- なぜ`amount_`を private にするのか？
- なぜ`add`や`subtract`が新しいインスタンスを返すのか？
- 値オブジェクトとプリミティブ型の違いは？

#### エンティティ（Entities）

**目的**: 同一性（アイデンティティ）が重要な概念を表現

**特徴**:

- 一意な ID
- ライフサイクルを通じた同一性
- 属性が変わっても同じエンティティ

**実装例**:

```cpp
// Transaction.hpp - 取引エンティティ
class Transaction {
public:
    Transaction(const std::string& id, ...);

    std::string getId() const { return id_; }

    // エンティティの等価性はIDで判定
    bool equals(const Transaction& other) const {
        return id_ == other.id_;
    }
private:
    std::string id_;  // 同一性の源泉
    Date date_;
    Category category_;
    Money amount_;
    std::string description_;
};
```

**学習ポイント**:

- 値オブジェクトとの違いは？
- なぜ ID が必要なのか？
- 属性を変更しても同じエンティティとみなされるのはなぜ？

#### 集約（Aggregates）

**目的**: 整合性を保つべき境界を定義

**特徴**:

- 集約ルート：外部からアクセスする唯一の窓口
- トランザクション境界
- ビジネスルールの保護

**実装例**:

```cpp
// Account.hpp - アカウント集約
class Account {
public:
    explicit Account(const std::string& accountId);

    // 集約ルートを通してのみトランザクションを追加
    void addTransaction(const Transaction& transaction) {
        // ビジネスルールのチェック（重複防止）
        for (const auto& t : transactions_) {
            if (t.getId() == transaction.getId()) {
                throw std::logic_error("重複したIDです");
            }
        }
        transactions_.push_back(transaction);
    }

    // 集約内の整合性を保つ
    Money calculateBalance() const;

private:
    std::string accountId_;  // 集約ルートのID
    std::vector<Transaction> transactions_;  // 管理下のエンティティ
};
```

**学習ポイント**:

- なぜ外部から直接`transactions_`にアクセスできないのか？
- 集約の境界をどう決めるべきか？
- トランザクション境界とは何か？

#### リポジトリ（Repositories）

**目的**: 永続化の詳細を隠蔽し、ドメインオブジェクトをコレクションのように扱う

**特徴**:

- ドメイン層にインターフェースを配置
- インフラ層に実装を配置
- 依存性の逆転

**実装例**:

```cpp
// IAccountRepository.hpp（ドメイン層）
class IAccountRepository {
public:
    virtual void save(const Account& account) = 0;
    virtual std::optional<Account> findById(const std::string& id) = 0;
};

// CsvAccountRepository.hpp（インフラ層）
class CsvAccountRepository : public IAccountRepository {
public:
    void save(const Account& account) override {
        // CSV固有の実装
    }
};
```

**学習ポイント**:

- なぜインターフェースと実装を分けるのか？
- ドメイン層がインフラ層に依存しないメリットは？
- リポジトリと DAO の違いは？

### 2. レイヤードアーキテクチャ

```
┌─────────────────────────────────┐
│    Presentation Layer           │  ← ユーザーインターフェース
│    (CLI, GUI, Web API)          │
└─────────────────────────────────┘
           ↓ 依存
┌─────────────────────────────────┐
│    Application Layer            │  ← ユースケース、調整役
│    (Use Cases)                  │
└─────────────────────────────────┘
           ↓ 依存
┌─────────────────────────────────┐
│    Domain Layer                 │  ← ビジネスロジックの核心
│    (Entities, Value Objects)    │
└─────────────────────────────────┘
           ↑ 依存の逆転
┌─────────────────────────────────┐
│    Infrastructure Layer         │  ← 技術的な実装詳細
│    (DB, File, External API)     │
└─────────────────────────────────┘
```

**依存性のルール**:

- 上位層は下位層に依存できる
- 下位層は上位層に依存してはいけない
- ドメイン層は他の層に依存しない（最も独立）

### 3. ユースケース（Application Services）

**目的**: アプリケーションの振る舞いを表現

**特徴**:

- ユーザーストーリーの実装
- ドメインオブジェクトの調整
- トランザクション管理

**実装例**:

```cpp
// RegisterTransactionUseCase.cpp
void RegisterTransactionUseCase::execute(const Request& request) {
    // 1. アカウントの取得または作成
    auto accountOpt = repository_->findById(request.accountId);
    Account account = accountOpt.has_value()
        ? accountOpt.value()
        : Account(request.accountId);

    // 2. トランザクションの作成
    Transaction transaction(...);

    // 3. ドメインロジックの実行
    account.addTransaction(transaction);  // ビジネスルール適用

    // 4. 永続化
    repository_->save(account);
}
```

**学習ポイント**:

- ユースケースとドメインロジックの違いは？
- ユースケースが「薄い」べき理由は？
- なぜビジネスロジックをドメイン層に置くのか？

## DDD の設計プロセス

### 1. ユビキタス言語の確立

ドメイン専門家と開発者が共通の言語を使う：

- 「取引（Transaction）」
- 「口座（Account）」
- 「カテゴリ（Category）」

### 2. 境界づけられたコンテキスト

大きなシステムを小さなコンテキストに分割：

- 家計簿コンテキスト
- 予算管理コンテキスト
- レポートコンテキスト

### 3. モデルの深化

繰り返し改善してモデルを洗練：

```
初期: int amount
  ↓
改善: Money amount  （値オブジェクト化）
  ↓
さらに: Money amount, Currency currency  （通貨対応）
```

## 実践的なヒント

### 良い設計の指標

✅ **ビジネスルールがドメイン層に集中している**

```cpp
// Good: ドメイン層にビジネスルール
class Account {
    void addTransaction(const Transaction& tx) {
        if (isDuplicateId(tx.getId())) {
            throw std::logic_error("重複");
        }
        transactions_.push_back(tx);
    }
};
```

❌ **ビジネスルールがアプリケーション層に散在**

```cpp
// Bad: ユースケースにビジネスルール
void UseCase::execute() {
    if (isDuplicateId(tx.getId())) {  // ここに書くべきでない
        throw std::logic_error("重複");
    }
    account.addTransaction(tx);
}
```

### よくある間違い

1. **貧血ドメインモデル**：ドメインオブジェクトが getter と setter しか持たない
2. **神クラス**：1 つのクラスに責務が集中しすぎ
3. **レイヤーの破壊**：ドメイン層がインフラ層に依存

## 次のステップ

1. コードを読んで実行してみる
2. 新しい機能を追加してみる（予算管理など）
3. 別のリポジトリ実装を作ってみる（SQLite など）
4. テストを書いてみる
5. 別のドメイン（図書館、在庫管理など）で実践

---

**設計は反復的なプロセスです。完璧を求めず、継続的な改善を心がけましょう！**
