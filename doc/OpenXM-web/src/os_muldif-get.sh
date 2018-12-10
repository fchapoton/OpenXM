#!/bin/sh
# $OpenXM: OpenXM/doc/OpenXM-web/src/os_muldif-get.sh,v 1.2 2017/06/12 04:29:12 takayama Exp $
OpenXM_HOME=/home/taka/OX4/OX64/OpenXM
#OpenXM_HOME=/home/nobuki/OX4/OpenXM
OX_MULDIF=${OpenXM_HOME}/src/asir-contrib/packages/src/os_muldif.rr
mkdir -p muldif
if true ; then
#if false ; then
(cd muldif ; curl http://www.ms.u-tokyo.ac.jp/~oshima/muldif/os_muldif.rr >os_muldif.rr)
(cd muldif ; curl http://www.ms.u-tokyo.ac.jp/~oshima/muldif/os_muldif.pdf >os_muldif.pdf)
(cd muldif ; curl http://www.ms.u-tokyo.ac.jp/~oshima/muldif/os_muldif.dvi >os_muldif.dvi)
fi
head -1 ${OX_MULDIF} >muldif/tmp-os_muldif.rr
cat muldif/os_muldif.rr >>muldif/tmp-os_muldif.rr
if diff -B -b -q muldif/tmp-os_muldif.rr ${OX_MULDIF} >/dev/null ; then
  echo "there is no change in os_muldif.rr"
else
  mail -s 'os_muldif_is_updated_check_ox_current_doc_other_docs' takayama@math.kobe-u.ac.jp </dev/null
fi
if true ; then
(cd muldif ; curl http://www.ms.u-tokyo.ac.jp/~oshima/muldif/changelog.pdf >changelog.pdf)
fi




 
