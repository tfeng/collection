"use strict";

var assert = require("assert"),
    Map = require("../lib/collection").Map,
    Set = require("../lib/collection").Set,
    Vector = require("../lib/collection").Vector;

describe('Set', function() {
  var array1, array2, array3;
  var s1, s2, s3;

  beforeEach(function() {
    array1 = [1,2,3,4,5,6,7,8,9,10];
    array2 = ["abc","def","g"];
    array3 = [];
    s1 = new Set(array1);
    s2 = new Set(array2);
    s3 = new Set();
  });

  describe("#add", function() {
    it("should add new elements to the sets", function() {
      assert.deepEqual(s1.add(8, 9, 10, 11, 12, 13, 14, 15).toArray(), [1,2,3,4,5,6,7,8,9,10,11,12,13,14,15]);
      assert.deepEqual(s1.toArray(), [1,2,3,4,5,6,7,8,9,10,11,12,13,14,15]);
      assert.deepEqual(s1.add("a", "b", "c", "a", "b", "c").toArray(), [1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,"a","b","c"]);

      // Add twice, and the result should be the same.
      assert.deepEqual(s2.add([1,2,3]).toArray(), ["abc","def","g",[1,2,3]]);
      assert.deepEqual(s2.add([1,2,3]).toArray(), ["abc","def","g",[1,2,3]]);
    });

    it("should not add duplicated elements to the sets", function() {
      s1.add(1, 2, 3, 4, 5, 6, 6, 6, 6);
      s2.add("abc", "ab");
      assert.equal(s1.size(), 10);
      assert.equal(s2.size(), 4);
      assert.equal(s3.size(), 0);
    });

    it("should throw error if argument is not provided", function() {
      assert.throws(function() {
        s1.add();
      }, Error);
    });
  });

  describe("#addAll", function() {
    it("should add all elements of an array to a set", function() {
      assert.deepEqual(s1.addAll([8,9,10,11,12,13,14,15]).toArray(), [1,2,3,4,5,6,7,8,9,10,11,12,13,14,15]);
      assert.deepEqual(s2.addAll(["f","g","h"]).toArray(), ["abc","def","f","g","h"]);
    });

    it("should add all elements of a vector to a set", function() {
      assert.deepEqual(s1.addAll(new Vector([8,9,10,11,12,13,14,15])).toArray(), [1,2,3,4,5,6,7,8,9,10,11,12,13,14,15]);
      assert.deepEqual(s2.addAll(new Vector(["f","g","h"])).toArray(), ["abc","def","f","g","h"]);
    });

    it("should add all elements of a set to a set", function() {
      assert.deepEqual(s1.addAll(new Set([8,9,10,11,12,13,14,15])).toArray(), [1,2,3,4,5,6,7,8,9,10,11,12,13,14,15]);
      assert.deepEqual(s2.addAll(new Set(["f","g","h"])).toArray(), ["abc","def","f","g","h"]);
    });

    it("should throw error if argument is not provided", function() {
      assert.throws(function() {
        s1.addAll();
      }, Error);
    });

    it("should throw error if argument is not array, vector or set", function() {
      assert.throws(function() {
        s1.addAll(1,2,3);
      }, Error);
      assert.throws(function() {
        s1.addAll("a", "b");
      }, Error);
      assert.throws(function() {
        s1.addAll(new Map());
      }, Error);
    });
  });

  describe("#clear", function() {
    it("should erase all the elements in the sets", function() {
      assert.deepEqual(s1.clear().toArray(), []);
      assert.deepEqual(s2.clear().toArray(), []);
      assert.deepEqual(s3.clear().toArray(), []);
      assert.deepEqual(s1.toArray(), []);
      assert.deepEqual(s2.toArray(), []);
      assert.deepEqual(s3.toArray(), []);
    });

    it("should throw error if argument is provided", function() {
      assert.throws(function() {
        s1.clear(1);
      }, Error);
    });
  });

  describe("#each", function() {
    it("should go through all the elements in the sets", function() {
      var i1 = 0;
      s1.each(function(v) {
        assert.equal(v, array1[i1++]);
      });

      var i2 = 0;
      s2.each(function(v) {
        assert.equal(v, array2[i2++]);
      });

      var i3 = 0;
      s3.each(function(v) {
        assert.equal(v, array3[i3++]);
      });
    });

    it("should tolerate errors in the callback function", function() {
      assert.throws(function() {
        s1.each(function() {
          notExisting == 1;
        });
      }, Error);

      assert.throws(function() {
        s1.each(function() {
          throw new Error();
        });
      }, Error);

      assert.throws(function() {
        s1.each(function(v) {
          v.doesNotExist();
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
          v1.remove(1);
        });
      }, Error);
    });
  });

  describe("#equals", function() {
    it("should compare equality of this set with another one", function() {
      assert(s1.equals(new Set(array1)));
      assert(s1.equals(new Set(array1.reverse())));
      assert(!s1.equals(new Set([1,2,3])));
      assert(!s1.equals(new Set()));
      assert(s1.equals(s1));
      assert(s2.equals(new Set(s2.toArray())));
      assert(s3.equals(new Set([])));
      assert(s3.equals(new Set()));

      var newSet = new Set([1,2,3]);
      s1.addAll(newSet.toArray());
      assert(s1.equals(new Set(array1)));

      s2.add(newSet);
      array2.push(new Set(newSet));
      assert(s2.equals(new Set(array2)));
    });

    it("should throw error if not exactly one argument is provided", function() {
      assert.throws(function() {
        s1.equals();
      }, Error);
      assert.throws(function() {
        s1.equals(1, 2);
      }, Error);
    });
  });

  describe("#filter", function() {
    it("should return all the values in the set that satisfy the requirement", function() {
      assert.deepEqual(s1.filter(function(v) {
        return v % 2 == 0;
      }), [2,4,6,8,10]);
      assert.deepEqual(s1.filter(function(v) {
        return v < 0;
      }), []);
      assert.deepEqual(s2.filter(function(v) {
        return v.length < 2;
      }), ["g"]);
    });

    it("should throw error if a function argument is not given", function() {
      assert.throws(function() {
        s1.filter();
      }, Error);
      assert.throws(function() {
        s1.filter(1);
      }, Error);
      assert.throws(function() {
        s1.filter(function(){}, 1);
      }, Error);
    });
  });

  describe("#find", function() {
    it("should find the first value in the set that satisfies the requirement", function() {
      assert.equal(s1.find(function(v) {
        return v % 2 == 0;
      }), 2);
      assert.equal(s1.find(function(v) {
        return v < 0;
      }), undefined);
      assert.equal(s2.find(function(v) {
        return v.length < 2;
      }), "g");
    });

    it("should throw error if a function argument is not given", function() {
      assert.throws(function() {
        s1.find();
      }, Error);
      assert.throws(function() {
        s1.find(1);
      }, Error);
      assert.throws(function() {
        s1.find(function(){}, 1);
      }, Error);
    });
  });

  describe("#get", function() {
    it("should get elements in each set one by one", function() {
      for (var i = 0; i < s1.size(); i++) {
        assert.equal(s1.get(i), array1[i]);
      }
      for (var i = 0; i < s2.size(); i++) {
        assert.equal(s2.get(i), array2[i]);
      }
      for (var i = 0; i < s3.size(); i++) {
        assert.equal(s3.get(i), array3[i]);
      }
    });

    it("should throw error if input is not unsigned integer", function() {
      assert.throws(function() {
        s1.get("abc");
      }, Error);
      assert.throws(function() {
        s1.get([]);
      }, Error);
      assert.throws(function() {
        s1.get(-1);
      }, Error);
    });

    it("should get multiple elements from a set at a time", function() {
      assert.deepEqual(s1.get(0, 1, 2, 3), [1,2,3,4]);
      assert.deepEqual(s1.get(1, 3, 5, 7, 9), [2,4,6,8,10]);
      assert.deepEqual(s1.get(undefined), undefined);
      assert.deepEqual(s1.get(1, undefined, 3, undefined, 5, null, 7, null, 9, 999), [2,,4,,6,,8,,10]);
    });
  });

  describe("#has", function() {
    it("should return existence of values in a set", function() {
      assert(s1.has(1));
      assert(s1.has(3));
      assert(!s1.has(11));
      assert(!s2.has(1));
      assert(s2.has("abc"));
      assert(s1.has(1, 2, 11), [true, true, false]);
    });

    it("should throw error if no argument is provided", function() {
      assert.throws(function() {
        s1.has();
      }, Error);
    });
  });

  describe("#index", function() {
    it("should return indexes of elements in the sets", function() {
      for (var i = 0; i < s1.size(); i++) {
        assert.equal(s1.index(i + 1), i);
      }
      for (var i = 0; i < s2.size(); i++) {
        assert.equal(s2.index(array2[i]), i);
      }
      for (var i = 0; i < s3.size(); i++) {
        assert.equal(s3.index(array3[i]), i);
      }
    });

    it("should return indexes of multiple elements at a time", function() {
      assert.deepEqual(s1.index(1,3,5,7), [0,2,4,6]);
      assert.deepEqual(s1.index(1,3,5,7,9,11,13), [0,2,4,6,8]);
      assert.deepEqual(s2.index("a","b","g","abc","f"), [,,2,0]);
    });
  });

  describe("#isEmpty", function() {
    it("should return whether a set is empty", function() {
      assert.equal(s1.isEmpty(), false);
      assert.equal(s2.isEmpty(), false);
      assert.equal(s3.isEmpty(), true);
    });
  });

  describe("#reduce", function() {
    it("should reduce a set into a single value", function() {
      assert.equal(s1.reduce(function(memo, v) {
        return memo + v;
      }, 0), 55);

      assert.equal(s2.reduce(function(memo, v) {
        return memo + v;
      }, ""), "abcdefg");
    });

    it("should throw error if a function argument is not given", function() {
      assert.throws(function() {
        s1.reduce();
      }, Error);
      assert.throws(function() {
        s1.reduce(1);
      }, Error);
      assert.throws(function() {
        s1.reduce(function(){}, 1, 2);
      }, Error);
    });
  });

  describe("#reduceRight", function() {
    it("should reduce a set from the right into a single value", function() {
      assert.equal(s1.reduceRight(function(memo, v) {
        return memo + v;
      }, 0), 55);

      assert.equal(s2.reduceRight(function(memo, v) {
        return memo + v;
      }, ""), "gdefabc");
    });

    it("should throw error if a function argument is not given", function() {
      assert.throws(function() {
        s1.reduceRight();
      }, Error);
      assert.throws(function() {
        s1.reduceRight(1);
      }, Error);
      assert.throws(function() {
        s1.reduceRight(function(){}, 1, 2);
      }, Error);
    });
  });

  describe("#remove", function() {
    it("should remove elements at the specified indexes of the sets", function() {
      assert.deepEqual(s1.remove(1, 3, 5).remove(7,9).toArray(), [2,4,6,8,10]);
      assert.deepEqual(s2.remove("abc", "def").toArray(), ["g"]);
      assert.deepEqual(s3.remove(0, 1).toArray(), []);
    });

    it("should allow undefined and null arguments", function() {
      var array1old = s1.toArray();
      assert.deepEqual(s1.add(undefined).remove(undefined).toArray(), array1old);
      assert.deepEqual(s1.add(null).remove(null).toArray(), array1old);
    });
  });

  describe("#removeAt", function() {
    it("should remove elements at the specified indexes of the sets", function() {
      assert.deepEqual(s1.removeAt(1, 3, 5).removeAt(7,9).toArray(), [1,3,5,7,8,9,10]);
      assert.deepEqual(s2.removeAt(0, 1, 2).toArray(), []);
      assert.deepEqual(s3.removeAt(0, 1).toArray(), []);
    });

    it("should throw error if index is not unsigned integer", function() {
      assert.throws(function() {
        s1.removeAt("a", 0);
      }, Error);
      assert.throws(function() {
        s1.removeAt(-1, 0);
      }, Error);
    });

    it("should allow undefined and null arguments", function() {
      assert.deepEqual(s1.removeAt(undefined).toArray(), s1.toArray());
      assert.deepEqual(s1.removeAt(null).toArray(), s1.toArray());
    });
  });

  describe("#removeLast", function() {
    it("should remove elements in the vector (from the end) one by one", function() {
      while (s1.size() > 0) {
        array1.pop();
        assert.deepEqual(s1.removeLast().toArray(), array1);
      }
      while (s2.size() > 0) {
        array2.pop();
        assert.deepEqual(s2.removeLast().toArray(), array2);
      }
      while (s3.size() > 0) {
        array3.pop();
        assert.deepEqual(v3.removeLast().toArray(), array3);
      }
      assert.equal(s1.size(), 0);
      assert.equal(s2.size(), 0);
      assert.equal(s3.size(), 0);
      assert.deepEqual(s1.removeLast().toArray(), []);
      assert.deepEqual(s2.removeLast().toArray(), []);
      assert.deepEqual(s3.removeLast().toArray(), []);
    });

    it("should throw error if argument is provided", function() {
      assert.throws(function() {
        s1.removeLast(1);
      }, Error);
      assert.throws(function() {
        s1.removeLast([]);
      }, Error);
    });
  });

  describe("#removeRange", function() {
    it("should remove elements from start (inclusive) to end (exclusive) indexes", function() {
      assert.deepEqual(s1.removeRange(1, 5).toArray(), [1,6,7,8,9,10]);
      assert.deepEqual(s1.removeRange(2, 3).toArray(), [1,6,8,9,10]);
      assert.deepEqual(s1.removeRange(3, 999).toArray(), [1,6,8]);
      assert.deepEqual(s3.removeRange(3, 999).toArray(), []);
    });

    it("should throw error if an index is missing or it is not unsigned integer", function() {
      assert.throws(function() {
        s1.removeRange(-1, 1);
      }, Error);
      assert.throws(function() {
        s1.removeRange(0, "a");
      }, Error);
      assert.throws(function() {
        s1.removeRange(0, "a");
      }, Error);
      assert.throws(function() {
        s1.removeRange("a", 1);
      }, Error);
    });
  });

  describe("#size", function() {
    it("should return sizes of the sets", function() {
      assert.equal(s1.size(), 10);
      assert.equal(s2.size(), 3);
      assert.equal(s3.size(), 0);
      assert.equal(new Set(s1).size(), 10);
    });

    it("should throw error if argument is provided", function() {
      assert.throws(function() {
        s1.size(1);
      }, Error)
    });
  });

  describe("#toArray", function() {
    it("should return arrays for the sets", function() {
      assert.deepEqual(s1.toArray(), array1);
      assert.deepEqual(s2.toArray(), array2);
      assert.deepEqual(s3.toArray(), array3);

      assert.deepEqual(new Set(s1).toArray(), array1);
      assert.deepEqual(new Set(s2).toArray(), array2);
      assert.deepEqual(new Set(s3).toArray(), array3);
    });
  });

  describe("#toString", function() {
    it("should convert the sets into strings", function() {
      assert.equal(s1.toString(), JSON.stringify(array1));
      assert.equal(s2.toString(), JSON.stringify(array2));
      assert.equal(s3.toString(), JSON.stringify(array3));
      assert.equal(new Set(s2).toString(), JSON.stringify(array2));
    });

    it("should throw error if argument is provided", function() {
      assert.throws(function() {
        s2.toString("abc");
      }, Error);
    });
  });
});
