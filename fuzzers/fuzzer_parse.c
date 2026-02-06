#include "yapb.h"
#include <stdint.h>
#include <stddef.h>

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
    YAPB_Packet_t pkt;

    if (YAPB_load(&pkt, data, size) != YAPB_OK) {
        return 0;
    }

    uint16_t count = 0;
    YAPB_get_elem_count(&pkt, &count);

    YAPB_Element_t elem;
    for (uint16_t i = 0; i < count && YAPB_get_error(&pkt) >= 0; i++) {
        YAPB_pop_next(&pkt, &elem);
    }

    return 0;
}
