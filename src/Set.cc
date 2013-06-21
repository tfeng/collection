#include "Set.h"

using namespace std;
using namespace v8;

typedef SetStorage Storage;


/*
 * class Set
 */

void Set::Init(Handle<Object> exports) {
  HandleScope scope;

  constructor = Persistent<FunctionTemplate>::New(FunctionTemplate::New(Set::New));
  constructor->InstanceTemplate()->SetInternalFieldCount(1); // for constructors
  constructor->SetClassName(String::NewSymbol("Set"));

  exports->Set(String::NewSymbol("Set"), constructor->GetFunction());
}

void Set::InitializeFields(Handle<Object> thisObject) {
  Collection<SetStorage>::InitializeFields(thisObject);

  thisObject->Set(String::NewSymbol("remove"), FunctionTemplate::New(Remove)->GetFunction());
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
    } else if (CollectionUtil::IsSupportedObject(args[0])) {
      argError = false;
    }
  }
  if (argError) {
    return ThrowException(Exception::Error(String::New("Argument must be an array, an object, or omitted.")));
  }

  Set* obj = new Set();
  obj->Wrap(args.This());

  obj->InitializeFields(args.This());
  obj->InitializeValues(args.This(), args[0]);

  return args.This();
}

Handle<Value> Set::Remove(const Arguments& args) {
  CHECK_ITERATING("remove", args);
  if (args.Length() == 0) {
    return ThrowException(Exception::Error(String::New("remove(value, ...) takes at least one argument.")));
  }

  HandleScope scope;
  Set* obj = ObjectWrap::Unwrap<Set>(args.This());
  for (int i = 0; i < args.Length(); i++) {
    Handle<Value> arg = args[i];
    SetStorage::iterator it = obj->storage.find((Persistent<Value>) arg);
    if (it != obj->storage.end()) {
      obj->storage.erase(it);
    }
  }
  return args.This();
}
