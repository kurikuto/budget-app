cmake_minimum_required(VERSION 3.20)
project(HouseholdBudgetApp VERSION 1.0 LANGUAGES CXX)

# C++23を使用（C++20にフォールバック可能）
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# コンパイラ警告設定
if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

# ソースファイル
set(DOMAIN_SOURCES
    src/domain/value_objects/Money.cpp
    src/domain/value_objects/Date.cpp
    src/domain/value_objects/Category.cpp
    src/domain/entities/Transaction.cpp
    src/domain/aggregates/Account.cpp
)

set(APPLICATION_SOURCES
    src/application/usecases/RegisterTransactionUseCase.cpp
    src/application/usecases/ViewBalanceUseCase.cpp
    src/application/usecases/ViewTransactionsByCategoryUseCase.cpp
)

set(INFRASTRUCTURE_SOURCES
    src/infrastructure/repositories/CsvAccountRepository.cpp
)

set(PRESENTATION_SOURCES
    src/presentation/cli/CliController.cpp
    src/main.cpp
)

add_executable(budget_app
    ${DOMAIN_SOURCES}
    ${APPLICATION_SOURCES}
    ${INFRASTRUCTURE_SOURCES}
    ${PRESENTATION_SOURCES}
)

target_include_directories(budget_app PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

# テスト用のターゲット（オプション）
option(BUILD_TESTS "Build tests" OFF)
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
