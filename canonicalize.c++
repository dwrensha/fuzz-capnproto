#include "test.capnp.h"
#include <capnp/any.h>
#include <capnp/message.h>
#include <capnp/serialize.h>
#include <kj/exception.h>
#include <kj/array.h>
#include <iostream>

int main() {
  kj::Array<capnp::word> canonical;
  bool equal = false;
  try {
    capnp::ReaderOptions options;

    // The default traversal limit of 8 * 1024 * 1024 causes
    // AFL to think that it has found "hang" bugs.
    options.traversalLimitInWords = 8 * 1024;

    capnp::StreamFdMessageReader message(0, options); // read from stdin
    MyStruct::Reader myStruct = message.getRoot<MyStruct>();
    canonical = capnp::canonicalize(myStruct);

    kj::ArrayPtr<const capnp::word> segments[1] = {canonical.asPtr()};
    capnp::SegmentArrayMessageReader reader(kj::arrayPtr(segments, 1));

    auto originalAny = message.getRoot<capnp::AnyPointer>();

    // Discard cases where the original message is null.
    KJ_ASSERT(!originalAny.isNull());

    equal = originalAny == reader.getRoot<capnp::AnyPointer>();
  } catch (kj::Exception& e) {
    // Probably some kind of decoding exception.
    exit(0);
  }

  KJ_ASSERT(equal);

  kj::ArrayPtr<const capnp::word> segments[1] = {canonical.asPtr()};
  capnp::SegmentArrayMessageReader reader(kj::arrayPtr(segments, 1));
  KJ_ASSERT(reader.isCanonical());

  kj::Array<capnp::word> canonical2;
  {
    capnp::ReaderOptions options;
    options.traversalLimitInWords = 8 * 1024;

    MyStruct::Reader myStruct = reader.getRoot<MyStruct>();
    canonical2 = capnp::canonicalize(myStruct);
  }

  KJ_ASSERT(canonical.size() == canonical2.size());
  auto b1 = canonical.asBytes();
  auto b2 = canonical2.asBytes();
  for (int idx = 0; idx < b1.size(); ++idx) {
    KJ_ASSERT(b1[idx] == b2[idx]);
  }

  return 0;
}
