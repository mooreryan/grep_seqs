# Grep Sequences

Grep from sequence files. Currenty it only works with a list of IDs given in a file.

## Compile

```
gcc grep_ids.c vendor/*.c -I./vendor -lz -Wall -g -O3 -o grep_ids
```

## Run

```
bin/grep_ids <ids.txt> <seqs.fa> > subset_of_seqs.fa
```

It works with fastA, fastQ, and regardless of whether the seq files are gzipped.
