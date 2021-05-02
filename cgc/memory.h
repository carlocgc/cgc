#pragma once
#include "type_traits.h"

namespace cgc
{
	class ref_counter : private non_copyable
	{
		int m_count;

	public:
		ref_counter() : m_count(1) {}
		ref_counter(ref_counter const&) = delete;
		ref_counter(ref_counter&&) = delete;
		~ref_counter() = default;

		int get_count() const { return m_count; }
		int decrement() { return --m_count; }
		int increment() { return ++m_count; }

		ref_counter& operator=(ref_counter const&) = delete;
		ref_counter& operator=(ref_counter&&) = delete;
	};

	template<typename T>
	class base_ptr
	{
	protected:
		T* m_ptr;
		ref_counter* m_ref_counter;

		base_ptr() : m_ptr(nullptr), m_ref_counter(nullptr) {}
		explicit base_ptr(T* ptr) : m_ptr(ptr), m_ref_counter(new ref_counter{}) {}
		virtual ~base_ptr()
		{
			if (m_ref_counter && m_ref_counter->decrement() <= 0)
			{
				delete m_ptr;
				delete m_ref_counter;
			}

			m_ptr = nullptr;
			m_ref_counter = nullptr;
		}

	public:
		base_ptr(base_ptr const&) = delete;
		base_ptr(base_ptr&&) = delete;

		explicit operator bool() const { return m_ptr != nullptr; }
		bool operator==(base_ptr const& other) { return m_ptr == other.m_ptr; }
		bool operator!=(base_ptr const& other) { return m_ptr != other.m_ptr; }
		base_ptr& operator=(base_ptr const&) = delete;
		base_ptr& operator=(base_ptr&&) = delete;
	};

	template<typename T>
	class unique_ptr : public base_ptr<T>
	{
	public:
		unique_ptr() : base_ptr<T>() {}
		explicit unique_ptr(T* ptr) : base_ptr<T>(ptr) {}
		unique_ptr(unique_ptr const& other) = delete;
		unique_ptr(unique_ptr&& other) noexcept : base_ptr<T>()
		{
			this->m_ptr = other.m_ptr;
			other.m_ptr = nullptr;
			this->m_ref_counter = other.m_ref_counter;
			other.m_ref_counter = nullptr;
		}

		~unique_ptr() override = default;

		unique_ptr& operator=(unique_ptr const&) = delete;
		unique_ptr& operator=(unique_ptr&& other) noexcept
		{
			this->m_ptr = other.m_ptr;
			other.m_ptr = nullptr;
			this->m_ref_counter = other.m_ref_counter;
			other.m_ref_counter = nullptr;
			return *this;
		}
	};

	template<typename T>
	class weak_ptr<T>;

	template<typename T>
	class shared_ptr : public base_ptr<T>
	{
	public:
		shared_ptr() : base_ptr<T>() {}
		explicit shared_ptr(T* ptr) : base_ptr<T>(ptr) {}
		shared_ptr(shared_ptr<T> const& other) : base_ptr<T>()
		{
			if (this == &other) return *this;
			this->m_ptr = other.m_ptr;
			this->m_ref_counter = other.m_ref_counter;
			this->m_ref_counter->increment();
		}
		shared_ptr(shared_ptr<T>&& other) noexcept : base_ptr<T>()
		{
			this->m_ptr = other.m_ptr;
			other.m_ptr = nullptr;
			this->m_ref_counter = other.m_ref_counter;
			other.m_ref_counter = nullptr;
		}

		~shared_ptr() override = default;

		shared_ptr<T>& operator=(shared_ptr<T> const& other)
		{
			if (this == &other) return *this;
			this->m_ptr = other.m_ptr;
			this->m_ref_counter = other.m_ref_counter;
			this->m_ref_counter->increment();
			return *this;
		}
		shared_ptr<T>& operator=(shared_ptr<T>&& other) noexcept
		{
			this->m_ptr = other.m_ptr;
			other.m_ptr = nullptr;
			this->m_ref_counter = other.m_ref_counter;
			other.m_ref_counter = nullptr;
			return *this;
		}

		weak_ptr<T>& get_weak();
	};

	template<typename T>
	class weak_ptr
	{
		bool m_expired;
		shared_ptr<T>* m_parent;

	public:
		weak_ptr() : m_expired(true), m_parent(nullptr) {}
		weak_ptr(weak_ptr<T> const& other) : m_expired(other.m_expired), m_parent(other.m_parent) {}
		weak_ptr(weak_ptr<T>&& other) noexcept : m_expired(other.m_expired), m_parent(other.m_parent) { other.m_expired = true; other.m_parent = nullptr; }
		explicit weak_ptr(shared_ptr<T> const& ptr) : m_expired(false), m_parent(&ptr) {}
		~weak_ptr() = default;

		weak_ptr<T>& operator=(weak_ptr<T> const& other)
		{
			if (this == &other) return *this;
			this.m_expired = other.m_expired;
			this.m_parent = other.m_parent;
			return *this;
		}
		weak_ptr<T>& operator=(weak_ptr<T>&& other) noexcept
		{
			m_expired = other.m_expired;
			other.m_expired = true;
			m_parent = other.m_parent;
			other.m_parent = nullptr;
			return *this;
		}

		void set_expired()
		{
			m_expired = true;
			m_parent = nullptr;
		}

		shared_ptr<T>& is_valid()
		{
			if (m_expired || m_parent == nullptr)
			{
				return shared_ptr<T>();
			}

			return shared_ptr<T>(*m_parent);
		}
	};

	template<class T>
	inline weak_ptr<T>& shared_ptr<T>::get_weak()
	{
		return weak_ptr<T>(*this);
	}
}
