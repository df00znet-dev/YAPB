#include "yapb.h"
#include <string.h>
#include <arpa/inet.h>

// Helper to write uint16 in network byte order
static inline void write_u16(uint8_t *dst, uint16_t val) {
    uint16_t net = htons(val);
    memcpy(dst, &net, 2);
}

// Helper to write uint32 in network byte order
static inline void write_u32(uint8_t *dst, uint32_t val) {
    uint32_t net = htonl(val);
    memcpy(dst, &net, 4);
}

// Helper to write uint64 in network byte order
static inline void write_u64(uint8_t *dst, uint64_t val) {
    uint32_t high = htonl((uint32_t)(val >> 32));
    uint32_t low = htonl((uint32_t)(val & 0xFFFFFFFF));
    memcpy(dst, &high, 4);
    memcpy(dst + 4, &low, 4);
}

// Helper to read uint16 from network byte order
static inline uint16_t read_u16(const uint8_t *src) {
    uint16_t net;
    memcpy(&net, src, 2);
    return ntohs(net);
}

// Helper to read uint32 from network byte order
static inline uint32_t read_u32(const uint8_t *src) {
    uint32_t net;
    memcpy(&net, src, 4);
    return ntohl(net);
}

// Helper to read uint64 from network byte order
static inline uint64_t read_u64(const uint8_t *src) {
    uint32_t high, low;
    memcpy(&high, src, 4);
    memcpy(&low, src + 4, 4);
    return ((uint64_t)ntohl(high) << 32) | ntohl(low);
}

// Helper to check if at end of packet (for returning COMPLETE vs OK)
static inline YAPB_Result_t check_complete(YAPB_Packet_t *pkt) {
    return (pkt->pos >= pkt->buffer_size) ? YAPB_STS_COMPLETE : YAPB_OK;
}

// Helper to validate pop preconditions and type
static inline YAPB_Result_t _pop_validate(YAPB_Packet_t *pkt, void *out, YAPB_Type_t expected, size_t type_size) {
    if (pkt == NULL || out == NULL) {
        return YAPB_ERR_NULL_PTR;
    }
    // Check sticky error state
    if (pkt->error < 0) {
        return pkt->error;
    }
    if (pkt->mode != YAPB_MODE_READ) {
        pkt->error = YAPB_ERR_INVALID_MODE;
        return pkt->error;
    }
    if (pkt->pos >= pkt->buffer_size) {
        pkt->error = YAPB_ERR_NO_MORE_ELEMENTS;
        return pkt->error;
    }
    YAPB_Type_t type = (YAPB_Type_t)pkt->buffer[pkt->pos];
    if (type != expected) {
        pkt->error = YAPB_ERR_TYPE_MISMATCH;
        return pkt->error;
    }

    if (pkt->pos + 1 + type_size > pkt->buffer_size) {
        pkt->error = YAPB_ERR_INVALID_PACKET;
        return pkt->error;
    }
    
    pkt->pos++;  // consume type byte
    return YAPB_OK;
}

// Helper to validate push preconditions
static inline YAPB_Result_t push_validate(YAPB_Packet_t *pkt, const void *val, size_t needed) {
    if (pkt == NULL || val == NULL) {
        return YAPB_ERR_NULL_PTR;
    }
    // Check sticky error state
    if (pkt->error < 0) {
        return pkt->error;
    }
    if (pkt->mode != YAPB_MODE_WRITE) {
        pkt->error = YAPB_ERR_INVALID_MODE;
        return pkt->error;
    }
    if (pkt->pos + needed > pkt->buffer_size) {
        pkt->error = YAPB_ERR_BUFFER_TOO_SMALL;
        return pkt->error;
    }
    return YAPB_OK;
}

YAPB_Result_t YAPB_initialize(YAPB_Packet_t *pkt, uint8_t *buffer, size_t size) {
    if (pkt == NULL || buffer == NULL) {
        return YAPB_ERR_NULL_PTR;
    }
    if (size < YAPB_HEADER_SIZE) {
        return YAPB_ERR_BUFFER_TOO_SMALL;
    }

    pkt->buffer = buffer;
    pkt->buffer_size = size;
    pkt->pos = YAPB_HEADER_SIZE;
    pkt->mode = YAPB_MODE_WRITE;
    pkt->error = YAPB_OK;  // clear error state

    // Write 0 length to header as safety measure in case finalize is forgotten
    write_u32(buffer, 0);

    return YAPB_OK;
}

YAPB_Result_t YAPB_finalize(YAPB_Packet_t *pkt, size_t *out_len) {
    if (pkt == NULL) {
        return YAPB_ERR_NULL_PTR;
    }
    if (pkt->mode != YAPB_MODE_WRITE) {
        return YAPB_ERR_INVALID_MODE;
    }

    write_u32(pkt->buffer, (uint32_t)pkt->pos);

    if (out_len != NULL) {
        *out_len = pkt->pos;
    }

    return YAPB_OK;
}

YAPB_Result_t YAPB_load(YAPB_Packet_t *pkt, const uint8_t *data, size_t size) {
    if (pkt == NULL || data == NULL) {
        return YAPB_ERR_NULL_PTR;
    }
    if (size < YAPB_HEADER_SIZE) {
        return YAPB_ERR_BUFFER_TOO_SMALL;
    }

    uint32_t pkt_len = read_u32(data);
    if (pkt_len > size || pkt_len < YAPB_HEADER_SIZE) {
        return YAPB_ERR_INVALID_PACKET;
    }

    pkt->buffer = (uint8_t *)data;
    pkt->buffer_size = pkt_len;
    pkt->pos = YAPB_HEADER_SIZE;
    pkt->mode = YAPB_MODE_READ;
    pkt->error = YAPB_OK;  // clear error state

    return YAPB_OK;
}

// ============ Push functions ============

YAPB_Result_t YAPB_push_i8(YAPB_Packet_t *pkt, const int8_t *val) {
    YAPB_Result_t r = push_validate(pkt, val, 1 + 1);
    if (r != YAPB_OK) return r;

    pkt->buffer[pkt->pos++] = YAPB_INT8;
    pkt->buffer[pkt->pos++] = (uint8_t)*val;
    return YAPB_OK;
}

YAPB_Result_t YAPB_push_i16(YAPB_Packet_t *pkt, const int16_t *val) {
    YAPB_Result_t r = push_validate(pkt, val, 1 + 2);
    if (r != YAPB_OK) return r;

    pkt->buffer[pkt->pos++] = YAPB_INT16;
    write_u16(pkt->buffer + pkt->pos, (uint16_t)*val);
    pkt->pos += 2;
    return YAPB_OK;
}

YAPB_Result_t YAPB_push_i32(YAPB_Packet_t *pkt, const int32_t *val) {
    YAPB_Result_t r = push_validate(pkt, val, 1 + 4);
    if (r != YAPB_OK) return r;

    pkt->buffer[pkt->pos++] = YAPB_INT32;
    write_u32(pkt->buffer + pkt->pos, (uint32_t)*val);
    pkt->pos += 4;
    return YAPB_OK;
}

YAPB_Result_t YAPB_push_i64(YAPB_Packet_t *pkt, const int64_t *val) {
    YAPB_Result_t r = push_validate(pkt, val, 1 + 8);
    if (r != YAPB_OK) return r;

    pkt->buffer[pkt->pos++] = YAPB_INT64;
    write_u64(pkt->buffer + pkt->pos, (uint64_t)*val);
    pkt->pos += 8;
    return YAPB_OK;
}

YAPB_Result_t YAPB_push_float(YAPB_Packet_t *pkt, const float *val) {
    YAPB_Result_t r = push_validate(pkt, val, 1 + 4);
    if (r != YAPB_OK) return r;

    pkt->buffer[pkt->pos++] = YAPB_FLOAT;
    uint32_t bits;
    memcpy(&bits, val, 4);
    write_u32(pkt->buffer + pkt->pos, bits);
    pkt->pos += 4;
    return YAPB_OK;
}

YAPB_Result_t YAPB_push_double(YAPB_Packet_t *pkt, const double *val) {
    YAPB_Result_t r = push_validate(pkt, val, 1 + 8);
    if (r != YAPB_OK) return r;

    pkt->buffer[pkt->pos++] = YAPB_DOUBLE;
    uint64_t bits;
    memcpy(&bits, val, 8);
    write_u64(pkt->buffer + pkt->pos, bits);
    pkt->pos += 8;
    return YAPB_OK;
}

YAPB_Result_t YAPB_push_blob(YAPB_Packet_t *pkt, const uint8_t *data, uint16_t len) {
    if (pkt == NULL) {
        return YAPB_ERR_NULL_PTR;
    }
    if (pkt->error < 0) {
        return pkt->error;
    }
    if (len > 0 && data == NULL) {
        return YAPB_ERR_NULL_PTR;
    }
    if (pkt->mode != YAPB_MODE_WRITE) {
        pkt->error = YAPB_ERR_INVALID_MODE;
        return pkt->error;
    }
    if (pkt->pos + 1 + 2 + len > pkt->buffer_size) {
        pkt->error = YAPB_ERR_BUFFER_TOO_SMALL;
        return pkt->error;
    }

    pkt->buffer[pkt->pos++] = YAPB_BLOB;
    write_u16(pkt->buffer + pkt->pos, len);
    pkt->pos += 2;
    if (len > 0) {
        memcpy(pkt->buffer + pkt->pos, data, len);
        pkt->pos += len;
    }
    return YAPB_OK;
}

YAPB_Result_t YAPB_push_nested(YAPB_Packet_t *pkt, const YAPB_Packet_t *nested) {
    if (pkt == NULL || nested == NULL || nested->buffer == NULL) {
        return YAPB_ERR_NULL_PTR;
    }
    if (pkt->error < 0) {
        return pkt->error;
    }
    if (pkt->mode != YAPB_MODE_WRITE) {
        pkt->error = YAPB_ERR_INVALID_MODE;
        return pkt->error;
    }

    uint32_t nested_len = read_u32(nested->buffer);
    if (pkt->pos + 1 + nested_len > pkt->buffer_size) {
        pkt->error = YAPB_ERR_BUFFER_TOO_SMALL;
        return pkt->error;
    }

    pkt->buffer[pkt->pos++] = YAPB_NESTED_PKT;
    memcpy(pkt->buffer + pkt->pos, nested->buffer, nested_len);
    pkt->pos += nested_len;
    return YAPB_OK;
}

// ============ Pop functions ============

YAPB_Result_t YAPB_pop_i8(YAPB_Packet_t *pkt, int8_t *out) {
    YAPB_Result_t r = _pop_validate(pkt, out, YAPB_INT8, 1);
    if (r != YAPB_OK) return r;
    *out = (int8_t)pkt->buffer[pkt->pos++];
    return check_complete(pkt);
}

YAPB_Result_t YAPB_pop_i16(YAPB_Packet_t *pkt, int16_t *out) {
    YAPB_Result_t r = _pop_validate(pkt, out, YAPB_INT16, 2);
    if (r != YAPB_OK) return r;
    *out = (int16_t)read_u16(pkt->buffer + pkt->pos);
    pkt->pos += 2;
    return check_complete(pkt);
}

YAPB_Result_t YAPB_pop_i32(YAPB_Packet_t *pkt, int32_t *out) {
    YAPB_Result_t r = _pop_validate(pkt, out, YAPB_INT32, 4);
    if (r != YAPB_OK) return r;
    *out = (int32_t)read_u32(pkt->buffer + pkt->pos);
    pkt->pos += 4;
    return check_complete(pkt);
}

YAPB_Result_t YAPB_pop_i64(YAPB_Packet_t *pkt, int64_t *out) {
    YAPB_Result_t r = _pop_validate(pkt, out, YAPB_INT64, 8);
    if (r != YAPB_OK) return r;
    *out = (int64_t)read_u64(pkt->buffer + pkt->pos);
    pkt->pos += 8;
    return check_complete(pkt);
}

YAPB_Result_t YAPB_pop_float(YAPB_Packet_t *pkt, float *out) {
    YAPB_Result_t r = _pop_validate(pkt, out, YAPB_FLOAT, 4);
    if (r != YAPB_OK) return r;
    uint32_t bits = read_u32(pkt->buffer + pkt->pos);
    memcpy(out, &bits, 4);
    pkt->pos += 4;
    return check_complete(pkt);
}

YAPB_Result_t YAPB_pop_double(YAPB_Packet_t *pkt, double *out) {
    YAPB_Result_t r = _pop_validate(pkt, out, YAPB_DOUBLE, 8);
    if (r != YAPB_OK) return r;
    uint64_t bits = read_u64(pkt->buffer + pkt->pos);
    memcpy(out, &bits, 8);
    pkt->pos += 8;
    return check_complete(pkt);
}

YAPB_Result_t YAPB_pop_blob(YAPB_Packet_t *pkt, const uint8_t **data, uint16_t *len) {
    if (len == NULL) return YAPB_ERR_NULL_PTR;

    if (pkt->pos + 3 > pkt->buffer_size) {
        pkt->error = YAPB_ERR_INVALID_PACKET;
        return pkt->error;
    }
    *len = read_u16(pkt->buffer + pkt->pos + 1);
    YAPB_Result_t r = _pop_validate(pkt, data, YAPB_BLOB, *len + 2);
    if (r != YAPB_OK) return r;
    pkt->pos += 2;

    *data = pkt->buffer + pkt->pos;
    pkt->pos += *len;
    return check_complete(pkt);
}

YAPB_Result_t YAPB_pop_nested(YAPB_Packet_t *pkt, YAPB_Packet_t *out) {

    if (pkt->pos + YAPB_HEADER_SIZE + 1 > pkt->buffer_size) {
        pkt->error = YAPB_ERR_INVALID_PACKET;
        return pkt->error;
    }
    uint32_t nested_len = read_u32(pkt->buffer + pkt->pos + 1); //the header starts with len

    YAPB_Result_t r = _pop_validate(pkt, out, YAPB_NESTED_PKT, nested_len);
    if (r != YAPB_OK) return r;

    r = YAPB_load(out, pkt->buffer + pkt->pos, nested_len);
    if (r != YAPB_OK) {
        pkt->error = r;
        return pkt->error;
    }

    pkt->pos += nested_len;
    return check_complete(pkt);
}

YAPB_Result_t YAPB_get_elem_count(const YAPB_Packet_t *pkt, uint16_t *out_count) {
    if (pkt == NULL || out_count == NULL) {
        return YAPB_ERR_NULL_PTR;
    }
    if (pkt->mode != YAPB_MODE_READ) {
        return YAPB_ERR_INVALID_MODE;
    }

    uint16_t count = 0;
    size_t scan_pos = YAPB_HEADER_SIZE;
    size_t data_end = pkt->buffer_size;

    while (scan_pos < data_end) {
        YAPB_Type_t type = (YAPB_Type_t)pkt->buffer[scan_pos++];

        size_t skip;
        switch (type) {
            case YAPB_INT8:
                skip = 1;
                break;
            case YAPB_INT16:
                skip = 2;
                break;
            case YAPB_INT32:
                skip = 4;
                break;
            case YAPB_INT64:
                skip = 8;
                break;
            case YAPB_FLOAT:
                skip = 4;
                break;
            case YAPB_DOUBLE:
                skip = 8;
                break;
            case YAPB_BLOB:
                if (scan_pos + 2 > data_end) {
                    return YAPB_ERR_INVALID_PACKET;
                }
                skip = 2 + read_u16(pkt->buffer + scan_pos);
                break;
            case YAPB_NESTED_PKT:
                if (scan_pos + YAPB_HEADER_SIZE > data_end) {
                    return YAPB_ERR_INVALID_PACKET;
                }
                skip = read_u32(pkt->buffer + scan_pos);
                break;
            default:
                return YAPB_ERR_INVALID_PACKET;
        }

        if (scan_pos + skip > data_end) {
            return YAPB_ERR_INVALID_PACKET;
        }
        scan_pos += skip;
        count++;
    }

    *out_count = count;
    return YAPB_OK;
}

YAPB_Result_t YAPB_pop_next(YAPB_Packet_t *pkt, YAPB_Element_t *out) {
    if (pkt == NULL || out == NULL) {
        return YAPB_ERR_NULL_PTR;
    }
    if (pkt->error < 0) {
        return pkt->error;
    }
    if (pkt->mode != YAPB_MODE_READ) {
        pkt->error = YAPB_ERR_INVALID_MODE;
        return pkt->error;
    }
    if (pkt->pos >= pkt->buffer_size) {
        pkt->error = YAPB_ERR_NO_MORE_ELEMENTS;
        return pkt->error;
    }

    YAPB_Type_t type = (YAPB_Type_t)pkt->buffer[pkt->pos];
    out->type = type;

    switch (type) {
        case YAPB_INT8:    return YAPB_pop_i8(pkt, &out->val.i8);
        case YAPB_INT16:   return YAPB_pop_i16(pkt, &out->val.i16);
        case YAPB_INT32:   return YAPB_pop_i32(pkt, &out->val.i32);
        case YAPB_INT64:   return YAPB_pop_i64(pkt, &out->val.i64);
        case YAPB_FLOAT:   return YAPB_pop_float(pkt, &out->val.f);
        case YAPB_DOUBLE:  return YAPB_pop_double(pkt, &out->val.d);
        case YAPB_BLOB:    return YAPB_pop_blob(pkt, &out->val.blob.data, &out->val.blob.len);
        case YAPB_NESTED_PKT: return YAPB_pop_nested(pkt, &out->val.nested);
        default:
            pkt->error = YAPB_ERR_INVALID_PACKET;
            return pkt->error;
    }
}

YAPB_Result_t YAPB_get_error(const YAPB_Packet_t *pkt) {
    if (pkt == NULL) {
        return YAPB_ERR_NULL_PTR;
    }
    return pkt->error;
}

const char *YAPB_Result_str(YAPB_Result_t result) {
    switch (result) {
        case YAPB_STS_COMPLETE:         return "Complete";
        case YAPB_OK:                   return "OK";
        case YAPB_ERR_UNKNOWN:          return "Unknown error";
        case YAPB_ERR_NULL_PTR:         return "Null pointer";
        case YAPB_ERR_BUFFER_TOO_SMALL: return "Buffer too small";
        case YAPB_ERR_INVALID_MODE:     return "Invalid mode";
        case YAPB_ERR_TYPE_MISMATCH:    return "Type mismatch";
        case YAPB_ERR_NO_MORE_ELEMENTS: return "No more elements";
        case YAPB_ERR_INVALID_PACKET:   return "Invalid packet";
        default:                        return "Unknown";
    }
}
