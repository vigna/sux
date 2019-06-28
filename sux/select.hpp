#pragma once

namespace sux {
class Select {
public:
  virtual ~Select() = default;

  /**
   * select() - Index of the 1-bit with a given rank.
   * @rank: Number of 1-bits before the returned index.
   *
   * This method returns SIZE_MAX if no such an index exists.
   *
   */
  virtual size_t select(uint64_t rank) const = 0;

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
