#pragma once

#include <tiff.h>
#include <unordered_map>
#ifndef INLINE
#define INLINE inline __attribute__((always_inline))
#endif
template<typename Type>
class TStackObj;

template<typename Type>
class TStackPtr
{
    friend class TStackObj<Type>;
public:

    INLINE explicit TStackPtr(TStackObj<Type>& To)
        : Ptr(&To)
    {
        if(Ptr != nullptr)
        {
            MapKey = Ptr->AddViewer(this);
        }
    }

    INLINE explicit TStackPtr(TStackObj<Type>* To)
        : Ptr(To)
    {
        if(Ptr != nullptr)
        {
            MapKey = Ptr->AddViewer(this);
        }
    }

    TStackPtr(const TStackPtr& Other) = delete;
    TStackPtr(TStackPtr&& Other) = delete;

    INLINE TStackPtr()
        : Ptr(nullptr)
    {
    }

    INLINE ~TStackPtr()
    {
        if(Ptr != nullptr)
        {
            Ptr->RemoveViewer(MapKey);
        }
    }

    TStackPtr& operator=(TStackObj<Type>& To)
    {
        if(Ptr != nullptr)
        {
            Ptr->RemoveViewer(MapKey);
        }

        Ptr = &To;

        if(Ptr != nullptr)
        {
            MapKey = Ptr->AddViewer(this);
        }

        return *this;
    }

    TStackPtr& operator=(TStackObj<Type>* To)
    {
        if(Ptr != nullptr)
        {
            Ptr->RemoveViewer(MapKey);
        }

        Ptr = To;

        if(Ptr != nullptr)
        {
            MapKey = Ptr->AddViewer(this);
        }

        return *this;
    }
    
    INLINE void Reset()
    {
        if(Ptr != nullptr)
        {
            Ptr->RemoveViewer(MapKey);
            Ptr = nullptr;
        }
    }

    TStackPtr& operator=(TStackPtr) = delete;
    TStackPtr& operator=(const TStackPtr& Other) = delete;
    TStackPtr& operator=(TStackPtr&& Other) = delete;

    INLINE Type& operator*()
    {
        return **Ptr;
    }

    explicit inline constexpr operator bool() const noexcept
    {
        return Ptr != nullptr;
    }

private:

    INLINE Type& operator->()
    {
        return *Ptr;
    }

    TStackObj<Type>* Ptr;

    uint16 MapKey{0};

};

template<typename Type>
class TStackObj
{
    friend class TStackPtr<Type>;
public:

    INLINE explicit TStackObj(const Type Value)
        : Object(Value)
    {
    }

    INLINE explicit TStackObj(Type&& Value)
        : Object(std::move(Value))
    {
    }

    INLINE TStackObj()
        : Object(0)
    {
    }

    INLINE TStackObj(TStackObj&& Other) noexcept
        : Object(std::move(Other.Object))
        , Viewers(std::move(Other.Viewers))
        , MapKey(std::move(Other.MapKey))
    {
    }

    TStackObj(const TStackObj& Other) = delete;

    ~TStackObj()
    {
        for(std::pair<const uint16, TStackPtr<int>*>& Viewer : Viewers)
        {
            Viewer.second->Ptr = nullptr;
        }
    }

    INLINE TStackObj& operator=(const Type Value)
    {
        Object = Value;
        return *this;
    }

    INLINE TStackObj& operator=(Type&& Value)
    {
        Object = std::move(Value);
        return *this;
    }

    TStackObj& operator=(TStackObj&& Other) = delete;
    TStackObj& operator=(TStackObj Other) = delete;
    TStackObj& operator=(const TStackObj& Other) = delete;

    INLINE Type& operator*()
    {
        return Object;
    }

private:

    INLINE uint16 AddViewer(TStackPtr<Type>* NewViewer)
    {
        Viewers.emplace(++MapKey, NewViewer);
        return MapKey;
    }

    INLINE void RemoveViewer(const uint16 Key)
    {
        Viewers.erase(Key);
    }

    Type Object;

    std::unordered_map<uint16, TStackPtr<Type>*> Viewers;

    uint16 MapKey{0};

};

/* example
    TStackPtr<int32> B{nullptr};
    TStackPtr<int32> C{nullptr};
    {
        TStackObj<int32> A{54325};
        B = A;
        {
            TStackPtr<int32> D{A};
        }
        TStackObj<int32> E{std::move(A)};
        C = A;
        std::cout << *B;
    }
    if(B)
    {
        std::cout << *B;
    }
*/
