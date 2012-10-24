#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h>
#include <errno.h>

#include "pull_by_size.h"
#include "file_read.h"
#include "global.h"
#include "output.h"
#include "kseq.h"

__KS_GETC(gzread, BUFFER_SIZE)
__KS_GETUNTIL(gzread, BUFFER_SIZE)
__KSEQ_READ

extern char const *progname;

int pull_by_size(char *input_file, int min, int max,int length, int convert) {
  FILE *fp;
  int count=0,l;
  int is_fasta = 0; /* assume this is fastq */
  kseq_t *seq;

  /* open fasta file */
  fp = gzopen(input_file,"r");
  if (!fp) {
    fprintf(stderr,"%s - Couldn't open fasta file %s\n",progname,input_file);
    exit(EXIT_FAILURE);
  }

  seq = kseq_init(fp);
  /* search through list and see if this header matches */
  while((l = kseq_read(seq)) >= 0) {
	if (seq->qual.s == NULL)
		is_fasta = 1;
    if (min > 0 && max > 0) { /* got a min and max */
      if (l >= min && l <= max) {
        count++;
		if (convert)
			is_fasta ? print_fastq_seq(seq) : print_fasta_seq(seq,length);
		else
			is_fasta ? print_fasta_seq(seq,length) : print_fastq_seq(seq);
      }
    } else if (min > 0 || max > 0) { /* either  min or max is 0 */
      if (min > 0 && l >= min) {
        count++;
		if (convert)
			is_fasta ? print_fastq_seq(seq) : print_fasta_seq(seq,length);
		else
			is_fasta ? print_fasta_seq(seq,length) : print_fastq_seq(seq);
      } else if (max > 0 && l <= max) {
        count++;
		if (convert)
			is_fasta ? print_fastq_seq(seq) : print_fasta_seq(seq,length);
		else
			is_fasta ? print_fasta_seq(seq,length) : print_fastq_seq(seq);
      }
    } else {
		count++;
		if (convert)
			is_fasta ? print_fastq_seq(seq) : print_fasta_seq(seq,length);
		else
			is_fasta ? print_fasta_seq(seq,length) : print_fastq_seq(seq);
    }
  }
  kseq_destroy(seq);
  gzclose(fp); /* done reading file */

  return count;
}