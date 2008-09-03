#!/usr/bin/perl
# $OpenXM$

## asir �Υץ�����(�Ǥ������û��)ʸ��������˽񤭴�����.
## usage: asir2string.pl example.rr example.txt
## Bugs: (1) ʸ�����ƥ�����������Ѥ���Ƥ��ޤ�.
##       (2) �ȡ�����ζ�����̵�뤹��.  �㤨��
##           ��A = B + +C�� �� ��A=B++C�פȤʤ�.

sub main {
	($_) = @_;
	s#/\*.*?\*/##gs ;
	s#\\#\\\\#gs ;
	s#\s+# #gs ;
	s#end[\$;].*$##gs ;
	s#"#\\"#gs ;
	s#\s?([,=;<>\[\]\(\){}\+\-\*/%])\s?#\1#gs ;
	s#^\s?#"#gs ;
	s#\s?$#"#gs ;
	return $_;
}

local $/;

open(STDIN,  $ARGV[0]) if @ARGV > 0 ;
open(STDOUT, '>' . $ARGV[1]) if @ARGV > 1;
my $buffer;
$buffer = <STDIN>;

$buffer = main $buffer;

print $buffer;
