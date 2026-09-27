# binview

A lightweight C++11 binary data viewing and reading library

# Features

* header-only
* zero-copy
* no library or runtime dependencies
* platform-independent
* little-endian and big-endian support
* works with raw memory and contiguous containers
* supports read-only and mutable views

## Future

* floating-point reads and writes
* ULEB128 and SLEB128 support
* bit-level reader and writer
* additional binary encoding utilities


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

## Benchmarks

The runtime benchmark uses [nanobench](https://github.com/martinus/nanobench), and CMake fetches it only when `BINVIEW_BUILD_BENCHMARKS` is enabled (off by default).

```bash
cmake -S . -B build-bench -DBINVIEW_BUILD_TESTS=OFF -DBINVIEW_BUILD_BENCHMARKS=ON -DCMAKE_BUILD_TYPE=Release
cmake --build build-bench
./build-bench/binview_runtime_benchmarks
```

One Release run on Windows x64 with `clang-cl`, using a 256-byte in-memory buffer and batches of 64 reads:

| Benchmark | Approx. time per read | Approx. throughput |
| --- | ---: | ---: |
| `read_le<uint32_t>` | 0.05 ns | 19.9 billion reads/s |
| Byte-loop reference | 0.05 ns | 19.6 billion reads/s |
| `View::read<uint32_t>` | 0.73 ns | 1.36 billion reads/s |
| Sequential `Reader::read_le<uint32_t>` | 0.48 ns | 2.07 billion reads/s |

These timings are from my machine and show the throughput of 64 reads with warm buffers, rather than the latency of a single read

The compile-time benchmark is a separate target:

```bash
cmake -E time cmake --build build-bench --target binview_compile_time_benchmark --clean-first
```

## Requirements

* C++11 or newer
* CMake 3.11 or newer

## License

This project is licensed under the MIT License. See [LICENSE](LICENSE) for details.
