/* $OpenXM$ */


$B0J2<$N4D6-JQ?t$N@_Dj$,I,MW(B
  setenv MAPLE maple-home-dir

$B%3%s%Q%$%k$KI,MW$J>pJs$O0J2<$N%X%k%W;2>H(B
  OpenMaple/C/Examples | External Code Examples  

$B$"$i$+$8$a0J2<$N=`Hw$,I,MW(B
  % cd $(OpenXM_HOME)/src/ox_toolkit
  % make && make install
  % cd $(OpenXM_HOME)/src/ox_ntl
  % make && make install

$B0J2<$N4D6-$GF0:n3NG'(B
 - Linux 2.6.18-6-686 i686 GNU/Linux



memo.

debian $B$N(B xterm $B$N;EMM$G(B LD_LIBRARY_PATH $B$,$o$?$i$J$$(B
http://www.math.kobe-u.ac.jp/OpenXM/1.2.2/OpenXM-FAQ-ja.txt

FAQ $B$K$O(B /lib $B$H$"$k$,(B, $OpenXM/lib $B$,$o$?$C$F$-$F$$$?(B.

> cd $OpenXM/lib
> ln -s /tool/maple11/bin.IBM_INTEL_LINUX/libmaplec.so
> ln -s /tool/maple11/bin.IBM_INTEL_LINUX/libmaple.so
> ln -s /tool/maple11/bin.IBM_INTEL_LINUX/libprocessor32.so
> ln -s /tool/maple11/bin.IBM_INTEL_LINUX/libhf.so


