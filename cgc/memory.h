#pragma once
#include <set>
#include "type_traits.h"

namespace Cgc
{
	template<typename T>
	class RefManager : NonCopyable, NonMovable
	{
		T* m_Ptr;
		int m_StrongCount;
		int m_WeakCount;

	public:
		explicit RefManager(T* ptr) : m_Ptr(ptr), m_StrongCount(1), m_WeakCount(0) {}
		~RefManager()
		{
			Destroy();
		}

		void AddStrong()
		{
			++m_StrongCount;
		}
		void RemoveStrong()
		{
			--m_StrongCount;
			if (m_StrongCount <= 0)
			{
				Destroy();
			}
		}
		void AddWeak()
		{
			++m_WeakCount;
		}
		void RemoveWeak()
		{
			--m_WeakCount;
		}
		bool IsValid()
		{
			return m_Ptr != nullptr;
		}
		T* GetPtr()
		{
			return m_Ptr;
		}
		int GetStrongCount() const
		{
			return m_StrongCount;
		}
		int GetWeakCount() const
		{
			return m_WeakCount;
		}
		void Destroy()
		{
			if (m_Ptr != nullptr)
			{
				delete m_Ptr; // TODO use custom deleter
				m_Ptr = nullptr;
			}
		}
	};

	template<typename T>
	class BasePtr : NonCopyable, NonMovable
	{
	protected:
		RefManager<T>* m_RefManager;

		BasePtr() : m_RefManager(nullptr) {}
		explicit BasePtr(T* ptr) : m_RefManager(new RefManager<T>{ ptr }) {}
		explicit BasePtr(RefManager<T>* ref_manager) : m_RefManager(ref_manager) {}
		virtual ~BasePtr()
		{
			if (m_RefManager && this->m_RefManager->GetStrongCount() <= 0 && this->m_RefManager->GetWeakCount() <= 0)
			{
				delete this->m_RefManager;
			}
		}

	public:
		explicit operator bool() const { return m_RefManager && m_RefManager->IsValid(); }
		bool operator==(BasePtr const& other) { return m_RefManager == other.m_RefManager; }
		bool operator!=(BasePtr const& other) { return m_RefManager != other.m_RefManager; }
	};

	template<typename T>
	class UniquePtr final : public BasePtr<T>
	{
	public:
		UniquePtr() : BasePtr<T>() {}
		explicit UniquePtr(T* ptr) : BasePtr<T>(ptr) {}
		UniquePtr(UniquePtr&& other) noexcept : BasePtr<T>()
		{
			this->m_RefManager = other.m_RefManager;
			other.m_RefManager = nullptr;
		}
		~UniquePtr() override
		{
			if (this->m_RefManager)
			{
				this->m_RefManager->RemoveStrong();
			}
		}

		UniquePtr& operator=(UniquePtr&& other) noexcept
		{
			this->m_RefManager = other.m_RefManager;
			other.m_RefManager = nullptr;
			return *this;
		}
	};

	template<typename>
	class WeakPtr;

	template<typename T>
	class SharedPtr final : public BasePtr<T>
	{
		friend class WeakPtr<T>;

	public:
		SharedPtr() : BasePtr<T>() {}
		explicit SharedPtr(T* ptr) : BasePtr<T>(ptr) {}
		SharedPtr(SharedPtr<T> const& other) : BasePtr<T>()
		{
			if (this == &other) return;
			this->m_RefManager = other.m_RefManager;
			if (this->m_RefManager)
			{
				this->m_RefManager->AddStrong();
			}
		}
		SharedPtr(SharedPtr<T>&& other) noexcept : BasePtr<T>()
		{
			this->m_RefManager = other.m_RefManager;
			other.m_RefManager = nullptr;
		}
		SharedPtr(RefManager<T>* ref_manager) : BasePtr<T>(ref_manager)
		{
			if (this->m_RefManager)
			{
				this->m_RefManager->AddStrong();
			}
		}

		~SharedPtr() override
		{
			if (this->m_RefManager)
			{
				this->m_RefManager->RemoveStrong();
			}
		}

		SharedPtr<T>& operator=(SharedPtr<T> const& other)
		{
			if (this == &other) return *this;
			this->m_RefManager = other.m_RefManager;
			if (this->m_RefManager)
			{
				this->m_RefManager->AddStrong();
			}
			return *this;
		}
		SharedPtr<T>& operator=(SharedPtr<T>&& other) noexcept
		{
			this->m_RefManager = other.m_RefManager;
			other.m_RefManager = nullptr;
			return *this;
		}

		WeakPtr<T> GetWeak()
		{
			return WeakPtr<T>(*this);
		}
	};

	template<typename T>
	class WeakPtr final : public BasePtr<T>
	{
	public:
		WeakPtr() : BasePtr<T>() {}
		WeakPtr(WeakPtr<T> const& other) : BasePtr<T>(other.m_RefManager)
		{
			if (this == &other || !this->m_RefManager)
			{
				return;
			}
			this->m_RefManager->AddWeak();
		}
		WeakPtr(WeakPtr<T>&& other) noexcept : BasePtr<T>(other.m_RefManager)
		{
			other.m_RefManager = nullptr;
			if (this->m_RefManager)
			{
				this->m_RefManager->AddWeak();
			}
		}
		explicit WeakPtr(SharedPtr<T> const& sptr) : BasePtr<T>(sptr.m_RefManager)
		{
			if (this->m_RefManager)
			{
				this->m_RefManager->AddWeak();
			}
		}
		~WeakPtr() override
		{
			if (this->m_RefManager)
			{
				this->m_RefManager->RemoveWeak();
			}
		}

		WeakPtr<T>& operator=(WeakPtr<T> const& other)
		{
			if (this == &other) return *this;
			this->m_RefManager = other.m_RefManager;
			if (this->m_RefManager)
			{
				this->m_RefManager->AddWeak();
			}
			return *this;
		}
		WeakPtr<T>& operator=(WeakPtr<T>&& other) noexcept
		{
			this->m_RefManager = other.m_RefManager;
			other.m_RefManager = nullptr;
			if (this->m_RefManager)
			{
				this->m_RefManager->AddWeak();
			}
			return *this;
		}

		SharedPtr<T> TryLock()
		{
			if (!this->m_RefManager || !this->m_RefManager->IsValid())
			{
				return SharedPtr<T>{};
			}

			return SharedPtr<T>{this->m_RefManager};
		}
	};
}
