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
  virtual uint64_t rank(const size_t pos) const = 0;

  /**
   * rank() - Numbers of 1-bits between a given range.
   * @from: Starting index of the bit vector.
   * @to: Ending index of the bit vector.
   *
   */
  virtual uint64_t rank(const size_t from, const size_t to) const = 0;

  /**
   * rankZero() - Numbers of 0-bits preceding a specified position.
   * @pos: An index of the bit vector.
   *
   */
  virtual uint64_t rankZero(const size_t pos) const = 0;

  /**
   * rankZero() - Number of 0-bits between a given range.
   * @from: Starting index of the bit vector.
   * @to: Ending index of the bit vector.
   *
   */
  virtual uint64_t rankZero(const size_t from, const size_t to) const = 0;

  /**
   * size() - length (in bits) of the bitvector.
   *
   */
  virtual size_t size() const = 0;
};
} // namespace sux
