#pragma once

// Define EventType in the global namespace. Add more event types as needed by the engine.
enum EventType
{
    EventType_None = 0,
    // Example placeholder values - add real event types in your project as needed
    EventType_Custom = 1
};

// Message struct in the global namespace. It contains at least an EventType so
// existing code that uses "msg.type" continues to work.
struct Message
{
    EventType type;
    void* sender = nullptr; // optional sender pointer

    Message(EventType t = EventType_None, void* s = nullptr)
        : type(t), sender(s) {}
};
