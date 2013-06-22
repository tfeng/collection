"use strict";

var assert = require("assert"),
    Set = require("../lib/collection").Set

describe('Set', function(){
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

  describe("#isEmpty", function() {
    it("should return whether a set is empty", function() {
      assert.equal(s1.isEmpty(), false);
      assert.equal(s2.isEmpty(), false);
      assert.equal(s3.isEmpty(), true);
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
      assert.equal(s1.toString(), array1.toString());
      assert.equal(s2.toString(), array2.toString());
      assert.equal(s3.toString(), array3.toString());
      assert.equal(new Set(s2).toString(), array2.toString());
    });

    it("should throw error if argument is provided", function() {
      assert.throws(function() {
        s2.toString("abc");
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
      assert.deepEqual(s1.get(undefined), []);
      assert.deepEqual(s1.get(1, undefined, 3, undefined, 5, null, 7, null, 9, 999), [2,4,6,8,10]);
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
});
