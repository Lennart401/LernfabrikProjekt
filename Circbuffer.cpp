#include "Circbuffer.h"
#include <stdint.h>

namespace circbuffer {

namespace {
    const uint16_t buffer_size = 1 * 200;
    uint16_t buffer_start = 0; // read from buffer start
    uint16_t buffer_end = 0; // write to buffer end

    Row buffer[buffer_size];
} // namespace

Row* ReadRow() {
    Row* row = &buffer[buffer_start];
    buffer_start = (buffer_start + 1) % buffer_size;

    return row;
}

void WriteRow(Row* row) {
    buffer[buffer_end] = *row;
    buffer_end = (buffer_end + 1) % buffer_size;

    // keep pushing buffer_start
    if (buffer_end == buffer_start) {
        buffer_start = (buffer_start + 1) % buffer_size;
    }
}

bool IsBufferFull() {
    return buffer_end == buffer_start;
}

} // namespace circbuffer