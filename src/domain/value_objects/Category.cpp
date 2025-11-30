#include "domain/value_objects/Category.hpp"
#include <unordered_map>
#include <format> // C++20: 文字列フォーマット
#include <array>  // std::array用

namespace domain
{
  namespace value_objects
  {

    std::string Category::toString() const
    {
      using enum Type;

      static constexpr std::array<std::pair<Type, std::string_view>, 8> typeNames{{{INCOME, "収入"},
                                                                                   {FOOD, "食費"},
                                                                                   {HOUSING, "住居費"},
                                                                                   {TRANSPORT, "交通費"},
                                                                                   {ENTERTAINMENT, "娯楽"},
                                                                                   {UTILITIES, "光熱費"},
                                                                                   {HEALTHCARE, "医療費"},
                                                                                   {OTHER, "その他"}}};

      for (const auto &[type, name] : typeNames)
      {
        if (type_ == type)
        {
          return std::string(name);
        }
      }
      return "不明";
    }

    Category Category::fromString(std::string_view str)
    {
      using enum Type;

      static const std::unordered_map<std::string_view, Type> stringToType{
          {"収入", INCOME},
          {"INCOME", INCOME},
          {"食費", FOOD},
          {"FOOD", FOOD},
          {"住居費", HOUSING},
          {"HOUSING", HOUSING},
          {"交通費", TRANSPORT},
          {"TRANSPORT", TRANSPORT},
          {"娯楽", ENTERTAINMENT},
          {"ENTERTAINMENT", ENTERTAINMENT},
          {"光熱費", UTILITIES},
          {"UTILITIES", UTILITIES},
          {"医療費", HEALTHCARE},
          {"HEALTHCARE", HEALTHCARE},
          {"その他", OTHER},
          {"OTHER", OTHER}};

      if (auto it = stringToType.find(str); it != stringToType.end())
      {
        return Category(it->second);
      }
      throw std::invalid_argument(std::format("不明なカテゴリ: {}", str));
    }

  } // namespace value_objects
} // namespace domain
