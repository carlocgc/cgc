#pragma once
#include "type_traits.h"

namespace cgc
{
	template<typename T>
	class base_ptr
	{
	protected:
		class ref_counter : private non_copyable
		{
			int m_Count;

		public:
			ref_counter() : m_Count(1) {}
			ref_counter(ref_counter const&) = delete;
			ref_counter(ref_counter&&) = delete;
			~ref_counter() = default;

			int get_count() const { return m_Count; }
			int decrement() { return --m_Count; }
			int increment() { return ++m_Count; }

			explicit operator bool() const { return m_Count > 0; }
			ref_counter& operator=(ref_counter const&) = delete;
			ref_counter& operator=(ref_counter&&) = delete;
		};

		T* m_Ptr;
		ref_counter* m_RefCounter;

		base_ptr() : m_Ptr(nullptr), m_RefCounter(nullptr) {}
		explicit base_ptr(T* ptr) : m_Ptr(ptr), m_RefCounter(new ref_counter()) {}
		virtual ~base_ptr()
		{
			if (m_RefCounter && m_RefCounter->decrement() <= 0)
			{
				delete m_RefCounter;
				delete m_Ptr;
			}

			m_Ptr = nullptr;
			m_RefCounter = nullptr;
		}

	public:
		base_ptr(base_ptr const&) = delete;
		base_ptr(base_ptr&&) = delete;

		explicit operator bool() const { return m_Ptr != nullptr; }
		bool operator==(base_ptr const& other) { return m_Ptr == other.m_Ptr; }
		bool operator!=(base_ptr const& other) { return m_Ptr != other.m_Ptr; }
		base_ptr& operator=(base_ptr const&) = delete;
		base_ptr& operator=(base_ptr&&) = delete;
	};

	template<typename T>
	class shared_ptr : public base_ptr<T>
	{
	public:
		shared_ptr() : base_ptr<T>() {}
		explicit shared_ptr(T* ptr) : base_ptr<T>(ptr) {}
		shared_ptr(shared_ptr const& other) : base_ptr<T>()
		{
			this->m_Ptr = other.m_Ptr;
			this->m_RefCounter = other.m_RefCounter;
			this->m_RefCounter->increment();
		}
		shared_ptr(shared_ptr&& other) noexcept : base_ptr<T>()
		{
			this->m_Ptr = other.m_Ptr;
			other.m_Ptr = nullptr;
			this->m_RefCounter = other.m_RefCounter;
			other.m_RefCounter = nullptr;
		}

		virtual ~shared_ptr() override = default;

		shared_ptr& operator=(shared_ptr const& other)
		{
			this->m_Ptr = other.m_Ptr;
			this->m_RefCounter = other.m_RefCounter;
			this->m_RefCounter->increment();
			return *this;
		}
		shared_ptr& operator=(shared_ptr&& other) noexcept
		{
			this->m_Ptr = other.m_Ptr;
			other.m_Ptr = nullptr;
			this->m_RefCounter = other.m_RefCounter;
			other.m_RefCounter = nullptr;
			return *this;
		}
	};
}
