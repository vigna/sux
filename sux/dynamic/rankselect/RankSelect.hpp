#pragma once

#include "../../Rank.hpp"
#include "../../RankZero.hpp"
#include "../../Select.hpp"
#include "../../SelectZero.hpp"
#include "../../common.hpp"
#include "../DArray.hpp"

namespace sux::ranking {

/**
 * RankSelect - Dynamic rank & select data structure interface.
 * @bitvector: A bit vector of 64-bit words.
 * @length: The length (in words) of the bitvector.
 * @T: Underlining Fenwick tree of an ungiven <size_t> bound.
 *
 * This data structure indices starts from 0 and ends in @length-1.
 *
 */
class RankSelect : public Rank, public RankZero, public Select, public SelectZero {
public:
  virtual ~RankSelect() = default;

  /**
   * bitvector() - Returns the bit vector.
   *
   * It's a constant, so you can't change the junk inside unless you
   * explicitly cast away the constness (at your own risk).
   *
   */
  virtual const uint64_t *bitvector() const = 0;
  /**
   * update() - Replace a given word in the bitvector.
   * @index: index (in words) in the bitvector.
   * @word: new value for @bitvector[@index]
   *
   * This method returns the replaced word.
   *
   */
  virtual uint64_t update(size_t index, uint64_t word) = 0;

  /**
   * set() - Set (set to 1) a given bit in the bitvector.
   * @index: Index (in bits) in the bitvector.
   *
   * This method returns the previous value of such a bit.
   *
   */
  virtual bool set(size_t index) = 0;

  /**
   * clear() - Clear (set to 0) a given bit in the bitvector.
   * @index: Index (in bits) in the bitvector.
   *
   * This method returns the previous value of such a bit.
   *
   */
  virtual bool clear(size_t index) = 0;

  /**
   * toggle() - Change the value of a given bit in the bitvector.
   * @index: Index (in bits) in the bitvector.
   *
   * This method returns the previous value of such a bit.
   *
   */
  virtual bool toggle(size_t index) = 0;

  /**
   * bitCount() - Estimation of the size (in bits) of this structure.
   *
   */
  virtual size_t bitCount() const = 0;

  /**
   * Each RankSelect is serializable and deserializable with:
   * - friend std::ostream &operator<<(std::ostream &os, const RankSelect &bv);
   * - friend std::istream &operator>>(std::istream &is, RankSelect &bv);
   *
   * The data is stored and loaded with the network (big-endian) byte order to guarantee
   * compatibility on different architectures.
   *
   * The serialized data follows the compression and node ordering of the specific underlying
   * data structures without any compatibility layer.
   *
   */
};

} // namespace sux::ranking
