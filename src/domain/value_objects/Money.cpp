#include "domain/value_objects/Money.hpp"
#include <format>

namespace domain
{
  namespace value_objects
  {

    std::string Money::toString() const
    {
      return std::format("¥{}", amount_);
    }

  } // namespace value_objects
} // namespace domain
