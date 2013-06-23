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

A `map` can be viewed as a set of `entries`. Each `entry` is a (`key`, `value`) pair. No two `entries` with the same key exist in a `map`.

Internally, `map` is represented by a tree sorted by its `keys`, in a way similar to `set` that keeps its elements sorted. A `map`, however, has the additional capability of associating `values` with `keys`, but `values` do not pacticipate in sorting.

The `put` operation is to put an `entry` in to the map, so that the `key` of the `entry` are inserted into the proper location of the tree. If the same `key` already exists, its currently associated `value` will be replaced.

`Each` method of a `map` can be used to iterate over `entries`.

Installation
----------

See [node-gyp](https://github.com/TooTallNate/node-gyp) documentation (under Installation section) for software packages required to be installed before proceeding.

Install `collection.js` using the following command:

``` bash
$ npm install collection
```
