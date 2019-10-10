Sux
===

Welcome to the C++ part of the sux project.

The classes we provide fall into three categories:

* Static structures for ranking and selection based on the paper
"Broadword Implementation of Rank/Select Queries". We provide
also an implementation of the Elias-Fano representation
of monotone sequences that can be used as an opportunistic
bitvector representation.

* Fenwick trees with bounded leaf size, and associated dynamic structures
for ranking and selection based on the paper "Compact Fenwick Trees for
Dynamic Ranking and Selection" (with Stefano Marchini).

* Minimal perfect hashing functions based on the paper "RecSplit:
Minimal Perfect Hashing via Recursive Splitting" (with
Emmanuel Esposito and Thomas Mueller Graf).

All classes are heavily asserted. For testing speed, remember to use
`-DNDEBUG`.


seba (<mailto:sebastiano.vigna@unimi.it>)
