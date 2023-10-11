#pragma once

#include <memory>

template<typename T, unsigned long CAPACITY>
class CircularQueue
{
public:
	explicit CircularQueue()
		: head{ 0u }, tail{ 0u }, numElem{ 0u }
	{
		buf = new T[CAPACITY];
	}

	~CircularQueue() {
		delete[] buf;
	}

	T const& front() const {
		return buf[head];
	}

	T const& back() const {
		return &buf[tail - 1];
	}

	// CAUTION : all of the type C should have the same size!!!
	template<typename C, typename... Args>
	void push(Args&&... args) {
		if (full())
			pop();
		std::construct_at(&buf[tail], std::forward<Args>(args)...);
		tail = (tail + 1) % CAPACITY;
		++numElem;
	}

	template<typename C>
	void push(C&& c) {
		if (full())
			pop();
		std::construct_at(&buf[tail], std::forward<C>(c));
		tail = (tail + 1) % CAPACITY;
		++numElem;
	}

	void pop() {
		if (empty())
			return;
		head = (head + 1) % CAPACITY;
		--numElem;
	}

	void flush() {
		numElem = 0;
		head = tail = 0;
	}

	[[nodiscard]] bool empty() const { return numElem == 0; }
	[[nodiscard]] bool full() const { return numElem == CAPACITY; }
private:
	unsigned short head;
	unsigned short tail;
	unsigned short numElem;
	T* buf;
};

