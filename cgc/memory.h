#pragma once
#include <set>
#include "type_traits.h"

namespace Cgc
{
	template<typename T>
	class SharedPtr<T>;

	template<typename T>
	class IWeakReference
	{
	public:
		virtual ~IWeakReference() = 0;
		virtual void SetExpired() = 0;
		virtual SharedPtr<T>& IsValid() = 0;
	};

	template<typename T>
	class RefManager : private NonCopyable
	{
		int m_StrongCount;
		std::set<IWeakReference<T>*> m_WeakReferences;

		void Expire()
		{
			// TODO not thread safe
			for (auto weak_reference : m_WeakReferences)
			{
				weak_reference->SetExpired();
			}
		}

	public:
		RefManager() : m_StrongCount(1) {}
		RefManager(RefManager const&) = delete;
		RefManager(RefManager&&) = delete;
		~RefManager()
		{
			Expire();
		}

		void AddWeak(IWeakReference<T>* ptr)
		{
			// TODO not thread safe
			m_WeakReferences.insert(ptr);
		}
		void RemoveWeak(IWeakReference<T>* ptr)
		{
			// TODO not thread safe
			m_WeakReferences.erase(ptr);
		}

		int GetCount() const { return m_StrongCount; }
		int Increment() { return ++m_StrongCount; }
		int Decrement()
		{
			if (--m_StrongCount <= 0)
			{
				Expire();
			}
			return m_StrongCount;
		}

		RefManager& operator=(RefManager const&) = delete;
		RefManager& operator=(RefManager&&) = delete;
	};

	template<typename T>
	class BasePtr
	{
	protected:
		T* m_Ptr;
		RefManager<T>* m_RefCounter;

		BasePtr() : m_Ptr(nullptr), m_RefCounter(nullptr) {}
		explicit BasePtr(T* ptr) : m_Ptr(ptr), m_RefCounter(new RefManager<T>{}) {}
		BasePtr(T* ptr, RefManager<T> ref_manager) : m_Ptr(ptr), m_RefCounter(ref_manager) {}
		virtual ~BasePtr()
		{
			if (m_RefCounter && m_RefCounter->Decrement() <= 0)
			{
				delete m_Ptr;
				delete m_RefCounter;
			}

			m_Ptr = nullptr;
			m_RefCounter = nullptr;
		}

	public:
		BasePtr(BasePtr const&) = delete;
		BasePtr(BasePtr&&) = delete;

		explicit operator bool() const { return m_Ptr != nullptr; }
		bool operator==(BasePtr const& other) { return m_Ptr == other.m_Ptr; }
		bool operator!=(BasePtr const& other) { return m_Ptr != other.m_Ptr; }
		BasePtr& operator=(BasePtr const&) = delete;
		BasePtr& operator=(BasePtr&&) = delete;
	};

	template<typename T>
	class UniquePtr final : public BasePtr<T>
	{
	public:
		UniquePtr() : BasePtr<T>() {}
		explicit UniquePtr(T* ptr) : BasePtr<T>(ptr) {}
		UniquePtr(UniquePtr const& other) = delete;
		UniquePtr(UniquePtr&& other) noexcept : BasePtr<T>()
		{
			this->m_Ptr = other.m_Ptr;
			other.m_Ptr = nullptr;
			this->m_RefCounter = other.m_RefCounter;
			other.m_RefCounter = nullptr;
		}

		~UniquePtr() override = default;

		UniquePtr& operator=(UniquePtr const&) = delete;
		UniquePtr& operator=(UniquePtr&& other) noexcept
		{
			this->m_Ptr = other.m_Ptr;
			other.m_Ptr = nullptr;
			this->m_RefCounter = other.m_RefCounter;
			other.m_RefCounter = nullptr;
			return *this;
		}
	};

	template<typename T>
	class WeakPtr<T>;

	template<typename T>
	class SharedPtr final : public BasePtr<T>
	{
	public:
		SharedPtr() : BasePtr<T>() {}
		explicit SharedPtr(T* ptr) : BasePtr<T>(ptr) {}
		SharedPtr(SharedPtr<T> const& other) : BasePtr<T>()
		{
			if (this == &other) return *this;
			this->m_Ptr = other.m_Ptr;
			this->m_RefCounter = other.m_RefCounter;
			this->m_RefCounter->Increment();
		}
		SharedPtr(SharedPtr<T>&& other) noexcept : BasePtr<T>(other.m_Ptr, other.m_RefCounter)
		{
			other.m_Ptr = nullptr;			
			other.m_RefCounter = nullptr;
		}
		SharedPtr(T* ptr, RefManager<T>* ref_manager) : BasePtr<T>(ptr, ref_manager)
		{
			if (this->m_RefCounter)
			{
				this->m_RefCounter->Increment();
			}
		}

		~SharedPtr() override = default;

		SharedPtr<T>& operator=(SharedPtr<T> const& other)
		{
			if (this == &other) return *this;
			this->m_Ptr = other.m_Ptr;
			this->m_RefCounter = other.m_RefCounter;
			this->m_RefCounter->Increment();
			return *this;
		}
		SharedPtr<T>& operator=(SharedPtr<T>&& other) noexcept
		{
			this->m_Ptr = other.m_Ptr;
			other.m_Ptr = nullptr;
			this->m_RefCounter = other.m_RefCounter;
			other.m_RefCounter = nullptr;
			return *this;
		}

		WeakPtr<T>& GetWeak()
		{
			WeakPtr<T> weak;

			return weak;
		}
	};

	template<typename T>
	class WeakPtr final : IWeakReference<T>
	{
		friend SharedPtr<T>;
		bool m_Expired;
		RefManager<T>* m_RefCounter;
		T* m_Ptr;

	public:
		WeakPtr() : m_Expired(true), m_RefCounter(nullptr), m_Ptr(nullptr) {}
		WeakPtr(WeakPtr<T> const& other) : m_Expired(other.m_Expired), m_RefCounter(other.m_RefCounter), m_Ptr(other.m_Ptr)
		{
			if (!m_Expired && m_RefCounter)
			{
				m_RefCounter->AddWeak(this);
			}
		}
		WeakPtr(WeakPtr<T>&& other) noexcept : m_Expired(other.m_Expired), m_RefCounter(other.m_RefCounter), m_Ptr(other.m_Ptr)
		{
			if (!m_Expired && m_RefCounter)
			{
				m_RefCounter->RemoveWeak(&other);
				m_RefCounter->AddWeak(this);
			}
			other.m_Expired = true;
			other.m_RefCounter = nullptr;
		}
		explicit WeakPtr(SharedPtr<T> const& ptr) : m_Expired(false), m_RefCounter(ptr.m_RefCounter), m_Ptr(ptr.m_Ptr)
		{
			if (m_RefCounter)
			{
				m_RefCounter->AddWeak(this);
			}
		}
		~WeakPtr()
		{
			if (!m_Expired && m_RefCounter)
			{
				m_RefCounter->RemoveWeak(this);
			}
		}

		WeakPtr<T>& operator=(WeakPtr<T> const& other)
		{
			if (this == &other) return *this;
			this.m_Expired = other.m_Expired;
			this->m_RefCounter = other.m_RefCounter;
			return *this;
		}
		WeakPtr<T>& operator=(WeakPtr<T>&& other) noexcept
		{
			m_Expired = other.m_Expired;
			other.m_Expired = true;
			m_RefCounter = other.m_RefCounter;
			other.m_RefCounter = nullptr;
			return *this;
		}

		void SetExpired() override
		{
			m_Expired = true;
			m_RefCounter = nullptr;
		}

		SharedPtr<T>& IsValid() override
		{
			if (m_Expired || !m_RefCounter)
			{
				return SharedPtr<T>();
			}

			return SharedPtr<T>(m_Ptr, m_RefCounter);
		}
	};
}
