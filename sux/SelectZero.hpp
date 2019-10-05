#pragma once

#include "common.hpp"

namespace sux {

class SelectZero {
public:
  virtual ~SelectZero() = default;

  /**
   * selectZero() - Index of the 0-bit with a given rank.
   * @rank: Number of 0-bits before the returned index.
   *
   * This method returns SIZE_MAX if no sucn an index exists.
   *
   */
  virtual size_t selectZero(uint64_t rank) const = 0;
};

} // namespace sux
