#include "yapb.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define NUM_FILES 32
#define MAX_ELEMS 16
#define MAX_BLOB  64
#define PKT_BUF   4096

static void gen_packet(YAPB_Packet_t *pkt, uint8_t *buf, size_t buf_size, int depth) {
    YAPB_initialize(pkt, buf, buf_size);

    int count = 1 + rand() % MAX_ELEMS;
    for (int i = 0; i < count; i++) {
        int type = rand() % 8;
        /* avoid deep nesting */
        if (type == 7 && depth >= 2) type = rand() % 7;

        switch (type) {
            case 0: { int8_t v = (int8_t)rand(); YAPB_push_i8(pkt, &v); break; }
            case 1: { int16_t v = (int16_t)rand(); YAPB_push_i16(pkt, &v); break; }
            case 2: { int32_t v = (int32_t)rand(); YAPB_push_i32(pkt, &v); break; }
            case 3: { int64_t v = ((int64_t)rand() << 32) | rand(); YAPB_push_i64(pkt, &v); break; }
            case 4: { float v = (float)rand() / (float)rand(); YAPB_push_float(pkt, &v); break; }
            case 5: { double v = (double)rand() / (double)rand(); YAPB_push_double(pkt, &v); break; }
            case 6: {
                uint16_t len = rand() % (MAX_BLOB + 1);
                uint8_t blob[MAX_BLOB];
                for (uint16_t j = 0; j < len; j++) blob[j] = (uint8_t)rand();
                YAPB_push_blob(pkt, blob, len);
                break;
            }
            case 7: {
                uint8_t nbuf[PKT_BUF / 2];
                YAPB_Packet_t nested;
                gen_packet(&nested, nbuf, sizeof(nbuf), depth + 1);
                size_t nlen;
                YAPB_finalize(&nested, &nlen);
                YAPB_push_nested(pkt, &nested);
                break;
            }
        }

        if (YAPB_get_error(pkt) < 0) break;
    }
}

int main(void) {
    srand((unsigned)time(NULL));

    for (int i = 0; i < NUM_FILES; i++) {
        uint8_t buf[PKT_BUF];
        YAPB_Packet_t pkt;

        gen_packet(&pkt, buf, sizeof(buf), 0);
        size_t len;
        YAPB_finalize(&pkt, &len);

        char name[32];
        snprintf(name, sizeof(name), "%d.bin", i);
        FILE *f = fopen(name, "wb");
        if (f) {
            fwrite(buf, 1, len, f);
            fclose(f);
        }
    }

    return 0;
}
