# binview

A lightweight C++11 binary data viewing and reading library

# Features

* header-only
* zero-copy
* 0 dependencies
* platform-independent
* little-endian and big-endian support
* works with raw memory and contiguous containers
* supports read-only and mutable views

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

`View` can be created from raw memory, while `make_view()` supports contiguous containers such as `std::vector` and `std::array`.

```cpp
#include <array>
#include <cstdint>
#include <vector>

std::vector< std::uint8_t > bytes = { 0x10, 0x20, 0x30, 0x40 };
std::array< std::uint8_t, 4 > array = { 0x50, 0x60, 0x70, 0x80 };

auto vector_view = binview::make_view( bytes );
auto array_view = binview::make_view( array );
```

For mutable containers, use `make_mutable_view()`:

```cpp
auto mutable_view = binview::make_mutable_view( bytes );
mutable_view[ 0 ] = 0xFF;
```

### Reader

For sequential binary parsing. `read()` defaults to little-endian:

```cpp
binview::Reader reader{ view };

std::uint16_t value;
if( reader.read( value ) )
{
    // ...
}
```

Big-endian reads are also supported:

```cpp
reader.read_be( value );
```

### Writer

For sequential binary writing. `write()` defaults to little-endian:

```cpp
binview::MutableView view = binview::make_mutable_view( bytes );
binview::Writer writer{ view };

writer.write( std::uint16_t( 0x1234 ) );
writer.write( std::uint32_t( 0xDEADBEEF ) );
```

Big-endian writes are also supported:

```cpp
writer.write_be( std::uint32_t( 0x12345678 ) );
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
