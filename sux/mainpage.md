
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

        sux::Rank9Sel rs(v, n);

  will create a rank/select structure using Rank9 and Select9.

- Assuming `v` is a bit vector (i.e., an array of `uint64_t`) and `n` the number
  of bits represented therein, to create a dynamic rank/select data structure
  using a byte-compressed Fenwick tree in classical Fenwick layout over
  strides of sixteen words use

        sux::fenwick::Stride<ByteF, 16>(v, n)

  By modifying the two template parameters you can use a different stride or
  a different Fenwick tree structure. A stride is scanned sequentially: the
  Fenwick tree keeps track of the number of ones appearing at the border
  of each stride.

- Assuming `v` is vector of nonnegative values(i.e., an array of `uint64_t`)
  and `n` the number of values, to create a non-compressed Fenwick tree
  with level-order layout use

        sux::fenwick::FixedL(v, n);

  By modifying the two template parameters you can use a different stride or
  a different Fenwick tree structure.

- To create a minimal perfect hash function from a vector of strings `keys`, with
  leaf size 8 and bucket size 128, use

        sux::RecSplit<8> rs(keys, 128)

  The structure can be saved with sux::RecSplit.dump() and loaded with 
  sux::RecSplit.load().
