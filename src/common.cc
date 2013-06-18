#include "common.h"
#include "Vector.h"

using namespace v8;


/*
 * class SetComparator
 */
bool ValueComparator::operator() (const Handle<Value>& value1, const Handle<Value>& value2) const {
  ValueComparator comparator;

  if (value1.IsEmpty()) {
    return !value2.IsEmpty();
  }

  if (value1->IsFalse()) {
    return !value2->IsFalse();
  }

  if (value1->IsTrue()) {
    return !value2->IsTrue();
  }

  if (value1->IsNumber()) {
    if (value2->IsNumber()) {
      return value1->ToNumber()->Value() < value2->ToNumber()->Value();
    } else {
      return true;
    }
  }

  if (value1->IsDate()) {
    if (value2->IsDate()) {
      return Handle<Date>::Cast(value1)->NumberValue() < Handle<Date>::Cast(value2)->NumberValue();
    } else {
      return true;
    }
  }

  if (value1->IsString()) {
    if (value2->IsString()) {
      Handle<String> s1 = value1->ToString();
      Handle<String> s2 = value2->ToString();
      int len1 = s1->Utf8Length();
      int len2 = s2->Utf8Length();
      int min = len1 < len2 ? len1 : len2;
      char buffer1[min];
      char buffer2[min];
      s1->WriteUtf8(buffer1, min);
      s2->WriteUtf8(buffer2, min);

      char *p1 = buffer1, *p2 = buffer2;
      for (int i = 0; i < min; i++) {
        char c1 = *p1++;
        char c2 = *p2++;
        if (c1 < c2) {
          return true;
        } else if (c1 > c2) {
          return false;
        }
      }

      return len1 < len2;
    } else {
      return true;
    }
  }

  if (value1->IsArray()) {
    if (value2->IsArray()) {
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
    } else {
      return true;
    }
  }

  if (Vector::constructor->HasInstance(value1)) {
    if (Vector::constructor->HasInstance(value2)) {
      Vector* vector1 = node::ObjectWrap::Unwrap<Vector>(Handle<Object>::Cast(value1));
      Vector* vector2 = node::ObjectWrap::Unwrap<Vector>(Handle<Object>::Cast(value2));
      std::vector< Persistent<Value> >::iterator it1 = vector1->vector.begin();
      std::vector< Persistent<Value> >::iterator it2 = vector2->vector.begin();
      while (it1 != vector1->vector.end() && it2 != vector2->vector.end()) {
        Handle<Value> element1 = *it1++;
        Handle<Value> element2 = *it2++;
        if (comparator(element1, element2)) {
          return true;
        } else if (comparator(element2, element1)) {
          return false;
        }
      }
      return it1 == vector1->vector.end() && it2 != vector2->vector.end();
    } else {
      return true;
    }
  }

  return false;
}
