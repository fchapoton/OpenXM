/* Prepare TeX index dribble output into an actual index.

   Version 1.45

   Copyright (C) 1987, 1991, 1992 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307. */

/*
   95.10.18 modified by Nobuyosi KAMEI <CXK03514@niftyserve.or.jp>
	���ܸ�EUC�������Ǥ���褦�ˤ�����
   95.10.19 modified by K. Handa <handa@etl.go.jp>
	\initial{...} �ν��Ϥ�Ĵ����
*/


#include <stdio.h>
#include <ctype.h>
#include <errno.h>
#include    <assert.h>
#include "getopt.h"

#define TEXINDEX_VERSION_STRING "GNU Texindex 2.0 for Texinfo release 3.4"
#define TEXINDEX_MODIFICATION_STRING "(modified for Japanese EUC encoding)"

#if defined (emacs)
#  include "../src/config.h"
/* Some s/os.h files redefine these. */
#  undef read
#  undef close
#  undef write
#  undef open
#endif

#if defined (HAVE_STRING_H)
#  include <string.h>
#endif /* HAVE_STRING_H */

#if !defined (HAVE_STRCHR)
char *strrchr ();
#endif /* !HAVE_STRCHR */

#if defined (STDC_HEADERS)
#  include <stdlib.h>
#else /* !STDC_HEADERS */
char *getenv (), *malloc (), *realloc ();
#endif /* !STDC_HEADERS */

#if defined (HAVE_UNISTD_H)
#  include <unistd.h>
#else /* !HAVE_UNISTD_H */
off_t lseek ();
#endif /* !HAVE_UNISTD_H */

#if !defined (HAVE_MEMSET)
#undef memset
#define memset(ptr, ignore, count) bzero (ptr, count)
#endif


char *mktemp ();

#if defined (VMS)
#  include <file.h>
#  define TI_NO_ERROR ((1 << 28) | 1)
#  define TI_FATAL_ERROR ((1 << 28) | 4)
#  define unlink delete
#else /* !VMS */
#  if defined (HAVE_SYS_FCNTL_H)
#    include <sys/types.h>
#    include <sys/fcntl.h>
#  endif /* HAVE_SYS_FCNTL_H */

#  if defined (_AIX) || !defined (_POSIX_VERSION)
#    include <sys/file.h>
#  else /* !AIX && _POSIX_VERSION */
#    if !defined (HAVE_SYS_FCNTL_H)
#      include <fcntl.h>
#    endif /* !HAVE_FCNTL_H */
#  endif /* !_AIX && _POSIX_VERSION */
#  define TI_NO_ERROR 0
#  define TI_FATAL_ERROR 1
#endif /* !VMS */

#if !defined (SEEK_SET)
#  define SEEK_SET 0
#  define SEEK_CUR 1
#  define SEEK_END 2
#endif /* !SEEK_SET */

#if !defined (errno)
extern int errno;
#endif
char *strerror ();

/* When sorting in core, this structure describes one line
   and the position and length of its first keyfield.  */
struct lineinfo
{
  u_short *text;		/* The actual text of the line. */
  union {
    u_short *text;	/* The start of the key (for textual comparison). */
    long number;	/* The numeric value (for numeric comparison). */
  } key;
  long keylen;		/* Length of KEY field. */
};

/* This structure describes a field to use as a sort key. */
struct keyfield
{
  int startwords;	/* Number of words to skip. */
  int startchars;	/* Number of additional chars to skip. */
  int endwords;		/* Number of words to ignore at end. */
  int endchars;		/* Ditto for characters of last word. */
  char ignore_blanks;	/* Non-zero means ignore spaces and tabs. */
  char fold_case;	/* Non-zero means case doesn't matter. */
  char reverse;		/* Non-zero means compare in reverse order. */
  char numeric;		/* Non-zeros means field is ASCII numeric. */
  char positional;	/* Sort according to file position. */
  char braced;		/* Count balanced-braced groupings as fields. */
};

/* Vector of keyfields to use. */
struct keyfield keyfields[3];

/* Number of keyfields stored in that vector.  */
int num_keyfields = 3;

/* Vector of input file names, terminated with a null pointer. */
char **infiles;

/* Vector of corresponding output file names, or NULL, meaning default it
   (add an `s' to the end). */
char **outfiles;

/* Length of `infiles'. */
int num_infiles;

/* Pointer to the array of pointers to lines being sorted. */
u_short **linearray;

/* The allocated length of `linearray'. */
long nlines;

/* Directory to use for temporary files.  On Unix, it ends with a slash.  */
char *tempdir;

/* Start of filename to use for temporary files.  */
char *tempbase;

/* Number of last temporary file.  */
int tempcount;

/* Number of last temporary file already deleted.
   Temporary files are deleted by `flush_tempfiles' in order of creation.  */
int last_deleted_tempcount;

/* During in-core sort, this points to the base of the data block
   which contains all the lines of data.  */
u_short *text_base;

/* Additional command switches .*/

/* Nonzero means do not delete tempfiles -- for debugging. */
int keep_tempfiles;

/* The name this program was run with. */
char *program_name;

/* ��̾ʸ�����첻�ơ��֥� */
int Vowel_Table[0x60];

/* Forward declarations of functions in this file. */

void decode_command ();
void sort_in_core ();
void sort_offline ();
u_short **parsefile ();
u_short *find_field ();
u_short *find_pos ();
long find_value ();
u_short *find_braced_pos (u_short *str, int words, int chars, int ignore_blanks);
u_short *find_braced_end ();
void writelines (u_short **linearray, int nlines, FILE *ostream);
int compare_field ();
int compare_full ();
long readline ();
int merge_files ();
int merge_direct ();
void pfatal_with_name ();
void fatal ();
void error ();
void *xmalloc (), *xrealloc ();
char *concat ();
char *maketempname ();
void flush_tempfiles ();
char *tempcopy ();
static	void	memory_error();
static int wstrncmp(const u_short *s1, const u_short *s2, size_t n);
static	int wfwrite(u_short *ptr, size_t  size,  size_t  nmemb,  FILE*stream);
static int wtoc(char *cptr, u_short const *ptr, int len);
static int  Vowel_of(int c);
static void init_Vowel_Table(void);

#define	EUC_BYTE(c) (0x00A1 <= c && c <= 0x00FE)
#define G1_CHAR(c) (c & 0xFF00)

#define MAX_IN_CORE_SORT 500000

void
main (argc, argv)
     int argc;
     char **argv;
{
  int i;

  tempcount = 0;
  last_deleted_tempcount = 0;

  program_name = strrchr (argv[0], '/');
  if (program_name != (char *)NULL)
    program_name++;
  else
    program_name = argv[0];

  /* Describe the kind of sorting to do. */
  /* The first keyfield uses the first braced field and folds case. */
  keyfields[0].braced = 1;
  keyfields[0].fold_case = 1;
  keyfields[0].endwords = -1;
  keyfields[0].endchars = -1;

  /* The second keyfield uses the second braced field, numerically. */
  keyfields[1].braced = 1;
  keyfields[1].numeric = 1;
  keyfields[1].startwords = 1;
  keyfields[1].endwords = -1;
  keyfields[1].endchars = -1;

  /* The third keyfield (which is ignored while discarding duplicates)
     compares the whole line. */
  keyfields[2].endwords = -1;
  keyfields[2].endchars = -1;

  decode_command (argc, argv);

  tempbase = mktemp (concat ("txiXXXXXX", "", ""));

  /* Process input files completely, one by one.  */

  for (i = 0; i < num_infiles; i++)
    {
      int desc;
      long ptr;
      char *outfile;

      desc = open (infiles[i], O_RDONLY, 0);
      if (desc < 0)
	pfatal_with_name (infiles[i]);
      lseek (desc, (off_t) 0, SEEK_END);
      ptr = (long) lseek (desc, (off_t) 0, SEEK_CUR);

      close (desc);

      outfile = outfiles[i];
      if (!outfile)
	{
	  outfile = concat (infiles[i], "s", "");
	}

      if (ptr < MAX_IN_CORE_SORT)
	/* Sort a small amount of data. */
	sort_in_core (infiles[i], ptr, outfile);
      else
	sort_offline (infiles[i], ptr, outfile);
    }

  flush_tempfiles (tempcount);
  exit (TI_NO_ERROR);
}

typedef struct
{
  char *long_name;
  char *short_name;
  int *variable_ref;
  int variable_value;
  char *arg_name;
  char *doc_string;
} TEXINDEX_OPTION;

TEXINDEX_OPTION texindex_options[] = {
  { "--keep", "-k", &keep_tempfiles, 1, (char *)NULL,
      "Keep temporary files around after processing" },
  { "--no-keep", 0, &keep_tempfiles, 0, (char *)NULL,
      "Do not keep temporary files around after processing (default)" },
  { "--output", "-o", (int *)NULL, 0, "FILE",
      "Send output to FILE" },
  { "--version", (char *)NULL, (int *)NULL, 0, (char *)NULL,
      "Show version information" },
  { "--help", "-h", (int *)NULL, 0, (char *)NULL, "Produce this listing" },
  { (char *)NULL, (char *)NULL, (int *)NULL, 0, (char *)NULL }
};

void
usage (result_value)
     int result_value;
{
  register int i;

  fprintf (stderr, "Usage: %s [OPTIONS] FILE...\n", program_name);
  fprintf (stderr, "  Generate a permuted index for the TeX files given.\n");
  fprintf (stderr, "  Usually FILE... is `foo.??' for the source file `foo.tex'.\n");
  fprintf (stderr, "  The OPTIONS are:\n");

  for (i = 0; texindex_options[i].long_name; i++)
    {
      fprintf (stderr, "    %s %s",
	       texindex_options[i].long_name,
	       texindex_options[i].arg_name ?
	       texindex_options[i].arg_name : "");

      if (texindex_options[i].short_name)
	fprintf (stderr, " \n    or %s %s",
		 texindex_options[i].short_name,
		 texindex_options[i].arg_name ?
		 texindex_options[i].arg_name : "");
      fprintf (stderr, "\t%s\n", texindex_options[i].doc_string);
    }
  exit (result_value);
}

/* Decode the command line arguments to set the parameter variables
   and set up the vector of keyfields and the vector of input files. */

void
decode_command (argc, argv)
     int argc;
     char **argv;
{
  int arg_index = 1;
  char **ip;
  char **op;

  /* Store default values into parameter variables. */

  tempdir = getenv ("TMPDIR");
#ifdef VMS
  if (tempdir == NULL)
    tempdir = "sys$scratch:";
#else
  if (tempdir == NULL)
    tempdir = "/tmp/";
  else
    tempdir = concat (tempdir, "/", "");
#endif

  keep_tempfiles = 0;

  /* Allocate ARGC input files, which must be enough.  */

  infiles = (char **) xmalloc (argc * sizeof (char *));
  outfiles = (char **) xmalloc (argc * sizeof (char *));
  ip = infiles;
  op = outfiles;

  while (arg_index < argc)
    {
      char *arg = argv[arg_index++];

      if (*arg == '-')
	{
	  if (strcmp (arg, "--version") == 0)
	    {
	      fprintf (stderr, "%s", TEXINDEX_VERSION_STRING);
#ifdef TEXINDEX_MODIFICATION_STRING
	      fprintf (stderr, " %s", TEXINDEX_MODIFICATION_STRING);
#endif
	      fprintf (stderr, "\n");
	      exit (0);
	    }
	  else if ((strcmp (arg, "--keep") == 0) ||
		   (strcmp (arg, "-k") == 0))
	    {
	      keep_tempfiles = 1;
	    }
	  else if ((strcmp (arg, "--help") == 0) ||
		   (strcmp (arg, "-h") == 0))
	    {
	      usage (0);
	    }
	  else if ((strcmp (arg, "--output") == 0) ||
		   (strcmp (arg, "-o") == 0))
	    {
	      if (argv[arg_index] != (char *)NULL)
		{
		  arg_index++;
		  if (op > outfiles)
		    *(op - 1) = argv[arg_index];
		}
	      else
		usage (1);
	    }
	  else
	    usage (1);
	}
      else
	{
	  *ip++ = arg;
	  *op++ = (char *)NULL;
	}
    }

  /* Record number of keyfields and terminate list of filenames. */
  num_infiles = ip - infiles;
  *ip = (char *)NULL;
  if (num_infiles == 0)
    usage (1);
}

/* Return a name for a temporary file. */

char *
maketempname (count)
     int count;
{
  char tempsuffix[10];
  sprintf (tempsuffix, "%d", count);
  return concat (tempdir, tempbase, tempsuffix);
}

/* Delete all temporary files up to TO_COUNT. */

void
flush_tempfiles (to_count)
     int to_count;
{
  if (keep_tempfiles)
    return;
  while (last_deleted_tempcount < to_count)
    unlink (maketempname (++last_deleted_tempcount));
}

/* Copy the input file open on IDESC into a temporary file
   and return the temporary file name. */

#define BUFSIZE 1024

char *
tempcopy (idesc)
     int idesc;
{
  char *outfile = maketempname (++tempcount);
  int odesc;
  char buffer[BUFSIZE];

  odesc = open (outfile, O_WRONLY | O_CREAT, 0666);

  if (odesc < 0)
    pfatal_with_name (outfile);

  while (1)
    {
      int nread = read (idesc, buffer, BUFSIZE);
      write (odesc, buffer, nread);
      if (!nread)
	break;
    }

  close (odesc);

  return outfile;
}

/* Compare LINE1 and LINE2 according to the specified set of keyfields. */

int
compare_full (line1, line2)
     u_short **line1, **line2;
{
  int i;

  /* Compare using the first keyfield;
     if that does not distinguish the lines, try the second keyfield;
     and so on. */

  for (i = 0; i < num_keyfields; i++)
    {
      long length1, length2;
      u_short *start1 = find_field (&keyfields[i], *line1, &length1);
      u_short *start2 = find_field (&keyfields[i], *line2, &length2);
      int tem = compare_field (&keyfields[i], start1, length1, *line1 - text_base,
			       start2, length2, *line2 - text_base);
      if (tem)
	{
	  if (keyfields[i].reverse)
	    return -tem;
	  return tem;
	}
    }

  return 0;			/* Lines match exactly. */
}

/* Compare LINE1 and LINE2, described by structures
   in which the first keyfield is identified in advance.
   For positional sorting, assumes that the order of the lines in core
   reflects their nominal order.  */

int
compare_prepared (line1, line2)
     struct lineinfo *line1, *line2;
{
  int i;
  int tem;
  u_short *text1, *text2;

  /* Compare using the first keyfield, which has been found for us already. */
  if (keyfields->positional)
    {
      if (line1->text - text_base > line2->text - text_base)
	tem = 1;
      else
	tem = -1;
    }
  else if (keyfields->numeric)
    tem = line1->key.number - line2->key.number;
  else
    tem = compare_field (keyfields, line1->key.text, line1->keylen, 0,
			 line2->key.text, line2->keylen, 0);
  if (tem)
    {
      if (keyfields->reverse)
	return -tem;
      return tem;
    }

  text1 = line1->text;
  text2 = line2->text;

  /* Compare using the second keyfield;
     if that does not distinguish the lines, try the third keyfield;
     and so on. */

  for (i = 1; i < num_keyfields; i++)
    {
      long length1, length2;
      u_short *start1 = find_field (&keyfields[i], text1, &length1);
      u_short *start2 = find_field (&keyfields[i], text2, &length2);
      int tem = compare_field (&keyfields[i], start1, length1, text1 - text_base,
			       start2, length2, text2 - text_base);
      if (tem)
	{
	  if (keyfields[i].reverse)
	    return -tem;
	  return tem;
	}
    }

  return 0;			/* Lines match exactly. */
}

/* Like compare_full but more general.
   You can pass any strings, and you can say how many keyfields to use.
   POS1 and POS2 should indicate the nominal positional ordering of
   the two lines in the input.  */

int
compare_general (str1, str2, pos1, pos2, use_keyfields)
     u_short *str1, *str2;
     long pos1, pos2;
     int use_keyfields;
{
  int i;

  /* Compare using the first keyfield;
     if that does not distinguish the lines, try the second keyfield;
     and so on. */

  for (i = 0; i < use_keyfields; i++)
    {
      long length1, length2;
      u_short *start1 = find_field (&keyfields[i], str1, &length1);
      u_short *start2 = find_field (&keyfields[i], str2, &length2);
      int tem = compare_field (&keyfields[i], start1, length1, pos1,
			       start2, length2, pos2);
      if (tem)
	{
	  if (keyfields[i].reverse)
	    return -tem;
	  return tem;
	}
    }

  return 0;			/* Lines match exactly. */
}

/* Find the start and length of a field in STR according to KEYFIELD.
   A pointer to the starting character is returned, and the length
   is stored into the int that LENGTHPTR points to.  */

u_short *
find_field (keyfield, str, lengthptr)
     struct keyfield *keyfield;
     u_short *str;
     long *lengthptr;
{
  u_short *start;
  u_short *end;
  u_short *(*fun) ();

  if (keyfield->braced)
    fun = find_braced_pos;
  else
    fun = find_pos;

  start = (*fun) (str, keyfield->startwords, keyfield->startchars,
		  keyfield->ignore_blanks);
  if (keyfield->endwords < 0)
    {
      if (keyfield->braced)
	end = find_braced_end (start);
      else
	{
	  end = start;
	  while (*end && *end != '\n')
	    end++;
	}
    }
  else
    {
      end = (*fun) (str, keyfield->endwords, keyfield->endchars, 0);
      if (end - str < start - str)
	end = start;
    }
  *lengthptr = end - start;
  return start;
}

/* Return a pointer to a specified place within STR,
   skipping (from the beginning) WORDS words and then CHARS chars.
   If IGNORE_BLANKS is nonzero, we skip all blanks
   after finding the specified word.  */

u_short *
find_pos (str, words, chars, ignore_blanks)
     u_short *str;
     int words, chars;
     int ignore_blanks;
{
  int i;
  u_short *p = str;

  for (i = 0; i < words; i++)
    {
      u_short c;
      /* Find next bunch of nonblanks and skip them. */
      while ((c = *p) == ' ' || c == '\t')
	p++;
      while ((c = *p) && c != '\n' && !(c == ' ' || c == '\t'))
	p++;
      if (!*p || *p == '\n')
	return p;
    }

  while (*p == ' ' || *p == '\t')
    p++;

  for (i = 0; i < chars; i++)
    {
      if (!*p || *p == '\n')
	break;
      p++;
    }
  return p;
}

/* Like find_pos but assumes that each field is surrounded by braces
   and that braces within fields are balanced. */

u_short *
find_braced_pos (str, words, chars, ignore_blanks)
     u_short *str;
     int words, chars;
     int ignore_blanks;
{
  int i;
  int bracelevel;
  u_short *p = str;
  u_short c;

  for (i = 0; i < words; i++)
    {
      bracelevel = 1;
      while ((c = *p++) != '{' && c != '\n' && c)
	/* Do nothing. */ ;
      if (c != '{')
	return p - 1;
      while (bracelevel)
	{
	  c = *p++;
	  if (c == '{')
	    bracelevel++;
	  if (c == '}')
	    bracelevel--;
	  if (c == 0 || c == '\n')
	    return p - 1;
	}
    }

  while ((c = *p++) != '{' && c != '\n' && c)
    /* Do nothing. */ ;

  if (c != '{')
    return p - 1;

  if (ignore_blanks)
    while ((c = *p) == ' ' || c == '\t')
      p++;

  for (i = 0; i < chars; i++)
    {
      if (!*p || *p == '\n')
	break;
      p++;
    }
  return p;
}

/* Find the end of the balanced-brace field which starts at STR.
   The position returned is just before the closing brace. */

u_short *
find_braced_end (str)
     u_short *str;
{
  int bracelevel;
  u_short *p = str;
  u_short c;

  bracelevel = 1;
  while (bracelevel)
    {
      c = *p++;
      if (c == '{')
	bracelevel++;
      if (c == '}')
	bracelevel--;
      if (c == 0 || c == '\n')
	return p - 1;
    }
  return p - 1;
}

long
find_value (start, length)
    u_short *start;
    long length;
    {
    long	val;

    while (length != 0L)
	{
	if (isdigit (*start))
	    break;
	length--;
	start++;
	}

    val = 0;
    while(isdigit(*start) && length > 0L)
	{
	val = val*10 + *start;
	start++;
	length--;
	}

    return val;
    }

/* Vector used to translate characters for comparison.
   This is how we make all alphanumerics follow all else,
   and ignore case in the first sorting.  */
int char_order[0x8000];

void
init_char_order (void)
    {
    int i;
  
    for (i = 1; i < 0x8000; i++)
	char_order[i] = i;

    for (i = '0'; i <= '9'; i++)
	char_order[i] += 512;

    for (i = 'a'; i <= 'z'; i++)
	{
	char_order[i] = 512 + i;
	char_order[i + 'A' - 'a'] = 512 + i;
	}

    /* �ʲ��Υ���ܥ��̵�� (i.e. char_order[XXX] = 0) */
    for (i = 0x2121; i <= 0x2132; i++) /* ���Ѷ��� �� �� */
        char_order[i] = 0;
    for (i = 0x213D; i <= 0x215B; i++) /* �� �� �� */
        char_order[i] = 0;

    /*	¥����ٹ����������Ⱦ�������Ҳ�̾/ʿ��̾�ν���	*/
    /*	    1.  ʿ��̾���Ҳ�̾��Ʊ���˰���  */
    /*		��/��(2421/2521)����/��(2473/2573)  */
    for (i = 0x2421; i < 0x2474; i++)
	char_order[i + 0x100] = i;

    /*	    2.	¥����ٹ����������Ⱦ��������	*/
    /* ������ ��	������	*/
    for (i = 0x2421; i < 0x242B; i += 2)
	char_order[i] = char_order[i + 0x100]    = i+1;
    /* ������ ��	������	*/
    for (i = 0x242C; i < 0x2443; i += 2)
	char_order[i] = char_order[i + 0x100]    = i-1;
    /* �� */
    char_order[0x2443]	= char_order[0x2543]	= 0x2443+1;
    /* �š��� ��	�ġ��� */
    for (i = 0x2445; i < 0x244A; i += 2)
	char_order[i] = char_order[i + 0x100]    = i-1;
    /* �Фѡ��ܤ�	��  �ϡ���  */
    for (i = 0x2450; i < 0x245E; i += 3)
	{
	char_order[i]	= char_order[i + 0x100]	    = i-1;
	char_order[i+1]	= char_order[i+1 + 0x100]   = i-1;
	}
    /* ����� ��	�����	*/
    for (i = 0x2463; i < 0x2469; i += 2)
	char_order[i] = char_order[i + 0x100] = i+1;
    /* ��	��  ��	*/
    char_order[0x246E]	= char_order[0x256E]	= 0x246E +1;
  
    /*	    3.	�ü��Ҳ�̾�ΰ���    */
    char_order[0x2574]	= 0x2426;	/* ��	��  �� */
    char_order[0x2575]	= 0x242B;	/* ��	��  �� */
    char_order[0x2576]	= 0x2431;	/* ��	��  �� */

    /*	JISX0208�Υ���ե��٥åȡ�����  */
    for (i = 0x2330; i <= 0x2339; i++) /* ������ -> 0-9 */
        char_order[i] = i - 0x2330 + 512;
    for (i = 0x2341; i <= 0x235A; i++) /* �����ڡ������ -> A-Z */
        char_order[i] = char_order[i + (0x2361 - 0x2341)]
	  = 512 + (i -  0x2341) + 'a';

    /* Ĺ����������ӥ롼����ǹԤʤ� */

    init_Vowel_Table();
    }

/* Compare two fields (each specified as a start pointer and a character count)
   according to KEYFIELD.
   The sign of the value reports the relation between the fields. */

int
compare_field (keyfield, start1, length1, pos1, start2, length2, pos2)
     struct keyfield *keyfield;
     u_short *start1;
     long length1;
     long pos1;
     u_short *start2;
     long length2;
     long pos2;
{
  if (keyfields->positional)
    {
      if (pos1 > pos2)
	return 1;
      else
	return -1;
    }
  if (keyfield->numeric)
    {
      long value = find_value (start1, length1) - find_value (start2, length2);
      if (value > 0)
	return 1;
      if (value < 0)
	return -1;
      return 0;
    }
  else
    {
    /* lexical �äƤ�? */
      u_short *p1 = start1;
      u_short *p2 = start2;
      u_short *e1 = start1 + length1;
      u_short *e2 = start2 + length2;
      u_short	lastchar1 = 0, lastchar2 = 0;
      int   c1, c2;

      do
	  {
	  int	co1, co2;

	  /* char_order[X] �� 0 �Τ�Ρʥ���ܥ�ˤ�̵�뤹�� */
	  c1 = c2 = 0;
	  while (p1 < e1)
	    {
	      if (char_order[c1 = *p1++]) break;
	      c1 = 0;
	    }
	  while (p2 < e2)
	    {
	      if (char_order[c2 = *p2++]) break;
	      c2 = 0;
	    }

	  /* Ĺ������ */
	  if (c1 == 0x213C)			/* �� */
	      c1    = Vowel_of(lastchar1);
	  if (c2 == 0x213C)			/* �� */
	      c2    = Vowel_of(lastchar2);
	  co1	= char_order[c1];
	  co2   = char_order[c2];
	  
	  if (co1 != co2)
	      return co1 - co2;
	  else
	      lastchar1 = c1,	lastchar2 = c2;

	  } while(c1);

      /* Strings are equal except possibly for case.  */
      p1 = start1;
      p2 = start2;
      while (1)
	{
	  int c1, c2;

	  if (p1 == e1)
	    c1 = 0;
	  else
	    c1 = *p1++;
	  if (p2 == e2)
	    c2 = 0;
	  else
	    c2 = *p2++;

	  if (c1 != c2)
	    /* Reverse sign here so upper case comes out last.  */
	    return c2 - c1;
	  if (!c1)
	    break;
	}

      return 0;
    }
}

/* A `struct linebuffer' is a structure which holds a line of text.
   `readline' reads a line from a stream into a linebuffer
   and works regardless of the length of the line.  */

struct linebuffer
{
  long size;
  u_short *buffer;
};

/* Initialize LINEBUFFER for use. */

void
initbuffer (linebuffer)
     struct linebuffer *linebuffer;
{
  linebuffer->size = 200;
  linebuffer->buffer = (u_short *) xmalloc (200*sizeof(u_short));
}

/* Read a line of text from STREAM into LINEBUFFER.
   Return the length of the line.  */

long
readline (linebuffer, stream)
     struct linebuffer *linebuffer;
     FILE *stream;
{
  u_short *buffer = linebuffer->buffer;
  u_short *p = linebuffer->buffer;
  u_short *end = p + linebuffer->size;

  while (1)
    {
      int c = getc (stream);
      if (p == end)
	{
	  buffer =
	      (u_short *) xrealloc (buffer,
				    (linebuffer->size *= 2)*sizeof(u_short));
	  p += buffer - linebuffer->buffer;
	  end += buffer - linebuffer->buffer;
	  linebuffer->buffer = buffer;
	}
      if (c < 0 || c == '\n')
	{
	  *p = 0;
	  break;
	}
      *p++ = c;
    }

  return p - buffer;
}

/* Sort an input file too big to sort in core.  */

void
sort_offline (infile, nfiles, total, outfile)
     char *infile;
     int nfiles;
     long total;
     char *outfile;
{
  /* More than enough. */
  int ntemps = 2 * (total + MAX_IN_CORE_SORT - 1) / MAX_IN_CORE_SORT;
  char **tempfiles = (char **) xmalloc (ntemps * sizeof (char *));
  FILE *istream = fopen (infile, "r");
  int i;
  struct linebuffer lb;
  long linelength;
  int failure = 0;

  initbuffer (&lb);

  /* Read in one line of input data.  */

  linelength = readline (&lb, istream);

  if (lb.buffer[0] != '\\' && lb.buffer[0] != '@')
    {
      error ("%s: not a texinfo index file", infile);
      return;
    }

  /* Split up the input into `ntemps' temporary files, or maybe fewer,
     and put the new files' names into `tempfiles' */

  for (i = 0; i < ntemps; i++)
    {
      char *outname = maketempname (++tempcount);
      FILE *ostream = fopen (outname, "w");
      long tempsize = 0;

      if (!ostream)
	pfatal_with_name (outname);
      tempfiles[i] = outname;

      /* Copy lines into this temp file as long as it does not make file
	 "too big" or until there are no more lines.  */

      while (tempsize + linelength + 1 <= MAX_IN_CORE_SORT)
	{
	  tempsize += linelength + 1;
	      {
	      u_short	*tp;

	      for (tp = lb.buffer; *tp; tp++)
		  putc(*tp, ostream);
	      }
	  putc ('\n', ostream);

	  /* Read another line of input data.  */

	  linelength = readline (&lb, istream);
	  if (!linelength && feof (istream))
	    break;

	  if (lb.buffer[0] != '\\' && lb.buffer[0] != '@')
	    {
	      error ("%s: not a texinfo index file", infile);
	      failure = 1;
	      goto fail;
	    }
	}
      fclose (ostream);
      if (feof (istream))
	break;
    }

  free (lb.buffer);

fail:
  /* Record number of temp files we actually needed.  */

  ntemps = i;

  /* Sort each tempfile into another tempfile.
    Delete the first set of tempfiles and put the names of the second
    into `tempfiles'. */

  for (i = 0; i < ntemps; i++)
    {
      char *newtemp = maketempname (++tempcount);
      sort_in_core (&tempfiles[i], MAX_IN_CORE_SORT, newtemp);
      if (!keep_tempfiles)
	unlink (tempfiles[i]);
      tempfiles[i] = newtemp;
    }

  if (failure)
    return;

  /* Merge the tempfiles together and indexify. */

  merge_files (tempfiles, ntemps, outfile);
}

/* Sort INFILE, whose size is TOTAL,
   assuming that is small enough to be done in-core,
   then indexify it and send the output to OUTFILE (or to stdout).  */

void
sort_in_core (infile, total, outfile)
     char *infile;
     long total;
     char *outfile;
{
  u_short **nextline;
  u_short *data = (u_short *) xmalloc ((total + 1)*sizeof(u_short));
  u_short *file_data;
  long file_size;
  int i;
  FILE *ostream = stdout;
  struct lineinfo *lineinfo;
  FILE *ifp;
  int	c;

  /* Read the contents of the file into the moby array `data'. */
  ifp	= fopen(infile, "r");
  if (!ifp)
    fatal ("failure reopening %s", infile);
  memset(data, 0, (total + 1)*sizeof(u_short));	/* zero clear */
  for (i = 0, c = fgetc(ifp); c != EOF; c = fgetc(ifp), i++)
      {
      if (EUC_BYTE(c))
	  {
	  data[i] = (c & 0x007F) << 8;
	  c = fgetc(ifp) & 0x007F;
	  }
      data[i]	+= c;
      }
  file_size = i;
  file_data = data;
  data[file_size] = 0;

  fclose(ifp);

  if (file_size > 0 && data[0] != '\\' && data[0] != '@')
    {
      error ("%s: not a texinfo index file", infile);
      return;
    }

  init_char_order ();

  /* Sort routines want to know this address. */

  text_base = data;

  /* Create the array of pointers to lines, with a default size
     frequently enough.  */

  nlines = total / 50;
  if (!nlines)
    nlines = 2;
  linearray = (u_short **) xmalloc (nlines * sizeof (u_short *));

  /* `nextline' points to the next free slot in this array.
     `nlines' is the allocated size.  */

  nextline = linearray;

  /* Parse the input file's data, and make entries for the lines.  */

  nextline = parsefile (nextline, file_data, file_size);

  if (nextline == 0)
    {
      error ("%s: not a texinfo index file", infile);
      return;
    }

  /* Sort the lines. */

  /* If we have enough space, find the first keyfield of each line in advance.
     Make a `struct lineinfo' for each line, which records the keyfield
     as well as the line, and sort them.  */

  lineinfo = (struct lineinfo *) malloc ((nextline - linearray) * sizeof (struct lineinfo));

  if (lineinfo)
    {
      struct lineinfo *lp;
      u_short **p;

      for (lp = lineinfo, p = linearray; p != nextline; lp++, p++)
	{
	  lp->text = *p;
	  lp->key.text = find_field (keyfields, *p, &lp->keylen);
	  if (keyfields->numeric)
	    lp->key.number = find_value (lp->key.text, lp->keylen);
	}

      qsort (lineinfo, nextline - linearray, sizeof (struct lineinfo), compare_prepared);

      for (lp = lineinfo, p = linearray; p != nextline; lp++, p++)
	  *p = lp->text;

      free (lineinfo);
    }
  else
    qsort (linearray, nextline - linearray, sizeof (u_short *), compare_full);

  /* Open the output file. */

  if (outfile)
    {
      ostream = fopen (outfile, "w");
      if (!ostream)
	pfatal_with_name (outfile);
    }

  writelines (linearray, nextline - linearray, ostream);
  if (outfile)
    fclose (ostream);

  free (linearray);
  free (data);
}

/* Parse an input string in core into lines.
   DATA is the input string, and SIZE is its length.
   Data goes in LINEARRAY starting at NEXTLINE.
   The value returned is the first entry in LINEARRAY still unused.
   Value 0 means input file contents are invalid.  */

u_short **
parsefile (nextline, data, size)
     u_short **nextline;
     u_short *data;
     long size;
{
  u_short *p, *end;
  u_short **line = nextline;

  p = data;
  end = p + size;
  *end = 0;

  while (p != end)
    {
      if (p[0] != '\\' && p[0] != '@')
	return 0;

      *line = p;
      while (*p && *p != '\n')
	p++;
      if (p != end)
	p++;

      line++;
      if (line == linearray + nlines)
	{
	  u_short **old = linearray;
	  linearray =
	      (u_short **) xrealloc (linearray,
				     sizeof (u_short *) * (nlines *= 4));
	  line += linearray - old;
	}
    }

  return line;
}

/* Indexification is a filter applied to the sorted lines
   as they are being written to the output file.
   Multiple entries for the same name, with different page numbers,
   get combined into a single entry with multiple page numbers.
   The first braced field, which is used for sorting, is discarded.
   However, its first character is examined, folded to lower case,
   and if it is different from that in the previous line fed to us
   a \initial line is written with one argument, the new initial.

   If an entry has four braced fields, then the second and third
   constitute primary and secondary names.
   In this case, each change of primary name
   generates a \primary line which contains only the primary name,
   and in between these are \secondary lines which contain
   just a secondary name and page numbers. */

/* The last primary name we wrote a \primary entry for.
   If only one level of indexing is being done, this is the last name seen. */
u_short *lastprimary;
/* Length of storage allocated for lastprimary. */
int lastprimarylength;

/* Similar, for the secondary name. */
u_short *lastsecondary;
int lastsecondarylength;

/* Zero if we are not in the middle of writing an entry.
   One if we have written the beginning of an entry but have not
   yet written any page numbers into it.
   Greater than one if we have written the beginning of an entry
   plus at least one page number. */
int pending;

/* The initial (for sorting purposes) of the last primary entry written.
   When this changes, a \initial {c} line is written */

u_short *lastinitial;

int lastinitiallength;

/* When we need a string of length 1 for the value of lastinitial,
   store it here.  */

u_short lastinitial1[2];

/* Initialize static storage for writing an index. */

void
init_index ()
{
  pending = 0;
  lastinitial = lastinitial1;
  lastinitial1[0] = 0;
  lastinitial1[1] = 0;
  lastinitiallength = 0;
  lastprimarylength = 100;
  lastprimary = (u_short *) xmalloc ((lastprimarylength + 1)*sizeof(u_short));
  memset (lastprimary, '\0', (lastprimarylength + 1)*sizeof(u_short));
  lastsecondarylength = 100;
  lastsecondary =
      (u_short *) xmalloc ((lastsecondarylength + 1)*sizeof(u_short));
  memset (lastsecondary, '\0', (lastsecondarylength + 1)*sizeof(u_short));
}

/* Indexify.  Merge entries for the same name,
   insert headers for each initial character, etc.  */

void
indexify (u_short *line, FILE *ostream)
{
  u_short *primary, *secondary, *pagenumber;
  int primarylength, secondarylength = 0, pagelength;
  int nosecondary;
  int initiallength;
  u_short *initial;
  u_short initial1[2];
  register u_short *p;

  /* First, analyze the parts of the entry fed to us this time. */

  p = find_braced_pos (line, 0, 0, 0);
  if (*p == '{')
    {
      initial = p;
      /* Get length of inner pair of braces starting at `p',
	 including that inner pair of braces.  */
      initiallength = find_braced_end (p + 1) + 1 - p;
    }
  else
    {
      initial = initial1;
      initial1[0] = *p & 0x7F7F;
      initial1[1] = 0;
      initiallength = 1;

      if (initial1[0] >= 'a' && initial1[0] <= 'z')
	initial1[0] -= ' ';
      else if (initial1[0] > 0x2000)
	initial1[0] = char_order[initial1[0]];
    }

  pagenumber = find_braced_pos (line, 1, 0, 0);
  pagelength = find_braced_end (pagenumber) - pagenumber;
  if (pagelength == 0)
    abort ();

  primary = find_braced_pos (line, 2, 0, 0);
  primarylength = find_braced_end (primary) - primary;

  secondary = find_braced_pos (line, 3, 0, 0);
  nosecondary = !*secondary;
  if (!nosecondary)
    secondarylength = find_braced_end (secondary) - secondary;

  /* If the primary is different from before, make a new primary entry. */
  if (wstrncmp (primary, lastprimary, primarylength))
    {
      /* Close off current secondary entry first, if one is open. */
      if (pending)
	{
	  fputs ("}\n", ostream);
	  pending = 0;
	}

      /* If this primary has a different initial, include an entry for
	 the initial. */
      if (initiallength != lastinitiallength ||
	  (*initial && wstrncmp (initial, lastinitial, initiallength)))
	{
	  fprintf (ostream, "\\initial {");
	  wfwrite (initial, initiallength, 1, ostream);
	  fprintf (ostream, "}\n", initial);
	  if (initial == initial1)
	    {
	      lastinitial = lastinitial1;
	      *lastinitial1 = *initial1;
	    }
	  else
	    {
	      lastinitial = initial;
	    }
	  lastinitiallength = initiallength;
	}

      /* Make the entry for the primary.  */
      if (nosecondary)
	fputs ("\\entry {", ostream);
      else
	fputs ("\\primary {", ostream);
      wfwrite (primary, primarylength, 1, ostream);
      if (nosecondary)
	{
	  fputs ("}{", ostream);
	  pending = 1;
	}
      else
	fputs ("}\n", ostream);

      /* Record name of most recent primary. */
      if (lastprimarylength < primarylength)
	{
	  lastprimarylength = primarylength + 100;
	  lastprimary =
	      (u_short *) xrealloc (lastprimary,
				    (1 + lastprimarylength)*sizeof(u_short));
	}
      memcpy (lastprimary, primary, primarylength*sizeof(u_short));
      lastprimary[primarylength] = 0;

      /* There is no current secondary within this primary, now. */
      lastsecondary[0] = 0;
    }

  /* Should not have an entry with no subtopic following one with a subtopic. */

  if (nosecondary && *lastsecondary)
      {
      char  tline[2001];
      int   len;
      
      len   = wtoc(tline, line, 2000);		/* 2000 is much enough ? */
      tline[len]   = '\0';
      error ("entry %s follows an entry with a secondary name", tline);
      }
  
  /* Start a new secondary entry if necessary. */
  if (!nosecondary && wstrncmp (secondary, lastsecondary, secondarylength))
    {
      if (pending)
	{
	  fputs ("}\n", ostream);
	  pending = 0;
	}

      /* Write the entry for the secondary.  */
      fputs ("\\secondary {", ostream);
      wfwrite (secondary, secondarylength, 1, ostream);
      fputs ("}{", ostream);
      pending = 1;

      /* Record name of most recent secondary. */
      if (lastsecondarylength < secondarylength)
	{
	  lastsecondarylength = secondarylength + 100;
	  lastsecondary =
	      (u_short *) xrealloc ((lastsecondary,
				     1 + lastsecondarylength)*sizeof(u_short));
	}
      memcpy (lastsecondary, secondary, secondarylength * sizeof(u_short));
      lastsecondary[secondarylength] = 0;
    }

  /* Here to add one more page number to the current entry. */
  if (pending++ != 1)
    fputs (", ", ostream);	/* Punctuate first, if this is not the first. */
  wfwrite (pagenumber, pagelength, 1, ostream);
}

/* Close out any unfinished output entry. */

void
finish_index (ostream)
     FILE *ostream;
{
  if (pending)
    fputs ("}\n", ostream);
  free (lastprimary);
  free (lastsecondary);
}

/* Copy the lines in the sorted order.
   Each line is copied out of the input file it was found in. */

void
writelines (linearray, nlines, ostream)
     u_short **linearray;
     int nlines;
     FILE *ostream;
{
  u_short **stop_line = linearray + nlines;
  u_short **next_line;

  init_index ();

  /* Output the text of the lines, and free the buffer space. */

  for (next_line = linearray; next_line != stop_line; next_line++)
    {
      /* If -u was specified, output the line only if distinct from previous one.  */
      if (next_line == linearray
      /* Compare previous line with this one, using only the
         explicitly specd keyfields. */
	  || compare_general (*(next_line - 1), *next_line, 0L, 0L, num_keyfields - 1))
	{
	  u_short *p = *next_line;
	  u_short c;

	  while ((c = *p++) && c != '\n')
	    /* Do nothing. */ ;
	  *(p - 1) = 0;
	  indexify (*next_line, ostream);
	}
    }

  finish_index (ostream);
}

/* Assume (and optionally verify) that each input file is sorted;
   merge them and output the result.
   Returns nonzero if any input file fails to be sorted.

   This is the high-level interface that can handle an unlimited
   number of files.  */

#define MAX_DIRECT_MERGE 10

int
merge_files (infiles, nfiles, outfile)
     char **infiles;
     int nfiles;
     char *outfile;
{
  char **tempfiles;
  int ntemps;
  int i;
  int value = 0;
  int start_tempcount = tempcount;

  if (nfiles <= MAX_DIRECT_MERGE)
    return merge_direct (infiles, nfiles, outfile);

  /* Merge groups of MAX_DIRECT_MERGE input files at a time,
     making a temporary file to hold each group's result.  */

  ntemps = (nfiles + MAX_DIRECT_MERGE - 1) / MAX_DIRECT_MERGE;
  tempfiles = (char **) xmalloc (ntemps * sizeof (char *));
  for (i = 0; i < ntemps; i++)
    {
      int nf = MAX_DIRECT_MERGE;
      if (i + 1 == ntemps)
	nf = nfiles - i * MAX_DIRECT_MERGE;
      tempfiles[i] = maketempname (++tempcount);
      value |= merge_direct (&infiles[i * MAX_DIRECT_MERGE], nf, tempfiles[i]);
    }

  /* All temporary files that existed before are no longer needed
     since their contents have been merged into our new tempfiles.
     So delete them.  */
  flush_tempfiles (start_tempcount);

  /* Now merge the temporary files we created.  */

  merge_files (tempfiles, ntemps, outfile);

  free (tempfiles);

  return value;
}

/* Assume (and optionally verify) that each input file is sorted;
   merge them and output the result.
   Returns nonzero if any input file fails to be sorted.

   This version of merging will not work if the number of
   input files gets too high.  Higher level functions
   use it only with a bounded number of input files.  */

int
merge_direct (infiles, nfiles, outfile)
     char **infiles;
     int nfiles;
     char *outfile;
{
  struct linebuffer *lb1, *lb2;
  struct linebuffer **thisline, **prevline;
  FILE **streams;
  int i;
  int nleft;
  int lossage = 0;
  int *file_lossage;
  struct linebuffer *prev_out = 0;
  FILE *ostream = stdout;

  if (outfile)
    {
      ostream = fopen (outfile, "w");
    }
  if (!ostream)
    pfatal_with_name (outfile);

  init_index ();

  if (nfiles == 0)
    {
      if (outfile)
	fclose (ostream);
      return 0;
    }

  /* For each file, make two line buffers.
     Also, for each file, there is an element of `thisline'
     which points at any time to one of the file's two buffers,
     and an element of `prevline' which points to the other buffer.
     `thisline' is supposed to point to the next available line from the file,
     while `prevline' holds the last file line used,
     which is remembered so that we can verify that the file is properly sorted. */

  /* lb1 and lb2 contain one buffer each per file. */
  lb1 = (struct linebuffer *) xmalloc (nfiles * sizeof (struct linebuffer));
  lb2 = (struct linebuffer *) xmalloc (nfiles * sizeof (struct linebuffer));

  /* thisline[i] points to the linebuffer holding the next available line in file i,
     or is zero if there are no lines left in that file.  */
  thisline = (struct linebuffer **)
    xmalloc (nfiles * sizeof (struct linebuffer *));
  /* prevline[i] points to the linebuffer holding the last used line
     from file i.  This is just for verifying that file i is properly
     sorted.  */
  prevline = (struct linebuffer **)
    xmalloc (nfiles * sizeof (struct linebuffer *));
  /* streams[i] holds the input stream for file i.  */
  streams = (FILE **) xmalloc (nfiles * sizeof (FILE *));
  /* file_lossage[i] is nonzero if we already know file i is not
     properly sorted.  */
  file_lossage = (int *) xmalloc (nfiles * sizeof (int));

  /* Allocate and initialize all that storage. */

  for (i = 0; i < nfiles; i++)
    {
      initbuffer (&lb1[i]);
      initbuffer (&lb2[i]);
      thisline[i] = &lb1[i];
      prevline[i] = &lb2[i];
      file_lossage[i] = 0;
      streams[i] = fopen (infiles[i], "r");
      if (!streams[i])
	pfatal_with_name (infiles[i]);

      readline (thisline[i], streams[i]);
    }

  /* Keep count of number of files not at eof. */
  nleft = nfiles;

  while (nleft)
    {
      struct linebuffer *best = 0;
      struct linebuffer *exch;
      int bestfile = -1;
      int i;

      /* Look at the next avail line of each file; choose the least one.  */

      for (i = 0; i < nfiles; i++)
	{
	  if (thisline[i] &&
	      (!best ||
	       0 < compare_general (best->buffer, thisline[i]->buffer,
				 (long) bestfile, (long) i, num_keyfields)))
	    {
	      best = thisline[i];
	      bestfile = i;
	    }
	}

      /* Output that line, unless it matches the previous one and we
	 don't want duplicates. */

      if (!(prev_out &&
	    !compare_general (prev_out->buffer,
			      best->buffer, 0L, 1L, num_keyfields - 1)))
	indexify (best->buffer, ostream);
      prev_out = best;

      /* Now make the line the previous of its file, and fetch a new
	 line from that file.  */

      exch = prevline[bestfile];
      prevline[bestfile] = thisline[bestfile];
      thisline[bestfile] = exch;

      while (1)
	{
	  /* If the file has no more, mark it empty. */

	  if (feof (streams[bestfile]))
	    {
	      thisline[bestfile] = 0;
	      /* Update the number of files still not empty. */
	      nleft--;
	      break;
	    }
	  readline (thisline[bestfile], streams[bestfile]);
	  if (thisline[bestfile]->buffer[0] || !feof (streams[bestfile]))
	    break;
	}
    }

  finish_index (ostream);

  /* Free all storage and close all input streams. */

  for (i = 0; i < nfiles; i++)
    {
      fclose (streams[i]);
      free (lb1[i].buffer);
      free (lb2[i].buffer);
    }
  free (file_lossage);
  free (lb1);
  free (lb2);
  free (thisline);
  free (prevline);
  free (streams);

  if (outfile)
    fclose (ostream);

  return lossage;
}

/* Print error message and exit.  */

void
fatal (format, arg)
     char *format, *arg;
{
  error (format, arg);
  exit (TI_FATAL_ERROR);
}

/* Print error message.  FORMAT is printf control string, ARG is arg for it. */
void
error (format, arg)
     char *format, *arg;
{
  printf ("%s: ", program_name);
  printf (format, arg);
  if (format[strlen (format) -1] != '\n')
    printf ("\n");
}

void
perror_with_name (name)
     char *name;
{
  char *s;

  s = strerror (errno);
  printf ("%s: ", program_name);
  printf ("%s; for file `%s'.\n", s, name);
}

void
pfatal_with_name (name)
     char *name;
{
  char *s;

  s = strerror (errno);
  printf ("%s: ", program_name);
  printf ("%s; for file `%s'.\n", s, name);
  exit (TI_FATAL_ERROR);
}

/* Return a newly-allocated string whose contents concatenate those of
   S1, S2, S3.  */

char *
concat (s1, s2, s3)
     char *s1, *s2, *s3;
{
  int len1 = strlen (s1), len2 = strlen (s2), len3 = strlen (s3);
  char *result = (char *) xmalloc (len1 + len2 + len3 + 1);

  strcpy (result, s1);
  strcpy (result + len1, s2);
  strcpy (result + len1 + len2, s3);
  *(result + len1 + len2 + len3) = 0;

  return result;
}

#if !defined (HAVE_STRERROR)
extern char *sys_errlist[];
extern int sys_nerr;

char *
strerror (num)
     int num;
{
  if (num >= sys_nerr)
    return ("");
  else
    return (sys_errlist[num]);
}
#endif /* !HAVE_STRERROR */

#if !defined (HAVE_STRCHR)
char *
strrchr (string, character)
     char *string;
     int character;
{
  register int i;

  for (i = strlen (string) - 1; i > -1; i--)
    if (string[i] == character)
      return (string + i);

  return ((char *)NULL);
}
#endif /* HAVE_STRCHR */

/* Just like malloc, but kills the program in case of fatal error. */
void *
xmalloc (nbytes)
     int nbytes;
{
  void *temp = (void *) malloc (nbytes);

  if (nbytes && temp == (void *)NULL)
    memory_error ("xmalloc", nbytes);

  return (temp);
}

/* Like realloc (), but barfs if there isn't enough memory. */
void *
xrealloc (pointer, nbytes)
     void *pointer;
     int nbytes;
{
  void *temp;

  if (!pointer)
    temp = (void *)xmalloc (nbytes);
  else
    temp = (void *)realloc (pointer, nbytes);

  if (nbytes && !temp)
    memory_error ("xrealloc", nbytes);

  return (temp);
}

void
memory_error (callers_name, bytes_wanted)
     char *callers_name;
     int bytes_wanted;
{
  char printable_string[80];

  sprintf (printable_string,
	   "Virtual memory exhausted in %s ()!  Needed %d bytes.",
	   callers_name, bytes_wanted);

  error (printable_string);
  abort();
}


static int
wstrncmp(const u_short *s1, const u_short *s2, size_t n)
    {
    int	    i;
    int	    d = 0;

    for (i=0; i<n && d == 0; i++)
	d   = s1[i] - s2[i];

    return  d;
    }


static int
wfwrite(u_short *ptr, size_t  size,  size_t  nmemb,  FILE*stream)
    {
    char    *cptr;
    int	    ret, len;

    cptr    = (char *)xmalloc(size * nmemb * sizeof(u_short)); /* Ч����! */
    len	= wtoc(cptr, ptr, size*nmemb);
    assert(len <= (size * nmemb * sizeof(u_short)));

    ret	= fwrite(cptr, len, nmemb, stream);
    free(cptr);
    return  ret;
    }
    

static int
wtoc(char *cptr, u_short const *ptr, int len)
    {
    char	*tptr;
    u_short	    c;
    int		    i;

    tptr    = cptr;
    for (i = 0; i < len; i++)
	{
	c   = ptr[i];
	if (G1_CHAR(c))
	    {
	    *tptr++ = c >> 8 | 0x0080;	/* to EUC */
	    c	&= 0x007F;
	    c	|= 0x0080;
	    }
	*tptr++	= c;
	}

    return  tptr - cptr;
    }


static int
Vowel_of(int c)
    {
    if (0x2521 <= c && c < 0x2577)		/* �Ҳ�̾�� */
	c   -= 0x100;				/* ʿ��̾�� */
    /* ��������������3������������ʿ��̾�Ϥʤ����������ɤ�����Ȳ��ꤹ�� */
    /* ���̤�����ˤʤ�ʤ�Ȧ��	*/

    if (0x2421 <= c && c < 0x2474)
	return	Vowel_Table[c - 0x2420];
    else
	return	c;				/* ʿ��̾�ʳ��Ϥ��Τޤ� */
    }


static void
init_Vowel_Table(void)
    {
    int	    i;

    /* ������ */
    for (i = 0x2421 - 0x2420; i < 0x2445 - 0x2420; i += 10)
	{
	Vowel_Table[i]	    = Vowel_Table[i+1]    = 0x2422; /* ���� */
	Vowel_Table[i+2]    = Vowel_Table[i+3]    = 0x2424; /* ���� */
	Vowel_Table[i+4]    = Vowel_Table[i+5]    = 0x2426; /* ���� */
	Vowel_Table[i+6]    = Vowel_Table[i+7]    = 0x2428; /* ���� */
	Vowel_Table[i+8]    = Vowel_Table[i+9]    = 0x242A; /* ���� */
	}
    Vowel_Table[0x243F - 0x2420]    = 0x2422;	/* �� */
    Vowel_Table[0x2440 - 0x2420]    = 0x2422;	/* �� */
    Vowel_Table[0x2441 - 0x2420]    = 0x2424;	/* �� */
    Vowel_Table[0x2442 - 0x2420]    = 0x2424;	/* �� */
    Vowel_Table[0x2443 - 0x2420]    = 0x2426;	/* �� */
    Vowel_Table[0x2444 - 0x2420]    = 0x2426;	/* �� */
    Vowel_Table[0x2445 - 0x2420]    = 0x2426;	/* �� */
    Vowel_Table[0x2446 - 0x2420]    = 0x2428;	/* �� */
    Vowel_Table[0x2447 - 0x2420]    = 0x2428;	/* �� */
    Vowel_Table[0x2448 - 0x2420]    = 0x242A;	/* �� */
    Vowel_Table[0x2449 - 0x2420]    = 0x242A;	/* �� */
    Vowel_Table[0x244A - 0x2420]    = 0x2422;	/* �� */
    Vowel_Table[0x244B - 0x2420]    = 0x2424;	/* �� */
    Vowel_Table[0x244C - 0x2420]    = 0x2426;	/* �� */
    Vowel_Table[0x244D - 0x2420]    = 0x2428;	/* �� */
    Vowel_Table[0x244E - 0x2420]    = 0x242A;	/* �� */
    /* �ϡ��� */
    for (i = 0x244F - 0x2420; i < 0x245E - 0x2420; i += 15)
	{
	Vowel_Table[i]	    = Vowel_Table[i+1]	= Vowel_Table[i+2]  = 0x2422;
	Vowel_Table[i+3]    = Vowel_Table[i+4]  = Vowel_Table[i+5]  = 0x2424;
	Vowel_Table[i+6]    = Vowel_Table[i+7]  = Vowel_Table[i+8]  = 0x2426;
	Vowel_Table[i+9]    = Vowel_Table[i+11] = Vowel_Table[i+12] = 0x2428;
	Vowel_Table[i+12]   = Vowel_Table[i+13] = Vowel_Table[i+14] = 0x242A;
	}
    Vowel_Table[0x245E - 0x2420]    = 0x2422;	/* �� */
    Vowel_Table[0x245F - 0x2420]    = 0x2424;	/* �� */
    Vowel_Table[0x2460 - 0x2420]    = 0x2426;	/* �� */
    Vowel_Table[0x2461 - 0x2420]    = 0x2428;	/* �� */
    Vowel_Table[0x2462 - 0x2420]    = 0x242A;	/* �� */
    Vowel_Table[0x2463 - 0x2420]    = 0x2422;	/* �� */
    Vowel_Table[0x2464 - 0x2420]    = 0x2422;	/* �� */
    Vowel_Table[0x2465 - 0x2420]    = 0x2426;	/* �� */
    Vowel_Table[0x2466 - 0x2420]    = 0x2426;	/* �� */
    Vowel_Table[0x2467 - 0x2420]    = 0x242A;	/* �� */
    Vowel_Table[0x2468 - 0x2420]    = 0x242A;	/* �� */
    Vowel_Table[0x2469 - 0x2420]    = 0x2422;	/* �� */
    Vowel_Table[0x246A - 0x2420]    = 0x2424;	/* �� */
    Vowel_Table[0x246B - 0x2420]    = 0x2426;	/* �� */
    Vowel_Table[0x246C - 0x2420]    = 0x2428;	/* �� */
    Vowel_Table[0x246D - 0x2420]    = 0x242A;	/* �� */
    Vowel_Table[0x246E - 0x2420]    = 0x2422;	/* �� */
    Vowel_Table[0x246F - 0x2420]    = 0x2422;	/* �� */
    /* ����� �ϡ����Τޤޡ��첻�ؤ��Ѵ��Ϥ��ʤ��� */
    for (i = 0x2470 - 0x2420; i < 0x2477 - 0x2420; i++)
	Vowel_Table[i]	= i;
    }
