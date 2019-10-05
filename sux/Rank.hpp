#pragma once

#include "common.hpp"

namespace sux {

class Rank {
public:
  virtual ~Rank() = default;

  /**
   * rank() - Numbers of 1-bits preceding a specified position.
   * @pos: An index of the bit vector.
   *
   */
  virtual uint64_t rank(size_t pos) const = 0;

  /**
   * rank() - Numbers of 1-bits between a given range.
   * @from: Starting index of the bit vector.
   * @to: Ending index of the bit vector.
   *
   */
  uint64_t rank(size_t from, size_t to) const { return rank(to) - rank(from); }

  /**
   * size() - length (in bits) of the bitvector.
   *
   */
  virtual size_t size() const = 0;
};

} // namespace sux
