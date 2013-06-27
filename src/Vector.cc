#include <algorithm>
#include "Vector.h"

using namespace std;
using namespace v8;


/*
 * class Vector
 */

Handle<Value> Vector::GetValue(const Persistent<Value>& value) const {
  HandleScope scope;
  return scope.Close(Local<Value>::New(value));
}

void Vector::Init(Handle<Object> exports) {
  HandleScope scope;

  constructor = Persistent<FunctionTemplate>::New(FunctionTemplate::New(New));
  constructor->InstanceTemplate()->SetInternalFieldCount(1); // for constructors
  constructor->SetClassName(String::NewSymbol("Vector"));

  exports->Set(String::NewSymbol("Vector"), constructor->GetFunction());
}

void Vector::InitializeFields(Handle<Object> thisObject) {
  IndexedCollection<Storage>::InitializeFields(thisObject);

  thisObject->Set(String::NewSymbol("remove"), FunctionTemplate::New(Remove)->GetFunction());
  thisObject->Set(String::NewSymbol("reverse"), FunctionTemplate::New(Reverse)->GetFunction());
  thisObject->Set(String::NewSymbol("set"), FunctionTemplate::New(Set)->GetFunction());
  thisObject->Set(String::NewSymbol("each"), FunctionTemplate::New(Each)->GetFunction());
  thisObject->Set(String::NewSymbol("map"), FunctionTemplate::New(Map)->GetFunction());
}

Handle<Value> Vector::New(const Arguments& args) {
  if (!args.IsConstructCall()) {
    return ThrowException(Exception::Error(String::New("Use the new operator to create instances of this object.")));
  }
  Vector* obj = new Vector();
  bool argError = true;
  if (args.Length() <= 1) {
    if (args[0]->IsUndefined()) {
      argError = false;
    } else if (args[0]->IsArray()) {
      argError = false;
    } else if (obj->IsSupportedObject(args[0])) {
      argError = false;
    }
  }
  if (argError) {
    delete obj;
    return ThrowException(Exception::Error(String::New("Argument must be an array, an object, or omitted.")));
  }

  obj->Wrap(args.This());

  obj->InitializeFields(args.This());
  obj->InitializeValues(args.This(), args[0]);

  return args.This();
}

Handle<Value> Vector::Remove(const Arguments& args) {
  CHECK_ITERATING(remove, args);
  if (args.Length() == 0) {
    return ThrowException(Exception::Error(String::New("remove(value, ...) takes at least one argument.")));
  }

  HandleScope scope;
  IndexedCollection<Storage>* obj = ObjectWrap::Unwrap< IndexedCollection<Storage> >(args.This());
  ValueComparator comparator;
  for (int i = 0; i < args.Length(); i++) {
    Handle<Value> arg = args[i];
    Storage::iterator it = obj->storage.begin();
    while (it != obj->storage.end()) {
      if (!comparator(*it, arg) && !comparator(arg, *it)) {
        it->Dispose();
        it = obj->storage.erase(it);
        break;
      } else {
        it++;
      }
    }
  }
  return args.This();
}

Handle<Value> Vector::Reverse(const Arguments& args) {
  CHECK_ITERATING(reverse, args);
  CHECK_DOES_NOT_TAKE_ARGUMENT(reverse, args);

  HandleScope scope;
  Vector* obj = ObjectWrap::Unwrap<Vector>(args.This());
  Storage::iterator left = obj->storage.begin();
  Storage::iterator right = obj->storage.end();
  while (left != right) {
    right--;
    if (left != right) {
      swap(*left, *right);
      left++;
    }
  }
  return args.This();
}

Handle<Value> Vector::Set(const Arguments& args) {
  CHECK_ITERATING(set, args);
  if (args.Length() != 2 || !(args[0]->IsUint32())) {
    return ThrowException(Exception::Error(String::New("set(index, value) takes an integer index and a value.")));
  }
  Vector* obj = ObjectWrap::Unwrap<Vector>(args.This());
  if (args[0]->Uint32Value() > obj->storage.size()) {
    return ThrowException(Exception::Error(String::New("Index is greater than size of this vector.")));
  }

  HandleScope scope;
  if (!args[0]->IsUndefined()) {
    uint32_t index = args[0]->Uint32Value();
    if (index < obj->storage.size()) {
      Storage::iterator it = obj->storage.begin() + index;
      it->Dispose();
      *it = Persistent<Value>::New(args[1]);
    } else {
      obj->storage.push_back(Persistent<Value>::New(args[1]));
    }
  }
  return args.This();
}

Handle<Value> Vector::Each(const Arguments& args) {
  return ObjectWrap::Unwrap<Vector>(args.This())->Iterate(_Each, args);
}

Handle<Value> Vector::_Each(const Arguments& args) {
  if (args.Length() != 1 || !(args[0]->IsFunction())) {
    return ThrowException(Exception::Error(String::New("each(function) takes a function argument.")));
  }

  HandleScope scope;
  Local<Object> global = Context::GetCurrent()->Global();
  Local<Function> function = Local<Function>::Cast(args[0]);
  TryCatch tryCatch;
  Vector* obj = ObjectWrap::Unwrap<Vector>(args.This());
  Local<Object> modifierValue = VectorModifier::constructor->GetFunction()->NewInstance(0, NULL);
  Storage::iterator it = obj->storage.begin();
  size_t i = 0;
  bool isFirst = true;
  while (it != obj->storage.end()) {
    Persistent<Value> value = *it;
    VectorModifier* modifier = ObjectWrap::Unwrap<VectorModifier>(modifierValue);
    modifier->isFirst = isFirst;
    modifier->isLast = it + 1 == obj->storage.end();
    modifier->index = i;
    isFirst = false;
    Handle<Value> parameters[2];
    parameters[0] = value;
    parameters[1] = modifierValue;
    Handle<Value> result = function->Call(global, 2, parameters);
    if (result.IsEmpty()) {
      modifier->clear(true);
      return ThrowException(tryCatch.Exception());
    }
    if (!modifier->replace.IsEmpty()) {
      it->Dispose();
      *it = modifier->replace;
    }
    if (modifier->insertedBefore.size() > 0) {
      obj->storage.insert(it, modifier->insertedBefore.begin(), modifier->insertedBefore.end());
      i += modifier->insertedBefore.size();
      it = obj->storage.begin() + i;
    }
    if (modifier->removed) {
      it->Dispose();
      obj->storage.erase(it);
    } else {
      it++;
      i++;
    }
    if (modifier->insertedAfter.size() > 0) {
      obj->storage.insert(it, modifier->insertedAfter.begin(), modifier->insertedAfter.end());
      i += modifier->insertedAfter.size();
      it = obj->storage.begin() + i;
    }
    modifier->clear(false);
    if (result->IsFalse()) {
      break;
    }
  }
  return args.This();
}

Handle<Value> Vector::Map(const Arguments& args) {
  return ObjectWrap::Unwrap<Vector>(args.This())->Iterate(_Map, args);
}

Handle<Value> Vector::_Map(const Arguments& args) {
  if (args.Length() != 1 || !(args[0]->IsFunction())) {
    return ThrowException(Exception::Error(String::New("map(function) takes a function argument.")));
  }

  HandleScope scope;
  Local<Object> global = Context::GetCurrent()->Global();
  Local<Function> function = Local<Function>::Cast(args[0]);
  TryCatch tryCatch;
  Vector* obj = ObjectWrap::Unwrap<Vector>(args.This());
  Storage::iterator it = obj->storage.begin();
  while (it != obj->storage.end()) {
    Persistent<Value> parameters[1];
    parameters[0] = *it;
    Handle<Value> result = function->Call(global, 1, parameters);
    if (result.IsEmpty()) {
      return ThrowException(tryCatch.Exception());
    }
    it->Dispose();
    *it++ = Persistent<Value>::New(result);
  }
  return args.This();
}


/*
 * class VectorModifier
 */

Persistent<FunctionTemplate> VectorModifier::constructor;

void VectorModifier::Init(Handle<Object> exports) {
  HandleScope scope;

  constructor = Persistent<FunctionTemplate>::New(FunctionTemplate::New(VectorModifier::New));
  constructor->InstanceTemplate()->SetInternalFieldCount(1); // for constructors
  constructor->SetClassName(String::NewSymbol("VectorModifier"));

  exports->Set(String::NewSymbol("VectorModifier"), constructor->GetFunction());
}

void VectorModifier::clear(bool dispose) {
  if (dispose) {
    replace.Dispose();
    Vector::Storage::iterator it = insertedBefore.begin();
    while (it != insertedBefore.end()) {
      (it++)->Dispose();
    }
    it = insertedAfter.begin();
    while (it != insertedAfter.end()) {
      (it++)->Dispose();
    }
  }

  replace = Persistent<Value>();
  removed = false;
  insertedBefore.clear();
  insertedAfter.clear();
}

VectorModifier::VectorModifier() {
  clear(false);
}

VectorModifier::~VectorModifier() {
  // Do not dispose the persistent handles, because they are being used by the vector.
}

Handle<Value> VectorModifier::New(const Arguments& args) {
  if (!args.IsConstructCall()) {
    return ThrowException(Exception::Error(String::New("Use the new operator to create instances of this object.")));
  }

  VectorModifier* obj = new VectorModifier();
  obj->Wrap(args.This());

  args.This()->Set(String::NewSymbol("isFirst"), FunctionTemplate::New(IsFirst)->GetFunction());
  args.This()->Set(String::NewSymbol("isLast"), FunctionTemplate::New(IsLast)->GetFunction());
  args.This()->Set(String::NewSymbol("index"), FunctionTemplate::New(Index)->GetFunction());

  args.This()->Set(String::NewSymbol("set"), FunctionTemplate::New(Set)->GetFunction());
  args.This()->Set(String::NewSymbol("remove"), FunctionTemplate::New(Remove)->GetFunction());
  args.This()->Set(String::NewSymbol("insertBefore"), FunctionTemplate::New(InsertBefore)->GetFunction());
  args.This()->Set(String::NewSymbol("insertAfter"), FunctionTemplate::New(InsertAfter)->GetFunction());

  return args.This();
}

Handle<Value> VectorModifier::IsFirst(const Arguments& args) {
  CHECK_DOES_NOT_TAKE_ARGUMENT(isFirst, args);

  HandleScope scope;
  VectorModifier* obj = ObjectWrap::Unwrap<VectorModifier>(args.This());
  return scope.Close(Boolean::New(obj->isFirst));
}

Handle<Value> VectorModifier::IsLast(const Arguments& args) {
  CHECK_DOES_NOT_TAKE_ARGUMENT(isLast, args);

  HandleScope scope;
  VectorModifier* obj = ObjectWrap::Unwrap<VectorModifier>(args.This());
  return scope.Close(Boolean::New(obj->isLast));
}

Handle<Value> VectorModifier::Index(const Arguments& args) {
  CHECK_DOES_NOT_TAKE_ARGUMENT(index, args);

  HandleScope scope;
  VectorModifier* obj = ObjectWrap::Unwrap<VectorModifier>(args.This());
  return scope.Close(Uint32::New((uint32_t) obj->index));
}

Handle<Value> VectorModifier::Set(const Arguments& args) {
  if (args.Length() != 1) {
    return ThrowException(Exception::Error(String::New("set(value) takes a function argument.")));
  }

  HandleScope scope;
  VectorModifier* obj = ObjectWrap::Unwrap<VectorModifier>(args.This());
  obj->replace = Persistent<Value>::New(args[0]);
  obj->removed = false;
  return args.This();
}

Handle<Value> VectorModifier::Remove(const Arguments& args) {
  CHECK_DOES_NOT_TAKE_ARGUMENT(remove, args);

  HandleScope scope;
  VectorModifier* obj = ObjectWrap::Unwrap<VectorModifier>(args.This());
  obj->removed = true;
  if (!obj->replace.IsEmpty()) {
    obj->replace.Dispose();
    obj->replace = Persistent<Value>();
  }
  return args.This();
}

Handle<Value> VectorModifier::InsertBefore(const Arguments& args) {
  if (args.Length() == 0) {
    return ThrowException(Exception::Error(String::New("insertBefore(value, ...) takes at least one argument.")));
  }

  HandleScope scope;
  VectorModifier* obj = ObjectWrap::Unwrap<VectorModifier>(args.This());
  for (int i = 0; i < args.Length(); i++) {
    obj->insertedBefore.push_back(Persistent<Value>::New(args[i]));
  }
  return args.This();
}

Handle<Value> VectorModifier::InsertAfter(const Arguments& args) {
  if (args.Length() == 0) {
    return ThrowException(Exception::Error(String::New("insertAfter(value, ...) takes at least one argument.")));
  }

  HandleScope scope;
  VectorModifier* obj = ObjectWrap::Unwrap<VectorModifier>(args.This());
  for (int i = 0; i < args.Length(); i++) {
    obj->insertedAfter.push_back(Persistent<Value>::New(args[i]));
  }
  return args.This();
}


template class InternalVector<Persistent<Value>, ValueComparator>;