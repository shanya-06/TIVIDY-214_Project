#include "Output.h"
#include <iostream>

std::ostream* Output::sink = &std::cout;

std::ostream& Output::stream() {
    return *sink;
}

void Output::redirect(std::ostream* newSink) {
    sink = newSink ? newSink : &std::cout;
}
