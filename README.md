# fancyarray
A templated structure of arrays of elements of different types.

The various data structures in this project are:
- `trivial_array_of_structs<T1, T2, ...>`, which stores a raw block of memory encoding an array of structures containing values of types `T1`, `T2`, ...
- `trivial_struct_of_arrays<T1, T2, ...>`, which stores a raw block of memory encoding a structure of several arrays containing values of types `T1`, `T2`, ...
- `tuple_of_vectors<T1, T2, ...>`, which internally stores a `std::tuple<std::vector<T1>, std::vector<T2>, ...>`
- `vector_of_tuples<T1, T2, ...>`, which internally stores a `std::vector<std::tuple<T1, T2, ...>>`

Feel free to use this and modify it to your liking.
