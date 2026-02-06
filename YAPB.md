# YAPB Architecture & API

## Packet Format

Every YAPB packet has a simple wire format:

```
[4-byte header: total packet length in network byte order]
[element 1: 1-byte type tag + value bytes in network byte order]
[element 2: ...]
...
```

### Type Tags

| Tag | Type | Value Size |
|-----|------|-----------|
| 0x00 | INT8 | 1 byte |
| 0x01 | INT16 | 2 bytes |
| 0x02 | INT32 | 4 bytes |
| 0x03 | INT64 | 8 bytes |
| 0x04 | FLOAT | 4 bytes |
| 0x05 | DOUBLE | 8 bytes |
| 0x0E | BLOB | 2-byte length + N raw bytes |
| 0x0F | NESTED_PKT | full nested packet (with its own 4-byte header) |

Tags 0x06-0x0D are reserved for future types.

## Core Concepts

### Write Mode

1. Call `YAPB_initialize()` with a buffer
2. Push elements with `YAPB_push_*()` functions
3. Call `YAPB_finalize()` to write the header length

### Read Mode

1. Call `YAPB_load()` with raw packet data
2. Pop elements with `YAPB_pop_*()` in the same order they were pushed
3. Each pop returns `YAPB_OK` (more data), `YAPB_STS_COMPLETE` (last element), or an error

### Sticky Errors

YAPB uses sticky errors (like `errno` or OpenGL). Once an error occurs, all subsequent push/pop calls return that same error immediately. This means you can chain operations and check once at the end:

```c
YAPB_pop_i8(&pkt, &field1);
YAPB_pop_i16(&pkt, &field2);
YAPB_pop_i32(&pkt, &field3);
if (YAPB_get_error(&pkt) < 0) {
    /* something went wrong */
}
```

### Forward Compatibility

Pop functions do NOT modify the output on error or YAPB_STS_COMPLETE. Initialize fields to defaults before popping - if the packet lacks that field, the default is preserved:

```c
uint16_t new_field = 42;       /* default for older packets */
YAPB_pop_u16(&pkt, &new_field); /* stays 42 if packet ended */
```

## API Reference

### Lifecycle

| Function | Description |
|----------|-------------|
| `YAPB_initialize(pkt, buf, size)` | Init packet for writing |
| `YAPB_finalize(pkt, &len)` | Write header, get total length |
| `YAPB_load(pkt, data, size)` | Load raw data for reading |

### Push (Write Mode)

| Function | Description |
|----------|-------------|
| `YAPB_push_i8/i16/i32/i64` | Push signed integer |
| `YAPB_push_u8/u16/u32/u64` | Push unsigned integer (inline wrappers) |
| `YAPB_push_float/double` | Push floating point |
| `YAPB_push_blob(pkt, data, len)` | Push raw bytes (max 65535) |
| `YAPB_push_nested(pkt, nested)` | Push a finalized packet inside another |

### Pop (Read Mode)

| Function | Description |
|----------|-------------|
| `YAPB_pop_i8/i16/i32/i64` | Pop signed integer |
| `YAPB_pop_u8/u16/u32/u64` | Pop unsigned integer (inline wrappers) |
| `YAPB_pop_float/double` | Pop floating point |
| `YAPB_pop_blob(pkt, &data, &len)` | Pop blob (pointer into packet buffer) |
| `YAPB_pop_nested(pkt, &nested)` | Pop nested packet |

### Query

| Function | Description |
|----------|-------------|
| `YAPB_get_error(pkt)` | Get sticky error state |
| `YAPB_get_elem_count(pkt, &count)` | Count elements without advancing position |
| `YAPB_Result_str(result)` | Get string name for result code |

## Important Notes

- All integer values are stored in **network byte order** (big-endian)
- `YAPB_pop_blob()` returns a pointer into the packet buffer - do not free it
- Nested packets contain their own 4-byte header within the parent
- The header length includes itself (minimum valid packet is 4 bytes)
- Buffer must be at least `YAPB_HEADER_SIZE` (4) bytes for initialization
