#include "Vector.h"

using namespace v8;

extern "C" void InitAll(Handle<Object> exports) {
  Vector::Init(exports);
  VectorModifier::Init(exports);
}

NODE_MODULE(NativeTypes, InitAll)
