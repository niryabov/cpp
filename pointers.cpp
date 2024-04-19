#include <iostream>
#include <memory>
#include <type_traits>

template <typename T>
class WeakPtr;

struct BaseControlBlock {
  size_t shared_count = 1;
  size_t weak_count = 0;

  virtual char* GetPtr() = 0;

  virtual void DeleteObj() = 0;

  virtual void DeleteCb() = 0;

  void DeleteShared() {
    --shared_count;
    if (shared_count == 0) {
      DeleteObj();
      if (weak_count == 0) {
        DeleteCb();
      }
    }
  }

  void DeleteWeak() {
    --weak_count;
    if (shared_count == 0 && weak_count == 0) {
      DeleteCb();
    }
  }

  virtual ~BaseControlBlock() = default;
};

template <typename T, typename Alloc = std::allocator<T>,
    typename Deleter = std::default_delete<T>>
struct ControlBlockRegular : BaseControlBlock {
  ControlBlockRegular(T* ptr) : object(ptr) {}
  ControlBlockRegular(T* ptr, Alloc alloc, Deleter deleter)
      : object(ptr), alloc(alloc), deleter(deleter) {}
  virtual void DeleteCb() override {
    BlockAlloc b_alloc(alloc);
    std::allocator_traits<BlockAlloc>::deallocate(b_alloc, this, 1);
  }
  virtual void DeleteObj() override { deleter(object); }
  virtual char* GetPtr() override { return reinterpret_cast<char*>(object); }

 protected:
  T* object;
  Alloc alloc;
  Deleter deleter;
  using BlockAlloc = typename std::allocator_traits<
      Alloc>::template rebind_alloc<ControlBlockRegular<T, Alloc, Deleter>>;
};

template <typename T, typename Alloc = std::allocator<T>>
struct ControlBlockMakeShared : BaseControlBlock {
  template <typename... Args>
  ControlBlockMakeShared(Alloc alloc, Args&&... args) {
    // object = reinterpret_cast<char*>(
    //     std::allocator_traits<Alloc>::allocate(alloc, 1));
    std::allocator_traits<Alloc>::construct(alloc, reinterpret_cast<T*>(object),
                                            std::forward<Args>(args)...);
  }
  virtual void DeleteObj() override {
    std::allocator_traits<Alloc>::destroy(alloc, reinterpret_cast<T*>(object));
  }
  virtual void DeleteCb() override {
    BlockAlloc b_alloc(alloc);
    std::allocator_traits<BlockAlloc>::deallocate(b_alloc, this, 1);
  }
  virtual char* GetPtr() override { return reinterpret_cast<char*>(object); }

 protected:
  using BlockAlloc = typename std::allocator_traits<
      Alloc>::template rebind_alloc<ControlBlockMakeShared<T, Alloc>>;
  // char* object;
  char object[sizeof(T)];

  Alloc alloc;
};

template <typename T>
class SharedPtr {
 private:
  BaseControlBlock* cb = nullptr;
  template <typename>
  friend class SharedPtr;
  template <typename>
  friend class WeakPtr;

 public:
  SharedPtr(){};
  SharedPtr(T* ptr) : cb(new ControlBlockRegular<T>(ptr)) {}

  SharedPtr(const SharedPtr& other) : cb(other.cb) {
    if (cb) {
      cb->shared_count++;
    }
  }
  template <typename U>
  SharedPtr(const SharedPtr<U>& other) : cb(other.cb) {
    if (cb) {
      cb->shared_count++;
    }
  }

  SharedPtr(BaseControlBlock* cb) : cb(cb) {}
  void swap(SharedPtr& other) { std::swap(cb, other.cb); }

  SharedPtr& operator=(const SharedPtr& other) {
    SharedPtr copy(other);
    swap(copy);
    return *this;
  }

  template <typename U>
  SharedPtr& operator=(const SharedPtr<U>& other) {
    SharedPtr copy(other);
    swap(copy);
    return *this;
  }

  SharedPtr(SharedPtr&& other) noexcept {
    cb = other.cb;
    other.cb = nullptr;
  }

  template <typename U>
  SharedPtr(SharedPtr<U>&& other) noexcept {
    cb = other.cb;
    other.cb = nullptr;
  }

  SharedPtr& operator=(SharedPtr&& other) {
    SharedPtr copy(std::move(other));
    swap(copy);
    return *this;
  }

  template <typename U>
  SharedPtr& operator=(SharedPtr<U>&& other) {
    SharedPtr copy(std::move(other));
    swap(copy);
    return *this;
  }

  size_t use_count() const { return cb->shared_count; }

  void reset(T* ptr = nullptr) {
    if (cb) {
      cb->DeleteShared();
    }
    if (ptr) {
      cb = new ControlBlockRegular<T>(ptr);
    } else {
      cb = nullptr;
    }
  }
  T* get() const {
    if (cb) {
      return reinterpret_cast<T*>(cb->GetPtr());
    }
    return nullptr;
  }
  T* operator->() const { return get(); }
  T& operator*() const { return *(get()); }

  template <typename U, typename Deleter, typename Alloc>
  SharedPtr(U* ptr, Deleter deleter, Alloc alloc) {
    using BlockAlloc = typename std::allocator_traits<
        Alloc>::template rebind_alloc<ControlBlockRegular<T, Alloc, Deleter>>;
    BlockAlloc b_alloc(alloc);
    cb = std::allocator_traits<BlockAlloc>::allocate(b_alloc, 1);
    new (reinterpret_cast<ControlBlockRegular<T, Deleter, Alloc>*>(cb))
        ControlBlockRegular(static_cast<T*>(ptr), alloc, deleter);
  }

  template <typename U, typename Deleter>
  SharedPtr(U* ptr, Deleter deleter)
      : SharedPtr(ptr, deleter, std::allocator<T>()) {}
  ~SharedPtr() {
    if (cb) {
      cb->DeleteShared();
    }
  }
};

template <typename T>
class WeakPtr {
 private:
  BaseControlBlock* cb = nullptr;
  template <typename>
  friend class SharedPtr;
  template <typename>
  friend class WeakPtr;

 public:
  WeakPtr() = default;
  WeakPtr(const SharedPtr<T>& shared) : cb(shared.cb) {
    if (cb) {
      cb->weak_count++;
    }
  }
  template <typename U>
  WeakPtr(const SharedPtr<U>& shared) : cb(shared.cb) {
    if (cb) {
      cb->weak_count++;
    }
  }
  WeakPtr(const WeakPtr<T>& shared) : cb(shared.cb) {
    if (cb) {
      cb->weak_count++;
    }
  }
  template <typename U>
  WeakPtr(const WeakPtr<U>& shared) : cb(shared.cb) {
    if (cb) {
      cb->weak_count++;
    }
  }
  WeakPtr& operator=(const WeakPtr& other) {
    WeakPtr copy(other);
    std::swap(cb, copy.cb);
    return *this;
  }
  template <typename U>
  WeakPtr& operator=(const WeakPtr<U>& other) {
    WeakPtr copy(other);
    std::swap(cb, copy.cb);
    return *this;
  }
  WeakPtr(WeakPtr&& other) {
    cb = other.cb;
    other.cb = nullptr;
  }
  template <typename U>
  WeakPtr(WeakPtr<U>&& other) {
    cb = other.cb;
    other.cb = nullptr;
  }
  WeakPtr& operator=(WeakPtr&& other) {
    WeakPtr copy(std::move(other));
    std::swap(cb, copy.cb);
    return *this;
  }
  template <typename U>
  WeakPtr& operator=(WeakPtr<U>&& other) {
    WeakPtr copy(std::move(other));
    std::swap(cb, copy.cb);
    return *this;
  }
  size_t use_count() const {
    if (cb) {
      return cb->shared_count;
    }
    return 0;
  }
  bool expired() const {
    if (cb) {
      return (cb->shared_count == 0);
    }
    return true;
  }
  SharedPtr<T> lock() const {
    if (expired()) {
      return SharedPtr<T>();
    }
    cb->shared_count++;
    return SharedPtr<T>(cb);
  }
  ~WeakPtr() {
    if (cb) {
      cb->DeleteWeak();
    }
  }
};

template <typename T, typename Alloc, typename... Args>
SharedPtr<T> allocateShared(const Alloc& alloc, Args&&... args) {
  using BlockAlloc = typename std::allocator_traits<
      Alloc>::template rebind_alloc<ControlBlockMakeShared<T, Alloc>>;
  BlockAlloc b_alloc(alloc);
  ControlBlockMakeShared<T, Alloc>* cb =
      std::allocator_traits<BlockAlloc>::allocate(b_alloc, 1);

  new (cb) ControlBlockMakeShared<T, Alloc>(alloc, std::forward<Args>(args)...);

  return SharedPtr<T>(cb);
}

template <typename T, typename... Args>
SharedPtr<T> makeShared(Args&&... args) {
  return allocateShared<T>(std::allocator<T>(), std::forward<Args>(args)...);
}
