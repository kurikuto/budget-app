# C++20/23 の新機能活用ガイド

このプロジェクトで使用されているモダン C++の機能を解説します。

## C++20 の主要機能

### 1. コンセプト（Concepts）

型の制約を明示的に表現できます。

```cpp
// 将来的な拡張例
template<typename T>
concept Numeric = std::is_arithmetic_v<T>;

template<Numeric T>
class Money {
    T amount_;
};
```

### 2. 三方比較演算子（Spaceship Operator `<=>`）

**使用箇所**: `Money.hpp`, `Date.hpp`, `Category.hpp`

```cpp
class Money {
public:
    // これだけで ==, !=, <, <=, >, >= が全て自動生成される
    [[nodiscard]] constexpr auto operator<=>(const Money& other) const noexcept = default;
    [[nodiscard]] constexpr bool operator==(const Money& other) const noexcept = default;
};
```

**メリット**:

- 6 つの比較演算子を 2 行で定義
- コードの重複を削減
- コンパイラが最適化

### 3. Ranges ライブラリ

**使用箇所**: `Account.cpp`

従来のループを宣言的な記述に変更：

```cpp
// C++17以前
int total = 0;
for (const auto& t : transactions_) {
    if (t.getCategory().isIncome()) {
        total += t.getAmount().getAmount();
    }
}

// C++20 Ranges
namespace views = std::views;
auto incomes = transactions_
    | views::filter([](const auto& t) { return t.getCategory().isIncome(); })
    | views::transform([](const auto& t) { return t.getAmount().getAmount(); });

int total = ranges::fold_left(incomes, 0, std::plus<>{});
```

**メリット**:

- 可読性の向上
- パイプライン処理（`|`）で意図が明確
- 遅延評価で効率的
- コンポーザブル（組み合わせ可能）

### 4. std::format

**使用箇所**: `Money.cpp`, `Date.cpp`, `CliController.cpp`

タイプセーフな文字列フォーマット：

```cpp
// C++17以前
std::ostringstream oss;
oss << std::setfill('0') << std::setw(4) << year;

// C++20
std::string result = std::format("{:04d}-{:02d}-{:02d}", year, month, day);
```

**メリット**:

- Python 風の構文
- タイプセーフ
- パフォーマンスが良い
- 読みやすい

### 5. std::span

**使用箇所**: `Account.hpp`

配列やベクターへの軽量な参照：

```cpp
// 所有権を持たない、軽量なビュー
std::span<const Transaction> getTransactions() const noexcept;
```

**メリット**:

- コピーコストゼロ
- 配列や vector を統一的に扱える
- 境界チェック可能

### 6. constexpr の拡張

**使用箇所**: `Money.hpp`, `Date.hpp`, `Category.hpp`

コンパイル時計算の範囲拡大：

```cpp
constexpr Money add(const Money& other) const {
    return Money(amount_ + other.amount_);
}

// コンパイル時に計算可能
constexpr Money total = Money(100).add(Money(200));
```

**メリット**:

- 実行時コストゼロ
- コンパイル時エラー検出
- より多くのコードを constexpr に

### 7. [[nodiscard]] 属性の強化

**使用箇所**: 全てのゲッターメソッド

戻り値の無視を警告：

```cpp
[[nodiscard]] constexpr int getAmount() const noexcept;

// 警告: 戻り値を無視している
money.getAmount();  // Warning!

// OK
int amount = money.getAmount();
```

### 8. using enum（C++20）

**使用箇所**: `Category.cpp`

enum のスコープを簡潔に：

```cpp
// C++17以前
return Category::Type::FOOD;

// C++20
using enum Type;
return FOOD;  // より簡潔
```

### 9. std::chrono の拡張

**使用箇所**: `Date.cpp`

カレンダー機能の追加：

```cpp
Date Date::today() {
    using namespace std::chrono;
    auto now = system_clock::now();
    auto days = floor<std::chrono::days>(now);
    year_month_day ymd{days};

    return Date(
        static_cast<int>(ymd.year()),
        static_cast<unsigned>(ymd.month()),
        static_cast<unsigned>(ymd.day())
    );
}
```

### 10. std::filesystem（C++17 継続）

**使用箇所**: `CsvAccountRepository.hpp`

ファイルパス操作の標準化：

```cpp
std::filesystem::path filePath_;

// パス操作が安全・簡潔に
filePath_.string()
filePath_.parent_path()
filePath_.filename()
```

## C++23 の主要機能

### 1. std::print / std::println（一部コンパイラで利用可能）

```cpp
// C++23（将来的に利用可能）
std::println("金額: {}", money.toString());
std::println("{:<15}{}", "カテゴリ", "金額");
```

### 2. ranges::fold_left（C++23）

**使用箇所**: `Account.cpp`

```cpp
// C++20の std::accumulate の代わり
int total = ranges::fold_left(incomes, 0, std::plus<>{});
```

### 3. std::expected（エラーハンドリング）

将来的な拡張例：

```cpp
// C++23
std::expected<Account, Error> findById(std::string_view id) {
    if (/* error */) {
        return std::unexpected(Error::NotFound);
    }
    return account;
}
```

## パフォーマンス比較

### メモリ効率

| 機能           | C++17           | C++20/23      | 改善       |
| -------------- | --------------- | ------------- | ---------- |
| 配列参照       | `const vector&` | `std::span`   | コピーなし |
| 文字列参照     | `const string&` | `string_view` | 軽量       |
| フィルタリング | ループ + vector | ranges        | 遅延評価   |

### コンパイル時計算

```cpp
// C++17: 実行時計算
Money calculateTotal() {
    Money total(0);
    for (const auto& item : items) {
        total = total.add(item);
    }
    return total;
}

// C++20: コンパイル時計算可能
constexpr Money calculateTotal() {
    Money total(0);
    for (const auto& item : items) {
        total = total.add(item);
    }
    return total;
}
```

## 実践的なヒント

### 1. ranges を使うべき場面

✅ **使うべき**:

- フィルタリング + 変換の組み合わせ
- 複数のコレクション操作
- 遅延評価が有利な場面

❌ **使わない方が良い**:

- シンプルなループ（1 回だけ）
- パフォーマンスが超重要な箇所（プロファイリング後）

### 2. constexpr を使うべき場面

✅ **使うべき**:

- 値オブジェクトの計算
- ユーティリティ関数
- 設定値の計算

❌ **使わない方が良い**:

- I/O 操作
- 例外を多用する箇所
- 複雑すぎる計算

### 3. std::format vs iostream

```cpp
// std::format の方が適切
std::cout << std::format("金額: ¥{:,}", 1000000) << "\n";

// iostream の方が適切（シンプルな出力）
std::cout << "Hello, World!\n";
```

## コンパイラサポート状況

| 機能           | GCC | Clang | MSVC   |
| -------------- | --- | ----- | ------ |
| Ranges         | 10+ | 13+   | 19.30+ |
| std::format    | 13+ | 14+   | 19.29+ |
| Spaceship      | 10+ | 10+   | 19.20+ |
| constexpr 拡張 | 10+ | 10+   | 19.25+ |
| std::span      | 10+ | 7+    | 19.26+ |

## ビルドオプション

### C++23 を試す（実験的）

```cmake
set(CMAKE_CXX_STANDARD 23)
```

### C++20 にフォールバック

```cmake
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
```

### C++17 で動作させる（互換性）

一部の機能を条件付きコンパイルで無効化：

```cpp
#if __cplusplus >= 202002L
    // C++20コード
#else
    // C++17フォールバック
#endif
```

## まとめ

このプロジェクトは C++20/23 の主要機能を実践的に学べる構成になっています：

1. **型安全性**: constexpr, concepts
2. **表現力**: ranges, format
3. **効率性**: span, string_view
4. **簡潔性**: spaceship operator, using enum

モダン C++を習得することで、より安全で読みやすく、効率的なコードが書けるようになります！
