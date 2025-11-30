# ビルドと実行ガイド

## 環境セットアップ

### Windows + Visual Studio

#### 必要なツール

- Visual Studio 2019 16.11 以降、または Visual Studio 2022（C++デスクトップ開発ワークロード）
- CMake 3.20 以上

**注意**: C++20/23 機能を使用するため、最新の Visual Studio が推奨されます。`std::format`は VS 2019 16.10 以降で利用可能です。

#### ビルド手順

1. **Developer Command Prompt for VS を起動**

2. **プロジェクトディレクトリに移動**

   ```powershell
   cd "c:\Users\k-ab6\Work\dev\C++"
   ```

3. **ビルドディレクトリを作成**

   ```powershell
   mkdir build
   cd build
   ```

4. **CMake でプロジェクトを生成**

   ```powershell
   cmake ..
   ```

5. **ビルド実行**

   ```powershell
   cmake --build . --config Debug
   # または Release ビルド
   cmake --build . --config Release
   ```

6. **実行**
   ```powershell
   .\Debug\budget_app.exe
   # または
   .\Release\budget_app.exe
   ```

### Windows + MinGW

#### 必要なツール

- MinGW-w64（GCC 13 以上推奨）
- CMake 3.20 以上

**注意**: GCC 13 未満では`std::format`が利用できない場合があります。

#### ビルド手順

```powershell
cd "c:\Users\k-ab6\Work\dev\C++"
mkdir build
cd build
cmake .. -G "MinGW Makefiles"
cmake --build .
.\budget_app.exe
```

### Linux / macOS

#### 必要なツール

- GCC 13 以上 または Clang 14 以上（C++20/23 機能のフルサポート）
- CMake 3.20 以上
- Make

**推奨コンパイラバージョン**:

- GCC 13+: `std::format`、Ranges 完全サポート
- Clang 14+: `std::format`、Ranges 完全サポート
- GCC 10-12: Ranges サポートあり、`std::format`は未サポート

#### ビルド手順

```bash
cd /path/to/project
mkdir build && cd build
cmake ..
make -j$(nproc)  # 並列ビルド
./budget_app
```

## CMake オプション

### デバッグビルド

```powershell
cmake .. -DCMAKE_BUILD_TYPE=Debug
cmake --build .
```

### リリースビルド（最適化有効）

```powershell
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .
```

### 特定のコンパイラを指定

```powershell
# Clangを使用
cmake .. -DCMAKE_CXX_COMPILER=clang++

# GCCを使用
cmake .. -DCMAKE_CXX_COMPILER=g++
```

## トラブルシューティング

### CMake が見つからない

```powershell
# CMakeをパスに追加するか、フルパスで指定
"C:\Program Files\CMake\bin\cmake.exe" ..
```

### コンパイルエラー：C++20/23 がサポートされていない

- コンパイラのバージョンを確認
- Visual Studio 2019 16.11 以降、GCC 10 以降、Clang 13 以降が必要
- `std::format`を使用する場合は GCC 13 以降、Clang 14 以降、MSVC 19.29 以降が必要

```powershell
# コンパイラバージョン確認
g++ --version
clang++ --version
```

C++20 の一部機能のみサポートされている場合、CMakeLists.txt で標準を変更：

```cmake
set(CMAKE_CXX_STANDARD 20)  # C++23からC++20に変更
```

### リンクエラー

```powershell
# クリーンビルドを試す
cd build
cmake --build . --target clean
cmake --build .
```

### ファイルが見つからないエラー

- `CMakeLists.txt`が正しいディレクトリにあるか確認
- ヘッダーファイルと実装ファイルのパスが一致しているか確認

## IDE での使用

### Visual Studio Code

1. **C++拡張機能をインストール**

   - C/C++ (Microsoft)
   - CMake Tools

2. **プロジェクトを開く**

   ```
   File > Open Folder > C++フォルダを選択
   ```

3. **CMake 構成を選択**

   - Ctrl+Shift+P → "CMake: Configure"

4. **ビルド**

   - Ctrl+Shift+P → "CMake: Build"
   - または F7

5. **実行**
   - Ctrl+F5（デバッグなし）
   - F5（デバッグ付き）

### Visual Studio 2019/2022

1. **CMake プロジェクトとして開く**

   ```
   File > Open > CMake... > CMakeLists.txtを選択
   ```

2. **構成を選択**

   - x64-Debug または x64-Release

3. **ビルド**

   - Ctrl+Shift+B

4. **実行**
   - Ctrl+F5

### CLion

1. **プロジェクトを開く**

   - Open → C++フォルダを選択

2. **ビルド**

   - Ctrl+F9

3. **実行**
   - Shift+F10

## データファイルの配置

アプリケーションは実行ファイルと同じディレクトリに `household_data.csv` を作成します。

**配置例**:

```
build/
├── Debug/
│   ├── budget_app.exe
│   └── household_data.csv  ← ここに作成される
└── Release/
    ├── budget_app.exe
    └── household_data.csv  ← ここに作成される
```

## 高度なビルドオプション

### 静的解析を有効化

```powershell
cmake .. -DCMAKE_CXX_CLANG_TIDY=clang-tidy
```

### テストのビルド（将来的に）

```powershell
cmake .. -DBUILD_TESTS=ON
cmake --build .
ctest
```

### インストール

```powershell
cmake --build . --target install
```

## パフォーマンス最適化

### リリースビルドでの最適化レベル

CMakeLists.txt で設定済み：

- MSVC: `/O2`（デフォルト）
- GCC/Clang: `-O3`（デフォルトは`-O2`）

### プロファイリング

```bash
# Linux/macOSでgprofを使用
cmake .. -DCMAKE_CXX_FLAGS="-pg"
cmake --build .
./budget_app
gprof budget_app gmon.out > analysis.txt
```

## クリーンアップ

```powershell
# ビルドディレクトリを削除
cd ..
Remove-Item -Recurse -Force build

# または、ビルド成果物のみクリア
cd build
cmake --build . --target clean
```

---

**Happy Coding! 🚀**
