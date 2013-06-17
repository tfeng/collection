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

  afterEach(function() {
  });

  describe("#length", function() {
    it("should return lengths of the vectors", function() {
      assert.equal(v1.length(), 10);
      assert.equal(v2.length(), 3);
      assert.equal(v3.length(), 0);
      assert.equal(new Vector(v1).length(), 10);
    });

    it("should throw error if argument is provided", function() {
      assert.throws(function() {
        v1.length(1);
      }, Error)
    });
  });

  describe("#inspect", function() {
    it("should return inspect string for the vectors", function() {
      assert.equal(v1.inspect(), "[1,2,3,4,5,6,7,8,9,10]");
      assert.equal(v2.inspect(), "[\"abc\",\"def\",\"g\"]");
      assert.equal(v3.inspect(), "[]");
      assert.equal(new Vector(v1).inspect(), "[1,2,3,4,5,6,7,8,9,10]");
    });
  });

  describe("#toArray", function() {
    it("should return arrays for the vectors", function() {
      assert.deepEqual(v1.toArray(), array1);
      assert.deepEqual(v2.toArray(), array2);
      assert.deepEqual(v3.toArray(), array3);
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

      v1.add(new Vector([1,2,3]));
      array1.push(new Vector([1,2,3]));
      // BE AWARE: Two distinct vectors are never equal using JS ==. vector.equals() is not deep equal.
      assert(!v1.equals(new Vector(array1)));
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
      for (var i = 0; i < v1.length(); i++) {
        assert.equal(v1.get(i), array1[i]);
      }
      for (var i = 0; i < v2.length(); i++) {
        assert.equal(v2.get(i), array2[i]);
      }
      for (var i = 0; i < v3.length(); i++) {
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

  describe("#remove", function() {
    it("should remove elements at the specified indexes of the vectors", function() {
      assert.deepEqual(v1.remove(1, 3, 5).remove(7,9).toArray(), [1,3,5,7,8,9,10]);
      assert.deepEqual(v2.remove(0, 1, 2).toArray(), []);
      assert.deepEqual(v3.remove(0, 1).toArray(), []);
    });

    it("should throw error if index is not unsigned integer", function() {
      assert.throws(function() {
        v1.remove("a", 0);
      }, Error);
      assert.throws(function() {
        v1.remove(-1, 0);
      }, Error);
    });

    it("should allow undefined and null arguments", function() {
      assert.deepEqual(v1.remove(undefined).toArray(), v1.toArray());
      assert.deepEqual(v1.remove(null).toArray(), v1.toArray());
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
      while (v1.length() > 0) {
        array1.pop();
        assert.deepEqual(v1.removeLast().toArray(), array1);
      }
      while (v2.length() > 0) {
        array2.pop();
        assert.deepEqual(v2.removeLast().toArray(), array2);
      }
      while (v3.length() > 0) {
        array3.pop();
        assert.deepEqual(v3.removeLast().toArray(), array3);
      }
      assert.equal(v1.length(), 0);
      assert.equal(v2.length(), 0);
      assert.equal(v3.length(), 0);
      assert.deepEqual(v1.removeLast(), []);
      assert.deepEqual(v2.removeLast(), []);
      assert.deepEqual(v3.removeLast(), []);
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

});
