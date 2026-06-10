# Custom C++ Heap Allocator

- inspired by [salar's video](https://www.youtube.com/watch?v=H8vCBFX62Qg)

![split_available_block().png](./excalidraw/split_available_block().png)

![block.png](./excalidraw/block.png)

- we can see that the size of `struct Block` is 24 bytes
- this would not change regardless of how we order the members because of the way padding works

```bash
cmake -B build
cmake --build build
./build/heap
```
