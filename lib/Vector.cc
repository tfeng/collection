#include "Vector.h"

using namespace v8;

#define CHECK_DOES_NOT_TAKE_ARGUMENT(method) \
do { \
  if (args.Length() > 0) { \
    return ThrowException(Exception::Error(String::New(#method "() does not take any argument."))); \
  } \
} while (false)


/*
 * class Vector
 */

Persistent<FunctionTemplate> Vector::constructor;

Vector::Vector() {
}

Vector::~Vector() {
  std::vector< Persistent<Value> >::iterator it = vector.begin();
  while (it != vector.end()) {
    (it++)->Dispose();
  }
}

void Vector::Init(Handle<Object> exports) {
  HandleScope scope;

  constructor = Persistent<FunctionTemplate>::New(FunctionTemplate::New(Vector::New));
  constructor->InstanceTemplate()->SetInternalFieldCount(1); // for constructors
  constructor->SetClassName(String::NewSymbol("Vector"));

  NODE_SET_PROTOTYPE_METHOD(constructor, "length", Length);
  NODE_SET_PROTOTYPE_METHOD(constructor, "inspect", Inspect);
  NODE_SET_PROTOTYPE_METHOD(constructor, "toArray", ToArray);
  NODE_SET_PROTOTYPE_METHOD(constructor, "toString", ToString);
  NODE_SET_PROTOTYPE_METHOD(constructor, "equals", Equals);

  NODE_SET_PROTOTYPE_METHOD(constructor, "add", Add);
  NODE_SET_PROTOTYPE_METHOD(constructor, "get", Get);
  NODE_SET_PROTOTYPE_METHOD(constructor, "set", Set);
  NODE_SET_PROTOTYPE_METHOD(constructor, "remove", Remove);
  NODE_SET_PROTOTYPE_METHOD(constructor, "removeRange", RemoveRange);
  NODE_SET_PROTOTYPE_METHOD(constructor, "removeLast", RemoveLast);
  NODE_SET_PROTOTYPE_METHOD(constructor, "clear", Clear);

  NODE_SET_PROTOTYPE_METHOD(constructor, "each", Each);
  NODE_SET_PROTOTYPE_METHOD(constructor, "map", Map);
  NODE_SET_PROTOTYPE_METHOD(constructor, "reduce", Reduce);
  NODE_SET_PROTOTYPE_METHOD(constructor, "reduceRight", ReduceRight);
  NODE_SET_PROTOTYPE_METHOD(constructor, "find", Find);
  NODE_SET_PROTOTYPE_METHOD(constructor, "filter", Filter);

  exports->Set(String::NewSymbol("Vector"), constructor->GetFunction());
}

bool Vector::HasInstance(Handle<Value> value) {
  return value->IsObject() && constructor->HasInstance(Handle<Object>::Cast(value));
}

Handle<Value> Vector::New(const Arguments& args) {
  if (!args.IsConstructCall()) {
    return ThrowException(Exception::Error(String::New("Use the new operator to create instances of this object.")));
  }
  bool argError = true;
  if (args.Length() <= 1) {
    if (args[0]->IsUndefined()) {
      argError = false;
    } else if (args[0]->IsArray()) {
      argError = false;
    } else if (HasInstance(args[0])) {
      argError = false;
    }
  }
  if (argError) {
    return ThrowException(Exception::Error(String::New("Argument must be an array, another vector, or omitted.")));
  }

  Vector* obj = new Vector();
  if (args[0]->IsArray()) {
    Handle<Array> array = Handle<Array>::Cast(args[0]);
    for (uint32_t i = 0; i < array->Length(); i++) {
      obj->vector.push_back(Persistent<Value>::New(array->Get(i)));
    }
  } else if (args[0]->IsObject()) {
    Vector* other = ObjectWrap::Unwrap<Vector>(Handle<Object>::Cast(args[0]));
    std::vector< Persistent<Value> >::iterator it = other->vector.begin();
    while (it != other->vector.end()) {
      obj->vector.push_back(Persistent<Value>::New(*it++));
    }
  }
  obj->Wrap(args.This());
  return args.This();
}

Handle<Value> Vector::Length(const Arguments& args) {
  CHECK_DOES_NOT_TAKE_ARGUMENT("length");

  HandleScope scope;
  Vector* obj = ObjectWrap::Unwrap<Vector>(args.This());
  return scope.Close(Number::New(obj->vector.size()));
}

Handle<Value> Vector::Inspect(const Arguments& args) {
  HandleScope scope;
  Local<Object> global = Context::GetCurrent()->Global();
  Local<Object> JSON = Local<Object>::Cast(global->Get(String::New("JSON")));
  Local<Function> stringify = Local<Function>::Cast(JSON->Get(String::New("stringify")));
  Vector* obj = ObjectWrap::Unwrap<Vector>(args.This());
  Handle<Value> stringifyArgs[] = { obj->ToArray(args) };
  return scope.Close(stringify->Call(Context::GetCurrent()->Global(), 1, stringifyArgs));
}

Handle<Value> Vector::ToArray(const Arguments& args) {
  HandleScope scope;
  Vector* obj = ObjectWrap::Unwrap<Vector>(args.This());
  Local<Array> array = Array::New(obj->vector.size());
  std::vector< Persistent<Value> >::iterator it = obj->vector.begin();
  for (uint32_t i = 0; it != obj->vector.end(); it++, i++) {
    array->Set(i, Local<Value>::New(*it));
  }
  return scope.Close(array);
}

Handle<Value> Vector::ToString(const Arguments& args) {
  CHECK_DOES_NOT_TAKE_ARGUMENT("toString");

  HandleScope scope;
  Vector* obj = ObjectWrap::Unwrap<Vector>(args.This());
  return scope.Close(obj->ToArray(args)->ToString());
}

Handle<Value> Vector::Equals(const Arguments& args) {
  if (args.Length() != 1) {
    return ThrowException(Exception::Error(String::New("equals(value) takes one argument.")));
  }

  HandleScope scope;
  if (HasInstance(args[0])) {
    Vector* obj1 = ObjectWrap::Unwrap<Vector>(args.This());
    std::vector< Persistent<Value> >::iterator it1 = obj1->vector.begin();
    Vector* obj2 = ObjectWrap::Unwrap<Vector>(Handle<Object>::Cast(args[0]));
    std::vector< Persistent<Value> >::iterator it2 = obj2->vector.begin();
    while (it1 != obj1->vector.end() && it2 != obj2->vector.end()) {
      Persistent<Value> v1 = *it1++;
      Persistent<Value> v2 = *it2++;
      if (!v1->Equals(v2)) {
        return scope.Close(Boolean::New(false));
      }
    }
    if (it1 != obj1->vector.end() || it2 != obj2->vector.end()) {
      return scope.Close(Boolean::New(false));
    } else {
      return scope.Close(Boolean::New(true));
    }
  } else {
    return scope.Close(Boolean::New(false));
  }
}

Handle<Value> Vector::Add(const Arguments& args) {
  if (args.Length() == 0) {
    return ThrowException(Exception::Error(String::New("add(value) takes at least one argument.")));
  }

  HandleScope scope;
  Vector* obj = ObjectWrap::Unwrap<Vector>(args.This());
  for (int i = 0; i < args.Length(); i++) {
    obj->vector.push_back(Persistent<Value>::New(args[i]));
  }
  return args.This();
}

Handle<Value> Vector::Get(const Arguments& args) {
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
  Vector* obj = ObjectWrap::Unwrap<Vector>(args.This());
  if (args.Length() == 1 && args[0]->IsUint32()) {
    uint32_t index = args[0]->Uint32Value();
    if (index < obj->vector.size()) {
      return scope.Close(Local<Value>::New(obj->vector[index]));
    }
  } else {
    Handle<Array> array = Array::New();
    for (int i = 0, j = 0; i < args.Length(); i++) {
      Handle<Value> arg = args[i];
      if (arg->IsUint32()) {
        uint32_t index = arg->Uint32Value();
        if (index < obj->vector.size()) {
          array->Set(j++, Local<Value>::New(obj->vector[index]));
        }
      }
    }
    return scope.Close(array);
  }
  return Undefined();
}

Handle<Value> Vector::Set(const Arguments& args) {
  if (args.Length() != 2 || !(args[0]->IsUint32())) {
    return ThrowException(Exception::Error(String::New("set(index, value) takes an integer index and a value.")));
  }

  HandleScope scope;
  Vector* obj = ObjectWrap::Unwrap<Vector>(args.This());
  if (!args[0]->IsUndefined()) {
    uint32_t index = args[0]->Uint32Value();
    if (index < obj->vector.size()) {
      std::vector< Persistent<Value> >::iterator it = obj->vector.begin() + index;
      it->Dispose();
      *it = Persistent<Value>::New(args[1]);
    }
  }
  return args.This();
}

Handle<Value> Vector::Remove(const Arguments& args) {
  if (args.Length() == 0) {
    return ThrowException(Exception::Error(String::New("remove(index, ...) takes at least one argument.")));
  }
  for (int i = 0; i < args.Length(); i++) {
    Handle<Value> arg = args[i];
    if (!(arg->IsUndefined()) && !(arg->IsNull()) && !(arg->IsUint32())) {
      return ThrowException(Exception::Error(String::New("remove(index, ...) takes only integer arguments.")));
    }
  }

  HandleScope scope;
  Vector* obj = ObjectWrap::Unwrap<Vector>(args.This());
  for (int i = 0, removed = 0; i < args.Length(); i++) {
    Handle<Value> arg = args[i];
    if (arg->IsUint32()) {
      uint32_t index = arg->Uint32Value() - removed;
      if (index < obj->vector.size()) {
        std::vector< Persistent<Value> >::iterator it = obj->vector.begin() + index;
        it->Dispose();
        obj->vector.erase(it);
        removed++;
      }
    }
  }
  return args.This();
}

Handle<Value> Vector::RemoveRange(const Arguments& args) {
  if (args.Length() != 2) {
    return ThrowException(Exception::Error(String::New("removeRange(start, end) takes two arguments.")));
  }
  if (!(args[0]->IsUint32()) || !(args[1]->IsUint32())) {
    return ThrowException(Exception::Error(String::New("removeRange(start, end) takes only integer arguments.")));
  }

  HandleScope scope;
  Vector* obj = ObjectWrap::Unwrap<Vector>(args.This());
  uint32_t start = args[0]->Uint32Value();
  uint32_t end = args[1]->Uint32Value();
  if (end > obj->vector.size()) {
    end = obj->vector.size();
  }
  if (obj->vector.size() == 0 || start >= obj->vector.size() || end <= start) {
    return args.This();
  }

  std::vector< Persistent<Value> >::iterator it = obj->vector.begin() + start;
  while (it != obj->vector.begin() + end && it != obj->vector.end()) {
    (it++)->Dispose();
  }

  obj->vector.erase(obj->vector.begin() + start, obj->vector.begin() + end);
  return args.This();
}

Handle<Value> Vector::RemoveLast(const Arguments& args) {
  if (args.Length() > 0) {
    return ThrowException(Exception::Error(String::New("removeLast() does not take any argument.")));
  }

  HandleScope scope;
  Vector* obj = ObjectWrap::Unwrap<Vector>(args.This());
  if (obj->vector.size() > 0) {
    (obj->vector.end() - 1)->Dispose();
    obj->vector.pop_back();
  }
  return args.This();
}

Handle<Value> Vector::Clear(const Arguments& args) {
  CHECK_DOES_NOT_TAKE_ARGUMENT("clear");

  HandleScope scope;
  Vector* obj = ObjectWrap::Unwrap<Vector>(args.This());

  std::vector< Persistent<Value> >::iterator it = obj->vector.begin();
  while (it != obj->vector.end()) {
    (it++)->Dispose();
  }

  obj->vector.clear();
  return args.This();
}

Handle<Value> Vector::Each(const Arguments& args) {
  if (args.Length() != 1 || !(args[0]->IsFunction())) {
    return ThrowException(Exception::Error(String::New("each(function) takes a function argument.")));
  }

  HandleScope scope;
  Local<Object> global = Context::GetCurrent()->Global();
  Local<Function> function = Local<Function>::Cast(args[0]);
  TryCatch tryCatch;
  Vector* obj = ObjectWrap::Unwrap<Vector>(args.This());
  Local<Object> modifierValue = VectorModifier::constructor->GetFunction()->NewInstance(0, (Handle<Value>[]){});
  std::vector< Persistent<Value> >::iterator it = obj->vector.begin();
  int i = 0;
  bool isFirst = true;
  while (it != obj->vector.end()) {
    Persistent<Value> value = *it;
    VectorModifier* modifier = ObjectWrap::Unwrap<VectorModifier>(modifierValue);
    modifier->isFirst = isFirst;
    modifier->isLast = it + 1 == obj->vector.end();
    modifier->index = i;
    isFirst = false;
    Handle<Value> result = function->Call(global, 2, (Handle<Value>[]){value, modifierValue});
    if (result.IsEmpty()) {
      modifier->clear(true);
      return ThrowException(tryCatch.Exception());
    }
    if (!modifier->replace.IsEmpty()) {
      it->Dispose();
      *it = modifier->replace;
    }
    if (modifier->insertedBefore.size() > 0) {
      obj->vector.insert(it, modifier->insertedBefore.begin(), modifier->insertedBefore.end());
      i += modifier->insertedBefore.size();
      it = obj->vector.begin() + i;
    }
    if (modifier->removed) {
      it->Dispose();
      obj->vector.erase(it);
    } else {
      it++;
      i++;
    }
    if (modifier->insertedAfter.size() > 0) {
      obj->vector.insert(it, modifier->insertedAfter.begin(), modifier->insertedAfter.end());
      i += modifier->insertedAfter.size();
      it = obj->vector.begin() + i;
    }
    modifier->clear(false);
  }
  return args.This();
}

Handle<Value> Vector::Map(const Arguments& args) {
  if (args.Length() != 1 || !(args[0]->IsFunction())) {
    return ThrowException(Exception::Error(String::New("map(function) takes a function argument.")));
  }

  HandleScope scope;
  Local<Object> global = Context::GetCurrent()->Global();
  Local<Function> function = Local<Function>::Cast(args[0]);
  TryCatch tryCatch;
  Vector* obj = ObjectWrap::Unwrap<Vector>(args.This());
  std::vector< Persistent<Value> >::iterator it = obj->vector.begin();
  while (it != obj->vector.end()) {
    Persistent<Value> value = *it;
    Handle<Value> result = function->Call(global, 1, (Handle<Value>[]){value});
    if (result.IsEmpty()) {
      return ThrowException(tryCatch.Exception());
    }
    it->Dispose();
    *it++ = Persistent<Value>::New(result);
  }
  return args.This();
}

Handle<Value> Vector::Reduce(const Arguments& args) {
  if (args.Length() != 2 || !(args[0]->IsFunction())) {
    return ThrowException(Exception::Error(String::New("reduce(function, memo) takes a function argument and a memo argument.")));
  }

  HandleScope scope;
  Local<Object> global = Context::GetCurrent()->Global();
  Local<Function> function = Local<Function>::Cast(args[0]);
  TryCatch tryCatch;
  Handle<Value> memo = args[1];
  Vector* obj = ObjectWrap::Unwrap<Vector>(args.This());
  std::vector< Persistent<Value> >::iterator it = obj->vector.begin();
  while (it != obj->vector.end()) {
    Persistent<Value> value = *it++;
    memo = function->Call(global, 2, (Handle<Value>[]){memo, value});
    if (memo.IsEmpty()) {
      return ThrowException(tryCatch.Exception());
    }
  }
  return scope.Close(memo);
}

Handle<Value> Vector::ReduceRight(const Arguments& args) {
  if (args.Length() != 2 || !(args[0]->IsFunction())) {
    return ThrowException(Exception::Error(String::New("reduceRight(function, memo) takes a function argument and a memo argument.")));
  }

  HandleScope scope;
  Local<Object> global = Context::GetCurrent()->Global();
  Local<Function> function = Local<Function>::Cast(args[0]);
  TryCatch tryCatch;
  Handle<Value> memo = args[1];
  Vector* obj = ObjectWrap::Unwrap<Vector>(args.This());
  std::vector< Persistent<Value> >::iterator it = obj->vector.end();
  while (it != obj->vector.begin()) {
    Persistent<Value> value = *--it;
    memo = function->Call(global, 2, (Handle<Value>[]){memo, value});
    if (memo.IsEmpty()) {
      return ThrowException(tryCatch.Exception());
    }
  }
  return scope.Close(memo);
}

Handle<Value> Vector::Find(const Arguments& args) {
  if (args.Length() != 1 || !(args[0]->IsFunction())) {
    return ThrowException(Exception::Error(String::New("find(function) takes a function argument.")));
  }

  HandleScope scope;
  Local<Object> global = Context::GetCurrent()->Global();
  Local<Function> function = Local<Function>::Cast(args[0]);
  TryCatch tryCatch;
  Vector* obj = ObjectWrap::Unwrap<Vector>(args.This());
  std::vector< Persistent<Value> >::iterator it = obj->vector.begin();
  while (it != obj->vector.end()) {
    Persistent<Value> value = *it++;
    Handle<Value> result = function->Call(global, 1, (Handle<Value>[]){value});
    if (result.IsEmpty()) {
      return ThrowException(tryCatch.Exception());
    } else if (result->IsTrue()) {
      return value;
    }
  }
  return Undefined();
}

Handle<Value> Vector::Filter(const Arguments& args) {
  if (args.Length() != 1 || !(args[0]->IsFunction())) {
    return ThrowException(Exception::Error(String::New("find(function) takes a function argument.")));
  }

  HandleScope scope;
  Local<Object> global = Context::GetCurrent()->Global();
  Local<Function> function = Local<Function>::Cast(args[0]);
  TryCatch tryCatch;
  Vector* obj = ObjectWrap::Unwrap<Vector>(args.This());
  Local<Array> array = Array::New();
  std::vector< Persistent<Value> >::iterator it = obj->vector.begin();
  int i = 0;
  while (it != obj->vector.end()) {
    Persistent<Value> value = *it++;
    Handle<Value> result = function->Call(global, 1, (Handle<Value>[]){value});
    if (result.IsEmpty()) {
      return ThrowException(tryCatch.Exception());
    } else if (result->IsTrue()) {
      array->Set(i++, Local<Value>::New(value));
    }
  }
  return scope.Close(array);
}


/*
 * class VectorModifier
 */

Persistent<FunctionTemplate> VectorModifier::constructor;

VectorModifier::VectorModifier() {
  clear(false);
}

VectorModifier::~VectorModifier() {
}

void VectorModifier::Init(Handle<Object> exports) {
  HandleScope scope;

  constructor = Persistent<FunctionTemplate>::New(FunctionTemplate::New(VectorModifier::New));
  constructor->InstanceTemplate()->SetInternalFieldCount(1); // for constructors
  constructor->SetClassName(String::NewSymbol("VectorModifier"));

  exports->Set(String::NewSymbol("VectorModifier"), constructor->GetFunction());
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

void VectorModifier::clear(bool dispose) {
  if (dispose) {
    replace.Dispose();
    std::vector< Persistent<Value> >::iterator it = insertedBefore.begin();
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

Handle<Value> VectorModifier::IsFirst(const Arguments& args) {
  CHECK_DOES_NOT_TAKE_ARGUMENT("isFirst");

  HandleScope scope;
  VectorModifier* obj = ObjectWrap::Unwrap<VectorModifier>(args.This());
  return scope.Close(Boolean::New(obj->isFirst));
}

Handle<Value> VectorModifier::IsLast(const Arguments& args) {
  CHECK_DOES_NOT_TAKE_ARGUMENT("isLast");

  HandleScope scope;
  VectorModifier* obj = ObjectWrap::Unwrap<VectorModifier>(args.This());
  return scope.Close(Boolean::New(obj->isLast));
}

Handle<Value> VectorModifier::Index(const Arguments& args) {
  CHECK_DOES_NOT_TAKE_ARGUMENT("index");

  HandleScope scope;
  VectorModifier* obj = ObjectWrap::Unwrap<VectorModifier>(args.This());
  return scope.Close(Uint32::New(obj->index));
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
  CHECK_DOES_NOT_TAKE_ARGUMENT("remove");

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