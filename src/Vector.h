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
 * class InternalVector
 */

template < class T, class Comparator, class Alloc = allocator<T> > class InternalVector : public vector<T, Alloc> {
  public:
    inline typename vector<T, Alloc>::const_iterator find(const T& value) const {
      Comparator comparator;
      typename vector<T>::const_iterator it = vector<T, Alloc>::begin();
      while (it != vector<T>::end()) {
        if (!comparator(*it, value) && !comparator(value, *it)) {
          break;
        }
        it++;
      }
      return it;
    }
};

/*
 * class Vector
 */

class Vector : public IndexedCollection< InternalVector<Persistent<Value>, ValueComparator> > {
  public:
    typedef InternalVector<Persistent<Value>, ValueComparator> Storage;

    virtual Handle<Value> GetValue(const Storage::value_type& value) const;

    static void Init(Handle<Object> exports);

  protected:
    virtual void InitializeFields(Handle<Object> thisObject);

  private:
    static Handle<Value> New(const Arguments& args);

    static Handle<Value> Remove(const Arguments& args);
    static Handle<Value> Reverse(const Arguments& args);
    static Handle<Value> Set(const Arguments& args);

    static Handle<Value> Each(const Arguments& args);
    static Handle<Value> _Each(const Arguments& args);
    static Handle<Value> Map(const Arguments& args);
    static Handle<Value> _Map(const Arguments& args);

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

  protected:
    static Handle<Value> New(const Arguments& args);

    static Handle<Value> IsFirst(const Arguments& args);
    static Handle<Value> IsLast(const Arguments& args);
    static Handle<Value> Index(const Arguments& args);

    static Handle<Value> Set(const Arguments& args);
    static Handle<Value> Remove(const Arguments& args);
    static Handle<Value> InsertBefore(const Arguments& args);
    static Handle<Value> InsertAfter(const Arguments& args);

  private:
    VectorModifier();
    virtual ~VectorModifier();

    bool isFirst;
    bool isLast;
    size_t index;
    Persistent<Value> replace;
    bool removed;
    Vector::Storage insertedBefore;
    Vector::Storage insertedAfter;

    friend class Vector;
};

#endif
