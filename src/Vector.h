#ifndef COLLECTION_VECTOR_H
#define COLLECTION_VECTOR_H

#include <vector>
#include <node.h>
#include "common.h"

using namespace v8;

class VectorModifier;

class Vector : public node::ObjectWrap {
  public:
    static void Init(Handle<Object> exports);
    static bool HasInstance(v8::Handle<v8::Value> val);

    static Persistent<FunctionTemplate> constructor;

  private:
    Vector();
    ~Vector();

    static Handle<Value> New(const Arguments& args);

    static Handle<Value> Length(const Arguments& args);
    static Handle<Value> Inspect(const Arguments& args);
    static Handle<Value> IsEmpty(const Arguments& args);
    static Handle<Value> ToArray(const Arguments& args);
    static Handle<Value> ToString(const Arguments& args);
    static Handle<Value> Equals(const Arguments& args);

    static Handle<Value> Add(const Arguments& args);
    static Handle<Value> Get(const Arguments& args);
    static Handle<Value> Set(const Arguments& args);
    static Handle<Value> Reverse(const Arguments& args);
    static Handle<Value> Remove(const Arguments& args);
    static Handle<Value> RemoveRange(const Arguments& args);
    static Handle<Value> RemoveLast(const Arguments& args);
    static Handle<Value> Clear(const Arguments& args);

    static Handle<Value> Each(const Arguments& args);
    static Handle<Value> Map(const Arguments& args);
    static Handle<Value> Reduce(const Arguments& args);
    static Handle<Value> ReduceRight(const Arguments& args);
    static Handle<Value> Find(const Arguments& args);
    static Handle<Value> Filter(const Arguments& args);

    std::vector< Persistent<Value> > vector;

    friend class ValueComparator;
    friend class VectorModifier;
};

class VectorModifier : public node::ObjectWrap {
  public:
    static void Init(Handle<Object> exports);

    void clear(bool dispose);

    static Persistent<FunctionTemplate> constructor;

  private:
    VectorModifier();
    ~VectorModifier();

    static Handle<Value> New(const Arguments& args);

    static Handle<Value> IsFirst(const Arguments& args);
    static Handle<Value> IsLast(const Arguments& args);
    static Handle<Value> Index(const Arguments& args);

    static Handle<Value> Set(const Arguments& args);
    static Handle<Value> Remove(const Arguments& args);
    static Handle<Value> InsertBefore(const Arguments& args);
    static Handle<Value> InsertAfter(const Arguments& args);

    bool isFirst;
    bool isLast;
    uint32_t index;
    Persistent<Value> replace;
    bool removed;
    std::vector< Persistent<Value> > insertedBefore;
    std::vector< Persistent<Value> > insertedAfter;

    friend class Vector;
};

#endif
