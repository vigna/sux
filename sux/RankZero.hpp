#pragma once

#include "common.hpp"

namespace sux {

class RankZero {
public:
  virtual ~RankZero() = default;

  /**
   * rankZero() - Numbers of 0-bits preceding a specified position.
   * @pos: An index of the bit vector.
   *
   */
  virtual uint64_t rankZero(size_t pos) const = 0;

  /**
   * rankZero() - Number of 0-bits between a given range.
   * @from: Starting index of the bit vector.
   * @to: Ending index of the bit vector.
   *
   */
  uint64_t rankZero(size_t from, size_t to) const { return rankZero(to) - rankZero(from); }

  /**
   * size() - length (in bits) of the bitvector.
   *
   */
  virtual size_t size() const = 0;
};

} // namespace sux
