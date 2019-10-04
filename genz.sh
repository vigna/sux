#!/bin/bash

cp simple_select.cpp simple_select_zero.cpp
cp simple_select.h simple_select_zero.h

sed -i -e 's/bits\[/~bits\[/g' simple_select_zero.cpp
sed -i -e 's/simple_select/simple_select_zero/g' simple_select_zero.h
sed -i -e 's/simple_select/simple_select_zero/g' simple_select_zero.cpp
sed -i -e 's/select(/select_zero(/g' simple_select_zero.cpp
sed -i -e 's/select(/select_zero(/g' simple_select_zero.h
sed -i -e 's/r = rank9.rank( t )/r = t - rank9.rank( t )/' simple_select_zero.cpp
sed -i -e 's/r = rank9.rank( i )/r = i - rank9.rank( i )/' simple_select_zero.cpp
sed -i -e '/c <= num_bits/iif ( num_bits % 64 != 0 ) c -= 64 - num_bits % 64;' simple_select_zero.cpp

cp simple_select_half.cpp simple_select_zero_half.cpp
cp simple_select_half.h simple_select_zero_half.h

sed -i -e 's/bits\[/~bits\[/g' simple_select_zero_half.cpp
sed -i -e 's/simple_select_half/simple_select_zero_half/g' simple_select_zero_half.h
sed -i -e 's/simple_select_half/simple_select_zero_half/g' simple_select_zero_half.cpp
sed -i -e 's/select(/select_zero(/g' simple_select_zero_half.cpp
sed -i -e 's/select(/select_zero(/g' simple_select_zero_half.h
sed -i -e 's/r = rank9.rank( t )/r = t - rank9.rank( t )/' simple_select_zero_half.cpp
sed -i -e 's/r = rank9.rank( i )/r = i - rank9.rank( i )/' simple_select_zero_half.cpp
sed -i -e '/c <= num_bits/iif ( num_bits % 64 != 0 ) c -= 64 - num_bits % 64;' simple_select_zero_half.cpp
