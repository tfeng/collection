collection.js
==========

### Node.js cross-platform native collection library

JavaScript has limited support for collections. Associate array and objects are normally used in case where lists, sets and maps are needed. Unfortunately, the performance is not always as desired, especially for websites built and run with Node.js that need to be highly efficient.

`collection.js` is a native implementation of such data types as vector, set and map, based on their respective underlying implementation in C++. These data types are highly efficient due to the fact that operations on these data types take advantage of the underlying data structures that represent them, and that most of the heavy-lifting work is done at the C++ level, such as looking up a key in a map and modifying elements of a vector while iterating over them.

Experimental result has shown that in many use cases, `collection.js` significantly improves not only the time it takes to run an algorithm with intensive data access or manipulation, but also the amount of data objects that can fit in Node.js' process memory.

### Supported Collection Types

#### Vector

With resizable array as its underlying data structure, `vector` is an ideal choice for random access. Operations of the `vector` collection type allow users to access elements by index in constant time. The array may potentially grow while new elements are being added to the `vector`, whereas the amortized time is still constant.

A set of handy functions are provided for processing elements in `vector`, such as `find`, `filter`, `map` and `reduce`. Among these, there is also a very useful function `each`, which not only allows one to iterate over elements in the `vector`, but also to modify its elements on the fly, in a way iterator in C++ and Java does.

#### Set

A `set` is a sorted tree in its data representation. Existence of an element can be tested efficiently by traversing the tree instead of going through all the contained elements. As opposed to `hash set`, such a data structure does not offer constant lookup time, but it is generally more compact, and does not require a good `hash function`.

`Set` also provides an `each` function that can be used to traverse its elements in order (defined below). It, however, does not allows to modify elements on the fly because such modification may alter the traversal.

#### Map

A `map` can be viewed as a set of `entries`. Each entry is a (`key`, `value`) pair. No two entries with the same key exist in a `map`.

Internally, `map` is represented by a tree sorted by its keys, in a way similar to `set` that keeps its elements sorted. A `map`, however, has the additional capability of associating values with keys, but values do not participate in sorting.

The `put` operation is to put an entry in to the map, so that the key of the entry are inserted into the proper location of the tree. If the same key already exists, its currently associated value will be replaced.

`Each` method of a `map` can be used to iterate over entries.

Setup
----------

### Prerequisite

See [node-gyp](https://github.com/TooTallNate/node-gyp) documentation (under Installation section) for software packages required to be installed before proceeding.

### Installation

Install `collection.js` using the following command:

``` bash
$ npm install collection
```

### Testing

After installing `collection.js`, start `node` to see whether it is working as expected.

```node
$ node
> var Vector = require("collection").Vector,
      Set = require("collection").Set,
      Map = require("collection").Map;
undefined
> var v = new Vector([1,2,3]),
      s = new Set(["a","b","c"]),
      m = new Map({"1":"a","2":"b","3":"c"});
undefined
> v.toString();
'[1,2,3]'
> s.toArray();
[ 'a', 'b', 'c' ]
> m.toObject()
{ '1': 'a',
  '2': 'b',
  '3': 'c' }
> v.each(function(v, m) { m.set(v+1); }).toArray();
[ 2, 3, 4 ]
>
```

API Documentation
----------

### Vector

Examples below assume vector `v` is initialized with elements `1, 2, 3, 4`:

```node
> var v = new Vector([1,2,3,4]);
undefined
> v.toString();
'[1,2,3,4]'
```

#### add(value, ...)

Add one or more values to the end of this vector. If a value is an array or a collection, the same instance of array or collection is added to the vector.

*Return:* This vector.

```node
> v.add(5,6,7,8).toArray();
[ 1, 2, 3, 4, 5, 6, 7, 8 ]
```

#### addAll(object)

Add all the elements in `object` to the end of this vector. Supported types of `object` parameter are array, vector and set.

*Return:* This vector.

```node
> v.addAll(new Vector([5,6,7])).toArray();
[ 1, 2, 3, 4, 5, 6, 7 ]
> v.addAll(["x","y","z"]).toArray();
[ 1, 2, 3, 4, 5, 6, 7, 'x', 'y', 'z' ]
```

#### clear()

Remove all elements from this vector.

*Return:* This vector.

#### each(callback)

Iterate over elements of this vector, and invoke the `callback` function for each element. The callback function should be of the form `function(v, m) { ... }`. `v` is the element. `m` is a `vector modifier`, which may be used to inspect properties of the iteration as well as modify the vector. The callback function may return `false` indicating the iteration should stop immediately. Any other returned value is ignored.

*Vector modifier:* It is the object `m` passed into the callback function for `each`. It supports the following operations: `isFirst()` to return whether the current invocation is the first of the iteration. `isLast()` to return whether the current invocation is the last. `index()` to return the current index. `set(value)` to set the substitute the current element. `remove()` to remove the current element. `insertBefore(value, ...)` to insert new elements before the current element. `insertAfter(value, ...)` to insert new elements after the current element. All changes are recorded during the callback executes, and are made only after the callback returns. Newly inserted elements are not included in the iteration.

*Return:* This vector.

```node
> v.each(function(v, m){if(v%2==0){console.log(v); return false;}});
2
> v.each(function(v, m){if(m.isFirst()) console.log(v);});
1
> v.each(function(v, m){console.log(m.index()); m.insertBefore(0);}).toArray();
0
2
4
6
[ 0, 1, 0, 2, 0, 3, 0, 4 ]
> v.each(function(v, m){m.set(0);}).toArray();
[ 0, 0, 0, 0, 0, 0, 0, 0 ]
> v.each(function(v, m){m.remove();}).toArray();
[]
```

#### equals(object)

Test whether this vector equals the given object.

*Equality:* In `collection.js`, equality is defined as `deep equality` between two objects, including their types and values. For example, a vector never equals an array because their types are different, though a vector may be converted into an array using the `toArray` method. Similarly, a vector containing integer value `1` does not equal anther one containing string `"1"`. Two vectors equal if and only if the contain equal elements in the same order.

*Return:* `true` or `false`.

```node
> v.equals(new Vector([1,2,3,4]))
true
```

#### filter(callback)

Return in an array all elements that make the `callback` function return `true`. The callback function should be of the form `function(v) { ... }`.

*Return:* An array of elements for which the callback function returns `true`.

```node
> v.filter(function(v){return v%2==0});
[ 2, 4 ]
```

#### find(callback)

Return the first element of this vector that makes the `callback` function return `true`. The callback function should be of the form `function(v) { ... }`.

*Return:* The first element of this vector for which the callback function returns `true`.

```node
> v.find(function(v){return v%2==0});
2
```

#### get(index, ...)

Get elements of this vector at the specified indexes.

*Return:* If only 1 index is given as parameter, element of this vector at that index; if multiple indexes are given, an array of elements at those indexes.

```node
> v.get(0)
1
> v.get(0,1,5,3)
[ 1, 2, , 4 ]
```

#### has(value, ...)

Check whether the given values exist in the vector.

*Return:* If only 1 value is given as parameter, `true` or `false`; if multiple values are given, an array of boolean values, each identifying whether the corresponding value exists.

```node
> v.has(2,4,6)
[ true, true, false ]
```

#### index(value, ...)

Return indexes of the specified elements in this vector.

*Return:* If only 1 value is given as parameter, index of that element, or `undefined` if not found; if multiple values are given, an array of indexes.

```node
> v.index(2)
1
> v.index(1,3,5,7)
[ 0, 2 ]
> v.index(1,5,7,3)
[ 0, , , 2 ]
```

#### isEmpty()

Test whether the vector is empty.

*Return:* `true` or `false`.

```node
> v.isEmpty()
false
```

#### map(callback)

Iterates over all elements of this vector, invoke the `callback` function for each element, and when the callback function returns, replace the current element with a new value. The callback function should be of the form `function(v){ ... }`. Its returned value substitutes elements of this vector, one at a time.

*Return:* This vector.

```node
> v.map(function(v){return v*2}).toArray();
[ 2, 4, 6, 8 ]
> v.map(function(v){}).toArray();
[ undefined, undefined, undefined, undefined ]
```

#### reduce(callback, memo)

Iterates over all elements of this vector and invoke the `callback` function for each element. Each time a `memo` value is passed into the callback, and the return value of the callback becomes the memo value of the next iteration. The return value in the last iteration is the return value of the `reduce` function itself. The callback should be of the form `function(memo, v){ ... }`.

*Return:* The return value of the callback function in the last iteration.

```node
> v.reduce(function(memo, v){return memo+v}, 0);
10
> v.reduce(function(memo, v){return memo*v}, 1);
24
```

#### reduceRight(callback, memo)

Perform the same function as `reduce`, but iterate elements of this vector in reverse order.

*Return:* The return value of the callback function in the last iteration.

```node
> v.reduceRight(function(memo, v){return v}, 5);
1
```

#### remove(value, ...)

Remove elements from this vector.

*Return:* This vector.

```node
> v.remove(1,3,5).toArray();
[ 2, 4 ]
```

#### removeAt(index, ...)

Remove elements at the specified indexes from this vector. For any index greater than size of this vector, there is no effect.

*Return:* This vector.

```node
> v.removeAt(0,3,5).toArray();
[ 2, 3 ]
```

#### removeLast()

Remove last element of this vector. If this vector is empty, there is no effect.

*Return:* This vector.

```node
> v.removeLast().removeLast().toArray();
[ 1, 2 ]
> v.removeLast().removeLast().removeLast().removeLast().toArray();
[]
```

#### removeRange(start, end)

Remove elements between the `start` index (inclusive) and the `end` index (exclusive). If `start` is greater than or equal to size of this vector, there is no effect. If `end` is greater than or equal to size of this vector, all elements from `start` index are removed.

*Return:* This vector.

```node
> v.removeRange(9,20).toArray();
[ 1, 2, 3, 4 ]
> v.removeRange(0,0).toArray();
[ 1, 2, 3, 4 ]
> v.removeRange(0,1).toArray();
[ 2, 3, 4 ]
> v.removeRange(1,999).toArray();
[ 2 ]
```

#### reverse()

Reverse this vector.

*Return:* This vector.

```node
> v.reverse().toArray();
[ 4, 3, 2, 1 ]
> v.reverse().toArray();
[ 1, 2, 3, 4 ]
```

#### set(index, value)

Set a new value at a given index. If the index is less than size of this vector, existing value is substituted by the given new value. If the index equals size of this vector, the given value is added to the end of the vector. If the index is greater than size of this vector, an `Error` is thrown.

*Return:* This vector.

```node
> v.set(2,5).toArray();
[ 1, 2, 5, 4 ]
> v.set(4,6).toArray();
[ 1, 2, 5, 4, 6 ]
```

#### size()

Check the size of the vector.

*Return:* Number of elements in this vector.

```node
> v.size()
4
```

#### toArray()

Convert this vector into an array that contains the same elements. If the vector contains an array or a collection as its element, the same instance of array or collection will also be an element of the resulting array.

*Return:* An array.

```node
> v.toArray()
[ 1, 2, 3, 4 ]
> v.add(["a", "b"]).toArray();
[ 1, 2, 3, 4, [ 'a', 'b' ] ]
> v.toArray()[4].push("c");
3
> v.toArray()
[ 1, 2, 3, 4, [ 'a', 'b', 'c' ] ]
```

#### toString()

Convert this vector into a string.

*Return:* A string.

```node
> v.toString();
'[1,2,3,4]'
```

### Set

Examples below assume set `s` is initialized with elements `1, 2, 3, 4`:

```node
> var s = new Set([1,2,3,4]);
undefined
> s.toString();
'[1,2,3,4]'
```

#### add(value, ...)

Add one or more values to this set, and keep the elements in sorted order. If a value is an array or a collection, the same instance of array or collection is added to the set.

*Return:* This set.

```node
> s.add(5,6,7,8).toArray();
[ 1, 2, 3, 4, 5, 6, 7, 8 ]
> s.add(0,2,4,6,8).toArray()
[ 0, 1, 2, 3, 4, 5, 6, 7, 8 ]
```

#### addAll(object)

Add all the elements in `object` to this set, and keep the elements in sorted order. Supported types of `object` parameter are array, vector and set.

*Return:* This set.

```node
> s.addAll(new Vector([3,4,5,6,7])).toArray();
[ 1, 2, 3, 4, 5, 6, 7 ]
> s.addAll(["x","y","z"]).toArray();
[ 1, 2, 3, 4, 5, 6, 7, 'x', 'y', 'z' ]
> s.addAll(new Vector([0,1,2,3])).toArray();
[ 0, 1, 2, 3, 4, 5, 6, 7, 'x', 'y', 'z' ]
```

#### clear()

Remove all elements from this set.

*Return:* This set.

#### each(callback)

Iterate over elements of this set, and invoke the `callback` function for each element. The callback function should be of the form `function(v) { ... }`. `v` is the element. The callback function may return `false` indicating the iteration should stop immediately. Any other returned value is ignored.

*Return:* This vector.

```node
> s.each(function(v){if(v%2==0){console.log(v); return false;}});
2
```

#### equals(object)

Test whether this set equals the given object. (See vector's `equals` function for definition of equality.)

*Return:* `true` or `false`.

```node
> s.equals(new Set([4,3,2,1]))
true
```

#### filter(callback)

Return in an array all elements that make the `callback` function return `true`. The callback function should be of the form `function(v) { ... }`.

*Return:* An array of elements for which the callback function returns `true`.

```node
> s.filter(function(v){return v%2==0});
[ 2, 4 ]
```

#### find(callback)

Return the first element of this set that makes the `callback` function return `true`. The callback function should be of the form `function(v) { ... }`.

*Return:* The first element of this set for which the callback function returns `true`.

```node
> s.find(function(v){return v%2==0});
2
```

#### get(index, ...)

Get elements of this set at the specified indexes.

*Return:* If only 1 index is given as parameter, element of this set at that index; if multiple indexes are given, an array of elements at those indexes.

```node
> s.get(0)
1
> s.get(0,1,5,3)
[ 1, 2, , 4 ]
```

#### has(value, ...)

Check whether the given values exist in the set.

*Return:* If only 1 value is given as parameter, `true` or `false`; if multiple values are given, an array of boolean values, each identifying whether the corresponding value exists.

```node
> s.has(2,4,6)
[ true, true, false ]
```

#### isEmpty()

Test whether the set is empty.

*Return:* `true` or `false`.

```node
> s.isEmpty()
false
```

#### reduce(callback, memo)

Iterates over all elements of this set and invoke the `callback` function for each element. Each time a `memo` value is passed into the callback, and the return value of the callback becomes the memo value of the next iteration. The return value in the last iteration is the return value of the `reduce` function itself. The callback should be of the form `function(memo, v){ ... }`.

*Return:* The return value of the callback function in the last iteration.

```node
> s.reduce(function(memo, v){return memo+v}, 0);
10
> s.reduce(function(memo, v){return memo*v}, 1);
24
```

#### reduceRight(callback, memo)

Perform the same function as `reduce`, but iterate elements of this set in reverse order.

*Return:* The return value of the callback function in the last iteration.

```node
> s.reduceRight(function(memo, v){return v}, 5);
1
```

#### remove(value, ...)

Remove elements from this set.

*Return:* This set.

```node
> s.remove(1,3,5).toArray();
[ 2, 4 ]
```

#### removeAt(index, ...)

Remove elements at the specified indexes from this set. For any index greater than size of this set, there is no effect.

*Return:* This set.

```node
> s.removeAt(0,3,5).toArray();
[ 2, 3 ]
```

#### removeLast()

Remove last element of this set. If this set is empty, there is no effect.

*Return:* This vector.

```node
> s.removeLast().removeLast().toArray();
[ 1, 2 ]
> s.removeLast().removeLast().removeLast().removeLast().toArray();
[]
```

#### removeRange(start, end)

Remove elements between the `start` index (inclusive) and the `end` index (exclusive). If `start` is greater than or equal to size of this set, there is no effect. If `end` is greater than or equal to size of this set, all elements from `start` index are removed.

*Return:* This vector.

```node
> s.removeRange(9,20).toArray();
[ 1, 2, 3, 4 ]
> s.removeRange(0,0).toArray();
[ 1, 2, 3, 4 ]
> s.removeRange(0,1).toArray();
[ 2, 3, 4 ]
> s.removeRange(1,999).toArray();
[ 2 ]
```

#### size()

Check the size of the set.

*Return:* Number of elements in this set.

```node
> s.size()
4
```

#### toArray()

Convert this set into an array that contains the same elements. If the set contains an array or a collection as its element, the same instance of array or collection will also be an element of the resulting array.

*Return:* An array.

```node
> s.toArray()
[ 1, 2, 3, 4 ]
> s.add(["a", "b"]).toArray();
[ 1, 2, 3, 4, [ 'a', 'b' ] ]
> s.toArray()[4].push("c");
3
> s.toArray()
[ 1, 2, 3, 4, [ 'a', 'b', 'c' ] ]
```

#### toString()

Convert this set into a string.

*Return:* A string.

```node
> s.toString()
'[1,2,3,4]'
```

### Map

A map is structured as a tree sorted by the keys. It provides the capability of looking up a value associated with a key.

A map can be converted into a JavaScript object, using the `toObject()` function. However, there is an important difference between the two, though `toObject()`, and subsequently `toString()` that depends to `toObject()`, obscure this difference. Keys in a map are allowed to be of any primitive data type, array, collection, or combination of these, wheres keys in a JavaScript object can only be strings. Therefore, if a value is associated with integer `1` in a map, one cannot retrieve this value back by invoking `get("1")`, where string `"1"` is used as the key for lookup.

Due to map's sorted nature, some functions below allow to access the map by an index, which points to the location of the sorted tree. A `entry` object is a common way of returning both the key and the value at that location.

*Entry:* An `entry` is a (`key`, `value`) pair in the map. An entry object provides a `key()` function to return the key, a `value()` function to return the value, a `toObject()` function to return an equivalent JavaScript object, and a `toString()` function to return the string that describes it.

Examples below assume set `m` is initialized with entries `{1:"a", 2:"b", 3:"c", 4:"d"}`:

```node
> var m = new Map({1:"a",2:"b","a":"b","c":"d"}).set(3,"c").set(4,"d");
undefined
> m.keys();
[ 3, 4, '1', '2', 'a', 'c' ]
> m.toObject();
{ '1': 'a', '2': 'b', '3': 'c', '4': 'd', a: 'b', c: 'd' }
> m.get(1);
undefined
> m.get("1");
'a'
> m.get(3);
'c'
```

#### get(key, ...)

Get value of this map associated with the specified keys.

*Return:* If only 1 key is given as parameter, value in this map associated with that key; if multiple keys are given, an array of values associated with those indexes.

```node
> m.get("1");
'a'
> m.get(1);
undefined
> m.get(1,4,"a",3);
[ , 'd', 'b', 'c' ]
```

#### getAt(index, ...)

Get entry of this map at the specified indexes.

*Return:* If only 1 index is given as parameter, entry of this map at that index; if multiple indexes are given, an array of entries at those indexes.

```node
> m.getAt(0).toObject();
{ key: 3, value: 'c' }
> m.getAt(1,4,8,3).toString();
'{"key":4,"value":"d"},{"key":"a","value":"b"},,{"key":"2","value":"b"}'
```