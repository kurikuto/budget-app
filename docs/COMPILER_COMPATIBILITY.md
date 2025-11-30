# コンパイラ互換性とトラブルシューティング

## 環境別のビルド方法

### Visual Studio Code の IntelliSense エラーについて

IntelliSense が `std::span` などの C++20 機能を認識しない場合がありますが、実際のコンパイルは成功する可能性があります。

#### IntelliSense の設定

`.vscode/c_cpp_properties.json` を作成：

```json
{
  "configurations": [
    {
      "name": "Win32",
      "includePath": ["${workspaceFolder}/**"],
      "defines": [],
      "compilerPath": "C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.xx.xxxxx/bin/Hostx64/x64/cl.exe",
      "cStandard": "c17",
      "cppStandard": "c++23",
      "intelliSenseMode": "msvc-x64"
    }
  ],
  "version": 4
}
```

### GCC/Clang で std::format が使えない場合

#### オプション 1: {fmt} ライブラリを使用

`std::format` は GCC 13 以降でサポートされています。それ以前のバージョンでは、{fmt}ライブラリを使用できます。

```cmake
# CMakeLists.txt
find_package(fmt REQUIRED)
target_link_libraries(budget_app PRIVATE fmt::fmt)
```

```cpp
// コード内
#include <fmt/format.h>
// std::format の代わりに fmt::format を使用
```

#### オプション 2: フォールバック実装

`std::format` が使えない場合のフォールバック：

```cpp
// Money.cpp
#if __has_include(<format>)
    #include <format>
    std::string Money::toString() const {
        return std::format("¥{}", amount_);
    }
#else
    #include <sstream>
    #include <iomanip>
    std::string Money::toString() const {
        std::ostringstream oss;
        oss << "¥" << amount_;
        return oss.str();
    }
#endif
```

### Ranges の fold_left が使えない場合

`std::ranges::fold_left` は C++23 の機能です。C++20 では `std::accumulate` を使用します。

#### Account.cpp の修正例

```cpp
// C++23版
#include <ranges>
int total = std::ranges::fold_left(incomes, 0, std::plus<>{});

// C++20フォールバック版
#include <numeric>
int total = std::accumulate(
    std::ranges::begin(incomes),
    std::ranges::end(incomes),
    0
);
```

### std::span が使えない場合

#### Account.hpp の代替実装

```cpp
// 元のコード
[[nodiscard]] std::span<const entities::Transaction> getTransactions() const noexcept;

// フォールバック版
[[nodiscard]] const std::vector<entities::Transaction>& getTransactions() const noexcept;
```

```cpp
// Account.cpp
const std::vector<entities::Transaction>& Account::getTransactions() const noexcept {
    return transactions_;
}
```

## コンパイラバージョンの確認

### GCC

```bash
g++ --version
g++ -std=c++23 -E -dM - < /dev/null | grep __cplusplus
```

### Clang

```bash
clang++ --version
clang++ -std=c++23 -E -dM - < /dev/null | grep __cplusplus
```

### MSVC (Visual Studio)

```powershell
cl.exe
# または
"C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.xx.xxxxx\bin\Hostx64\x64\cl.exe"
```

## C++20 のみでビルドする場合

### CMakeLists.txt の変更

```cmake
# C++23から C++20に変更
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
```

### コードの変更が必要な箇所

1. **fold_left → accumulate**

   `src/domain/aggregates/Account.cpp`:

   ```cpp
   // 変更前
   int total = ranges::fold_left(incomes, 0, std::plus<>{});

   // 変更後
   int total = std::accumulate(
       ranges::begin(incomes),
       ranges::end(incomes),
       0
   );
   ```

2. **インクルードの追加**

   ```cpp
   #include <numeric>  // std::accumulate用
   ```

## よくあるコンパイルエラーと解決法

### エラー 1: "no member named 'format' in namespace 'std'"

**原因**: コンパイラが std::format をサポートしていない

**解決法**:

1. コンパイラを GCC 13 以降にアップグレード
2. {fmt}ライブラリを使用
3. ostream を使用したフォールバック実装

### エラー 2: "no member named 'span' in namespace 'std'"

**原因**: C++20 の std::span がサポートされていない

**解決法**:

1. コンパイラをアップグレード（GCC 10+, Clang 7+, MSVC 19.26+）
2. `const std::vector&` を返すように変更

### エラー 3: "fold_left is not a member of std::ranges"

**原因**: C++23 機能

**解決法**:

```cpp
// std::accumulateに変更
#include <numeric>
int total = std::accumulate(
    ranges::begin(incomes),
    ranges::end(incomes),
    0
);
```

### エラー 4: "using enum" が認識されない

**原因**: C++20 以降の機能

**解決法**:

```cpp
// 変更前
using enum Type;
return FOOD;

// 変更後（C++17でも動作）
return Type::FOOD;
```

## 最小要件でビルド（C++17 版）

完全に C++17 でビルドしたい場合の変更点：

### 1. CMakeLists.txt

```cmake
set(CMAKE_CXX_STANDARD 17)
```

### 2. 三方比較演算子の削除

```cpp
// 削除
auto operator<=>(const Money& other) const noexcept = default;

// 追加
bool operator<(const Money& other) const noexcept {
    return amount_ < other.amount_;
}
// その他の比較演算子も手動で実装
```

### 3. Ranges の削除

```cpp
// 通常のループに戻す
for (const auto& t : transactions_) {
    if (t.getCategory().isIncome()) {
        total += t.getAmount().getAmount();
    }
}
```

### 4. std::format の削除

```cpp
#include <sstream>
std::ostringstream oss;
oss << "¥" << amount_;
return oss.str();
```

## ビルドテスト

### 最小限のテストプログラム

```cpp
// test_cpp20.cpp
#include <iostream>
#include <span>
#include <ranges>
#include <format>

int main() {
    std::cout << "C++20/23 features test\n";

    // span
    std::vector<int> vec = {1, 2, 3};
    std::span<int> s(vec);
    std::cout << "span: OK\n";

    // ranges
    auto filtered = vec | std::views::filter([](int i) { return i > 1; });
    std::cout << "ranges: OK\n";

    // format
    std::cout << std::format("format: {}\n", "OK");

    return 0;
}
```

```bash
# コンパイル
g++ -std=c++23 test_cpp20.cpp -o test
./test
```

## 推奨環境

| 機能        | 最小バージョン               | 推奨バージョン     |
| ----------- | ---------------------------- | ------------------ |
| std::span   | GCC 10, Clang 7, MSVC 19.26  | GCC 11+, Clang 13+ |
| Ranges      | GCC 10, Clang 13             | GCC 12+, Clang 15+ |
| std::format | GCC 13, Clang 14, MSVC 19.29 | GCC 13+, Clang 14+ |
| fold_left   | GCC 13, Clang 16             | GCC 14+, Clang 17+ |

## サポート

問題が解決しない場合：

1. コンパイラのバージョンを確認
2. エラーメッセージの全文を確認
3. CMake の出力を確認
4. [cppreference.com](https://en.cppreference.com/) で機能のサポート状況を確認
