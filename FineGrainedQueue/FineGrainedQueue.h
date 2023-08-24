#ifndef FINE_GRAINED_QUEUE_H
#define FINE_GRAINED_QUEUE_H

#include <mutex>

struct Node
{
    int value;
    Node* next;
    std::mutex* node_mutex;

    Node(int val) : value(val), next(nullptr), node_mutex(new std::mutex()) {}
    ~Node() { delete node_mutex; }
};

class FineGrainedQueue
{
private:
    Node* head;
    std::mutex* queue_mutex;

public:
    FineGrainedQueue();
    ~FineGrainedQueue();

    void insertIntoMiddle(int value, int pos);
    Node* getHead();
    void printQueue();
};

#endif // FINE_GRAINED_QUEUE_H
