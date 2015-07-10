#!/bin/sh

# This script compares the fingerprint evolution of two simulation runs.
# It takes two eventlog files and outputs a diff file that can be used to
# find the first event where the fingerprints don't match.
#
# Configure the simulation by adding the following lines to the ini file:
#
# record-eventlog = true
# fingerprint = 0000-0000

grep "E #" $1 | awk '{printf("# %s f %08x\n", $3, $13);}' > $1.tmp
grep "E #" $2 | awk '{printf("# %s f %08x\n", $3, $13);}' > $2.tmp
diff -U 1 $1.tmp $2.tmp > diffingerprints.diff
rm $1.tmp
rm $2.tmp
head -n 10 diffingerprints.diff
