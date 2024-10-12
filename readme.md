# Memalloc: Custom Memory Allocator

Memalloc is a custom memory allocator implementation written in C. It provides efficient memory management functions similar to the standard `malloc`, `free`, and `realloc`.

## Features

- Custom `malloc` implementation for dynamic memory allocation
- `free` function to deallocate previously allocated memory
- `realloc` function to resize or move allocated memory blocks
- Efficient memory management using a free list approach
- Coalescing of adjacent free blocks to reduce fragmentation
- Splitting of large free blocks to minimize wasted space

## Usage

To use the memalloc functions in your project:

1. Include the `memalloc.h` header file in your C source files:


2. Use the following functions for memory management:

- `void *malloc(size_t size)`: Allocate memory of the specified size
- `void free(void *ptr)`: Deallocate previously allocated memory
- `void *realloc(void *ptr, size_t size)`: Resize or move an allocated memory block

## Implementation Details

The memory allocator uses a free list approach to manage memory blocks. It maintains a list of free memory blocks and performs the following optimizations:

- Coalescing: Merges adjacent free blocks to reduce fragmentation
- Splitting: Divides large free blocks to minimize wasted space
- First-fit allocation strategy: Finds the first suitable free block for allocation

For more details, refer to the comments in the `memalloc.c` source file.

## Limitations

- This allocator is not thread-safe. Use appropriate synchronization mechanisms if used in a multi-threaded environment.
- The implementation assumes a contiguous memory model and may not be suitable for all systems or use cases.

## Contributing

Contributions to improve the memory allocator are welcome. Please submit pull requests or open issues to discuss potential enhancements.

## License

This project is open-source and available under the [MIT License](LICENSE).
