#include "Map.h"

using namespace std;
using namespace v8;


/*
 * class Map
 */

Handle<Value> Map::GetValue(const Storage::value_type& value) const {
  HandleScope scope;
  Persistent<Value> parameters[2];
  parameters[0] = value.first;
  parameters[1] = value.second;
  Local<Object> object = MapEntry::constructor->GetFunction()->NewInstance(2, parameters);
  return scope.Close(object);
}

void Map::Init(Handle<Object> exports) {
  HandleScope scope;

  constructor = Persistent<FunctionTemplate>::New(FunctionTemplate::New(New));
  constructor->InstanceTemplate()->SetInternalFieldCount(1); // for constructors
  constructor->SetClassName(String::NewSymbol("Map"));

  exports->Set(String::NewSymbol("Map"), constructor->GetFunction());
}

void Map::InitializeFields(Handle<Object> thisObject) {
  Collection<Storage>::InitializeFields(thisObject);

  thisObject->Set(String::NewSymbol("get"), FunctionTemplate::New(Get)->GetFunction());
  thisObject->Set(String::NewSymbol("getAt"), FunctionTemplate::New(GetAt)->GetFunction());
  thisObject->Set(String::NewSymbol("keys"), FunctionTemplate::New(Keys)->GetFunction());
  thisObject->Set(String::NewSymbol("remove"), FunctionTemplate::New(Remove)->GetFunction());
  thisObject->Set(String::NewSymbol("set"), FunctionTemplate::New(Set)->GetFunction());
  thisObject->Set(String::NewSymbol("setAll"), FunctionTemplate::New(SetAll)->GetFunction());
  thisObject->Set(String::NewSymbol("toObject"), FunctionTemplate::New(ToObject)->GetFunction());
  thisObject->Set(String::NewSymbol("toString"), FunctionTemplate::New(ToString)->GetFunction());
}

void Map::InitializeValues(Handle<Object> thisObject, Handle<Value> argument) {
  if (IsSupportedObject(argument)) {
    Map* obj = ObjectWrap::Unwrap<Map>(thisObject);
    Handle<Object> initObject = Handle<Object>::Cast(argument);
    if (Map::constructor->HasInstance(argument)) {
      Map* init = ObjectWrap::Unwrap<Map>(initObject);
      Storage::iterator it = init->storage.begin();
      while (it != init->storage.end()) {
        obj->storage[Persistent<Value>::New(it->first)] = Persistent<Value>::New(it->second);
        it++;
      }
    } else {
      Local<Array> propertyNames = initObject->GetPropertyNames();
      for (size_t i = 0; i < propertyNames->Length(); i++) {
        Local<Value> key = propertyNames->Get((uint32_t) i)->ToString();
        Local<Value> value = initObject->Get(key);
        Storage::iterator it = obj->storage.find((Persistent<Value>) key);
        if (it == obj->storage.end()) {
          obj->storage[Persistent<Value>::New(key)] = Persistent<Value>::New(value);
        } else {
          it->second.Dispose();
          it->second = Persistent<Value>::New(value);
        }
      }
    }
  }
}

bool Map::IsSupportedObject(Handle<Value> value) {
  return value->IsObject();
}

bool Map::IsSupportedType(Handle<Value> value) {
  return IsSupportedObject(value);
}

Handle<Value> Map::New(const Arguments& args) {
  if (!args.IsConstructCall()) {
    return ThrowException(Exception::Error(String::New("Use the new operator to create instances of this object.")));
  }
  bool argError = true;
  if (args.Length() <= 1) {
    if (args[0]->IsUndefined()) {
      argError = false;
    } else if (args[0]->IsObject()) {
      argError = false;
    }
  }
  if (argError) {
    return ThrowException(Exception::Error(String::New("Argument must be an object or omitted.")));
  }

  Map* obj = new Map();
  obj->Wrap(args.This());

  obj->InitializeFields(args.This());
  obj->InitializeValues(args.This(), args[0]);

  return args.This();
}

Handle<Value> Map::Get(const Arguments& args) {
  if (args.Length() == 0) {
    return ThrowException(Exception::Error(String::New("get(key, ...) takes at least one argument.")));
  }

  HandleScope scope;
  Map* obj = ObjectWrap::Unwrap<Map>(args.This());
  if (args.Length() == 1) {
    Storage::iterator it = obj->storage.find((Persistent<Value>) args[0]);
    if (it != obj->storage.end()) {
      return scope.Close(Local<Value>::New(it->second));
    }
  } else {
    Handle<Array> array = Array::New();
    for (int i = 0; i < args.Length(); i++) {
      Handle<Value> arg = args[i];
      Storage::iterator it = obj->storage.find((Persistent<Value>) arg);
      if (it != obj->storage.end()) {
        array->Set(i, Local<Value>::New(it->second));
      }
    }
    return scope.Close(array);
  }
  return Undefined();
}

Handle<Value> Map::GetAt(const Arguments& args) {
  if (args.Length() == 0) {
    return ThrowException(Exception::Error(String::New("getAt(index, ...) takes at least one argument.")));
  }
  for (int i = 0; i < args.Length(); i++) {
    Handle<Value> arg = args[i];
    if (!(arg->IsUndefined()) && !(arg->IsNull()) && !(arg->IsUint32())) {
      return ThrowException(Exception::Error(String::New("getAt(index, ...) takes only integer arguments.")));
    }
  }

  return Collection<Storage>::Get(args);
}

Handle<Value> Map::Keys(const Arguments& args) {
  CHECK_DOES_NOT_TAKE_ARGUMENT(keys, args);

  HandleScope scope;
  Map* obj = ObjectWrap::Unwrap<Map>(args.This());
  Local<Array> array = Array::New();
  Storage::iterator it = obj->storage.begin();
  int i = 0;
  while (it != obj->storage.end()) {
    array->Set(i++, (it++)->first);
  }
  return scope.Close(array);
}

Handle<Value> Map::Remove(const Arguments& args) {
  CHECK_ITERATING(remove, args);
  if (args.Length() == 0) {
    return ThrowException(Exception::Error(String::New("remove(key, ...) takes at least one argument.")));
  }

  HandleScope scope;
  Map* obj = ObjectWrap::Unwrap<Map>(args.This());
  for (int i = 0; i < args.Length(); i++) {
    Handle<Value> arg = args[i];
    Storage::iterator it = obj->storage.find((Persistent<Value>) arg);
    if (it != obj->storage.end()) {
      Persistent<Value> key = it->first;
      Persistent<Value> value = it->second;
      obj->storage.erase(it);
      key.Dispose();
      value.Dispose();
    }
  }
  return scope.Close(args.This());
}

Handle<Value> Map::Set(const Arguments& args) {
  CHECK_ITERATING(set, args);
  if (args.Length() != 2) {
    return ThrowException(Exception::Error(String::New("set(key, value) takes a key and a value.")));
  }

  HandleScope scope;
  Map* obj = ObjectWrap::Unwrap<Map>(args.This());
  Storage::iterator it = obj->storage.find((Persistent<Value>) args[0]);
  if (it == obj->storage.end()) {
    obj->storage[Persistent<Value>::New(args[0])] = Persistent<Value>::New(args[1]);
  } else {
    it->second.Dispose();
    it->second = Persistent<Value>::New(args[1]);
  }

  return args.This();
}

Handle<Value> Map::SetAll(const Arguments& args) {
  CHECK_ITERATING(setAll, args);
  if (args.Length() != 1 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("setAll(object) takes one object argument.")));
  }

  HandleScope scope;
  Map* obj = ObjectWrap::Unwrap<Map>(args.This());
  obj->InitializeValues(args.This(), args[0]);

  return args.This();
}

Handle<Value> Map::ToObject(const Arguments& args) {
  CHECK_DOES_NOT_TAKE_ARGUMENT(toObject, args);

  HandleScope scope;
  Map* obj = ObjectWrap::Unwrap<Map>(args.This());
  Local<Object> result = Object::New();
  Storage::iterator it = obj->storage.begin();
  while (it != obj->storage.end()) {
    result->Set(it->first, Local<Value>::New(it->second));
    it++;
  }
  return scope.Close(result);
}

Handle<Value> Map::ToString(const Arguments& args) {
  CHECK_DOES_NOT_TAKE_ARGUMENT(toString, args);

  HandleScope scope;
  return scope.Close(CollectionUtil::Stringify(ToObject(args)));
}


/*
 * class MapEntry
 */

Persistent<FunctionTemplate> MapEntry::constructor;

void MapEntry::Init(Handle<Object> exports) {
  HandleScope scope;

  constructor = Persistent<FunctionTemplate>::New(FunctionTemplate::New(MapEntry::New));
  constructor->InstanceTemplate()->SetInternalFieldCount(1); // for constructors
  constructor->SetClassName(String::NewSymbol("MapEntry"));

  exports->Set(String::NewSymbol("MapEntry"), constructor->GetFunction());
}

Handle<Value> MapEntry::New(const Arguments& args) {
  if (!args.IsConstructCall()) {
    return ThrowException(Exception::Error(String::New("Use the new operator to create instances of this object.")));
  }
  if (args.Length() != 2) {
    return ThrowException(Exception::Error(String::New("Constructor of MapEntry takes two arguments.")));
  }

  Handle<Object> thisObject = args.This();
  MapEntry* obj = new MapEntry();
  obj->Wrap(thisObject);

  obj->key = Persistent<Value>::New(args[0]);
  obj->value = Persistent<Value>::New(args[1]);

  thisObject->Set(String::NewSymbol("key"), FunctionTemplate::New(GetKey)->GetFunction());
  thisObject->Set(String::NewSymbol("value"), FunctionTemplate::New(GetValue)->GetFunction());
  thisObject->Set(String::NewSymbol("toObject"), FunctionTemplate::New(ToObject)->GetFunction());
  thisObject->Set(String::NewSymbol("toString"), FunctionTemplate::New(ToString)->GetFunction());

  return args.This();
}

Handle<Value> MapEntry::GetKey(const Arguments& args) {
  CHECK_DOES_NOT_TAKE_ARGUMENT(key, args);

  HandleScope scope;
  MapEntry* obj = ObjectWrap::Unwrap<MapEntry>(args.This());
  return scope.Close(Local<Value>::New(obj->key));
}

Handle<Value> MapEntry::GetValue(const Arguments& args) {
  CHECK_DOES_NOT_TAKE_ARGUMENT(value, args);

  HandleScope scope;
  MapEntry* obj = ObjectWrap::Unwrap<MapEntry>(args.This());
  return scope.Close(Local<Value>::New(obj->value));
}

Handle<Value> MapEntry::ToObject(const Arguments& args) {
  CHECK_DOES_NOT_TAKE_ARGUMENT(value, args);

  HandleScope scope;
  MapEntry* obj = ObjectWrap::Unwrap<MapEntry>(args.This());
  Local<Object> result = Object::New();
  result->Set(String::NewSymbol("key"), obj->key);
  result->Set(String::NewSymbol("value"), obj->value);
  return scope.Close(result);
}

Handle<Value> MapEntry::ToString(const Arguments& args) {
  CHECK_DOES_NOT_TAKE_ARGUMENT(toString, args);

  HandleScope scope;
  return scope.Close(CollectionUtil::Stringify(ToObject(args)));
}
