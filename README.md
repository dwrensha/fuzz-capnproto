# Fuzzing [Cap'n Proto](https://github.com/sandstorm-io/capnproto) with [AFL](http://lcamtuf.coredump.cx/afl/)

This repo contains the fuzzing setup that resulted in
[this](https://github.com/sandstorm-io/capnproto/pull/423) bug report.


Instructions:

  1. Build the Cap'n Proto C++ runtime library with `afl-g++`
  2. Compile the included schema: `capnp compile -oc++ test.capnp`
  3. Build `canonicalize.c++` with the AFL-instrumented capnp runtime statically linked, e.g.:

```
$ ~/src/afl-2.39b/afl-g++ canonicalize.c++ -o canonicalize -I ~/src/capnproto/c++/build-afl/include/ ~/src/capnproto/c++/build-afl/lib/libcapnp.a  ~/src/capnproto/c++/build-afl/lib/libkj.a
```
  4. Run the fuzzer, e.g.:

```
~/src/afl-2.39b/afl-fuzz -i input/ -o output/ -- ./canonicalize
```