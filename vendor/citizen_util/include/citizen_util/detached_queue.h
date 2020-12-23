#pragma once
#include <atomic>
#include <mutex>
#include <intrin.h>

#include "type_helpers.h"

namespace fx
{
	template<typename T>
	struct intrusive_deque_key
	{
		intrusive_deque_key* next = nullptr;
		intrusive_deque_key* prev = nullptr;

		T* get(member_reference_t<T, intrusive_deque_key> ref)
		{
			return ptr_at<T>(this, -make_offset(ref));
		}

		const T* get(member_reference_t<T, intrusive_deque_key> ref) const
		{
			return make_mutable(this)->get(std::move(ref));
		}
	};

	template<typename T>
	struct intrusive_deque
	{
		intrusive_deque_key<T>* head = nullptr;
		intrusive_deque_key<T>* tail = nullptr;

		void push_back(intrusive_deque_key<T>* ptr)
		{
			ptr->prev = head;
			ptr->next = nullptr;

			if (head != nullptr)
				head->next = ptr;

			if (tail == nullptr)
				tail = ptr;

			head = ptr;
		}

		void push_front(intrusive_deque_key<T>* ptr)
		{
			ptr->next = tail;
			ptr->prev = nullptr;

			if (tail != nullptr)
				tail->prev = ptr;

			if (head == nullptr)
				head = ptr;

			tail = ptr;
		}

		void erase(intrusive_deque_key<T>* ptr)
		{
			if (head == ptr)
				head = ptr->next;
			if (tail == ptr)
				tail = ptr->prev;

			if (ptr->next != nullptr)
				ptr->next->prev = ptr->prev;
			if (ptr->prev != nullptr)
				ptr->prev->next = ptr->next;
		}
	};

	template<typename T>
	struct intrusive_queue_key
	{
		intrusive_queue_key* next = nullptr;

		T* get(member_reference_t<T, intrusive_queue_key> ref)
		{
			return ptr_at<T>(this, -make_offset(ref));
		}

		const T* get(member_reference_t<T, intrusive_queue_key> ref) const
		{
			return make_mutable(this)->get(std::move(ref));
		}
	};

	template<typename T>
	struct intrusive_mpsc_queue
	{
		using key = intrusive_queue_key<T>;

		void enqueue(key* node)
		{
			node->next = nullptr;
			auto* prev = (key*)_InterlockedExchangePointer((void* volatile*)&this->head, node);
			prev->next = node;
		}

		void enqueue_bulk(key* first, key* last)
		{
			last->next = nullptr;
			auto* prev = (key*)_InterlockedExchangePointer((void* volatile*)&this->head, last);
			prev->next = first;
		}

		key* dequeue()
		{
			auto* ntail = this->tail;
			auto* next = ntail->next;

			if (ntail == &this->stub)
			{
				if (next == nullptr)
					return nullptr;

				this->tail = next;
				ntail = next;
				next = next->next;
			}

			if (next)
			{
				this->tail = next;
				return ntail;
			}

			auto* nhead = this->head;
			if (ntail != nhead)
				return nullptr;

			enqueue(&this->stub);
			next = ntail->next;

			if (next)
			{
				this->tail = next;
				return ntail;
			}

			return nullptr;
		}

	private:
		key            stub = {};
		key* volatile  head = &stub;
		key* tail = &stub;
	};
}