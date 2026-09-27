# binview

A lightweight C++11 binary data viewing and reading library

# Features

- header-only
- zero-copy
- 0 dependencies
- platform-independent
- little-endian and big-endian support
- works with raw memory and contigious containers

## Usage

```cpp
#include <binview/binview.hpp>
#include <cstdint>

const std::uint8_t data[] = { 0x4D, 0x5A, 0x90, 0x00 };

binview::View view( data );

std::uint16_t magic = 0;
if( view.read( 0, magic ) )
{
    // ...
}
```

### Containers

`View` can be created from raw memory, while `make_view()` supports contiguous containers such as `std::vector` and `std::array`

```cpp
#include <array>
#include <cstdint>
#include <vector>

std::vector< std::uint8_t > bytes = { 0x10, 0x20, 0x30, 0x40 };
std::array< std::uint8_t, 4 > array = { 0x50, 0x60, 0x70, 0x80 };

auto vector_view = binview::make_view( bytes );
auto array_view = binview::make_view( array );
```

### Reader

For sequential binary parsing:

```cpp
binview::Reader reader{ view };

std::uint16_t value;
if (reader.read_le(value))
{
    // ...
}
```

Big-endian reads are also supported:

```cpp
reader.read_be(value);
```

## Building

```bash
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build build
ctest --test-dir build --output-on-failure
```

## Requirements

* C++11 or newer
* CMake 3.10 or newer

## License

This project is licensed under the MIT License. See [LICENSE](LICENSE) for details.