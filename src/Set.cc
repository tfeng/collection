#include "Set.h"

using namespace v8;


/*
 * class Set
 */

Persistent<FunctionTemplate> Set::constructor;

Set::Set() {
}

Set::~Set() {
  std::set< Persistent<Value> >::const_iterator it = set.begin();
  while (it != set.end()) {
    (const_cast< Persistent<Value>& >(*it)).Dispose();
  }
}

void Set::Init(Handle<Object> exports) {
  HandleScope scope;

  constructor = Persistent<FunctionTemplate>::New(FunctionTemplate::New(Set::New));
  constructor->InstanceTemplate()->SetInternalFieldCount(1); // for constructors
  constructor->SetClassName(String::NewSymbol("Set"));

  NODE_SET_PROTOTYPE_METHOD(constructor, "size", Size);
  NODE_SET_PROTOTYPE_METHOD(constructor, "inspect", Inspect);
  NODE_SET_PROTOTYPE_METHOD(constructor, "isEmpty", IsEmpty);
  NODE_SET_PROTOTYPE_METHOD(constructor, "toArray", ToArray);

  exports->Set(String::NewSymbol("Set"), constructor->GetFunction());
}

bool Set::HasInstance(Handle<Value> value) {
  return value->IsObject() && constructor->HasInstance(Handle<Object>::Cast(value));
}

Handle<Value> Set::New(const Arguments& args) {
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
    return ThrowException(Exception::Error(String::New("Argument must be an array, another set, or omitted.")));
  }

  Set* obj = new Set();
  if (args[0]->IsArray()) {
    Handle<Array> array = Handle<Array>::Cast(args[0]);
    for (uint32_t i = 0; i < array->Length(); i++) {
      obj->set.insert(Persistent<Value>::New(array->Get(i)));
    }
  } else if (args[0]->IsObject()) {
    Set* other = ObjectWrap::Unwrap<Set>(Handle<Object>::Cast(args[0]));
    std::set< Persistent<Value> >::const_iterator it = other->set.begin();
    while (it != other->set.end()) {
      obj->set.insert(Persistent<Value>::New(*it++));
    }
  }
  obj->Wrap(args.This());
  return args.This();
}

Handle<Value> Set::Size(const Arguments& args) {
  CHECK_DOES_NOT_TAKE_ARGUMENT("size");

  HandleScope scope;
  Set* obj = ObjectWrap::Unwrap<Set>(args.This());
  return scope.Close(Number::New(obj->set.size()));
}

Handle<Value> Set::Inspect(const Arguments& args) {
  HandleScope scope;
  Local<Object> global = Context::GetCurrent()->Global();
  Local<Object> JSON = Local<Object>::Cast(global->Get(String::New("JSON")));
  Local<Function> stringify = Local<Function>::Cast(JSON->Get(String::New("stringify")));
  Set* obj = ObjectWrap::Unwrap<Set>(args.This());
  Handle<Value> stringifyArgs[] = { obj->ToArray(args) };
  return scope.Close(stringify->Call(global, 1, stringifyArgs));
}

Handle<Value> Set::IsEmpty(const Arguments& args) {
  CHECK_DOES_NOT_TAKE_ARGUMENT("isEmpty");

  HandleScope scope;
  Set* obj = ObjectWrap::Unwrap<Set>(args.This());
  return scope.Close(Boolean::New(obj->set.empty()));
}

Handle<Value> Set::ToArray(const Arguments& args) {
  HandleScope scope;
  Set* obj = ObjectWrap::Unwrap<Set>(args.This());
  Local<Array> array = Array::New(obj->set.size());
  std::set< Persistent<Value> >::const_iterator it = obj->set.begin();
  for (uint32_t i = 0; it != obj->set.end(); it++, i++) {
    array->Set(i, Local<Value>::New(*it));
  }
  return scope.Close(array);
}

