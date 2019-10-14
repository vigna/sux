
Welcome to the C++ part of the [Sux](http://sux.di.unimi.it/) project.

Available classes
-----------------

The classes we provide fall into three categories:

* Static structures for ranking and selection based on the paper "Broadword
  Implementation of Rank/Select Queries". We provide also an implementation of
  the Elias-Fano representation of monotone sequences that can be used as an
  opportunistic bitvector representation.

* Fenwick trees with bounded leaf size, and associated dynamic structures for
  ranking and selection based on the paper "Compact Fenwick Trees for Dynamic
  Ranking and Selection" (with Stefano Marchini).

* Minimal perfect hashing functions based on the paper "RecSplit: Minimal
  Perfect Hashing via Recursive Splitting" (with Emmanuel Esposito and Thomas
  Mueller Graf).

All classes are heavily asserted. For testing speed, remember to use `-DNDEBUG`.

We do not provide libraries: you are invited to include the sources with your
code.

Examples
--------

- Assuming `v` is a bit vector (i.e., an array of `uint64_t`) and `n` the number
  of bits represented therein,

        sux::bits::Rank9Sel rs(v, n);

  will create a rank/select structure using Rank9 and Select9.

- Assuming `v` is a bit vector (i.e., an array of `uint64_t`) and `n` the number
  of bits represented therein, to create a dynamic rank/select data structure
  using a fixed-size Fenwick tree in classical Fenwick layout over
  strides of sixteen words you must use

        sux::bits::Stride<sux::util::FixedF, 16>(v, n)

  Such a structure is ideal if ranking is the most common operation. If selection
  is the most common operation a structure based on a byte-compressed level-order
  tree is faster:

        sux::bits::Stride<sux::util::ByteL, 16>(v, n)

  In general, by modifying the two template parameters you can use a
  different stride or a different Fenwick tree structure. A stride is
  scanned sequentially: the Fenwick tree keeps track of the number of ones
  appearing at the border of each stride. Structures with single-word
  strides should be allocated as follows:

        sux::bits::Word<sux::util::FixedF>(v, n)

- Similary, if `v` is a list of `n` values bounded by 10000 a fixed-size
  Fenwick tree in classical Fenwick layout can be created by

        sux::util::FixedF<10000>(v, n)

  Such a tree is ideal if prefix sums are the most common operations. If
  find is the most operation, again a byte-compressed level-order
  structure is faster:

        sux::util::ByteL<10000>(v, n)

  For maximum compression, in particular if the bound is very small,
  you can use a bit-compressed tree, which however will be a bit slower:

        sux::util::BitF<3>(v, n)

- To create a minimal perfect hash function from a vector of strings `keys`, with
  leaf size 8 and bucket size 100, use

        sux::function::RecSplit<8> rs(keys, 100)

  It will use abount 1.8 bits per key. Increasing the leaf and bucket
  sizes gives much more compact structures (1.56 bits per key), at the
  price of a slower construction time:

        sux::function::RecSplit<16> rs(keys, 2000)

  The class RecSplit implements the operator
  sux::function::RecSplit::operator()(const std::string &key), so you
  can obtain the number associated with a string key `k` with `rs(k)`.
