$OpenXM: OpenXM/doc/OpenXM-web/readme-ja.txt,v 1.1 2005/02/10 12:56:36 takayama Exp $
http://www.openxm.org 
(http://www.math.kobe-u.ac.jp/OpenXM)
$B$N99?7J}K!(B.

OpenXM/src $B$G(B make install
OpenXM/src $B$G(B make install-document
$B$7$F(B, OpenXM/lib, OpenXM/doc $BEy$r:G?7HG$K$9$kI,MW$,$"$k(B.
[ bug: (cd OpenXM/src/asir-contrib/packages/doc ;  make install-html-help)
   $B$7$J$$$H(B asir-contrib $B$N(B html $B%^%K%e%"%k$,99?7$5$l$J$$(B. ]

$B$^$?(B index.html $BEy$r99?7$9$kI,MW$,$"$k(B.

(cd OpenXM/rc; make ; source dot.bashrc)  [ bash $B$N>l9g(B]
$B$r$d$C$F(B OpenXM $B4D6-$rMxMQ$G$-$k$h$&$K$7$F$+$i(B 
OpenXM-web $B$G(B make $B$r;O$a$k$3$H(B.
make clean  (make clean-everything)
$B$7$F$+$i(B
make all
$B$9$k$3$H$K$h$j(B /home/web/OpenXM/Current $B$,99?7$5$l$k(B.

OpenXM-web $B$GJT=8$9$Y$-%U%!%$%k(B.
top.html, 
index.html (head)

Release $BHG$N>l9g$O(B 
cd /home/web/OpenXM
cp -r Current /home/web/OpenXM/x.y.z (version number)
/home/web/OpenXM/x.y.z/index.html  $B$rJT=8$9$k(B.
$B$=$N8e(B, /home/web/OpenXM/x.y.z $B$O(B freeze $B$9$k(B.

tar-ball release $BHG(B/ head $BHG$N:n@.J}K!(B.
cd misc/packages/Generic/openxm
 make install-tar-to-ftp-area
 ftp.math.kobe-u.ac.jp/pub/OpenXM/head/openxm-head.tar.gz $B$X%3%T!<$5$l$k(B.
release $BHG$N>l9g$3$l$NL>A0$rJQ$($F(B
 ftp.math.kobe-u.ac.jp/pub/OpenXM/x.y.z/openxm-x.y.z.tar.gz $B$X$b%3%T!<(B.



