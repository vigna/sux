
Welcome to the C++ part of the [Sux](http://sux.di.unimi.it/) project.

Available classes
-----------------

The classes we provide fall into three categories:

* Static structures for ranking and selection based on the paper
  ["Broadword Implementation of Rank/Select
  Queries"](http://vigna.di.unimi.it/papers.php#VigBIRSQ).
  We provide also an implementation of the Elias-Fano representation of
  monotone sequences that can be used as an opportunistic bitvector
  representation.

* Fenwick trees with bounded leaf size, and associated dynamic structures for
  ranking and selection based on the paper ["Compact Fenwick Trees for
  Dynamic Ranking and Selection"](http://vigna.di.unimi.it/papers.php#MaVCFTDRS) (with
  Stefano Marchini).

* Minimal perfect hashing functions based on the paper ["RecSplit: Minimal
  Perfect Hashing via Recursive Splitting"](http://vigna.di.unimi.it/papers.php#EGVRS) 
  (with Emmanuel Esposito and Thomas Mueller Graf).

All classes are heavily asserted. For testing speed, remember to use `-DNDEBUG`.

All provided classes are templates, so you just have to copy the files in
the `sux` directory somewhere in your include path.

Examples
--------

- Assuming `v` is a bit vector represented by an array of `uint64_t` and `n` 
  is the number of bits represented therein,

        #include <sux/bits/Rank9Sel.hpp>

        sux::bits::Rank9Sel rs(v, n);

  will create a rank/select structure using Rank9 and Select9. Note that the
  bit vector is not copied, so if you change its contents the results will
  be unpredictable.

- Assuming again that `v` is a bit vector and `n` the number
  of bits represented therein, to create a dynamic rank/select data structure
  using a fixed-size Fenwick tree in classical Fenwick layout over
  strides of sixteen words you must use

        #include <sux/bits/StrideDynRankSel.hpp>
        #include <sux/util/FenwickFixedF.hpp>

        sux::bits::StrideDynRankSel<sux::util::FenwickFixedF, 16> drs(v, n)

  Note that you should subsequently modify the contents of `v` only through
  the mutation methods of StrideDynRankSel, or the results will be
  unpredictable.

  Such a structure is ideal if ranking is the most common operation. If selection
  is the most common operation a structure based on a byte-compressed level-order
  tree is faster:

        #include <sux/bits/StrideDynRankSel.hpp>
        #include <sux/util/FenwickByteL.hpp>

        sux::bits::StrideDynRankSel<sux::util::FenwickByteL, 16> drs(v, n);

  In general, by modifying the two template parameters you can use a
  different stride or a different Fenwick tree structure. A stride is
  scanned sequentially: the Fenwick tree keeps track of the number of ones
  appearing at the border of each stride. Structures with single-word
  strides should be allocated as follows:

        #include <sux/bits/WordDynRankSel.hpp>
        #include <sux/util/FenwickFixedF.hpp>

        sux::bits::WordDynRankSel<sux::util::FenwickFixedF> drs(v, n);

  Note that after the construction you should modify the vector `v` only 
  by means of the methods of the structure.

- Similary, if `v` is a list of `n` values bounded by 10000 a fixed-size
  Fenwick tree in classical Fenwick layout can be created by

        #include <sux/util/FenwickFixedF.hpp>

        sux::util::FenwickFixedF<10000> ft(v, n);

  Note that the vector `v` is only read at construction time, and it is
  not subsequently accessed by the tree.

  Such a tree is ideal if prefix sums are the most common operations. If
  find is the most operation, again a byte-compressed level-order
  structure is faster:

        #include <sux/util/FenwickByteL.hpp>

        sux::util::FenwickByteL<10000> ft(v, n);

  For maximum compression, in particular if the bound is very small,
  you can use a bit-compressed tree, which however will be a bit slower:

        #include <sux/util/FenwickBitF.hpp>

        sux::util::FenwickBitF<3> ft(v, n);

  Note that `v` is read at construction time, but it is not referenced
  afterwards.

- To create a minimal perfect hash function from a vector of strings `keys`, with
  leaf size 8 and bucket size 100, use

        #include <sux/function/RecSplit.hpp>

        sux::function::RecSplit<8> mph(keys, 100)

  It will use abount 1.8 bits per key. Increasing the leaf and bucket
  sizes gives much more compact structures (1.56 bits per key), at the
  price of a slower construction time:

        #include <sux/function/RecSplit.hpp>

        sux::function::RecSplit<16> mph(keys, 2000)

  The class sux::function::RecSplit implements the operator
  sux::function::RecSplit::operator()(const string &key) , so you
  can obtain the number associated with a string key `k` with `mph(k)`.

Memory allocation
-----------------

Almost all data structures make it possible to allocate memory in
different ways, and in particular to use transparent large pages (usually,
2MiB) on Linux. All allocations are based on the class sux::util::Vector,
which is a zero-cost wrapper around an array (no bound checks), and has
a template parameter for choosing the allocation strategy depending
on an item of sux::util::AllocType.

For example,

        #include <sux/util/FenwickFixedF.hpp>

        sux::util::FenwickFixedF<10000, MALLOC> f(v, n)

creates a Fenwick tree as above, using a standard `malloc()` call, which
is usually the default, and the most compatible approach, but

        #include <sux/util/FenwickFixedF.hpp>

        sux::util::FenwickFixedF<10000, TRANSHUGEPAGE> f(v, n)

will try to use transparent huge pages instead, if available.

Analogously,

        #include <sux/function/RecSplit.hpp>

        sux::function::RecSplit<8, TRANSHUGEPAGE> rs(keys, 100)

creates a RecSplit instance using transparent huge pages.

If you want to test a static rank/select structure using a certain
allocation strategy, it is usually a good idea to allocate _both_ the
underlying bit vector _and_ the rank/select structure using the same
method. You can use sux::util::Vector to this purpose:

        #include <sux/util/Vector.hpp>
        #include <sux/bits/Rank9.hpp>

        sux::util::Vector<uint64_t, SMALLPAGE> v(1000);
        // Modify v
        sux::bits::Rank9<SMALLPAGE> r(&v, num_bits);

where `num_bits` is less than 64000. The `&` operator applied to an
instance of sux::util::Vector returns a pointer to its backing array.
