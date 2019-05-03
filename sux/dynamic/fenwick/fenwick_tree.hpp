#ifndef __FENWICK_FENWICKTREE_HPP__
#define __FENWICK_FENWICKTREE_HPP__

#include "../common.hpp"
#include "../darray.hpp"

namespace hft::fenwick {

/**
 * FenwickTree - Fenwick Tree data structure interface.
 * @sequence: An integer vector.
 * @size: The length of the sequence.
 * @BOUND: maximum value that @sequence can store.
 *
 * This data structure indices starts from 1 and ends in @size.
 *
 */
class FenwickTree {
public:
  virtual ~FenwickTree() = default;

  /**
   * prefix() - Compute the prefix sum.
   * @idx: Length of the prefix sum.
   *
   * Sum the elements in the range (0 .. @idx], returns zero when @idx
   * is zero.
   *
   */
  virtual uint64_t prefix(size_t idx) const = 0;

  /**
   * add() - Increment an element of the sequence (not the tree).
   * @idx: Index (starting from 1) of the element.
   * @inc: Value to sum.
   *
   * You are allowed to use negative values for the increment, but keep in mind you should respect
   * the structure boundaries.
   *
   */
  virtual void add(size_t idx, int64_t inc) = 0;

  /**
   * find() - Search the index of the closest (less or equal than) prefix.
   * @val: Prefix to search.
   *
   * If @val is an l-value reference its value will be changed with the distance between the found
   * and the searched prefix (i.e. the difference between the prefix and @val).
   *
   * This method returns zero if such an element doesn't exists (i.e. there are no prefixes that are
   * greater or equal to @val).
   *
   */
  virtual size_t find(uint64_t *val) const = 0;
  size_t find(uint64_t val) const { return find(&val); }

  /**
   * compFind() - Complement find.
   * @val: Prefix to search.
   *
   * This method search the index whose its prefix its the closest to MAXVAL-@val. MAXVAL is the
   * maximum possibile value for such a prefix (@sequence is therefore bounded).
   *
   * The same considerations made for FenwickTree::find() holds.
   *
   */
  virtual size_t compFind(uint64_t *val) const = 0;
  size_t compFind(uint64_t val) const { return compFind(&val); }

  /**
   * size() - Returns the length of the sequence.
   *
   */
  virtual size_t size() const = 0;

  /**
   * bitCount() - Estimation of the size (in bits) of this structure.
   *
   */
  virtual size_t bitCount() const = 0;

  /**
   * Each FenwickTree is serializable and deserializable with:
   * - friend std::ostream &operator<<(std::ostream &os, const FenwickTree &ft);
   * - friend std::istream &operator>>(std::istream &is, FenwickTree &ft);
   *
   * The data is stored and loaded with the network (big-endian) byte order to guarantee
   * compatibility on different architectures.
   *
   * The serialized data follows the compression and node ordering of the specific Fenwick tree
   * without any compatibility layer (e.g. if you serialize a FixedF, you cannot deserialize the
   * very same data with a ByteL).
   *
   */
};

} // namespace hft::fenwick

#endif // __FENWICK_FENWICKTREE_HPP__
