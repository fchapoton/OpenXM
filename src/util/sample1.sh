#!/bin/csh
oxreplace --oldx 'bbb2b9cdcab8b8a5' --new 'References' $*
oxreplace --oldx 'a4b3a4cecab8bdf1a4cba4c4a4a4a4c62e2e2e' --new 'About this document' $1
oxreplace --oldx 'cabfc0ae' --new 'Heisei ' $*
oxreplace --oldx 'c7af' --new '.' $*
oxreplace --oldx 'b7ee' --new '.' $*
oxreplace --oldx 'c6fc' --new '.' $*

# bbb2b9cdcab8b8a5 $B;29MJ88%(B (EUC)
# a4b3a4cecab8bdf1a4cba4c4a4a4a4c62e2e2e $B$3$NJ8=q$K$D$$$F(B...
# cabfc0ae $BJ?@.(B 
# c7af $BG/(B 
# b7ee $B7n(B
# c6fc $BF|(B
