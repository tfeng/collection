#include <cstring>
#include "common.h"
#include "Map.h"
#include "Set.h"
#include "Vector.h"

using namespace node;
using namespace std;
using namespace v8;


/*
 * class ValueComparator
 */

bool ValueComparator::operator()(const Handle<Value>& value1, const Handle<Value>& value2) const {
  if (GetTypeScore(value1) < GetTypeScore(value2)) {
    return true;
  } else if (GetTypeScore(value1) > GetTypeScore(value2)) {
    return false;
  }

  ValueComparator comparator;

  if (value1.IsEmpty()) {
    return false;
  }

  if (value1->IsUndefined()) {
    return false;
  }

  if (value1->IsNull()) {
    return false;
  }

  if (value1->IsBoolean()) {
    return value1->BooleanValue() < value2->BooleanValue();
  }

  if (value1->IsNumberObject()) {
    return value1->ToNumber()->Value() < value2->ToNumber()->Value();
  }

  if (value1->IsNumber()) {
    return value1->IntegerValue() < value2->IntegerValue();
  }

  if (value1->IsDate()) {
    return Handle<Date>::Cast(value1)->NumberValue() < Handle<Date>::Cast(value2)->NumberValue();
  }

  if (value1->IsString()) {
    Handle<String> s1 = value1->ToString();
    Handle<String> s2 = value2->ToString();
    int len1 = s1->Utf8Length();
    int len2 = s2->Utf8Length();
    int min = len1 < len2 ? len1 : len2;
    char* buffer1 = new char[min + 1];
    char* buffer2 = new char[min + 1];
    s1->WriteUtf8(buffer1, min);
    s2->WriteUtf8(buffer2, min);
    buffer1[min] = buffer2[min] = '\0';
    int result = strcmp(buffer1, buffer2);
    delete []buffer1;
    delete []buffer2;
    return result < 0 || (result == 0 && len1 < len2);
  }

  if (value1->IsArray()) {
    Handle<Array> array1 = Handle<Array>::Cast(value1);
    Handle<Array> array2 = Handle<Array>::Cast(value2);
    uint32_t len1 = array1->Length();
    uint32_t len2 = array2->Length();
    uint32_t min = len1 < len2 ? len1 : len2;
    for (uint32_t i = 0; i < min; i++) {
      Handle<Value> element1 = array1->Get(i);
      Handle<Value> element2 = array2->Get(i);
      if (comparator(element1, element2)) {
        return true;
      } else if (comparator(element2, element1)) {
        return false;
      }
    }
    return len1 < len2;
  }

  if (Map::constructor->HasInstance(value1)) {
    Map* object1 = ObjectWrap::Unwrap<Map>(Handle<Object>::Cast(value1));
    Map* object2 = ObjectWrap::Unwrap<Map>(Handle<Object>::Cast(value2));
    return *object1 < *object2;
  }

  if (Set::constructor->HasInstance(value1)) {
    Set* object1 = ObjectWrap::Unwrap<Set>(Handle<Object>::Cast(value1));
    Set* object2 = ObjectWrap::Unwrap<Set>(Handle<Object>::Cast(value2));
    return *object1 < *object2;
  }

  if (Vector::constructor->HasInstance(value1)) {
    Vector* object1 = ObjectWrap::Unwrap<Vector>(Handle<Object>::Cast(value1));
    Vector* object2 = ObjectWrap::Unwrap<Vector>(Handle<Object>::Cast(value2));
    return *object1 < *object2;
  }

  return false;
}

bool ValueComparator::operator()(const pair< Handle<Value>, Handle<Value> >& pair1, const pair< Handle<Value>, Handle<Value> >& pair2) const {
  return (*this)(pair1.first, pair2.first) || (!(*this)(pair2.first, pair1.first) && (*this)(pair1.second, pair2.second));
}

int ValueComparator::GetTypeScore(const Handle<Value>& value) const {
  if (value.IsEmpty()) {
    return 0;
  } else if (value->IsUndefined()) {
    return 1;
  } else if (value->IsNull()) {
    return 2;
  } else if (value->IsBoolean()) {
    return 3;
  } else if (value->IsNumberObject()) {
    return 4;
  } else if (value->IsNumber()) {
    return 5;
  } else if (value->IsDate()) {
    return 6;
  } else if (value->IsString()) {
    return 7;
  } else if (value->IsArray()) {
    return 8;
  } else if (Set::constructor->HasInstance(value)) {
    return 9;
  } else if (Vector::constructor->HasInstance(value)) {
    return 10;
  } else {
    return -1;
  }
}


/*
 * class Collection
 */

template <class Storage> bool Collection<Storage>::HasInstance(Handle<Value> value) {
  return value->IsObject() && constructor->HasInstance(Handle<Object>::Cast(value));
}

template <class Storage> bool Collection<Storage>::IsIterating() const {
  return iterationLevel > 0;
}

template <class Storage> Collection<Storage>::Collection() : iterationLevel(0) {
}

template <class Storage> Collection<Storage>::~Collection() {
  typename Storage::const_iterator it = storage.begin();
  while (it != storage.end()) {
    CollectionUtil::Dispose(*it++);
  }
}

template <class Storage> void Collection<Storage>::InitializeFields(Handle<Object> thisObject) {
  thisObject->Set(String::NewSymbol("isEmpty"), FunctionTemplate::New(IsEmpty)->GetFunction());
  thisObject->Set(String::NewSymbol("size"), FunctionTemplate::New(Size)->GetFunction());
  thisObject->Set(String::NewSymbol("equals"), FunctionTemplate::New(Equals)->GetFunction());
  thisObject->Set(String::NewSymbol("clear"), FunctionTemplate::New(Clear)->GetFunction());

  thisObject->Set(String::NewSymbol("each"), FunctionTemplate::New(Each)->GetFunction());
}

template <class Storage> Handle<Value> Collection<Storage>::Iterate(Handle<Value> (*iterator)(const Arguments&), const Arguments& args) {
  Handle<Value> result;
  iterationLevel++;
  try {
    result = iterator(args);
    iterationLevel--;
  } catch (...) {
    iterationLevel--;
  }
  return result;
}

template <class Storage> Handle<Value> Collection<Storage>::New(const Arguments& args) {
  return ThrowException(Exception::Error(String::New("Cannot create instance of this type.")));
}

template <class Storage> bool Collection<Storage>::operator<(const Collection<Storage>& other) const {
  ValueComparator comparator;
  typename Storage::const_iterator it1 = storage.begin();
  typename Storage::const_iterator it2 = other.storage.begin();
  while (it1 != storage.end() && it2 != other.storage.end()) {
    typename Storage::value_type element1 = *it1++;
    typename Storage::value_type element2 = *it2++;
    if (comparator(element1, element2)) {
      return true;
    } else if (comparator(element2, element1)) {
      return false;
    }
  }
  return it1 == storage.end() && it2 != other.storage.end();
}

template <class Storage> Handle<Value> Collection<Storage>::IsEmpty(const Arguments& args) {
  CHECK_DOES_NOT_TAKE_ARGUMENT(isEmpty, args);

  HandleScope scope;
  Collection<Storage>* obj = ObjectWrap::Unwrap< Collection<Storage> >(args.This());
  return scope.Close(Boolean::New(obj->storage.empty()));
}

template <class Storage> Handle<Value> Collection<Storage>::Size(const Arguments& args) {
  CHECK_DOES_NOT_TAKE_ARGUMENT(size, args);

  HandleScope scope;
  Collection<Storage>* obj = ObjectWrap::Unwrap< Collection<Storage> >(args.This());
  return scope.Close(Uint32::New((uint32_t) obj->storage.size()));
}

template <class Storage> Handle<Value> Collection<Storage>::Equals(const Arguments& args) {
  if (args.Length() != 1) {
    return ThrowException(Exception::Error(String::New("equals(value) takes one argument.")));
  }

  HandleScope scope;
  if (HasInstance(args[0])) {
    Collection<Storage>* obj1 = ObjectWrap::Unwrap< Collection<Storage> >(args.This());
    typename Storage::iterator it1 = obj1->storage.begin();
    Collection<Storage>* obj2 = ObjectWrap::Unwrap< Collection<Storage> >(Handle<Object>::Cast(args[0]));
    typename Storage::iterator it2 = obj2->storage.begin();
    ValueComparator comparator;
    while (it1 != obj1->storage.end() && it2 != obj2->storage.end()) {
      typename Storage::value_type v1 = *it1++;
      typename Storage::value_type v2 = *it2++;
      if (comparator(v1, v2) || comparator(v2, v1)) {
        return scope.Close(Boolean::New(false));
      }
    }
    if (it1 != obj1->storage.end() || it2 != obj2->storage.end()) {
      return scope.Close(Boolean::New(false));
    } else {
      return scope.Close(Boolean::New(true));
    }
  } else {
    return scope.Close(Boolean::New(false));
  }
}

template <class Storage> Handle<Value> Collection<Storage>::Clear(const Arguments& args) {
  CHECK_ITERATING(clear, args);
  CHECK_DOES_NOT_TAKE_ARGUMENT(clear, args);

  HandleScope scope;
  Collection<Storage>* obj = ObjectWrap::Unwrap< Collection<Storage> >(args.This());

  typename Storage::iterator it = obj->storage.begin();
  while (it != obj->storage.end()) {
    CollectionUtil::Dispose(*it++);
  }

  obj->storage.clear();
  return args.This();
}

template <class Storage> Handle<Value> Collection<Storage>::Each(const Arguments& args) {
  return ObjectWrap::Unwrap< Collection<Storage> >(args.This())->Iterate(_Each, args);
}

template <class Storage> Handle<Value> Collection<Storage>::_Each(const Arguments& args) {
  if (args.Length() != 1 || !(args[0]->IsFunction())) {
    return ThrowException(Exception::Error(String::New("each(function) takes a function argument.")));
  }

  HandleScope scope;
  Local<Object> global = Context::GetCurrent()->Global();
  Local<Function> function = Local<Function>::Cast(args[0]);
  TryCatch tryCatch;
  Collection<Storage>* obj = ObjectWrap::Unwrap< Collection<Storage> >(args.This());
  typename Storage::const_iterator it = obj->storage.begin();
  while (it != obj->storage.end()) {
    Handle<Value> parameters[1];
    parameters[0] = obj->GetValue(*it++);
    Handle<Value> result = function->Call(global, 1, parameters);
    if (result.IsEmpty()) {
      return ThrowException(tryCatch.Exception());
    }
    if (result->IsFalse()) {
      break;
    }
  }
  return args.This();
}


/*
 * class IndexedCollection
 */

template <class Storage> void IndexedCollection<Storage>::InitializeFields(Handle<Object> thisObject) {
  Collection<Storage>::InitializeFields(thisObject);

  thisObject->Set(String::NewSymbol("toArray"), FunctionTemplate::New(ToArray)->GetFunction());
  thisObject->Set(String::NewSymbol("toString"), FunctionTemplate::New(ToString)->GetFunction());

  thisObject->Set(String::NewSymbol("add"), FunctionTemplate::New(Add)->GetFunction());
  thisObject->Set(String::NewSymbol("addAll"), FunctionTemplate::New(AddAll)->GetFunction());
  thisObject->Set(String::NewSymbol("get"), FunctionTemplate::New(Get)->GetFunction());
}

template <class Storage> void IndexedCollection<Storage>::InitializeValues(Handle<Object> thisObject, Handle<Value> argument) {
  if (argument->IsArray()) {
    AddValues(thisObject, Handle<Array>::Cast(argument));
  } else if (IsSupportedObject(argument)) {
    AddValues(thisObject, Handle<Object>::Cast(argument));
  }
}

template <class Storage> bool IndexedCollection<Storage>::IsSupportedObject(Handle<Value> value) {
  if (Set::constructor->HasInstance(value) ||
      Vector::constructor->HasInstance(value)) {
    IndexedCollection<Storage>* object = ObjectWrap::Unwrap< IndexedCollection<Storage> >(Handle<Object>::Cast(value));
    typename Storage::const_iterator it = object->storage.begin();
    while (it != object->storage.end()) {
      if (!IsSupportedType(*it++)) {
        return false;
      }
    }
    return true;
  }

  return false;
}

template <class Storage> bool IndexedCollection<Storage>::IsSupportedType(Handle<Value> value) {
  if (value.IsEmpty() ||
      value->IsUndefined() ||
      value->IsNull() ||
      value->IsBoolean() ||
      value->IsNumberObject() ||
      value->IsNumber() ||
      value->IsDate() ||
      value->IsString()) {
    return true;
  }

  if (value->IsArray()) {
    Handle<Array> array = Handle<Array>::Cast(value);
    for (uint32_t i = 0; i < array->Length(); i++) {
      if (!IsSupportedType(array->Get(i))) {
        return false;
      }
    }
    return true;
  }

  return IsSupportedObject(value);
}

template <class Storage> Handle<Value> IndexedCollection<Storage>::ToArray(const Arguments& args) {
  HandleScope scope;
  IndexedCollection<Storage>* obj = ObjectWrap::Unwrap< IndexedCollection<Storage> >(args.This());
  Local<Array> array = Array::New((uint32_t) obj->storage.size());
  typename Storage::iterator it = obj->storage.begin();
  for (uint32_t i = 0; it != obj->storage.end(); it++, i++) {
    array->Set(i, obj->GetValue(*it));
  }
  return scope.Close(array);
}

template <class Storage> Handle<Value> IndexedCollection<Storage>::ToString(const Arguments& args) {
  CHECK_DOES_NOT_TAKE_ARGUMENT(toString, args);

  HandleScope scope;
  return scope.Close(ToArray(args)->ToString());
}

template <class Storage> void IndexedCollection<Storage>::AddValue(Handle<Object> collection, Handle<Value> value) {
  if (Set::constructor->HasInstance(collection)) {
    Set* object = ObjectWrap::Unwrap<Set>(collection);
    object->storage.insert(Persistent<Value>::New(value));
  } else if (Vector::constructor->HasInstance(collection)) {
    Vector* object = ObjectWrap::Unwrap<Vector>(collection);
    object->storage.push_back(Persistent<Value>::New(value));
  }
}

template <class Storage> void IndexedCollection<Storage>::AddValues(Handle<Object> collection, Handle<Array> array) {
  if (Set::constructor->HasInstance(collection)) {
    Set* object = ObjectWrap::Unwrap<Set>(collection);
    for (uint32_t i = 0; i < array->Length(); i++) {
      object->storage.insert(Persistent<Value>::New(array->Get(i)));
    }
  } else if (Vector::constructor->HasInstance(collection)) {
    Vector* object = ObjectWrap::Unwrap<Vector>(collection);
    for (uint32_t i = 0; i < array->Length(); i++) {
      object->storage.push_back(Persistent<Value>::New(array->Get(i)));
    }
  }
}

template <class Storage> void IndexedCollection<Storage>::AddValues(Handle<Object> collection, Handle<Object> other) {
  IndexedCollection<Storage>* object = ObjectWrap::Unwrap< IndexedCollection<Storage> >(other);
  typename Storage::iterator it = object->storage.begin();
  while (it != object->storage.end()) {
    AddValue(collection, *it++);
  }
}

template <class Storage> Handle<Value> IndexedCollection<Storage>::Add(const Arguments& args) {
  CHECK_ITERATING(add, args);
  if (args.Length() == 0) {
    return ThrowException(Exception::Error(String::New("add(value, ...) takes at least one argument.")));
  }

  HandleScope scope;
  IndexedCollection<Storage>* obj = ObjectWrap::Unwrap< IndexedCollection<Storage> >(args.This());
  typename Storage::iterator end = obj->storage.end();
  for (int i = 0; i < args.Length(); i++) {
    end = obj->storage.insert(end, Persistent<Value>::New(args[i]));
    end++;
  }
  return args.This();
}

template <class Storage> Handle<Value> IndexedCollection<Storage>::AddAll(const Arguments& args) {
  CHECK_ITERATING(addAll, args);
  if (args.Length() != 1 || !(args[0]->IsArray())) {
    return ThrowException(Exception::Error(String::New("addAll(array) takes one array argument.")));
  }

  HandleScope scope;
  IndexedCollection<Storage>* obj = ObjectWrap::Unwrap< IndexedCollection<Storage> >(args.This());
  Handle<Array> array = Handle<Array>::Cast(args[0]);
  typename Storage::iterator end = obj->storage.end();
  for (uint32_t i = 0; i < array->Length(); i++) {
    end = obj->storage.insert(end, Persistent<Value>::New(array->Get(i)));
    end++;
  }
  return args.This();
}

template <class Storage> Handle<Value> IndexedCollection<Storage>::Get(const Arguments& args) {
  if (args.Length() == 0) {
    return ThrowException(Exception::Error(String::New("get(index, ...) takes at least one argument.")));
  }
  for (int i = 0; i < args.Length(); i++) {
    Handle<Value> arg = args[i];
    if (!(arg->IsUndefined()) && !(arg->IsNull()) && !(arg->IsUint32())) {
      return ThrowException(Exception::Error(String::New("get(index, ...) takes only integer arguments.")));
    }
  }

  HandleScope scope;
  IndexedCollection<Storage>* obj = ObjectWrap::Unwrap< IndexedCollection<Storage> >(args.This());
  if (args.Length() == 1 && args[0]->IsUint32()) {
    uint32_t index = args[0]->Uint32Value();
    if (index < obj->storage.size()) {
      typename Storage::const_iterator it = obj->storage.begin();
      advance(it, index);
      return scope.Close(Local<Value>::New(*it));
    }
  } else {
    Handle<Array> array = Array::New();
    for (int i = 0, j = 0; i < args.Length(); i++) {
      Handle<Value> arg = args[i];
      if (arg->IsUint32()) {
        uint32_t index = arg->Uint32Value();
        if (index < obj->storage.size()) {
          typename Storage::const_iterator it = obj->storage.begin();
          advance(it, index);
          array->Set(j++, Local<Value>::New(*it));
        }
      }
    }
    return scope.Close(array);
  }
  return Undefined();
}


/*
 * class CollectionUtil
 */

void CollectionUtil::Dispose(Persistent<Value> value) {
  value.Dispose();
}

void CollectionUtil::Dispose(pair< Persistent<Value>, Persistent<Value> > pair) {
  pair.first.Dispose();
  pair.second.Dispose();
}


/*
 * Templated classes
 */

template<class Storage> Persistent<FunctionTemplate> Collection<Storage>::constructor;

template class Collection<Map::Storage>;
template class Collection<Set::Storage>;
template class Collection<Vector::Storage>;
template class IndexedCollection<Set::Storage>;
template class IndexedCollection<Vector::Storage>;
