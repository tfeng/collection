#ifndef COLLECTION_SET_H
#define COLLECTION_SET_H

#include <set>
#include <node.h>
#include "common.h"

using namespace std;
using namespace v8;


/*
 * class Set
 */

class Set : public IndexedCollection< set<Persistent<Value>, ValueComparator> > {
  public:
    typedef set<Persistent<Value>, ValueComparator> Storage;

    virtual Handle<Value> GetValue(const Storage::value_type& value) const;

    static void Init(Handle<Object> exports);

  protected:
    virtual void InitializeFields(Handle<Object> thisObject);

    static Handle<Value> New(const Arguments& args);

    static Handle<Value> Remove(const Arguments& args);

    static Handle<Value> Each(const Arguments& args);
    static Handle<Value> _Each(const Arguments& args);
};

#endif
