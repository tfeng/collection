#ifndef COLLECTION_SET_H
#define COLLECTION_SET_H

#include <set>
#include <node.h>
#include "common.h"

using namespace v8;

class Set : public node::ObjectWrap {
  public:
    static void Init(Handle<Object> exports);
    static bool HasInstance(v8::Handle<v8::Value> val);

    static Persistent<FunctionTemplate> constructor;

  private:
    Set();
    ~Set();

    static Handle<Value> New(const Arguments& args);

    static Handle<Value> Size(const Arguments& args);
    static Handle<Value> Inspect(const Arguments& args);
    static Handle<Value> IsEmpty(const Arguments& args);
    static Handle<Value> ToArray(const Arguments& args);

    std::set< Persistent<Value>, ValueComparator > set;
};

#endif
