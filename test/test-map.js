"use strict";

var assert = require("assert"),
    Map = require("../lib/collection").Map;

describe('Map', function() {
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

  describe("#equals", function() {
    it("should compare equality of this map with another one", function() {
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

    it("should throw error if not exactly one argument is provided", function() {
      assert.throws(function() {
        m1.equals();
      }, Error);
      assert.throws(function() {
        m1.equals(1, 2);
      }, Error);
    });
  });

  describe("#filter", function() {
    it("should return all the entries in the map that satisfy the requirement", function() {
      assert.deepEqual(m1.filter(function(v) {
        return v.key() % 2 == 0;
      }).toString(), '{"key":"2","value":"b"},{"key":"4","value":"d"}');
      assert.deepEqual(m1.filter(function(v) {
        return v.key() < 0;
      }), []);
      assert.deepEqual(m2.filter(function(v) {
        return v.value() < "e";
      }).toString(), '{"key":"1","value":"a"},{"key":"2","value":"b"},{"key":"3","value":"c"},{"key":"4","value":"d"}');
    });

    it("should throw error if a function argument is not given", function() {
      assert.throws(function() {
        m1.filter();
      }, Error);
      assert.throws(function() {
        m1.filter(1);
      }, Error);
      assert.throws(function() {
        m1.filter(function(){}, 1);
      }, Error);
    });
  });

  describe("#find", function() {
    it("should find the first entry in the map that satisfies the requirement", function() {
      assert.equal(m1.find(function(v) {
        return v.key() % 2 == 0;
      }).value(), "b");
      assert.equal(m1.find(function(v) {
        return v.value() == "e";
      }).key(), 5);
      assert.equal(m2.find(function(v) {
        return v.key() == "";
      }), undefined);
    });

    it("should throw error if a function argument is not given", function() {
      assert.throws(function() {
        m1.find();
      }, Error);
      assert.throws(function() {
        m1.find(1);
      }, Error);
      assert.throws(function() {
        m1.find(function(){}, 1);
      }, Error);
    });
  });

  describe("#get", function() {
    it("should get values in the maps using the keys", function() {
      Object.keys(o1).forEach(function(key) {
        assert.equal(m1.get(parseInt(key)), undefined); // All keys in m1 are integers.
        assert.equal(m1.get(key), o1[key]);
      });

      Object.keys(o2).forEach(function(key) {
        assert.equal(m2.get(parseInt(key)), undefined); // All keys in m1 are integers.
        assert.equal(m2.get(key), o2[key]);
      });

      m1.keys().forEach(function(key) {
        assert(m1.get(key) != undefined);
      });

      m2.keys().forEach(function(key) {
        assert(m2.get(key) != undefined);
      });

      // For non-existing keys, undefined should be returned.
      assert.deepEqual(m3.get("x", "y", "z", "a", "b", "c"), ["a", "b", "c"]);
    });

    it("should throw error if no argument is given", function() {
      assert.throws(function() {
        m1.get();
      }, Error);
    });
  });

  describe("#getAt", function() {
    it("should get entries in each map one by one", function() {
      for (var i = 0; i < m1.size(); i++) {
        assert.equal(m1.getAt(i).key(), i + 1);
      }
      for (var i = 0; i < m2.size(); i++) {
        assert.equal(m2.getAt(i).key(), i + 1);
      }
    });

    it("should throw error if input is not unsigned integer", function() {
      assert.throws(function() {
        m1.getAt("abc");
      }, Error);
      assert.throws(function() {
        m1.getAt([]);
      }, Error);
      assert.throws(function() {
        m1.getAt(-1);
      }, Error);
    });

    it("should get multiple entries from a map at a time", function() {
      assert.deepEqual(m1.getAt(0, 1, 2, 3).map(function(v){return v.key()}), [1,2,3,4]);
      assert.deepEqual(m2.getAt(1, 3, 5, 7, 9).map(function(v){return v.key()}), [2,4,6,8]);
      assert.deepEqual(m2.getAt(undefined), undefined);
      assert.deepEqual(m2.getAt(1, undefined, 3, undefined, 5, null, 7, null, 9, 999).map(function(v){return v.key()}), [2,,4,,6,,8]);
    });
  });

  describe("#has", function() {
    it("should return whether values exist in the maps", function() {
      Object.keys(o1).forEach(function(key) {
        assert(!m1.has(parseInt(key))); // All keys in m1 are integers.
        assert(m1.has(key));
      });

      Object.keys(o2).forEach(function(key) {
        assert(!m2.has(parseInt(key))); // All keys in m1 are integers.
        assert(m2.has(key));
      });

      m1.keys().forEach(function(key) {
        assert(m1.has(key));
      });

      m2.keys().forEach(function(key) {
        assert(m2.has(key));
      });

      // For non-existing keys, undefined should be returned.
      assert.deepEqual(m3.has("x", "y", "z", "a", "b", "c"), [true, true, true, false, false, false]);

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

    it("should throw error if argument is given", function() {
      assert.throws(function() {
        m1.isEmpty(1);
      }, Error);
    });
  });

  describe("#reduce", function() {
    it("should reduce a map into a single value", function() {
      assert.equal(m1.reduce(function(memo, v) {
        return memo + parseInt(v.key());
      }, 0), 15);

      assert.equal(m2.reduce(function(memo, v) {
        return memo + v.value();
      }, ""), "abcdefghi");
    });

    it("should throw error if a function argument is not given", function() {
      assert.throws(function() {
        m1.reduce();
      }, Error);
      assert.throws(function() {
        m1.reduce(1);
      }, Error);
      assert.throws(function() {
        m1.reduce(function(){}, 1, 2);
      }, Error);
    });
  });

  describe("#reduceRight", function() {
    it("should reduce a map from the right into a single value", function() {
      assert.equal(m1.reduceRight(function(memo, v) {
        return memo + parseInt(v.key());
      }, 0), 15);

      assert.equal(m2.reduceRight(function(memo, v) {
        return memo + v.value();
      }, ""), "ihgfedcba");
    });

    it("should throw error if a function argument is not given", function() {
      assert.throws(function() {
        m1.reduceRight();
      }, Error);
      assert.throws(function() {
        m1.reduceRight(1);
      }, Error);
      assert.throws(function() {
        m1.reduceRight(function(){}, 1, 2);
      }, Error);
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

    it("should allow undefined and null arguments", function() {
      var object = m1.toObject();
      assert.notDeepEqual(m1.set(undefined, 1).toObject(), object);
      assert.deepEqual(m1.remove(undefined).toObject(), object);
      assert.notDeepEqual(m1.set(null, 1).toObject(), object);
      assert.deepEqual(m1.remove(null).toObject(), object);
    });

    it("should throw error if no argument is provided", function() {
      assert.throws(function() {
        m1.remove();
      }, Error);
    });
  });

  describe("#removeAt", function() {
    it("should remove elements in the maps at specified indexes", function() {
      assert(m1.has("1"));
      assert(!m1.removeAt(0).has("1"));
      assert.deepEqual(m1.has("2", "3", "4", "5", "11", 2, 3, 4, 5, 11), [true,true,true,true,false,false,false,false,false,false]);
      assert.deepEqual(m1.remove("2", "3", 2, 3).has("2", "3", "4", "5", "11", 2, 3, 4, 5, 11), [false,false,true,true,false,false,false,false,false,false]);
    });

    it("should throw error if indexes are not positive integers", function() {
      assert.throws(function() {
        m1.removeAt("1");
      }, Error);
      assert.throws(function() {
        m1.removeAt();
      }, Error);
      assert.throws(function() {
        m1.removeAt(-1);
      }, Error);
    });
  });

  describe("#removeLast", function() {
    it("should remove the last key in the map", function() {
      assert.deepEqual(m1.removeLast().keys(), ["1","2","3","4"]);
      assert.deepEqual(m1.removeLast().keys(), ["1","2","3"]);
      assert.deepEqual(m1.removeLast().keys(), ["1","2"]);
      assert.deepEqual(m1.removeLast().keys(), ["1"]);
      assert.deepEqual(m1.removeLast().keys(), []);
      assert.deepEqual(m1.removeLast().keys(), []);
    });

    it("should throw error if arguments are given", function() {
      assert.throws(function() {
        m1.removeLast(1);
      }, Error)
    });
  });

  describe("#removeRange", function() {
    it("should remove elements in the specified range of indexes", function() {
      assert.deepEqual(m2.removeRange(0, 3).keys(), ["4","5","6","7","8","9"]);
      assert.deepEqual(m2.removeRange(4,10).keys(), ["4","5","6","7"]);
    });

    it("should throw error if indexes are not positive integers", function() {
      assert.throws(function() {
        m1.removeRange(1);
      }, Error);
      assert.throws(function() {
        m1.removeRange(1, undefined);
      }, Error);
      assert.throws(function() {
        m1.removeRange(-1, 1);
      }, Error);
      assert.throws(function() {
        m1.removeRange("a", 1);
      }, Error);
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
      assert.deepEqual(m1.setAll(m1).toObject(), o2);
      assert(m4.setAll(m1).equals(m1));
    });
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

  describe("#toArray", function() {
    it("should convert a set into an array", function() {
      assert.equal(m1.toArray().toString(), '{"key":"1","value":"a"},{"key":"2","value":"b"},{"key":"3","value":"c"},{"key":"4","value":"d"},{"key":"5","value":"e"}');
      assert.equal(m2.toArray().toString(), '{"key":"1","value":"a"},{"key":"2","value":"b"},{"key":"3","value":"c"},{"key":"4","value":"d"},{"key":"5","value":"e"},{"key":"6","value":"f"},{"key":"7","value":"g"},{"key":"8","value":"h"},{"key":"9","value":"i"}');
      assert.equal(m3.toArray().toString(), '{"key":"x","value":"a"},{"key":"y","value":"b"},{"key":"z","value":"c"}');
      assert.equal(m4.toArray().toString(), '');
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
});
