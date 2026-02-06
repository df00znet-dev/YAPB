#include "munit.h"
#include "yapb.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/* ======== Initialize / Finalize ======== */

static MunitResult test_init_finalize(const MunitParameter params[], void *data) {
    (void)params; (void)data;
    uint8_t buf[256];
    YAPB_Packet_t pkt;

    munit_assert_int(YAPB_initialize(&pkt, buf, sizeof(buf)), ==, YAPB_OK);
    size_t len;
    munit_assert_int(YAPB_finalize(&pkt, &len), ==, YAPB_OK);
    munit_assert_size(len, ==, YAPB_HEADER_SIZE);
    return MUNIT_OK;
}

static MunitResult test_init_null(const MunitParameter params[], void *data) {
    (void)params; (void)data;
    uint8_t buf[16];
    YAPB_Packet_t pkt;

    munit_assert_int(YAPB_initialize(NULL, buf, sizeof(buf)), ==, YAPB_ERR_NULL_PTR);
    munit_assert_int(YAPB_initialize(&pkt, NULL, sizeof(buf)), ==, YAPB_ERR_NULL_PTR);
    return MUNIT_OK;
}

static MunitResult test_init_too_small(const MunitParameter params[], void *data) {
    (void)params; (void)data;
    uint8_t buf[2];
    YAPB_Packet_t pkt;

    munit_assert_int(YAPB_initialize(&pkt, buf, sizeof(buf)), ==, YAPB_ERR_BUFFER_TOO_SMALL);
    return MUNIT_OK;
}

/* ======== Push / Pop integers ======== */

static MunitResult test_i8_roundtrip(const MunitParameter params[], void *data) {
    (void)params; (void)data;
    uint8_t buf[256];
    YAPB_Packet_t pkt;

    YAPB_initialize(&pkt, buf, sizeof(buf));
    int8_t val = -42;
    munit_assert_int(YAPB_push_i8(&pkt, &val), ==, YAPB_OK);
    size_t len;
    YAPB_finalize(&pkt, &len);

    YAPB_Packet_t rpkt;
    munit_assert_int(YAPB_load(&rpkt, buf, len), ==, YAPB_OK);
    int8_t out = 0;
    YAPB_Result_t r = YAPB_pop_i8(&rpkt, &out);
    munit_assert_int(r, ==, YAPB_STS_COMPLETE);
    munit_assert_int8(out, ==, -42);
    return MUNIT_OK;
}

static MunitResult test_i16_roundtrip(const MunitParameter params[], void *data) {
    (void)params; (void)data;
    uint8_t buf[256];
    YAPB_Packet_t pkt;

    YAPB_initialize(&pkt, buf, sizeof(buf));
    int16_t val = -1234;
    YAPB_push_i16(&pkt, &val);
    size_t len;
    YAPB_finalize(&pkt, &len);

    YAPB_Packet_t rpkt;
    YAPB_load(&rpkt, buf, len);
    int16_t out = 0;
    YAPB_Result_t r = YAPB_pop_i16(&rpkt, &out);
    munit_assert_int(r, ==, YAPB_STS_COMPLETE);
    munit_assert_int16(out, ==, -1234);
    return MUNIT_OK;
}

static MunitResult test_i32_roundtrip(const MunitParameter params[], void *data) {
    (void)params; (void)data;
    uint8_t buf[256];
    YAPB_Packet_t pkt;

    YAPB_initialize(&pkt, buf, sizeof(buf));
    int32_t val = -100000;
    YAPB_push_i32(&pkt, &val);
    size_t len;
    YAPB_finalize(&pkt, &len);

    YAPB_Packet_t rpkt;
    YAPB_load(&rpkt, buf, len);
    int32_t out = 0;
    YAPB_Result_t r = YAPB_pop_i32(&rpkt, &out);
    munit_assert_int(r, ==, YAPB_STS_COMPLETE);
    munit_assert_int32(out, ==, -100000);
    return MUNIT_OK;
}

static MunitResult test_i64_roundtrip(const MunitParameter params[], void *data) {
    (void)params; (void)data;
    uint8_t buf[256];
    YAPB_Packet_t pkt;

    YAPB_initialize(&pkt, buf, sizeof(buf));
    int64_t val = -9876543210LL;
    YAPB_push_i64(&pkt, &val);
    size_t len;
    YAPB_finalize(&pkt, &len);

    YAPB_Packet_t rpkt;
    YAPB_load(&rpkt, buf, len);
    int64_t out = 0;
    YAPB_Result_t r = YAPB_pop_i64(&rpkt, &out);
    munit_assert_int(r, ==, YAPB_STS_COMPLETE);
    munit_assert_int64(out, ==, -9876543210LL);
    return MUNIT_OK;
}

/* ======== Push / Pop unsigned ======== */

static MunitResult test_u16_roundtrip(const MunitParameter params[], void *data) {
    (void)params; (void)data;
    uint8_t buf[256];
    YAPB_Packet_t pkt;

    YAPB_initialize(&pkt, buf, sizeof(buf));
    uint16_t val = 65000;
    YAPB_push_u16(&pkt, &val);
    size_t len;
    YAPB_finalize(&pkt, &len);

    YAPB_Packet_t rpkt;
    YAPB_load(&rpkt, buf, len);
    uint16_t out = 0;
    YAPB_Result_t r = YAPB_pop_u16(&rpkt, &out);
    munit_assert_int(r, ==, YAPB_STS_COMPLETE);
    munit_assert_uint16(out, ==, 65000);
    return MUNIT_OK;
}

/* ======== Float / Double ======== */

static MunitResult test_float_roundtrip(const MunitParameter params[], void *data) {
    (void)params; (void)data;
    uint8_t buf[256];
    YAPB_Packet_t pkt;

    YAPB_initialize(&pkt, buf, sizeof(buf));
    float val = 3.14f;
    YAPB_push_float(&pkt, &val);
    size_t len;
    YAPB_finalize(&pkt, &len);

    YAPB_Packet_t rpkt;
    YAPB_load(&rpkt, buf, len);
    float out = 0;
    YAPB_Result_t r = YAPB_pop_float(&rpkt, &out);
    munit_assert_int(r, ==, YAPB_STS_COMPLETE);
    munit_assert_float(out, ==, 3.14f);
    return MUNIT_OK;
}

static MunitResult test_double_roundtrip(const MunitParameter params[], void *data) {
    (void)params; (void)data;
    uint8_t buf[256];
    YAPB_Packet_t pkt;

    YAPB_initialize(&pkt, buf, sizeof(buf));
    double val = 2.718281828;
    YAPB_push_double(&pkt, &val);
    size_t len;
    YAPB_finalize(&pkt, &len);

    YAPB_Packet_t rpkt;
    YAPB_load(&rpkt, buf, len);
    double out = 0;
    YAPB_Result_t r = YAPB_pop_double(&rpkt, &out);
    munit_assert_int(r, ==, YAPB_STS_COMPLETE);
    munit_assert_double(out, ==, 2.718281828);
    return MUNIT_OK;
}

/* ======== Blob ======== */

static MunitResult test_blob_roundtrip(const MunitParameter params[], void *data) {
    (void)params; (void)data;
    uint8_t buf[256];
    YAPB_Packet_t pkt;

    YAPB_initialize(&pkt, buf, sizeof(buf));
    const uint8_t blob[] = {0xDE, 0xAD, 0xBE, 0xEF};
    YAPB_push_blob(&pkt, blob, sizeof(blob));
    size_t len;
    YAPB_finalize(&pkt, &len);

    YAPB_Packet_t rpkt;
    YAPB_load(&rpkt, buf, len);
    const uint8_t *out_data;
    uint16_t out_len;
    YAPB_Result_t r = YAPB_pop_blob(&rpkt, &out_data, &out_len);
    munit_assert_int(r, ==, YAPB_STS_COMPLETE);
    munit_assert_uint16(out_len, ==, 4);
    munit_assert_memory_equal(4, out_data, blob);
    return MUNIT_OK;
}

static MunitResult test_blob_empty(const MunitParameter params[], void *data) {
    (void)params; (void)data;
    uint8_t buf[256];
    YAPB_Packet_t pkt;

    YAPB_initialize(&pkt, buf, sizeof(buf));
    YAPB_push_blob(&pkt, NULL, 0);
    size_t len;
    YAPB_finalize(&pkt, &len);

    YAPB_Packet_t rpkt;
    YAPB_load(&rpkt, buf, len);
    const uint8_t *out_data;
    uint16_t out_len;
    YAPB_Result_t r = YAPB_pop_blob(&rpkt, &out_data, &out_len);
    munit_assert_int(r, ==, YAPB_STS_COMPLETE);
    munit_assert_uint16(out_len, ==, 0);
    return MUNIT_OK;
}

/* ======== Nested packet ======== */

static MunitResult test_nested_roundtrip(const MunitParameter params[], void *data) {
    (void)params; (void)data;
    uint8_t inner_buf[128];
    uint8_t outer_buf[256];
    YAPB_Packet_t inner, outer;

    /* Build inner packet */
    YAPB_initialize(&inner, inner_buf, sizeof(inner_buf));
    int32_t ival = 999;
    YAPB_push_i32(&inner, &ival);
    YAPB_finalize(&inner, NULL);

    /* Build outer packet containing nested */
    YAPB_initialize(&outer, outer_buf, sizeof(outer_buf));
    int8_t tag = 1;
    YAPB_push_i8(&outer, &tag);
    YAPB_push_nested(&outer, &inner);
    size_t len;
    YAPB_finalize(&outer, &len);

    /* Read back */
    YAPB_Packet_t rpkt;
    YAPB_load(&rpkt, outer_buf, len);
    int8_t out_tag = 0;
    YAPB_pop_i8(&rpkt, &out_tag);
    munit_assert_int8(out_tag, ==, 1);

    YAPB_Packet_t nested_out;
    YAPB_Result_t r = YAPB_pop_nested(&rpkt, &nested_out);
    munit_assert_int(r, ==, YAPB_STS_COMPLETE);

    int32_t out_ival = 0;
    r = YAPB_pop_i32(&nested_out, &out_ival);
    munit_assert_int(r, ==, YAPB_STS_COMPLETE);
    munit_assert_int32(out_ival, ==, 999);
    return MUNIT_OK;
}

/* ======== Multiple elements ======== */

static MunitResult test_multi_element(const MunitParameter params[], void *data) {
    (void)params; (void)data;
    uint8_t buf[256];
    YAPB_Packet_t pkt;

    YAPB_initialize(&pkt, buf, sizeof(buf));
    int8_t a = 10;
    int16_t b = 2000;
    int32_t c = 300000;
    YAPB_push_i8(&pkt, &a);
    YAPB_push_i16(&pkt, &b);
    YAPB_push_i32(&pkt, &c);
    size_t len;
    YAPB_finalize(&pkt, &len);

    YAPB_Packet_t rpkt;
    YAPB_load(&rpkt, buf, len);
    int8_t oa = 0; int16_t ob = 0; int32_t oc = 0;
    munit_assert_int(YAPB_pop_i8(&rpkt, &oa), ==, YAPB_OK);
    munit_assert_int(YAPB_pop_i16(&rpkt, &ob), ==, YAPB_OK);
    munit_assert_int(YAPB_pop_i32(&rpkt, &oc), ==, YAPB_STS_COMPLETE);
    munit_assert_int8(oa, ==, 10);
    munit_assert_int16(ob, ==, 2000);
    munit_assert_int32(oc, ==, 300000);
    return MUNIT_OK;
}

/* ======== Element count ======== */

static MunitResult test_elem_count(const MunitParameter params[], void *data) {
    (void)params; (void)data;
    uint8_t buf[256];
    YAPB_Packet_t pkt;

    YAPB_initialize(&pkt, buf, sizeof(buf));
    int8_t a = 1; int16_t b = 2; int32_t c = 3;
    YAPB_push_i8(&pkt, &a);
    YAPB_push_i16(&pkt, &b);
    YAPB_push_i32(&pkt, &c);
    size_t len;
    YAPB_finalize(&pkt, &len);

    YAPB_Packet_t rpkt;
    YAPB_load(&rpkt, buf, len);
    uint16_t count = 0;
    munit_assert_int(YAPB_get_elem_count(&rpkt, &count), ==, YAPB_OK);
    munit_assert_uint16(count, ==, 3);
    return MUNIT_OK;
}

/* ======== Sticky errors ======== */

static MunitResult test_sticky_error(const MunitParameter params[], void *data) {
    (void)params; (void)data;
    uint8_t buf[256];
    YAPB_Packet_t pkt;

    YAPB_initialize(&pkt, buf, sizeof(buf));
    int8_t val = 1;
    YAPB_push_i8(&pkt, &val);
    size_t len;
    YAPB_finalize(&pkt, &len);

    YAPB_Packet_t rpkt;
    YAPB_load(&rpkt, buf, len);

    /* Pop wrong type to trigger error */
    int16_t bad = 0;
    munit_assert_int(YAPB_pop_i16(&rpkt, &bad), ==, YAPB_ERR_TYPE_MISMATCH);

    /* Subsequent pop should also fail (sticky) */
    int8_t good = 0;
    munit_assert_int(YAPB_pop_i8(&rpkt, &good), ==, YAPB_ERR_TYPE_MISMATCH);

    munit_assert_int(YAPB_get_error(&rpkt), ==, YAPB_ERR_TYPE_MISMATCH);
    return MUNIT_OK;
}

/* ======== Type mismatch ======== */

static MunitResult test_type_mismatch(const MunitParameter params[], void *data) {
    (void)params; (void)data;
    uint8_t buf[256];
    YAPB_Packet_t pkt;

    YAPB_initialize(&pkt, buf, sizeof(buf));
    int32_t val = 42;
    YAPB_push_i32(&pkt, &val);
    size_t len;
    YAPB_finalize(&pkt, &len);

    YAPB_Packet_t rpkt;
    YAPB_load(&rpkt, buf, len);
    int8_t wrong = 0;
    munit_assert_int(YAPB_pop_i8(&rpkt, &wrong), ==, YAPB_ERR_TYPE_MISMATCH);
    return MUNIT_OK;
}

/* ======== Mode errors ======== */

static MunitResult test_push_in_read_mode(const MunitParameter params[], void *data) {
    (void)params; (void)data;
    uint8_t buf[256];
    YAPB_Packet_t pkt;

    YAPB_initialize(&pkt, buf, sizeof(buf));
    YAPB_finalize(&pkt, NULL);

    YAPB_Packet_t rpkt;
    YAPB_load(&rpkt, buf, YAPB_HEADER_SIZE);
    int8_t val = 1;
    munit_assert_int(YAPB_push_i8(&rpkt, &val), ==, YAPB_ERR_INVALID_MODE);
    return MUNIT_OK;
}

static MunitResult test_pop_in_write_mode(const MunitParameter params[], void *data) {
    (void)params; (void)data;
    uint8_t buf[256];
    YAPB_Packet_t pkt;

    YAPB_initialize(&pkt, buf, sizeof(buf));
    int8_t out = 0;
    munit_assert_int(YAPB_pop_i8(&pkt, &out), ==, YAPB_ERR_INVALID_MODE);
    return MUNIT_OK;
}

/* ======== Buffer overflow ======== */

static MunitResult test_buffer_overflow(const MunitParameter params[], void *data) {
    (void)params; (void)data;
    uint8_t buf[YAPB_HEADER_SIZE + 2]; /* only room for header + 2 bytes */
    YAPB_Packet_t pkt;

    YAPB_initialize(&pkt, buf, sizeof(buf));
    int32_t val = 42;
    munit_assert_int(YAPB_push_i32(&pkt, &val), ==, YAPB_ERR_BUFFER_TOO_SMALL);
    return MUNIT_OK;
}

/* ======== Result string ======== */

static MunitResult test_result_str(const MunitParameter params[], void *data) {
    (void)params; (void)data;
    munit_assert_string_equal(YAPB_Result_str(YAPB_OK), "OK");
    munit_assert_string_equal(YAPB_Result_str(YAPB_STS_COMPLETE), "Complete");
    munit_assert_string_equal(YAPB_Result_str(YAPB_ERR_NULL_PTR), "Null pointer");
    return MUNIT_OK;
}

/* ======== Load invalid ======== */

static MunitResult test_load_invalid(const MunitParameter params[], void *data) {
    (void)params; (void)data;
    YAPB_Packet_t pkt;

    /* Too small */
    uint8_t tiny[] = {0, 0};
    munit_assert_int(YAPB_load(&pkt, tiny, sizeof(tiny)), ==, YAPB_ERR_BUFFER_TOO_SMALL);

    /* Header claims more data than available */
    uint8_t bad[8] = {0};
    bad[0] = 0; bad[1] = 0; bad[2] = 0; bad[3] = 100; /* pkt_len = 100 */
    munit_assert_int(YAPB_load(&pkt, bad, sizeof(bad)), ==, YAPB_ERR_INVALID_PACKET);
    return MUNIT_OK;
}

/* ======== Forward compatibility (pop past end) ======== */

static MunitResult test_forward_compat(const MunitParameter params[], void *data) {
    (void)params; (void)data;
    uint8_t buf[256];
    YAPB_Packet_t pkt;

    /* Write only one field */
    YAPB_initialize(&pkt, buf, sizeof(buf));
    int8_t val = 7;
    YAPB_push_i8(&pkt, &val);
    size_t len;
    YAPB_finalize(&pkt, &len);

    /* Read back - first pop succeeds, second uses default */
    YAPB_Packet_t rpkt;
    YAPB_load(&rpkt, buf, len);
    int8_t field1 = 0;
    uint16_t field2 = 42; /* default */
    YAPB_pop_i8(&rpkt, &field1);
    YAPB_pop_u16(&rpkt, &field2);
    munit_assert_int8(field1, ==, 7);
    munit_assert_uint16(field2, ==, 42); /* unchanged - forward compat */
    return MUNIT_OK;
}

/* ======== pop_next ======== */

static MunitResult test_pop_next_all_types(const MunitParameter params[], void *data) {
    (void)params; (void)data;
    uint8_t buf[512];
    YAPB_Packet_t pkt;

    YAPB_initialize(&pkt, buf, sizeof(buf));
    int8_t v8 = -1;           YAPB_push_i8(&pkt, &v8);
    int16_t v16 = -2000;      YAPB_push_i16(&pkt, &v16);
    int32_t v32 = 100000;     YAPB_push_i32(&pkt, &v32);
    int64_t v64 = 9876543210LL; YAPB_push_i64(&pkt, &v64);
    float vf = 1.5f;          YAPB_push_float(&pkt, &vf);
    double vd = 2.5;          YAPB_push_double(&pkt, &vd);
    const uint8_t blob[] = {0xCA, 0xFE};
    YAPB_push_blob(&pkt, blob, sizeof(blob));

    uint8_t inner_buf[64];
    YAPB_Packet_t inner;
    YAPB_initialize(&inner, inner_buf, sizeof(inner_buf));
    int8_t ni = 77;
    YAPB_push_i8(&inner, &ni);
    YAPB_finalize(&inner, NULL);
    YAPB_push_nested(&pkt, &inner);

    size_t len;
    YAPB_finalize(&pkt, &len);

    YAPB_Packet_t rpkt;
    YAPB_load(&rpkt, buf, len);
    YAPB_Element_t elem;

    munit_assert_int(YAPB_pop_next(&rpkt, &elem), ==, YAPB_OK);
    munit_assert_int(elem.type, ==, YAPB_INT8);
    munit_assert_int8(elem.val.i8, ==, -1);

    munit_assert_int(YAPB_pop_next(&rpkt, &elem), ==, YAPB_OK);
    munit_assert_int(elem.type, ==, YAPB_INT16);
    munit_assert_int16(elem.val.i16, ==, -2000);

    munit_assert_int(YAPB_pop_next(&rpkt, &elem), ==, YAPB_OK);
    munit_assert_int(elem.type, ==, YAPB_INT32);
    munit_assert_int32(elem.val.i32, ==, 100000);

    munit_assert_int(YAPB_pop_next(&rpkt, &elem), ==, YAPB_OK);
    munit_assert_int(elem.type, ==, YAPB_INT64);
    munit_assert_int64(elem.val.i64, ==, 9876543210LL);

    munit_assert_int(YAPB_pop_next(&rpkt, &elem), ==, YAPB_OK);
    munit_assert_int(elem.type, ==, YAPB_FLOAT);
    munit_assert_float(elem.val.f, ==, 1.5f);

    munit_assert_int(YAPB_pop_next(&rpkt, &elem), ==, YAPB_OK);
    munit_assert_int(elem.type, ==, YAPB_DOUBLE);
    munit_assert_double(elem.val.d, ==, 2.5);

    munit_assert_int(YAPB_pop_next(&rpkt, &elem), ==, YAPB_OK);
    munit_assert_int(elem.type, ==, YAPB_BLOB);
    munit_assert_uint16(elem.val.blob.len, ==, 2);
    munit_assert_memory_equal(2, elem.val.blob.data, blob);

    munit_assert_int(YAPB_pop_next(&rpkt, &elem), ==, YAPB_STS_COMPLETE);
    munit_assert_int(elem.type, ==, YAPB_NESTED_PKT);
    int8_t nested_val = 0;
    YAPB_pop_i8(&elem.val.nested, &nested_val);
    munit_assert_int8(nested_val, ==, 77);

    return MUNIT_OK;
}

static MunitResult test_pop_next_empty(const MunitParameter params[], void *data) {
    (void)params; (void)data;
    uint8_t buf[64];
    YAPB_Packet_t pkt;

    YAPB_initialize(&pkt, buf, sizeof(buf));
    YAPB_finalize(&pkt, NULL);

    YAPB_Packet_t rpkt;
    YAPB_load(&rpkt, buf, YAPB_HEADER_SIZE);
    YAPB_Element_t elem;
    munit_assert_int(YAPB_pop_next(&rpkt, &elem), ==, YAPB_ERR_NO_MORE_ELEMENTS);
    return MUNIT_OK;
}

/* ======== Corpus files ======== */

static MunitResult test_corpus_bins(const MunitParameter params[], void *data) {
    (void)params; (void)data;
    int files_tested = 0;

    for (int i = 0; i < 256; i++) {
        char path[64];
        snprintf(path, sizeof(path), "%d.bin", i);
        FILE *f = fopen(path, "rb");
        if (!f) continue;

        fseek(f, 0, SEEK_END);
        long fsize = ftell(f);
        fseek(f, 0, SEEK_SET);

        if (fsize <= 0 || fsize > 65536) { fclose(f); continue; }

        uint8_t *buf = malloc((size_t)fsize);
        munit_assert_not_null(buf);
        size_t nread = fread(buf, 1, (size_t)fsize, f);
        fclose(f);
        munit_assert_size(nread, ==, (size_t)fsize);

        YAPB_Packet_t pkt;
        YAPB_Result_t r = YAPB_load(&pkt, buf, nread);
        if (r == YAPB_OK) {
            uint16_t count = 0;
            YAPB_get_elem_count(&pkt, &count);

            YAPB_Element_t elem;
            for (uint16_t j = 0; j < count && YAPB_get_error(&pkt) >= 0; j++) {
                YAPB_pop_next(&pkt, &elem);
            }
        }

        free(buf);
        files_tested++;
    }

    munit_logf(MUNIT_LOG_INFO, "tested %d corpus files", files_tested);
    munit_assert_int(files_tested, >, 0);
    return MUNIT_OK;
}

/* ======== Test suite ======== */

static MunitTest tests[] = {
    { "/init/finalize",      test_init_finalize,     NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
    { "/init/null",          test_init_null,          NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
    { "/init/too_small",     test_init_too_small,     NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
    { "/roundtrip/i8",       test_i8_roundtrip,       NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
    { "/roundtrip/i16",      test_i16_roundtrip,      NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
    { "/roundtrip/i32",      test_i32_roundtrip,      NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
    { "/roundtrip/i64",      test_i64_roundtrip,      NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
    { "/roundtrip/u16",      test_u16_roundtrip,      NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
    { "/roundtrip/float",    test_float_roundtrip,    NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
    { "/roundtrip/double",   test_double_roundtrip,   NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
    { "/roundtrip/blob",     test_blob_roundtrip,     NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
    { "/roundtrip/blob_empty", test_blob_empty,       NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
    { "/roundtrip/nested",   test_nested_roundtrip,   NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
    { "/roundtrip/multi",    test_multi_element,      NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
    { "/query/elem_count",   test_elem_count,         NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
    { "/error/sticky",       test_sticky_error,       NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
    { "/error/type_mismatch", test_type_mismatch,     NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
    { "/error/push_read",    test_push_in_read_mode,  NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
    { "/error/pop_write",    test_pop_in_write_mode,  NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
    { "/error/overflow",     test_buffer_overflow,    NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
    { "/error/result_str",   test_result_str,         NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
    { "/load/invalid",       test_load_invalid,       NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
    { "/compat/forward",     test_forward_compat,     NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
    { "/pop_next/all_types", test_pop_next_all_types, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
    { "/pop_next/empty",     test_pop_next_empty,     NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
    { "/corpus/bins",        test_corpus_bins,        NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
    { NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL }
};

static const MunitSuite suite = {
    "/yapb", tests, NULL, 1, MUNIT_SUITE_OPTION_NONE
};

int main(int argc, char *argv[]) {
    return munit_suite_main(&suite, NULL, argc, argv);
}
