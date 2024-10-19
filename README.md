# STC (Standard Comba Library)
Stc is a simple header-only C library with quality of life features.
It aims to simplify and enhance basic language features which the C language lacks, such as dynamic arrays, better strings, and hashmaps.

It is subdivided in modules, but they are mostly interconnected with each other.
Test files should offer a simple example of how to use the library.

## stc_mem
An implementation for a growable Arena allocator and a growable Pool allocator.
An Arena is a region-based allocator, which works by allocating a big chunk of memory, and allocating data contiguously.
The arena doesn't keep track of the allocated data, so it isn't possible to free pointers. However, the arena can be freed all at once. This behaviour is extremely useful, as very often we need 'scratch' objects which we use for a short amount of time, and we can release when we're done.

Arena also simplifies the problem of 'lifetimes', which is hard to deal with using a variable size allocator such as malloc. As we have to always pass the arena when we alloc, we make allocations 'explicit', so we know data depending on the arena will live until the arena lives too.

An example:
```c
  Arena a1 = {0};
  Arena a2 = {0};
  MyType* bigArrayOfStructs = arena_alloc(&a1, 10000 * sizeof(MyType));
  MyOtherType* bigStruct = arena_alloc(&a1, sizeof(MyOtherType));

  MyType* anotherArrayOfStructs = arena_alloc(&a2, 5000 * sizeof(MyType));

  // ... do some stuff with the data...

  // We're done with anotherArrayOfStructs
  arena_free(&a2);
  
  // We're done with the other data
  arena_free(&a1);
```

If the region is full, a new region will be allocated and will point to the old one. This way, no reallocations occurs, and all the pointers won't be invalidated. The arena can thus grow indefinetely, and the user doesn't have to deal with complex allocations. It is only sufficient to free the arena at the end of its usage.

The Pool allocator on the other hand, reserves a big chunk of static sized blocks, useful if we need a big number of the same objects, and we also need to be able to free them independently.
The Pool keeps track of allocations with a free list. 
(Fun fact, the pool stores its data in an Arena!)

```c
  // We tell the pool how big the chunks should be
  Pool p = pool_new(sizeof(MyType));
  MyType* myTypes[100];

  for(int i=0; i<100; ++i) {
    myTypes[i] = pool_alloc(&p);
  }

  // wee can free some objects 
  pool_free(&p, myTypes[0]);
  pool_free(&p, myTypes[1]);
  pool_free(&p, myTypes[2]);

  // do some stuff with the data...

  // we destroy the pool
  pool_reset(&p);
```

The pool grows in size if it is filled, in a similiar way to the Arena.

## stc_list
An implementation for a generic dynamic array, using a simple struct with data, length, and capacity.
It mainly consists of macros, but they are fairly simple, so not very dangerous.
If you want to make a generic list, you first have to 'define' it, giving its name and type, as so:

```c 
  list_define(int, ListOfInts);
```

A big enough chunk of memory is allocated at first. When we fill the array, we simply reallocate it to double its size. The macro takes care of assignin the new address to the old variable!
Be warned that with reallocation, the array will change address, so if you have other references to it, they will all become dangling. It is usually wise to always have only a single reference to a dynamic array.

```c 
  ListOfInts list = {0};

  for (int i=0; i<100; ++i) list_push(list, i);

  list_free(list);
```

More generic macros are implemented, such as foreach, concat, filter, and map.
An arena allocated list is also avaible, with its respective macros.

## stc_str
Implementation of a string view and owned string.
A String View (str) is a simple fat pointer, or slice, of a static C string (char*).
Basically, it points to the static string, and has a length.
It isn't NULL terminated, so it won't work with libc string functions, especially such as printf.

A str can be converted to a C string with cstr(), which is malloc-allocated, or zstr(), which is allocated in a scratch arena, freeable with zstrings_free().

A big number of utilities and string management are avaible for the str type. The best thing is that they are memory safe!
Operations on String Views NEVER allocate any new memory, with the exception of cstr() and zstr().

As a simple example:
```c
  str s = str_from("Hello World!");
  str hello = str_slice(s.data, 0, 5); // contains 'Hello'
  str world = str_skip_until_char(s, ' '); // contains 'World!'

  str trimmed = str_trim(str_from("    help...    ")); // contains "help..."
  str_cmp(hello, world); // will output -1, and it is a memory safe strcmp!

  char* cstring = cstr(s);
  printf("%s %s", cstring, zstr(s));  // this will print a NULL terminated C string, which works with printf

  // C strings have to be freed, Zstrings are freed all at once
  free(cstring);
  zstrings_free();
```

The moment we need to edit a String View, we need an Owned String.
An Owned String (String) is an arena allocated string, which permits modfications.
A String is always allocated, and every string operation always allocates a new string, so the problem of dangling references is non existent, as long as the arena is alive.
The only exception is str_append(), which might reallocate.

Owned Strings can be converted to Views with STR().
String supports all the str operations, returning a view to the owned string.
It isn't NULL terminated, so like str, you need to convert it to a C string with zstring().

```c
  Arena a = {0};

  str view = str_from("Hello World!");

  String owned = string_from(&a, view);
  String upper = str_to_upper(&a, owned); // contains "HELLO WORLD!"

  String concat = str_concat(&a, owned, s, str_from("New data!")); // new string is allocated, owned is untouched

  str_append(&a, &owned, s, str_from("New data!")); // append is the only operations which edits in-place, owned is modified and eventually reallocated

  // format string, will contain 'Some numbers: 69, 420, and some data!'
  String fmt = str_format(&a, "Some numbers: %d, %d, and %s", 69, 420, " some data!");

  // all strings are freed
  arena_free(&a);
```

## stc_matrix
A simple generic implementation of a fixed size 2D array.

## stc_map
An implementation for a generic dynamic dictionary, using an hashtable with linear probing for dealing with hash collisions.
The keys are always a string view.
Like the lists, they first must be defined:

```c
  map_define(int, MapOfInts);
```

```c
  MapOfInts map = {0};
  map_init(50);

  map_MapOfInts_insert(&map, str_from("test"), 5);
  int value = map_MapOfInts_get(&map, str_from("test"));
```

The map will be reallocated if it is filled.