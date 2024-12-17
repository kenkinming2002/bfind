# bfind
Tl;dr - GNU find does not support breadth first traversal. I want breadth first
traversal.

## Building and installation
```sh
$ make -j
$ make install
```

## Caveat
 - Hidden directories (directories starting with dot) are ignored.
 - Only directories are printed.

In other word, unlike GNU find which support a plethora of options to filter
out search results, the behavior of bfind is hardcoded. This may change in the
future if the author feel like implementing it, but feature parity with GNU
find is not to be expected.
