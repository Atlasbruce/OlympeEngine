#pragma once
#include <string>

namespace SystemLogSink
{
    // Called by system logger streambuf to append text to the UI log window
    void AppendToLogWindow(const std::string& text);
}
