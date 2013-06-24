#ifndef COLLECTION_MAP_H
#define COLLECTION_MAP_H

#include <map>
#include <node.h>
#include "common.h"

using namespace std;
using namespace v8;


/*
 * class Set
 */

class Map : public Collection< map<Persistent<Value>, Persistent<Value>, ValueComparator> > {
  public:
    typedef map<Persistent<Value>, Persistent<Value>, ValueComparator> Storage;

    virtual Handle<Value> GetValue(const Storage::value_type& value) const;

    static void Init(Handle<Object> exports);

  protected:
    virtual void InitializeFields(Handle<Object> thisObject);
    virtual void InitializeValues(Handle<Object> thisObject, Handle<Value> argument);
    virtual bool IsSupportedObject(Handle<Value> value);
    virtual bool IsSupportedType(Handle<Value> value);

    static Handle<Value> New(const Arguments& args);

    static Handle<Value> Get(const Arguments& args);
    static Handle<Value> GetAt(const Arguments& args);
    static Handle<Value> Keys(const Arguments& args);
    static Handle<Value> Remove(const Arguments& args);
    static Handle<Value> Set(const Arguments& args);
    static Handle<Value> SetAll(const Arguments& args);
    static Handle<Value> ToObject(const Arguments& args);
    static Handle<Value> ToString(const Arguments& args);
};


/*
 * class Entry
 */

class MapEntry : public ObjectWrap {
  public:
    static void Init(Handle<Object> exports);

    static Persistent<FunctionTemplate> constructor;

    static Handle<Value> New(const Arguments& args);

    static Handle<Value> GetKey(const Arguments& args);
    static Handle<Value> GetValue(const Arguments& args);
    static Handle<Value> ToObject(const Arguments& args);
    static Handle<Value> ToString(const Arguments& args);

  private:
    Handle<Value> key;
    Handle<Value> value;

    friend class Map;
};

#endif
