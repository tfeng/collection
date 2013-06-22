"use strict";

var assert = require("assert"),
    Vector = require("../lib/collection").Vector

describe('Vector', function(){
  var array1, array2, array3;
  var v1, v2, v3;

  beforeEach(function() {
    array1 = [1,2,3,4,5,6,7,8,9,10];
    array2 = ["abc","def","g"];
    array3 = [];
    v1 = new Vector(array1);
    v2 = new Vector(array2);
    v3 = new Vector();
  });

  describe("#size", function() {
    it("should return sizes of the vectors", function() {
      assert.equal(v1.size(), 10);
      assert.equal(v2.size(), 3);
      assert.equal(v3.size(), 0);
      assert.equal(new Vector(v1).size(), 10);
    });

    it("should throw error if argument is provided", function() {
      assert.throws(function() {
        v1.size(1);
      }, Error)
    });
  });

  describe("#isEmpty", function() {
    it("should return whether a vector is empty", function() {
      assert.equal(v1.isEmpty(), false);
      assert.equal(v2.isEmpty(), false);
      assert.equal(v3.isEmpty(), true);
    });
  });

  describe("#toArray", function() {
    it("should return arrays for the vectors", function() {
      assert.deepEqual(v1.toArray(), array1);
      assert.deepEqual(v2.toArray(), array2);
      assert.deepEqual(v3.toArray(), array3);

      assert.deepEqual(new Vector(v1).toArray(), array1);
      assert.deepEqual(new Vector(v2).toArray(), array2);
      assert.deepEqual(new Vector(v3).toArray(), array3);
    });
  });

  describe("#toString", function() {
    it("should convert the vectors into strings", function() {
      assert.equal(v1.toString(), array1.toString());
      assert.equal(v2.toString(), array2.toString());
      assert.equal(v3.toString(), array3.toString());
      assert.equal(new Vector(v2).toString(), array2.toString());
    });

    it("should throw error if argument is provided", function() {
      assert.throws(function() {
        v2.toString("abc");
      }, Error);
    });
  });

  describe("#equals", function() {
    it("should compare equality of this vector with another one", function() {
      assert(v1.equals(new Vector(array1)));
      assert(!v1.equals(new Vector([1,2,3])));
      assert(!v1.equals(new Vector()));
      assert(v1.equals(v1));
      assert(v2.equals(new Vector(v2.toArray())));
      assert(v3.equals(new Vector([])));
      assert(v3.equals(new Vector()));

      var newVector = new Vector([1,2,3]);
      v1.add(newVector);
      array1.push(newVector);
      assert(v1.equals(new Vector(array1)));

      v2.add(newVector);
      array2.push(new Vector(newVector));
      assert(v2.equals(new Vector(array2)));
    });

    it("should throw error if not exactly one argument is provided", function() {
      assert.throws(function() {
        v1.equals();
      }, Error);
      assert.throws(function() {
        v1.equals(1, 2);
      }, Error);
    });
  });

  describe("#add", function() {
    it("should add new elements to the end of the vectors", function() {
      assert.deepEqual(v1.add(11, 12, 13, 14, 15).toArray(), [1,2,3,4,5,6,7,8,9,10,11,12,13,14,15]);
      assert.deepEqual(v1.toArray(), [1,2,3,4,5,6,7,8,9,10,11,12,13,14,15]);
      assert.deepEqual(v1.add("a", "b", "c").toArray(), [1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,"a","b","c"]);
      assert.deepEqual(v2.add([1,2,3]).toArray(), ["abc","def","g",[1,2,3]]);
    });

    it("should throw error if argument is not provided", function() {
      assert.throws(function() {
        v1.add();
      }, Error);
    });
  });

  describe("#get", function() {
    it("should get elements in each vector one by one", function() {
      for (var i = 0; i < v1.size(); i++) {
        assert.equal(v1.get(i), array1[i]);
      }
      for (var i = 0; i < v2.size(); i++) {
        assert.equal(v2.get(i), array2[i]);
      }
      for (var i = 0; i < v3.size(); i++) {
        assert.equal(v3.get(i), array3[i]);
      }
    });

    it("should throw error if input is not unsigned integer", function() {
      assert.throws(function() {
        v1.get("abc");
      }, Error);
      assert.throws(function() {
        v1.get([]);
      }, Error);
      assert.throws(function() {
        v1.get(-1);
      }, Error);
    });

    it("should get multiple elements from a vector at a time", function() {
      assert.deepEqual(v1.get(0, 1, 2, 3), [1,2,3,4]);
      assert.deepEqual(v1.get(1, 3, 5, 7, 9), [2,4,6,8,10]);
      assert.deepEqual(v1.get(undefined), []);
      assert.deepEqual(v1.get(1, undefined, 3, undefined, 5, null, 7, null, 9, 999), [2,4,6,8,10]);
    });
  });

  describe("#clear", function() {
    it("should erase all the elements in the vectors", function() {
      assert.deepEqual(v1.clear().toArray(), []);
      assert.deepEqual(v2.clear().toArray(), []);
      assert.deepEqual(v3.clear().toArray(), []);
      assert.deepEqual(v1.toArray(), []);
      assert.deepEqual(v2.toArray(), []);
      assert.deepEqual(v3.toArray(), []);
    });

    it("should throw error if argument is provided", function() {
      assert.throws(function() {
        v1.clear(1);
      }, Error);
    });
  });

  describe("#index", function() {
    it("should return indexes of elements in the vectors", function() {
      for (var i = 0; i < v1.size(); i++) {
        assert.equal(v1.index(i + 1), i);
      }
      for (var i = 0; i < v2.size(); i++) {
        assert.equal(v2.index(array2[i]), i);
      }
      for (var i = 0; i < v3.size(); i++) {
        assert.equal(v3.index(array3[i]), i);
      }
    });
  });

  describe("#set", function() {
    it("should set elements to new values", function() {
      assert.deepEqual(v1.set(0, "a").set(1, "b").set(2, "c").toArray(), ["a","b","c",4,5,6,7,8,9,10]);
      assert.deepEqual(v2.set(2, "xyz").set(3, "!@#").toArray(), ["abc","def","xyz"]);
      assert.deepEqual(v3.set(0, 1).set(1, 2).set(2, 3).toArray(), []);
    });

    it("should throw error if index is not unsigned integer", function() {
      assert.throws(function() {
        v1.set("a", 0);
      }, Error);
      assert.throws(function() {
        v1.set(-1, 0);
      }, Error);
      assert.throws(function() {
        v1.set(undefined, 0);
      }, Error);
    });
  });

  describe("#reverse", function() {
    it("should reverse all the elements in the vectors", function() {
      assert.deepEqual(v1.reverse().toArray(), array1.reverse());
      assert.deepEqual(v2.reverse().toArray(), array2.reverse());
      assert.deepEqual(v3.reverse().toArray(), array3.reverse());

      assert.deepEqual(v1.toArray(), array1);
      assert.deepEqual(v2.toArray(), array2);
      assert.deepEqual(v3.toArray(), array3);
    });
  });

  describe("#remove", function() {
    it("should remove elements at the specified indexes of the vectors", function() {
      assert.deepEqual(v1.remove(1, 3, 5).remove(7,9).toArray(), [2,4,6,8,10]);
      assert.deepEqual(v2.remove("abc", "def").toArray(), ["g"]);
      assert.deepEqual(v3.remove(0, 1).toArray(), []);
    });

    it("should allow undefined and null arguments", function() {
      var array1old = v1.toArray();
      assert.deepEqual(v1.add(undefined).remove(undefined).toArray(), array1old);
      assert.deepEqual(v1.add(null).remove(null).toArray(), array1old);
    });
  });

  describe("#removeAt", function() {
    it("should remove elements at the specified indexes of the vectors", function() {
      assert.deepEqual(v1.removeAt(1, 3, 5).removeAt(7,9).toArray(), [1,3,5,7,8,9,10]);
      assert.deepEqual(v2.removeAt(0, 1, 2).toArray(), []);
      assert.deepEqual(v3.removeAt(0, 1).toArray(), []);
    });

    it("should throw error if index is not unsigned integer", function() {
      assert.throws(function() {
        v1.removeAt("a", 0);
      }, Error);
      assert.throws(function() {
        v1.removeAt(-1, 0);
      }, Error);
    });

    it("should allow undefined and null arguments", function() {
      assert.deepEqual(v1.removeAt(undefined).toArray(), v1.toArray());
      assert.deepEqual(v1.removeAt(null).toArray(), v1.toArray());
    });
  });

  describe("#removeRange", function() {
    it("should remove elements from start (inclusive) to end (exclusive) indexes", function() {
      assert.deepEqual(v1.removeRange(1, 5).toArray(), [1,6,7,8,9,10]);
      assert.deepEqual(v1.removeRange(2, 3).toArray(), [1,6,8,9,10]);
      assert.deepEqual(v1.removeRange(3, 999).toArray(), [1,6,8]);
      assert.deepEqual(v3.removeRange(3, 999).toArray(), []);
    });

    it("should throw error if an index is missing or it is not unsigned integer", function() {
      assert.throws(function() {
        v1.removeRange(-1, 1);
      }, Error);
      assert.throws(function() {
        v1.removeRange(0, "a");
      }, Error);
      assert.throws(function() {
        v1.removeRange(0, "a");
      }, Error);
      assert.throws(function() {
        v1.removeRange("a", 1);
      }, Error);
    });
  });

  describe("#removeLast", function() {
    it("should remove elements in the vector (from the end) one by one", function() {
      while (v1.size() > 0) {
        array1.pop();
        assert.deepEqual(v1.removeLast().toArray(), array1);
      }
      while (v2.size() > 0) {
        array2.pop();
        assert.deepEqual(v2.removeLast().toArray(), array2);
      }
      while (v3.size() > 0) {
        array3.pop();
        assert.deepEqual(v3.removeLast().toArray(), array3);
      }
      assert.equal(v1.size(), 0);
      assert.equal(v2.size(), 0);
      assert.equal(v3.size(), 0);
      assert.deepEqual(v1.removeLast().toArray(), []);
      assert.deepEqual(v2.removeLast().toArray(), []);
      assert.deepEqual(v3.removeLast().toArray(), []);
    });

    it("should throw error if argument is provided", function() {
      assert.throws(function() {
        v1.removeLast(1);
      }, Error);
      assert.throws(function() {
        v1.removeLast([]);
      }, Error);
    });
  });

  describe("#each", function() {
    it("should go through all the elements in the vectors", function() {
      var i1 = 0;
      v1.each(function(v) {
        assert.equal(v, array1[i1++]);
      });

      var i2 = 0;
      v2.each(function(v) {
        assert.equal(v, array2[i2++]);
      });

      var i3 = 0;
      v3.each(function(v) {
        assert.equal(v, array3[i3++]);
      });
    });

    it("should use modifier to test properties of the iteration", function() {
      var i1 = 0;
      v1.each(function(v, m) {
        assert.equal(m.isFirst(), i1 == 0);
        assert.equal(m.isLast(), i1 == array1.length - 1);
        assert.equal(m.index(), i1++);
      });

      var i2 = 0;
      v2.each(function(v, m) {
        assert.equal(m.isFirst(), i2 == 0);
        assert.equal(m.isLast(), i2 == array2.length - 1);
        assert.equal(m.index(), i2++);
      });

      var i3 = 0;
      v3.each(function(v, m) {
        assert.equal(m.isFirst(), i3 == 0);
        assert.equal(m.isLast(), i3 == array3.length - 1);
        assert.equal(m.index(), i3++);
      });
    });

    it("should set values of the vectors by using modifier", function() {
      v1.each(function(v, m) {
        m.set(array1[0] + array1[array1.length - 1] - v);
      });
      assert.deepEqual(v1.toArray(), array1.reverse());

      var i2 = 0;
      v2.each(function(v, m) {
        // The last set() overrides previous remove()'s and set()'s.
        m.set(array2[array2.length - i2++ - 1]);
      });
      assert.deepEqual(v2.toArray(), array2.reverse());

      v3.each(function() {
        assert(false);
      });
    });

    it("should set/remove value with the last set()/remove() call to the modifier", function() {
      v1.each(function(v, m) {
        // The last set() overrides previous remove()'s and set()'s.
        m.remove().set(0).set(array1[0] + array1[array1.length - 1] - v);
      });
      assert.deepEqual(v1.toArray(), array1.reverse());

      v1.each(function(v, m) {
        // The last remove() overrides previous set()'s.
        m.set(0).set(v + 1).remove();
      });
      assert.deepEqual(v1.toArray(), []);

      v1.each(function(v, m) {
        m.set(0).set(v - 1).remove().remove();
      });
      assert.deepEqual(v1.toArray(), []);
    });

    it("should insert new elements before the current value by using modifier", function() {
      v1.each(function(v, m) {
        if (v % 2 == 0) {
          m.insertBefore(v / 2);
        }
      });
      assert.deepEqual(v1.toArray(), [1,1,2,3,2,4,5,3,6,7,4,8,9,5,10]);

      v1.each(function(v, m) {
        if (v % 3 == 0) {
          m.insertBefore(v / 3, v * 2 / 3);
        }
      });
      assert.deepEqual(v1.toArray(), [1,1,2,1,2,3,2,4,5,1,2,3,2,4,6,7,4,8,3,6,9,5,10]);

      v1.each(function(v, m) {
        if (v % 4 == 0) {
          m.insertBefore(v / 4).insertBefore(v / 2).insertBefore(v * 3 / 4);
        }
      });
      assert.deepEqual(v1.toArray(), [1,1,2,1,2,3,2,1,2,3,4,5,1,2,3,2,1,2,3,4,6,7,1,2,3,4,2,4,6,8,3,6,9,5,10]);
    });

    it("should insert new elements after the current value by using modifier", function() {
      v1.each(function(v, m) {
        if (v % 2 == 0) {
          m.insertAfter(v / 2);
        }
      });
      assert.deepEqual(v1.toArray(), [1,2,1,3,4,2,5,6,3,7,8,4,9,10,5]);

      v1.each(function(v, m) {
        if (v % 3 == 0) {
          m.insertAfter(v * 2 / 3, v / 3);
        }
      });
      assert.deepEqual(v1.toArray(), [1,2,1,3,2,1,4,2,5,6,4,2,3,2,1,7,8,4,9,6,3,10,5]);

      v1.each(function(v, m) {
        if (v % 4 == 0) {
          m.insertAfter(v * 3 / 4).insertBefore(v / 2).insertBefore(v / 4);
        }
      });
      assert.deepEqual(v1.toArray(), [1,2,1,3,2,1,2,1,4,3,2,5,6,2,1,4,3,2,3,2,1,7,4,2,8,6,2,1,4,3,9,6,3,10,5]);
    });

    it("should handle insert before/after and set by using the same modifier", function() {
      v1.each(function(v, m) {
        if (v % 3 == 0) {
          m.insertBefore(v / 3, v * 2 / 3);
          m.insertAfter(v * 2 / 3, v / 3);
          m.remove();
        }
      });
      assert.deepEqual(v1.toArray(), [1,2,1,2,2,1,4,5,2,4,4,2,7,8,3,6,6,3,10]);

      v1.each(function(v, m) {
        if (v % 2 == 0) {
          m.insertBefore(v / 2);
          m.insertAfter(v / 2);
          m.set(0);
        }
      });
      assert.deepEqual(v1.toArray(), [1,1,0,1,1,1,0,1,1,0,1,1,2,0,2,5,1,0,1,2,0,2,2,0,2,1,0,1,7,4,0,4,3,3,0,3,3,0,3,3,5,0,5]);
    });

    it("should tolerate errors in the callback function", function() {
      assert.throws(function() {
        v1.each(function() {
          notExisting == 1;
        });
      }, Error);

      assert.throws(function() {
        v1.each(function() {
          throw new Error();
        });
      }, Error);

      assert.throws(function() {
        v1.each(function(v, m) {
          m.doesNotExist();
        });
      }, Error);
    });

    it("should not allow state-changing functions to be invoked during iteration", function() {
      assert.throws(function() {
        v1.each(function() {
          v1.add(1);
        });
      }, Error);
      assert.throws(function() {
        v1.each(function() {
          v1.addAll([1]);
        });
      }, Error);
      assert.throws(function() {
        v1.each(function() {
          v1.clear();
        });
      }, Error);
      assert.throws(function() {
        v1.each(function() {
          v1.set(0, 0);
        });
      }, Error);
      assert.throws(function() {
        v1.each(function() {
          v1.reverse();
        });
      }, Error);
      assert.throws(function() {
        v1.each(function() {
          v1.remove(1);
        });
      }, Error);
      assert.throws(function() {
        v1.each(function() {
          v1.removeAt(1);
        });
      }, Error);
      assert.throws(function() {
        v1.each(function() {
          v1.removeRange(0, 1);
        });
      }, Error);
      assert.throws(function() {
        v1.each(function() {
          v1.removeLast();
        });
      }, Error);
    });
  });
});
