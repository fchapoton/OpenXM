$B3+H/(B debug $B4D6-(B.
   0. OpenXM $B$O(B ${HOME}/OX4 $B$N2<$K$"$k$H2>Dj$7$F$$$k(B. cf. Makefile
   1. OpenXM/src $B$G(B make install (debug install $B$G$b(B OK).
   2. make setup-cfep
     ( OpenXM home $B$,(B build/debug $BEy$N2<$X(B link $B$5$l$k$h$&$K(B)
   3. make install
      ( cfep $BMQ%D!<%k$r(B OpenXM $B0J2<(B $B$X%$%s%9%H!<%k(B.)

$BG[I[HG:n@.<j=g(B.

   1. OpenXM/src $B$G(B make install (debug install $B$G$b(B OK).
   2. make -f Makefile-doc  usage $B$K=>$$(B, tex $B4D6-Ey$r@_Dj(B.
   3. make -f Makefile-doc clean
      make -f Makefile-doc build-utf8-asir
      ( OpenXM/doc/cfep $B$K(B utf8 $B$G(B html document $B$r@8@.(B.)
     [ install-pdf-to-cfep $B%?!<%2%C%H$rJT=8(B ]
      make -f Makefile-doc install-pdf-to-cfep
   4. make install
      ( cfep $BMQ%D!<%k$r(B OpenXM $B0J2<(B $B$X%$%s%9%H!<%k(B.)
   5. xcode $B$G(B target $B$r(B release $B$K$7$F(B build.
   6. make dist-cfep
      (work $B$N2<$K(B $BG[I[HG(B cfep.app $B$r:n$k(B)
