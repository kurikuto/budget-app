# C++初心者向けガイド

このドキュメントは、C++初心者がこのプロジェクトのコードを理解するための補助資料です。

## 📚 基本的な C++の概念

### 1. ヘッダーファイル (.hpp) と実装ファイル (.cpp)

```cpp
// Money.hpp - 宣言（インターフェース）
class Money {
public:
    int getAmount() const;  // 関数の宣言のみ
private:
    int amount_;
};

// Money.cpp - 実装（詳細）
int Money::getAmount() const {
    return amount_;  // 実際の処理
}
```

**理由**:

- コンパイル時間の短縮
- インターフェースと実装の分離
- 再利用性の向上

### 2. namespace（名前空間）

```cpp
namespace domain {
    class Money { };
}

// 使用時
domain::Money money(100);
```

**目的**:

- 名前の衝突を防ぐ
- コードを論理的にグループ化

### 3. const と constexpr

```cpp
// const: 実行時定数
const int value = getValue();  // 実行時に決定

// constexpr: コンパイル時定数
constexpr int value = 100;     // コンパイル時に決定
```

**メリット**:

- `const`: 値の変更を防ぐ
- `constexpr`: より高速（コンパイル時計算）

### 4. explicit キーワード

```cpp
class Money {
public:
    explicit Money(int amount);  // explicitあり
};

// explicit なし
Money m1 = 100;        // OK（暗黙の変換）

// explicit あり
Money m2 = 100;        // エラー！
Money m3(100);         // OK（明示的）
Money m4 = Money(100); // OK（明示的）
```

**目的**: 意図しない型変換を防ぐ

### 5. 参照 (&) とポインタ (\*)

```cpp
// 値渡し: コピーが発生（遅い）
void func1(Money money) { }

// const参照渡し: コピーなし（速い、変更不可）
void func2(const Money& money) { }

// 参照渡し: コピーなし（速い、変更可）
void func3(Money& money) { }

// ポインタ: アドレスを渡す（nullptrの可能性あり）
void func4(Money* money) { }
```

**ベストプラクティス**:

- 読み取り専用: `const&`
- 変更する: `&`
- null の可能性がある: `*` または `std::optional`

## 🎯 このプロジェクトで使う重要な機能

### 1. [[nodiscard]] 属性

```cpp
[[nodiscard]] int getValue() const;

// 使用例
getValue();           // 警告: 戻り値を無視している
int x = getValue();   // OK
```

**目的**: 重要な戻り値の無視を防ぐ

### 2. noexcept 指定子

```cpp
int getValue() const noexcept {
    return value_;  // 例外を投げない
}
```

**メリット**:

- コンパイラの最適化
- 例外安全性の明示
- パフォーマンス向上

### 3. スマートポインタ

```cpp
// 生ポインタ（使わない！）
Money* money = new Money(100);
delete money;  // 忘れるとメモリリーク

// スマートポインタ（推奨！）
auto money = std::make_shared<Money>(100);
// 自動的にメモリが解放される
```

**種類**:

- `std::unique_ptr`: 単一所有権
- `std::shared_ptr`: 共有所有権（このプロジェクトで使用）
- `std::weak_ptr`: 循環参照を防ぐ

### 4. auto キーワード

```cpp
// 型を明示
std::shared_ptr<CsvAccountRepository> repo =
    std::make_shared<CsvAccountRepository>("file.csv");

// auto で簡潔に（型推論）
auto repo = std::make_shared<CsvAccountRepository>("file.csv");
```

**メリット**:

- タイプ量の削減
- リファクタリングが楽
- テンプレートコードが簡潔に

### 5. ラムダ式

```cpp
// 基本形: [キャプチャ](引数) { 処理 }
auto add = [](int a, int b) { return a + b; };
int result = add(5, 3);  // 8

// キャプチャ
int x = 10;
auto addX = [x](int a) { return a + x; };  // xをコピー
auto addY = [&x](int a) { return a + x; }; // xを参照

// このプロジェクトでの使用例
auto incomes = transactions
    | views::filter([](const auto& t) {
        return t.getCategory().isIncome();
    });
```

**キャプチャの種類**:

- `[]`: 何もキャプチャしない
- `[x]`: x をコピー
- `[&x]`: x を参照
- `[=]`: 全てをコピー
- `[&]`: 全てを参照

### 6. std::optional

```cpp
// 値が存在しない可能性を表現
std::optional<Account> findById(std::string_view id) {
    if (/* 見つからない */) {
        return std::nullopt;  // 値なし
    }
    return account;  // 値あり
}

// 使用例
auto result = findById("123");
if (result.has_value()) {
    Account account = result.value();
    // または
    Account account = *result;
}
```

**nullptr との違い**: より安全で表現力が高い

### 7. 構造化束縛 (C++17)

```cpp
std::map<std::string, int> data = {{"a", 1}, {"b", 2}};

// 従来
for (const auto& pair : data) {
    std::cout << pair.first << ": " << pair.second;
}

// 構造化束縛（読みやすい）
for (const auto& [key, value] : data) {
    std::cout << key << ": " << value;
}
```

## 🔍 C++20 の新機能（このプロジェクトで使用）

### 1. Ranges

```cpp
// 従来のループ
std::vector<int> numbers = {1, 2, 3, 4, 5};
std::vector<int> evens;
for (int n : numbers) {
    if (n % 2 == 0) {
        evens.push_back(n * 2);
    }
}

// Ranges（宣言的で読みやすい）
auto evens = numbers
    | views::filter([](int n) { return n % 2 == 0; })
    | views::transform([](int n) { return n * 2; });
```

**メリット**:

- 宣言的（何をするか明確）
- 遅延評価（効率的）
- 組み合わせ可能

### 2. 三方比較演算子 (<=>)

```cpp
// 従来（6つの演算子を全て定義）
class Money {
    bool operator==(const Money& other) const;
    bool operator!=(const Money& other) const;
    bool operator<(const Money& other) const;
    bool operator<=(const Money& other) const;
    bool operator>(const Money& other) const;
    bool operator>=(const Money& other) const;
};

// C++20（2行で完了！）
class Money {
    auto operator<=>(const Money& other) const = default;
    bool operator==(const Money& other) const = default;
};
```

### 3. std::format

```cpp
// 従来（読みにくい）
std::cout << "金額: " << amount << "円" << std::endl;

// std::format（読みやすい）
std::cout << std::format("金額: {}円", amount) << std::endl;

// フォーマット指定
std::format("{:04d}-{:02d}", year, month);  // 0埋め
std::format("{:>10}", text);                // 右寄せ
std::format("{:.2f}", value);               // 小数点2桁
```

## 🛡️ メモリ管理のベストプラクティス

### 1. RAII (Resource Acquisition Is Initialization)

```cpp
// 悪い例
void badFunction() {
    Money* money = new Money(100);
    // ... 処理 ...
    if (error) {
        return;  // メモリリーク！
    }
    delete money;
}

// 良い例（RAII）
void goodFunction() {
    auto money = std::make_unique<Money>(100);
    // ... 処理 ...
    if (error) {
        return;  // 自動的にメモリ解放
    }
    // ここでも自動的にメモリ解放
}
```

### 2. スタック vs ヒープ

```cpp
// スタック割り当て（高速、自動管理）
void stackExample() {
    Money money(100);  // スタック
    // 関数終了時に自動的に破棄
}

// ヒープ割り当て（柔軟、手動管理が必要）
void heapExample() {
    auto money = std::make_shared<Money>(100);  // ヒープ
    // shared_ptrが自動的に管理
}
```

**使い分け**:

- スタック: 小さいオブジェクト、寿命が短い
- ヒープ: 大きいオブジェクト、共有が必要

### 3. コピーとムーブ

```cpp
// コピー（遅い）
std::vector<Transaction> copy = original;

// ムーブ（速い）
std::vector<Transaction> moved = std::move(original);
// original は使用不可になる

// 関数での使用
void func(std::vector<Transaction>&& data) {  // 右辺値参照
    // dataをムーブで受け取る
}
```

## 🎨 コーディングスタイルのヒント

### 1. 変数の命名規則

```cpp
class MyClass {
private:
    int memberVariable_;     // メンバ変数（末尾にアンダースコア）
    static int staticVar_;   // 静的変数

public:
    void myFunction() {
        int localVariable;   // ローカル変数
        const int CONSTANT = 100;  // 定数（大文字）
    }
};
```

### 2. const の配置

```cpp
// 推奨: 右から読む
const int* ptr;           // ポインタが指す値がconst
int* const ptr;           // ポインタ自体がconst
const int* const ptr;     // 両方const

// メソッド
void func() const;        // このオブジェクトを変更しない
```

### 3. 初期化

```cpp
// 古い方法（避ける）
int x = 0;
Money m = Money(100);

// 現代的な方法（推奨）
int x{0};                 // 統一初期化
Money m{100};
auto ptr = std::make_shared<Money>(100);
```

## 📖 学習リソース

### 公式リファレンス

- [cppreference.com](https://en.cppreference.com/) - C++リファレンス
- [C++ Core Guidelines](https://isocpp.github.io/CppCoreGuidelines/) - ベストプラクティス

### おすすめ書籍

- 『Effective Modern C++』（Scott Meyers）
- 『C++プログラミング入門』
- 『独習 C++』

### オンライン学習

- [Learn C++](https://www.learncpp.com/)
- [C++ Tutorial - cplusplus.com](http://www.cplusplus.com/doc/tutorial/)

## ❓ よくある質問

### Q1: `const&` と `&` はどう使い分ける？

**A**: 値を変更しない場合は `const&`、変更する場合は `&` を使います。

### Q2: `auto` はいつ使うべき？

**A**: 型が明らかな場合、または型名が長い場合に使います。ただし、可読性を損なわない範囲で。

### Q3: なぜポインタを直接使わない？

**A**: メモリリークを防ぐため。スマートポインタが自動的にメモリを管理します。

### Q4: `std::move` はいつ使う？

**A**: 大きなオブジェクトの所有権を移譲する時。ただし、移動後の元のオブジェクトは使用不可になります。

### Q5: ヘッダーファイルに実装を書いてはダメ？

**A**: テンプレートや constexpr 関数以外は、コンパイル時間を短縮するため避けるのがベスト。

---

**困ったときは**: エラーメッセージを carefully に読み、cppreference.com で検索してみましょう！
