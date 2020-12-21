#include <Arduino.h>
#include "Row.h"

#ifndef BUFFER_H_
#define BUFFER_H_

namespace circbuffer {

bool IsBufferEmpty();
bool IsBufferFull();

Row* ReadRow();
void WriteRow(Row* row);

} // namespace circbuffer

#endif