#include <iostream>
#include <iomanip>
#include <thread>
#include "FineGrainedQueue.h"
#include <vector>
#include <chrono>
#include <ctime>
#include <string>
#include <sstream>

#define DOT_LENGTH 30

int queueSize = 23;
int threadsNumber = 11;
int nodesToInsert = 20000;
std::string prompt = "";
int basis = 1;

void fillQueue(FineGrainedQueue& queue, int start, int end)
{
	for (int i = start; i <= end; ++i)
	{
		queue.insertIntoMiddle(i, i);
	}
}

int getInt(const std::string& prompt, int bottom, int top, int defaultOption)
{
	int inputNumber;
	std::cout << prompt << "(from " << bottom << " to " << top << ", any other input gives the default value): ";
	std::cin >> inputNumber;

	if (std::cin.fail() || inputNumber > top || inputNumber < bottom)
	{
		inputNumber = defaultOption;
		std::cout << "Wrong input. Default value will be used: " << defaultOption << std::endl;
		std::cin.clear();
		std::cin.ignore();
	}
	return inputNumber;
}

void presentDuration(std::string name, double basis, double duration) {
	std::cout << std::setw(40) << name << "  " << std::string(DOT_LENGTH * duration / basis, '.') << std::endl;
}

void multuThreadInsertion(int threadsNumber)
{
	{
		FineGrainedQueue queueMultiThreaded;

		std::vector<std::thread> insertionThreads;
		auto startTime = std::chrono::high_resolution_clock::now();
		for (int i = 0; i < threadsNumber; ++i)
		{
			std::thread thread(fillQueue, std::ref(queueMultiThreaded), i * nodesToInsert / threadsNumber + 1, (i + 1) * nodesToInsert / threadsNumber);
			insertionThreads.emplace_back(std::move(thread)); // Move the thread to the vector
		}

		for (auto& thread : insertionThreads)
		{
			thread.join();
		}

		std::ostringstream prompt;
		auto endTime = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
		prompt << std::to_string(threadsNumber) << "  " << std::setw(4) << std::to_string(duration) << " ms";
		presentDuration(prompt.str(), basis, duration);
	}
}

// Check if the queue filled correctly
bool isEverythingInPlace(FineGrainedQueue& queue, int start, int end, int threadNumber)
{
	bool allInPlace = true;

	if (threadNumber > 1) {
		std::vector<std::thread> threads(threadNumber);

		for (int i = 0; i < threadNumber; ++i) {
			int threadStart = start + i * ((end - start + 1) / threadNumber);
			int threadEnd = (i == threadNumber - 1) ? end : threadStart + ((end - start + 1) / threadNumber) - 1;

			threads[i] = std::thread([&queue, threadStart, threadEnd, &allInPlace]() {
				bool localResult = isEverythingInPlace(queue, threadStart, threadEnd, 1);
				if (!localResult) {
					allInPlace = false;
				}
				});
		}

		for (int i = 0; i < threadNumber; ++i) {
			threads[i].join();
		}
	}
	else {
		bool inPlace;
		for (int i = start; i <= end; ++i) {
			inPlace = false;
			Node* current = queue.getHead();

			while (current->next != nullptr) {
				current = current->next;

				if (current->value == i) {
					//std::cout << i << " ";
					inPlace = true;
					break;
				}
			}
			if (!inPlace) {
				std::cout << "Error at " << i << std::endl;
				allInPlace = false;
			}
		}
	}
	return allInPlace;
}

int main()
{
	std::cout << "Comparison of queue operation in one thread and in several threads with fine-grained blocking\n\n";
	std::cout << "First stage. Visual comparison of the difference between single-thread and multi-thread executing.\n\n";
	queueSize = getInt("Input queue size ", 10, 1000, queueSize);
	threadsNumber = getInt("Input threads number ", 2, 20, threadsNumber);
	{
		FineGrainedQueue queue;
		fillQueue(queue, 1, queueSize);

		// Print the queue of single-threaded mode
		std::cout << "\nSingle-threaded mode" << std::endl;
		queue.printQueue();
	} // queue will be destructed here and its memory will be released

	// Create several threads for inserting values in multi-threaded mode
	std::vector<std::thread> threads;
	FineGrainedQueue queueMultiThreaded;
	for (int i = threadsNumber - 1; i >= 0; --i)
	{
		threads.emplace_back(fillQueue, std::ref(queueMultiThreaded), i * queueSize / threadsNumber + 1, (i == threadsNumber - 1) ? queueSize : (i + 1) * queueSize / threadsNumber);
	}

	// Join the threads
	for (auto& thread : threads)
	{
		thread.join();
	}

	// Print the queue of multi-threaded mode
	std::cout << "\nMulti-threaded mode (wrong order is due to insertion conditions)" << std::endl;
	queueMultiThreaded.printQueue();
	std::cout << std::endl;
	if (!isEverythingInPlace(queueMultiThreaded, 1, queueSize, 11))
		std::cout << "!!!  Insertion error   !!!\n";
	else
		std::cout << std::endl;
	// Comparison of insertion time
	{
		FineGrainedQueue queueSingleThreaded;

		std::cout << "Second stage. Duration comparison between single-thread and multi-thread executing (large number of nodes).\n\n";
		nodesToInsert = getInt("Input nodes number to insert ", 10000, 100000, nodesToInsert);
		auto startTime = std::chrono::high_resolution_clock::now();
		fillQueue(queueSingleThreaded, 1, nodesToInsert);
		auto endTime = std::chrono::high_resolution_clock::now();

		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
		std::cout << "\nTime taken for inserting " << nodesToInsert << " nodes \n";
		prompt = "in single-threaded mode:       " + std::to_string(duration) + " ms";
		basis = duration;
		presentDuration(prompt, basis, duration);
	}

	std::cout << "   in multi-threaded mode (threads):\n";
	multuThreadInsertion(2);
	multuThreadInsertion(4);
	multuThreadInsertion(8);
	multuThreadInsertion(16);
	multuThreadInsertion(32);
	multuThreadInsertion(64);
	multuThreadInsertion(128);
	multuThreadInsertion(256);

	return 0;
}
