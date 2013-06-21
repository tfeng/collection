#ifndef COLLECTION_COMMON_H
#define COLLECTION_COMMON_H

#include <string>
#include <node.h>

using namespace node;
using namespace v8;

class CollectionUtil;


/*
 * Macros
 */

#define CHECK_DOES_NOT_TAKE_ARGUMENT(method, args) \
do { \
  if (args.Length() > 0) { \
    return ThrowException(Exception::Error(String::New(#method "() does not take any argument."))); \
  } \
} while (false)

#define CHECK_ITERATING(method, args) \
do { \
  if (ObjectWrap::Unwrap< Collection<Storage> >(args.This())->IsIterating()) { \
    return ThrowException(Exception::Error(String::New(#method "() cannot be called while the collection is being iterated."))); \
  } \
} while (false)


/*
 * class SetComparator
 */

class ValueComparator {
  public:
    bool operator()(const Handle<Value>& value1, const Handle<Value>& value2) const;

  private:
    int GetTypeScore(const Handle<Value>& value1) const;
};


/*
 * class Collection
 */

template <class Storage> class Collection : public ObjectWrap  {
  public:
    static bool HasInstance(Handle<Value> val);

    static Persistent<FunctionTemplate> constructor;

    bool IsIterating() const;

  protected:
    Collection();
    virtual ~Collection();

    virtual void InitializeFields(Handle<Object> thisObject);
    virtual void InitializeValues(Handle<Object> thisObject, Handle<Value> argument);
    virtual bool operator<(const Collection<Storage>& other) const;

    Handle<Value> Iterate(Handle<Value> (*iterator)(const Arguments&), const Arguments& args);

    Storage storage;
    int iterationLevel;

  private:
    static Handle<Value> New(const Arguments& args);

    static Handle<Value> IsEmpty(const Arguments& args);
    static Handle<Value> Size(const Arguments& args);
    static Handle<Value> ToArray(const Arguments& args);
    static Handle<Value> ToString(const Arguments& args);

    static Handle<Value> Equals(const Arguments& args);

    static Handle<Value> Add(const Arguments& args);
    static Handle<Value> AddAll(const Arguments& args);
    static Handle<Value> Get(const Arguments& args);
    static Handle<Value> Clear(const Arguments& args);

    friend class CollectionUtil;
    friend class ValueComparator;
};


/*
 * class CollectionUtil
 */

class CollectionUtil {
  public:
    static bool IsSupportedObject(Handle<Value> val);
    static bool IsSupportedType(Handle<Value> val);
    static void AddValue(Handle<Object> collection, Handle<Value> value);
    static void AddValues(Handle<Object> collection, Handle<Array> array);
    static void AddValues(Handle<Object> collection, Handle<Object> other);
    static void Each(Handle<Object> collection, void (*processor)(Handle<Object>, Handle<Value>), Handle<Object> memo);
};

#endif
