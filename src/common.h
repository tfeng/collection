#ifndef COLLECTION_COMMON_H
#define COLLECTION_COMMON_H

#include <string>
#include <node.h>

using namespace v8;

#define CHECK_DOES_NOT_TAKE_ARGUMENT(method) \
do { \
  if (args.Length() > 0) { \
    return ThrowException(Exception::Error(String::New(#method "() does not take any argument."))); \
  } \
} while (false)

class ValueComparator {
  public:
    bool operator() (const Handle<Value>& value1, const Handle<Value>& value2) const;
};

#endif