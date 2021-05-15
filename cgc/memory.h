#pragma once
#include <set>
#include "type_traits.h"

namespace cgc
{	
	template<typename T>
	class ref_manager : public non_copyable, public non_movable
	{
		T* m_ptr;
		int m_strong_count;
		int m_weak_count;

	public:
		explicit ref_manager(T* ptr) : m_ptr(ptr), m_strong_count(1), m_weak_count(0) {}
		~ref_manager()
		{
			destroy();
		}

		void add_strong()
		{
			++m_strong_count;
		}
		void remove_strong()
		{
			--m_strong_count;
			if (m_strong_count <= 0)
			{
				destroy();
			}
		}
		void add_weak()
		{
			++m_weak_count;
		}
		void remove_weak()
		{
			--m_weak_count;
		}
		bool is_valid()
		{
			return m_ptr != nullptr;
		}
		T* get_ptr()
		{
			return m_ptr;
		}
		int get_strong_count() const
		{
			return m_strong_count;
		}
		int get_weak_count() const
		{
			return m_weak_count;
		}
		void destroy()
		{
			if (m_ptr != nullptr)
			{
				delete m_ptr; // TODO use custom deleter
				m_ptr = nullptr;
			}
		}
	};

	template<typename T>
	class base_ptr : non_copyable, non_movable
	{
	protected:
		ref_manager<T>* m_ref_manager;

		base_ptr() : m_ref_manager(nullptr) {}
		explicit base_ptr(T* ptr) : m_ref_manager(new ref_manager<T>{ ptr }) {}
		explicit base_ptr(ref_manager<T>* ref_manager) : m_ref_manager(ref_manager) {}
		virtual ~base_ptr()
		{
			if (m_ref_manager && this->m_ref_manager->get_strong_count() <= 0 && this->m_ref_manager->get_weak_count() <= 0)
			{
				delete this->m_ref_manager;
			}
		}

	public:
		explicit operator bool() const { return m_ref_manager && m_ref_manager->is_valid(); }
		bool operator==(base_ptr const& other) { return m_ref_manager == other.m_ref_manager; }
		bool operator!=(base_ptr const& other) { return m_ref_manager != other.m_ref_manager; }
	};

	template<typename T>
	class unique_ptr final : public base_ptr<T>
	{
	public:
		unique_ptr() : base_ptr<T>() {}
		explicit unique_ptr(T* ptr) : base_ptr<T>(ptr) {}
		unique_ptr(unique_ptr&& other) noexcept : base_ptr<T>()
		{
			this->m_ref_manager = other.m_ref_manager;
			other.m_ref_manager = nullptr;
		}
		~unique_ptr() override
		{
			if (this->m_ref_manager)
			{
				this->m_ref_manager->remove_strong();
			}
		}

		unique_ptr& operator=(unique_ptr&& other) noexcept
		{
			this->m_ref_manager = other.m_ref_manager;
			other.m_ref_manager = nullptr;
			return *this;
		}
	};

	template<typename>
	class weak_ptr;

	template<typename T>
	class shared_ptr final : public base_ptr<T>
	{
		friend class weak_ptr<T>;

	public:
		shared_ptr() : base_ptr<T>() {}
		explicit shared_ptr(T* ptr) : base_ptr<T>(ptr) {}
		shared_ptr(shared_ptr<T> const& other) : base_ptr<T>()
		{
			if (this == &other) return;
			this->m_ref_manager = other.m_ref_manager;
			if (this->m_ref_manager)
			{
				this->m_ref_manager->add_strong();
			}
		}
		shared_ptr(shared_ptr<T>&& other) noexcept : base_ptr<T>()
		{
			this->m_ref_manager = other.m_ref_manager;
			other.m_ref_manager = nullptr;
		}
		shared_ptr(ref_manager<T>* ref_manager) : base_ptr<T>(ref_manager)
		{
			if (this->m_ref_manager)
			{
				this->m_ref_manager->add_strong();
			}
		}

		~shared_ptr() override
		{
			if (this->m_ref_manager)
			{
				this->m_ref_manager->remove_strong();
			}
		}

		shared_ptr<T>& operator=(shared_ptr<T> const& other)
		{
			if (this == &other) return *this;
			this->m_ref_manager = other.m_ref_manager;
			if (this->m_ref_manager)
			{
				this->m_ref_manager->add_strong();
			}
			return *this;
		}
		shared_ptr<T>& operator=(shared_ptr<T>&& other) noexcept
		{
			this->m_ref_manager = other.m_ref_manager;
			other.m_ref_manager = nullptr;
			return *this;
		}

		weak_ptr<T> get_weak()
		{
			return weak_ptr<T>(*this);
		}
	};

	template<typename T>
	class weak_ptr final : public base_ptr<T>
	{
	public:
		weak_ptr() : base_ptr<T>() {}
		explicit weak_ptr(shared_ptr<T> const& sptr) : base_ptr<T>(sptr.m_ref_manager)
		{
			if (this->m_ref_manager)
			{
				this->m_ref_manager->add_weak();
			}
		}	
		weak_ptr(weak_ptr<T> const& other) : base_ptr<T>(other.m_ref_manager)
		{
			if (this == &other || !this->m_ref_manager)
			{
				return;
			}
			this->m_ref_manager->add_weak();
		}
		weak_ptr(weak_ptr<T>&& other) noexcept : base_ptr<T>(other.m_ref_manager)
		{
			other.m_ref_manager = nullptr;
			if (this->m_ref_manager)
			{
				this->m_ref_manager->add_weak();
			}
		}
		~weak_ptr() override
		{
			if (this->m_ref_manager)
			{
				this->m_ref_manager->remove_weak();
			}
		}

		weak_ptr<T>& operator=(weak_ptr<T> const& other)
		{
			if (this == &other) return *this;
			this->m_ref_manager = other.m_ref_manager;
			if (this->m_ref_manager)
			{
				this->m_ref_manager->add_weak();
			}
			return *this;
		}
		weak_ptr<T>& operator=(weak_ptr<T>&& other) noexcept
		{
			this->m_ref_manager = other.m_ref_manager;
			other.m_ref_manager = nullptr;
			if (this->m_ref_manager)
			{
				this->m_ref_manager->add_weak();
			}
			return *this;
		}

		shared_ptr<T> try_lock()
		{
			if (!this->m_ref_manager || !this->m_ref_manager->is_valid())
			{
				return shared_ptr<T>{};
			}

			return shared_ptr<T>{this->m_ref_manager};
		}
	};
}
