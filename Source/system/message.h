#pragma once

#include "system_consts.h"

// Message struct in the global namespace. It contains an EventType so
// existing code that uses "msg.type" continues to work.
struct Message
{
    EventType type;
    void* sender = nullptr; // optional sender pointer

    Message(EventType t = EventType::EventType_Default, void* s = nullptr)
        : type(t), sender(s) {}
};
