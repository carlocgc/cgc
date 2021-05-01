#pragma once
#include "type_traits.h"

namespace cgc
{
	class control_block : private non_copyable
	{
		int m_Count;

	public:
		control_block() : m_Count(1) {}
		control_block(control_block const&) = delete;
		control_block(control_block&&) = delete;
		~control_block() = default;

		int get_count() const { return m_Count; }
		int decrement() { return --m_Count; }
		int increment() { return ++m_Count; }

		operator bool() const { return m_Count > 0; }
		control_block& operator=(control_block const&) = delete;
		control_block& operator=(control_block&&) = delete;
	};

	template<typename T>
	class base_ptr
	{
	protected:
		T* m_Ptr;
		control_block* m_CBlock;

	public:
		base_ptr() : m_Ptr(nullptr), m_CBlock(nullptr) {}
		base_ptr(T* ptr) : m_Ptr(ptr), m_CBlock(new control_block()) {}
		base_ptr(base_ptr const&) = delete;
		base_ptr(base_ptr&&) = delete;

		virtual ~base_ptr()
		{
			if (m_CBlock && m_CBlock->decrement() <= 0)
			{
				delete m_CBlock;
				delete m_Ptr;
			}

			m_Ptr = nullptr;
			m_CBlock = nullptr;
		}

		operator bool() { return m_Ptr != nullptr; }
		bool operator==(base_ptr const& other) { return m_Ptr == other.m_Ptr; }
		bool operator!=(base_ptr const& other) { return m_Ptr != other.m_Ptr; }
		bool operator>=(base_ptr const& other) { return m_Ptr >= other.m_Ptr; }
		bool operator<=(base_ptr const& other) { return m_Ptr <= other.m_Ptr; }
		base_ptr& operator=(base_ptr const&) = delete;
		base_ptr& operator=(base_ptr&&) = delete;
	};

	template<typename T>
	class shared_ptr : public base_ptr<T>
	{
	public:
		shared_ptr() : base_ptr<T>() {}
		shared_ptr(T* ptr) : base_ptr<T>(ptr) {}
		shared_ptr(shared_ptr const& other) : base_ptr<T>()
		{
			this->m_Ptr = other.m_Ptr;
			this->m_CBlock = other.m_CBlock;
			this->m_CBlock->increment();
		}
		shared_ptr(shared_ptr&& other) noexcept : base_ptr<T>()
		{
			this->m_Ptr = other.m_Ptr;
			other.m_Ptr = nullptr;
			this->m_CBlock = other.m_CBlock;
			other.m_CBlock = nullptr;
		}

		virtual ~shared_ptr() = default;

		shared_ptr& operator=(shared_ptr const& other)
		{
			this->m_Ptr = other.m_Ptr;
			this->m_CBlock = other.m_CBlock;
			this->m_CBlock->increment();
			return *this;
		}
		shared_ptr& operator=(shared_ptr&& other)
		{
			this->m_Ptr = other.m_Ptr;
			other.m_Ptr = nullptr;
			this->m_CBlock = other.m_CBlock;
			other.m_CBlock = nullptr;
			return *this;
		}
	};
}
