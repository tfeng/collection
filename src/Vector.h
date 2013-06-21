#ifndef COLLECTION_VECTOR_H
#define COLLECTION_VECTOR_H

#include <vector>
#include <node.h>
#include "common.h"

using namespace node;
using namespace std;
using namespace v8;

class VectorModifier;


/*
 * class Vector
 */

typedef vector< Persistent<Value> > VectorStorage;

class Vector : public Collection<VectorStorage> {
  public:
    static void Init(Handle<Object> exports);

  protected:
    virtual void InitializeFields(Handle<Object> thisObject);

  private:
    static Handle<Value> New(const Arguments& args);

    static Handle<Value> Set(const Arguments& args);
    static Handle<Value> Reverse(const Arguments& args);
    static Handle<Value> Remove(const Arguments& args);
    static Handle<Value> RemoveAt(const Arguments& args);
    static Handle<Value> RemoveRange(const Arguments& args);
    static Handle<Value> RemoveLast(const Arguments& args);

    static Handle<Value> Each(const Arguments& args);
    static Handle<Value> _Each(const Arguments& args);
    static Handle<Value> Map(const Arguments& args);
    static Handle<Value> _Map(const Arguments& args);
    static Handle<Value> Reduce(const Arguments& args);
    static Handle<Value> _Reduce(const Arguments& args);
    static Handle<Value> ReduceRight(const Arguments& args);
    static Handle<Value> _ReduceRight(const Arguments& args);
    static Handle<Value> Find(const Arguments& args);
    static Handle<Value> _Find(const Arguments& args);
    static Handle<Value> Filter(const Arguments& args);
    static Handle<Value> _Filter(const Arguments& args);

    friend class ValueComparator;
    friend class VectorModifier;
};


/*
 * class VectorModifier
 */

class VectorModifier : public ObjectWrap {
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
    VectorStorage insertedBefore;
    VectorStorage insertedAfter;

    friend class Vector;
};

#endif
