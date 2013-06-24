#ifndef COLLECTION_COMMON_H
#define COLLECTION_COMMON_H

#include <string>
#include <node.h>

using namespace node;
using namespace std;
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
    bool operator()(const pair< Handle<Value>, Handle<Value> >& pair1, const pair< Handle<Value>, Handle<Value> >& pair2) const;
    bool Equals(const Handle<Value>& value1, const Handle<Value>& value2) const;
    bool Equals(const pair< Handle<Value>, Handle<Value> >& pair1, const pair< Handle<Value>, Handle<Value> >& pair2) const;

  private:
    int GetTypeScore(const Handle<Value>& value1) const;
};


/*
 * class Collection
 */

template <class Storage> class Collection : public ObjectWrap  {
  public:
    static bool HasInstance(Handle<Value> val);

    virtual Handle<Value> GetValue(const typename Storage::value_type& value) const = 0;

    static Persistent<FunctionTemplate> constructor;

    bool IsIterating() const;

    Storage storage;

  protected:
    Collection();
    virtual ~Collection();

    virtual void InitializeFields(Handle<Object> thisObject);
    virtual void InitializeValues(Handle<Object> thisObject, Handle<Value> argument) = 0;
    virtual bool operator<(const Collection<Storage>& other) const;
    virtual bool IsSupportedObject(Handle<Value> value) = 0;
    virtual bool IsSupportedType(Handle<Value> value) = 0;
    virtual Handle<Value> Iterate(Handle<Value> (*iterator)(const Arguments&), const Arguments& args);

    static Handle<Value> New(const Arguments& args);

    static Handle<Value> Clear(const Arguments& args);
    static Handle<Value> Equals(const Arguments& args);
    static Handle<Value> Get(const Arguments& args);
    static Handle<Value> Has(const Arguments& args);
    static Handle<Value> IsEmpty(const Arguments& args);
    static Handle<Value> RemoveAt(const Arguments& args);
    static Handle<Value> RemoveLast(const Arguments& args);
    static Handle<Value> RemoveRange(const Arguments& args);
    static Handle<Value> Size(const Arguments& args);
    static Handle<Value> ToArray(const Arguments& args);
    static Handle<Value> ToString(const Arguments& args);

    static Handle<Value> Each(const Arguments& args);
    static Handle<Value> _Each(const Arguments& args);
    static Handle<Value> Filter(const Arguments& args);
    static Handle<Value> _Filter(const Arguments& args);
    static Handle<Value> Find(const Arguments& args);
    static Handle<Value> _Find(const Arguments& args);
    static Handle<Value> Reduce(const Arguments& args);
    static Handle<Value> _Reduce(const Arguments& args);
    static Handle<Value> ReduceRight(const Arguments& args);
    static Handle<Value> _ReduceRight(const Arguments& args);

    int iterationLevel;

    friend class CollectionUtil;
    friend class ValueComparator;
};


/*
 * class IndexedCollection
 */

template <class Storage> class IndexedCollection : public Collection<Storage> {
  protected:
    virtual void InitializeFields(Handle<Object> thisObject);
    virtual void InitializeValues(Handle<Object> thisObject, Handle<Value> argument);
    virtual bool IsSupportedObject(Handle<Value> value);
    virtual bool IsSupportedType(Handle<Value> value);

    static void AddValue(Handle<Object> collection, Handle<Value> value);
    static void AddValues(Handle<Object> collection, Handle<Array> array);
    static void AddValues(Handle<Object> collection, Handle<Object> other);

    static Handle<Value> Add(const Arguments& args);
    static Handle<Value> AddAll(const Arguments& args);
    static Handle<Value> Index(const Arguments& args);
};


/*
 * class CollectionUtil
 */

class CollectionUtil {
  public:
    static void Dispose(Persistent<Value> value);
    static void Dispose(pair< Persistent<Value>, Persistent<Value> > pair);
    static Handle<Value> Stringify(Handle<Value> value);
};

#endif
