X11, Xcode$B$,I,MW(B.  nkf, wget $B$,(B $B%Q%9$NCf$KI,MW(B. (fink $B$OI,?\$G$J$$(B)
ptex$B4D6-(B: /usr/local/bin/ $B$K(B Mac $BMQ$N(B ptex_package_2005v2.1.dmg $B$G(BOK.

$B3+H/(B debug $B4D6-(B.
   0. OpenXM $B$O(B ${HOME}/OX4 $B$N2<$K$"$k$H2>Dj$7$F$$$k(B. cf. Makefile
   1. OpenXM/src $B$G(B make install (debug install $B$G$b(B OK).
   2. make setup-cfep
     ( OpenXM home $B$,(B build/debug $BEy$N2<$X(B link $B$5$l$k$h$&$K(B)
   2'. 10.9 $B$G$O(B ~/Library $B$N2<$N?<$$$H$3$m$K(B build/debug $B$,$G$-$k$h$&$J$N$G(B,
     ox_textmacs  $B$,L5$$$H%(%i!<$,=P$?$i(B, 
     make link-for-debug
     $B$G(B, link $B$r:n@.(B or Makefile $B$r$_$F<jF0(B.  2016.09.30
   3. make install
      ( cfep $BMQ%D!<%k$r(B OpenXM $B0J2<(B $B$X%$%s%9%H!<%k(B.)

$BG[I[HG:n@.<j=g(B.
($B;29M(B, Xcode $B$NMxMQK!(B, video $B$N5-O?(B. 2006$BG/(B2$B7n:"(B)

   0. OpenXM/src $B$G(B make configure-static  
      (static $B$,I,MW(B. cf. otool  2016.09.23)
   1. OpenXM/src $B$G(B make install (debug install $B$G$b(B OK).
   2. make -f Makefile-doc  usage $B$K=>$$(B, tex $B4D6-Ey$r@_Dj(B.
       (/usr/local/bin/ $B$K(B Mac $BMQ$N(B ptex_package_2005v2.1.dmg $B$G(BOK)
   3. make -f Makefile-doc clean
      make -f Makefile-doc build-utf8-asir
      ( OpenXM/doc/cfep $B$K(B utf8 $B$G(B html document $B$r@8@.(B.
        OpenXM/src/asir-contrib/packages/doc $B$G(B make install-html-doc 
        $B$r$b$&0lEY$d$C$F$*$/$H$$$$$+$b(B.
       )
     [ install-pdf-to-cfep $B%?!<%2%C%H$rJT=8(B, cfep/Doc/Intro $B$G(B next2.pdf $B$r@8@.(B. ]
      make -f Makefile-doc install-pdf-to-cfep
   4. make install
      ( cfep $BMQ%D!<%k$r(B OpenXM $B0J2<(B $B$X%$%s%9%H!<%k(B.)
   5. xcode $B$G(B target $B$r(B release $B$K$7$F(B build.
      (Product->Scheme->Edit scheme,  release)
   6. make dist-cfep
      (work $B$N2<$K(B $BG[I[HG(B cfep.app $B$r:n$k(B)   
    Todo. $B$^$@JQ(B. 10.9 $BBP1~$G$J$$(B. ~/Library/Developer $B$N2<$N(B cfep $B$r(B tar $B$7$F$$$J$$(B.
