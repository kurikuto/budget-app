#include "domain/entities/Transaction.hpp"
#include <stdexcept>

namespace domain
{
  namespace entities
  {

    Transaction::Transaction(
        std::string_view id,
        const value_objects::Date &date,
        const value_objects::Category &category,
        const value_objects::Money &amount,
        std::string_view description) : id_(id),
                                        date_(date),
                                        category_(category),
                                        amount_(amount),
                                        description_(description)
    {

      if (id.empty())
      {
        throw std::invalid_argument("トランザクションIDは空にできません");
      }
      if (description.empty())
      {
        throw std::invalid_argument("説明は空にできません");
      }
    }

  } // namespace entities
} // namespace domain
