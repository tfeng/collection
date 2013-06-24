"use strict";

var assert = require("assert"),
    Map = require("../lib/collection").Map;

describe('Set', function() {
  var o1, o2, o3, o4, m1, m2, m3, m4;

  beforeEach(function() {
    o1 = {"1": "a", "2": "b", "3": "c", "4": "d", "5": "e"};
    o2 = {"1": "a", "2": "b", "3": "c", "4": "d", "5": "e", "6": "f", "7": "g", "8": "h", "9": "i"};
    o3 = {"x": "a", "y": "b", "z": "c"};
    o4 = {};
    m1 = new Map(o1);
    m2 = new Map(o2);
    m3 = new Map(o3);
    m4 = new Map(o4);
  });

  describe("#size", function() {
    it("should return sizes of the maps", function() {
      assert.equal(m1.size(), 5);
      assert.equal(m2.size(), 9);
      assert.equal(m3.size(), 3);
      assert.equal(m4.size(), 0);
      assert.equal(new Map(m1).size(), 5);
    });

    it("should throw error if argument is provided", function() {
      assert.throws(function() {
        m1.size(1);
      }, Error)
    });
  });

  describe("#has", function() {
    it("should return existence of values in a set", function() {
      assert(m1.has("1"));
      assert(m1.has("3"));
      assert(!m1.has("11"));
      assert(m2.has("1"));
      assert(!m2.has("a"));
      assert(!m2.has("x"));
      assert(m3.has("x"));
      assert(m3.has("y"));
      assert(m1.has("1", "2", "11"), [true, true, false]);
      assert(m3.has("a", "b", "c", "x", "y", "z"), [false, false, false, true, true, true]);
    });

    it("should throw error if no argument is provided", function() {
      assert.throws(function() {
        m1.has();
      }, Error);
    });
  });

  describe("#isEmpty", function() {
    it("should return whether a map is empty", function() {
      assert.equal(m1.isEmpty(), false);
      assert.equal(m2.isEmpty(), false);
      assert.equal(m3.isEmpty(), false);
      assert.equal(m4.isEmpty(), true);
    });
  });

  describe("#toObject", function() {
    it("should return arrays for the sets", function() {
      assert.deepEqual(m1.toObject(), o1);
      assert.deepEqual(m2.toObject(), o2);
      assert.deepEqual(m3.toObject(), o3);
      assert.deepEqual(m4.toObject(), o4);

      assert.deepEqual(new Map(m1).toObject(), o1);
      assert.deepEqual(new Map(m2).toObject(), o2);
      assert.deepEqual(new Map(m3).toObject(), o3);
      assert.deepEqual(new Map(m4).toObject(), o4);
    });

    it("should throw error if argument is provided", function() {
      assert.throws(function() {
        m1.toObject("abc");
      }, Error);
    });
  });

  describe("#toString", function() {
    it("should convert the maps into strings", function() {
      assert.equal(m1.toString(), '{"1":"a","2":"b","3":"c","4":"d","5":"e"}');
      assert.equal(m2.toString(), '{"1":"a","2":"b","3":"c","4":"d","5":"e","6":"f","7":"g","8":"h","9":"i"}');
      assert.equal(m3.toString(), '{"x":"a","y":"b","z":"c"}');
      assert.equal(m4.toString(), '{}');
    });

    it("should throw error if argument is provided", function() {
      assert.throws(function() {
        m1.toString("abc");
      }, Error);
    });
  });

  describe("#equals", function() {
    it("should compare equality of this set with another one", function() {
      var n1 = new Map().set(1, "a").set(2, "b").set(3, "c").set(4, "d").set(5, "e");
      var n2 = new Map(m1).set("6", "f").set("7", "g").set("8", "h").set("9", "i");
      var n3 = new Map({"x": "a", "y": "b", "z": "c"});
      var n4 = new Map();
      assert(!m1.equals(n1));
      assert(m2.equals(n2));
      assert(m3.equals(n3));
      assert(m4.equals(n4));
      assert(!m1.equals(n2));
      assert(!m4.equals([]));
      assert(m2.equals(new Map(m2.toObject())));
    });
  });

  describe("#get", function() {
    it("should get values in the maps using the keys", function() {
      Object.keys(o1).forEach(function(key) {
        assert.equal(m1.get(parseInt(key)), undefined); // All keys in m1 are integers.
        assert.equal(m1.get(key), o1[key]);
      });

      // For non-existing keys, undefined should be returned.
      assert.deepEqual(m3.get("x", "y", "z", "a", "b", "c"), ["a", "b", "c"]);
    });
  });

  describe("#has", function() {
    it("should return whether values exist in the maps", function() {
      Object.keys(o1).forEach(function(key) {
        assert(!m1.has(parseInt(key))); // All keys in m1 are integers.
        assert(m1.has(key));
      });

      // For non-existing keys, undefined should be returned.
      assert.deepEqual(m3.has("x", "y", "z", "a", "b", "c"), [true, true, true, false, false, false]);
    });
  });

  describe("#remove", function() {
    it("should remove values of the keys in the maps", function() {
      Object.keys(o1).forEach(function(key) {
        assert(m1.remove(key).equals(m1)); // All keys in m1 are integers.
        delete o1[key];
        assert.deepEqual(m1.remove(parseInt(key)).toObject(), o1);
      });

      m2.remove("1", "2", "3", "x", "y", "z");
      assert.deepEqual(m2.toObject(), {"4": "d", "5": "e", "6": "f", "7": "g", "8": "h", "9": "i"});
    });
  });

  describe("#set", function() {
    it("should set new values of keys in the maps", function() {
      assert.deepEqual(m1.set("x", "y").toObject(), {"1": "a", "2": "b", "3": "c", "4": "d", "5": "e", "x": "y"});
      assert.deepEqual(m2.set("1", "abcd").set(9, "efgh").toObject(), {"1": "abcd", "2": "b", "3": "c", "4": "d", "5": "e", "6": "f", "7": "g", "8": "h", "9": "i"}); // The "9" entry overrides the 9 one in toObject(), because 10 < "10"
      assert.deepEqual(m3.set("1", "x").set("y", "y").toObject(), {"1": "x", "x": "a", "y": "y", "z": "c"});
      assert.deepEqual(m3.set("1", "x").set("y", "y").toObject(), {"z": "c", "1": "x", "x": "a", "y": "y"});
    });

    it("should throw error if not exactly two arguments are given", function() {
      assert.throws(function() {
        m1.set();
      }, Error);
      assert.throws(function() {
        m1.set(1);
      }, Error);
      assert.throws(function() {
        m1.set(1, 2, 3);
      }, Error);
      m1.set(1, undefined);
      m1.set(undefined, undefined);
    });
  });

  describe("#setAll", function() {
    it("should set multiple new values in a map at a time", function() {
      assert.deepEqual(m1.setAll({"1": "a", "2": "b", "3": "c"}).toObject(), o1);
      assert.deepEqual(m1.setAll(o2).toObject(), o2);
      assert(m4.setAll(m1).equals(m1));
    });
  });

  describe("#clear", function() {
    it("should erase all the keys in the maps", function() {
      assert.deepEqual(m1.clear().toObject(), {});
      assert.deepEqual(m2.clear().toObject(), {});
      assert.deepEqual(m3.clear().toObject(), {});
      assert.deepEqual(m4.clear().toObject(), {});
      assert.deepEqual(m1.toObject(), {});
      assert.deepEqual(m2.toObject(), {});
      assert.deepEqual(m3.toObject(), {});
      assert.deepEqual(m4.toObject(), {});
    });

    it("should throw error if argument is provided", function() {
      assert.throws(function() {
        m1.clear(1);
      }, Error);
    });
  });

  describe("#each", function() {
    it("should go through all the entries in the maps", function() {
      var i = 0;
      m1.each(function(entry) {
        assert.equal(entry.key(), Object.keys(o1)[i]);
        assert.equal(entry.value(), o1[entry.key()]);
        i++;
      });

      var i = 0;
      m2.each(function(entry) {
        assert.equal(entry.key(), Object.keys(o2)[i]);
        assert.equal(entry.value(), o2[entry.key()]);
        i++;
      });

      var i = 0;
      m3.each(function(entry) {
        assert.equal(entry.key(), Object.keys(o3)[i]);
        assert.equal(entry.value(), o3[entry.key()]);
        i++;
      });

      var i = 0;
      m4.each(function(entry) {
        assert.equal(entry.key(), Object.keys(o4)[i]);
        assert.equal(entry.value(), o4[entry.key()]);
        i++;
      });
    });

    it("should tolerate errors in the callback function", function() {
      assert.throws(function() {
        m1.each(function() {
          notExisting == 1;
        });
      }, Error);

      assert.throws(function() {
        m1.each(function() {
          throw new Error();
        });
      }, Error);

      assert.throws(function() {
        m1.each(function(v, m) {
          m.doesNotExist();
        });
      }, Error);
    });

    it("should not allow state-changing functions to be invoked during iteration", function() {
      assert.throws(function() {
        m1.each(function() {
          m1.remove(1);
        });
      }, Error);
      assert.throws(function() {
        m1.each(function() {
          m1.set(1, 1);
        });
      }, Error);
      assert.throws(function() {
        m1.each(function() {
          m1.setAll({1:1});
        });
      }, Error);
      assert.throws(function() {
        m1.each(function() {
          m1.clear();
        });
      }, Error);
    });
  });
});
