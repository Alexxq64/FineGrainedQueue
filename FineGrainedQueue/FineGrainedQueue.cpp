#include "FineGrainedQueue.h"
#include <iostream>
#include <iomanip>

FineGrainedQueue::FineGrainedQueue()
{
    head = new Node(0); // Dummy node
    queue_mutex = new std::mutex();
}

FineGrainedQueue::~FineGrainedQueue()
{
    Node* current = head;
    while (current != nullptr)
    {
        Node* next = current->next;
        delete current;
        current = next;
    }

    delete queue_mutex;
}

void FineGrainedQueue::insertIntoMiddle(int value, int pos)
{
    // Create the new node
    Node* newNode = new Node(value);

    // Lock the queue
    // if you uncomment next line you'll get very interesting effect
    // each insertion will block all action from other threads 
    // and the execution speed will drop to the single-threaded version 
    //std::lock_guard<std::mutex> lock(*queue_mutex);

    // Find the appropriate position in the list
    Node* current = head;
    int currentPos = 0;

    while (currentPos < pos - 1 && current->next != nullptr)
    {
        current = current->next;
        ++currentPos;
    }

    // Lock the current and next nodes for insertion
    std::lock_guard<std::mutex> currentLock(*current->node_mutex);
    if (current->next)
    {
        std::lock_guard<std::mutex> nextLock(*current->next->node_mutex);
    }

    // Insert the new node
    newNode->next = current->next;
    current->next = newNode;
}


Node* FineGrainedQueue::getHead()
{
    return head;
}

void FineGrainedQueue::printQueue()
{
    // Lock the queue
    std::lock_guard<std::mutex> lock(*queue_mutex);

    Node* current = head->next;
    int counter = 0;
    int positionsOnOneLine = 20;
    while (current != nullptr)
    {
        std::cout << std::setw(4)<< current->value;
        if (++counter  % positionsOnOneLine == 0) std::cout << std::endl;
        current = current->next;
    }
    std::cout << std::endl;
}




