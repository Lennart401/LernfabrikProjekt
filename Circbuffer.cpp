#include "Circbuffer.h"
#include <stdint.h>

namespace circbuffer {

namespace {
    const uint16_t buffer_size = 10 * 200;
    uint16_t buffer_start = 0; // read from buffer start
    uint16_t buffer_end = 0; // write to buffer end

    Row buffer[buffer_size];
} // namespace

void WriteRow(Row row) {
    buffer[buffer_end] = row;
    buffer_end++;

    if (buffer_end >= buffer_size) {
        buffer_end -= buffer_size;
    }
}

}