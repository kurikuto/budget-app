# クイック確認スクリプト

## ビルドテスト（Windows PowerShell）

```powershell
# プロジェクトディレクトリで実行
cd "c:\Users\k-ab6\Work\dev\C++"

# ビルドディレクトリを作成（既にある場合は不要）
if (-not (Test-Path "build")) {
    mkdir build
}

cd build

# CMakeで設定
cmake ..

# ビルド実行
cmake --build . --config Debug

# 成功したら実行
if ($LASTEXITCODE -eq 0) {
    Write-Host "✅ ビルド成功！" -ForegroundColor Green
    .\Debug\budget_app.exe
} else {
    Write-Host "❌ ビルド失敗。エラーメッセージを確認してください。" -ForegroundColor Red
}
```

## コンパイラのバージョン確認

```powershell
# Visual Studio のコンパイラ
cl.exe

# または
"C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\*\bin\Hostx64\x64\cl.exe"
```

## 必要なら：C++20 版にダウングレード

もしコンパイラが C++23 に対応していない場合：

### CMakeLists.txt を編集

```cmake
set(CMAKE_CXX_STANDARD 20)  # 23 → 20に変更
```

### fold_left を accumulate に変更

`src/domain/aggregates/Account.cpp` の 2 箇所を修正：

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

## トラブルシューティング

### エラー: "std::format" が見つからない

**GCC 12 以前、Clang 13 以前**の場合は `std::format` が未サポート。

#### 簡易的な修正（ostream にフォールバック）

`src/domain/value_objects/Money.cpp`:

```cpp
#include <sstream>  // std::formatの代わり

std::string Money::toString() const {
    std::ostringstream oss;
    oss << "¥" << amount_;
    return oss.str();
}
```

同様に `Date.cpp` と `Category.cpp` も修正。

---

**結論**: IntelliSense のエラーは無視して OK です。実際にビルドして確認してください！
