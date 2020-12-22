#include "Circbuffer.h"
#include <stdint.h>

namespace circbuffer {

namespace {
    const uint16_t buffer_num_rows = 1 * 200;
    uint16_t buffer_start = 0; // read from buffer start
    uint16_t buffer_end = 0; // write to buffer end
    bool full = false; // to determine if the buffer is full

    Row buffer[buffer_num_rows];
} // namespace

Row* ReadRow() {
    if (IsBufferEmpty()) return nullptr;

    Row* row = &buffer[buffer_start];
    buffer_start = (buffer_start + 1) % buffer_num_rows;
    full = false;

    return row;
}

void WriteRow(Row* row) {
    buffer[buffer_end] = *row;
    buffer_end = (buffer_end + 1) % buffer_num_rows;
    // advance start also if buffer is full
    if (full) buffer_start = (buffer_start + 1) % buffer_num_rows;
    // find out if buffer is full now
    else full = buffer_end == buffer_start;
}

bool IsBufferEmpty() {
    return (buffer_end == buffer_start) && !full;
}

bool IsBufferFull() {
    return full;
}

} // namespace circbuffer