#include "Map.h"
#include "Set.h"
#include "Vector.h"

using namespace v8;

extern "C" void InitAll(Handle<Object> exports) {
  Map::Init(exports);
  MapEntry::Init(exports);
  Set::Init(exports);
  Vector::Init(exports);
  VectorModifier::Init(exports);
}

NODE_MODULE(NativeTypes, InitAll)
