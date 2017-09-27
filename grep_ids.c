/*
Copyright 2017 Ryan Moore
Contact: moorer@udel.edu

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or (at
your option) any later version.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h>

#include "tommyhashlin.h"
#include "kseq.h"

KSEQ_INIT(gzFile, gzread)

typedef struct name_t {
  char* name;
  tommy_node node;
} name_t;

int name_compare(const void* arg, const void* name)
{
  return strcmp((const char*)arg,
                ((const struct name_t*)name)->name);
}

name_t* name_init()
{
  name_t* name = malloc(sizeof(name_t));

  return name;
}

void name_destroy(name_t* name)
{
  free(name->name);
  free(name);
}


int main(int argc, char *argv[])
{
  tommy_hashlin ids;
  tommy_hashlin_init(&ids);
  char* id = calloc(10240, sizeof(char*));
  name_t* tmpname;

  unsigned long num_ids = 0;
  unsigned long num_seqs = 0;
  long l;

  tommy_uint32_t id_hash;

  if (argc != 3) {
    fprintf(stderr,
            "Checks id against the ID part of the header only!\n"
            "Exact matches only!\n"
            "Usage: %s <ids.txt> <seqs.fa>\n", argv[0]);

    exit(1);
  }

  FILE* idsfp = fopen(argv[1], "r");
  if (!idsfp) {
    fprintf(stderr, "LOG -- error: could not open %s\n", argv[1]);

    return 2;
  }

  gzFile fastafp = gzopen(argv[2], "r");
  if (!fastafp) {
    fprintf(stderr, "LOG -- error: could not open %s\n", argv[1]);

    return 2;
  }

  kseq_t* seq;
  seq = kseq_init(fastafp);

  unsigned long lineidx = 0;
  while ((fscanf(idsfp, "%s", id) == 1)) {
    if (++line_idx % 10000 == 0) {
      fprintf(stderr, "Reading id line -- %lu\r", lineidx);
    }

    id_hash = tommy_strhash_u32(0, id);
    tmpname = tommy_hashlin_search(&ids,
                                   name_compare,
                                   id,
                                   id_hash);

    if (!tmpname) {
      ++num_ids;
      name_t* name = name_init();
      name->name = strdup(id);

      tommy_hashlin_insert(&ids,
                           &name->node,
                           name,
                           id_hash);
    }
  }

  fprintf(stderr, "INFO -- there were %lu unique ids\n", num_ids);

  while ((l = kseq_read(seq)) >=0) {
    if (++num_seqs % 10000 == 0) {
      fprintf(stderr, "LOG -- reading: %lu\r", num_seqs);
    }

    id_hash = tommy_strhash_u32(0, seq->name.s);
    tmpname = tommy_hashlin_search(&ids,
                                   name_compare,
                                   seq->name.s,
                                   id_hash);

    if (tmpname) {
      if (seq->qual.l && seq->comment.l) { /* fastq with comment */
        printf("@%s %s\n"
               "%s\n"
               "+\n"
               "%s\n",
               seq->name.s,
               seq->comment.s,
               seq->seq.s,
               seq->qual.s);
      } else if (seq->qual.l) { /* fastq no comment */
        printf("@%s\n"
               "%s\n"
               "+\n"
               "%s\n",
               seq->name.s,
               seq->seq.s,
               seq->qual.s);
      } else if (seq->comment.l) { /* fasta with comment */
        printf(">%s %s\n"
               "%s\n",
               seq->name.s,
               seq->comment.s,
               seq->seq.s);
      } else { /* fasta no comment */
        printf(">%s\n"
               "%s\n",
               seq->name.s,
               seq->seq.s);
      }
    }
  }


  tommy_hashlin_foreach(&ids, (tommy_foreach_func*)name_destroy);
  tommy_hashlin_done(&ids);

  kseq_destroy(seq);
  fclose(idsfp);
  gzclose(fastafp);

  free(id);
  return 0;
}
