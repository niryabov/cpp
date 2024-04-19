#include <array>
#include <iostream>
#include <memory>
#include <type_traits>
#include <iterator>
#include <vector>

template <size_t N>
class StackStorage {
 public:
  StackStorage() : shift(0) {}
  std::array<char, N> storage;
  size_t shift;
  StackStorage(const StackStorage& other) = delete;
  StackStorage& operator=(const StackStorage& other) = delete;
};

template <typename T, size_t N>
class StackAllocator {
 private:
 public:
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;
  using value_type = T;
  using type = T;
  StackStorage<N>* storage;

  StackAllocator(StackStorage<N>& st) : storage(&st) {}
  StackAllocator() {}
  StackAllocator(const StackAllocator<T, N>& other) : storage(other.storage) {}
  template <typename U>
  StackAllocator(const StackAllocator<U, N>& other) : storage(other.storage) {}
  StackAllocator<T, N>& operator=(StackAllocator<T, N> other) {
    storage = other.storage;
    return *this;
  }
  template <typename U>
  StackAllocator<T, N>& operator=(StackAllocator<U, N> other) {
    storage = other.storage;
    return *this;
  }

  template <typename U>
  struct rebind {
    using other = StackAllocator<U, N>;
  };

  T* allocate(std::size_t count) {
    size_t space = N - storage->shift;
    size_t delta_space = space;
    void* ptr = reinterpret_cast<void*>(storage->storage.data() + storage->shift);
    if (std::align(alignof(T), sizeof(T), ptr, space)) {
      T* result = reinterpret_cast<T*>(ptr);
      storage->shift += (delta_space - space) + (count * sizeof(T));
      return result;
    }
    return nullptr;
  }

  void deallocate(T* ptr, size_t count) {
    std::ignore = ptr;
    std::ignore = count;
  }
  ~StackAllocator() = default;
  template <typename U>
  bool operator==(const StackAllocator<U, N>& other) {
    return storage == other.storage;
  }
  template <typename U>
  bool operator!=(const StackAllocator<U, N>& other) {
    return storage != other.storage;
  }

  bool operator!=(const StackAllocator<T, N>& other) {
    return storage != other.storage;
  }
  bool operator==(const StackAllocator<T, N>& other) {
    return storage == other.storage;
  }
};

template <typename T, typename Alloc = std::allocator<T>>
class List {
 private:
  struct BaseNode {
    BaseNode* next;
    BaseNode* prev;
    BaseNode() {
      next = this;
      prev = this;
    }
  };
  struct Node : BaseNode {
    T value;
    Node(const T& object) : BaseNode(), value(object) {}
    Node() : value(T()) {}
  };
  using NodeAlloc =
      typename std::allocator_traits<Alloc>::template rebind_alloc<Node>;
  size_t sz_;
  [[no_unique_address]] NodeAlloc nodeAlloc;
  BaseNode fakeNode;

  void push_back() {
    Node* nw = std::allocator_traits<NodeAlloc>::allocate(nodeAlloc, 1);
    try {
      std::allocator_traits<NodeAlloc>::construct(nodeAlloc, nw);
    } catch (...) {
      std::allocator_traits<NodeAlloc>::deallocate(nodeAlloc, nw, 1);
      throw;
    }
    nw->prev = fakeNode.prev;
    nw->next = &fakeNode;
    fakeNode.prev->next = nw;
    fakeNode.prev = nw;
    ++sz_;
  }

 public:
  size_t size() const { return sz_; }

  List() : sz_(0) {
    fakeNode.next = &fakeNode;
    fakeNode.prev = &fakeNode;
  }

  List(const List& other, NodeAlloc othernd) : nodeAlloc(othernd) {
    fakeNode.next = &fakeNode;
    fakeNode.prev = &fakeNode;
    sz_ = 0;
    try {
      for (const auto& it : other) {
        this->push_back(it);
      }
    } catch (...) {
      this->~List();
      throw;
    }
  }

  List(size_t count, const T& value, Alloc alloc) : sz_(0), nodeAlloc(alloc) {
    fakeNode.next = &fakeNode;
    fakeNode.prev = &fakeNode;
    try {
      for (size_t i = 0; i < count; ++i) {
        this->push_back(value);
      }
    } catch (...) {
      this->~List();
      throw;
    }
  }
  List(size_t count, const T& value) : sz_(0) {
    fakeNode.next = &fakeNode;
    fakeNode.prev = &fakeNode;
    try {
      for (size_t i = 0; i < count; ++i) {
        this->push_back(value);
      }
    } catch (...) {
      this->~List();
      throw;
    }
  }
  List(size_t count) : sz_(0) {
    fakeNode.next = &fakeNode;
    fakeNode.prev = &fakeNode;
    try {
      for (size_t i = 0; i < count; ++i) {
        this->push_back();
      }
    } catch (...) {
      this->~List();
      throw;
    }
  }
  List(size_t count, Alloc alloc) : sz_(0), nodeAlloc(alloc) {
    fakeNode.next = &fakeNode;
    fakeNode.prev = &fakeNode;
    try {
      for (size_t i = 0; i < count; ++i) {
        this->push_back();
      }
    } catch (...) {
      this->~List();
      throw;
    }
  }
  List(Alloc alloc) : sz_(0), nodeAlloc(alloc) {
    fakeNode.next = &fakeNode;
    fakeNode.prev = &fakeNode;
  }

  List(const List& other)
      : nodeAlloc(std::allocator_traits<NodeAlloc>::
                  select_on_container_copy_construction(other.nodeAlloc)) {
    sz_ = 0;

    fakeNode.next = &fakeNode;
    fakeNode.prev = &fakeNode;
    try {
      for (const auto& it : other) {
        this->push_back(it);
      }
    } catch (...) {
      this->~List();
      throw;
    }
  }
  List(const List& other, Alloc alloc) : sz_(0), nodeAlloc(alloc) {
    fakeNode.next = &fakeNode;
    fakeNode.prev = &fakeNode;
    try {
      for (const auto& it : other) {
        this->push_back(it);
      }
    } catch (...) {
      this->~List();
      throw;
    }
  }
  List& operator=(const List& other) {
    if constexpr (std::allocator_traits<NodeAlloc>::
    propagate_on_container_copy_assignment::value) {
      List other_copy(other, other.nodeAlloc);
      std::swap(other_copy.sz_, sz_);
      std::swap(other_copy.fakeNode, fakeNode);
      std::swap(other_copy.nodeAlloc, nodeAlloc);
    } else {
      List other_copy(other);
      std::swap(other_copy.sz_, sz_);
      std::swap(other_copy.fakeNode, fakeNode);
      std::swap(other_copy.nodeAlloc, nodeAlloc);
    }
    return (*this);
  }

  Alloc get_allocator() { return Alloc(nodeAlloc); }

  template <bool IsConst>
  struct List_iterator {
   private:
   public:
    using Type = typename std::conditional<IsConst, const T, T>::type;
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = Type;
    using reference = Type&;
    using pointer = Type*;
    using difference_type = int64_t;
    List_iterator(BaseNode* node) : node(node) {}

    BaseNode* node;
    List_iterator operator++() {
      node = node->next;
      return *this;
    }
    List_iterator(const List_iterator<false>& other) { node = other.node; }

    List_iterator& operator=(const List_iterator<false>& other) {
      node = other.node;
      return *this;
    }
    List_iterator operator++(int) {
      List_iterator temp = *this;
      ++*this;
      return temp;
    }
    List_iterator operator--() {
      node = node->prev;
      return *this;
    }
    List_iterator operator--(int) {
      List_iterator temp = *this;
      --*this;
      return temp;
    }
    Type& operator*() { return reinterpret_cast<Node*>(node)->value; }
    Type* operator&() { return &node->value; }

    // operator List_iterator<true>() { return List_iterator<true>(node); }
    bool operator==(const List_iterator& other) const {
      return (other.node == node);
    }
    bool operator!=(const List_iterator& other) const {
      return (other.node != node);
    }
  };
  using iterator = List_iterator<false>;
  using const_iterator = List_iterator<true>;


  iterator begin() {
    iterator begin(fakeNode.next);
    return begin;
  }
  iterator end() {
    iterator end(&fakeNode);
    return end;
  }
  const_iterator cbegin() {
    const_iterator cbegin(fakeNode.next);
    return cbegin;
  }
  const_iterator cend() {
    const_iterator cend(&fakeNode);
    return cend;
  }
  const_iterator begin() const {
    const_iterator cbegin(fakeNode.next);
    return cbegin;
  }
  const_iterator end() const {
    const_iterator cend(const_cast<BaseNode*>(&fakeNode));
    return cend;
  }

  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;
  reverse_iterator rbegin() {
    return std::reverse_iterator<iterator>(this->end());
  }

  reverse_iterator rend() {
    reverse_iterator rend(&fakeNode);
    return rend;
  }
  const_reverse_iterator crbegin() {
    return std::reverse_iterator<iterator>(this->cend());
  }
  const_reverse_iterator crend() {
    const_reverse_iterator crend(&fakeNode);
    return crend;
  }
  const_reverse_iterator rbegin() const {
    return std::reverse_iterator<const_iterator>(
        const_cast<List*>(this)->cend());
  }
  const_reverse_iterator rend() const {
    const_reverse_iterator crend(&fakeNode);
    return crend;
  }
  void insert(const_iterator it, const T& value) {
    Node* nw = std::allocator_traits<NodeAlloc>::allocate(nodeAlloc, 1);
    try {
      std::allocator_traits<NodeAlloc>::construct(nodeAlloc, nw, value);

      // std::allocator_traits<Alloc>::construct(alloc, &(nw->value), value);
    } catch (...) {
      std::allocator_traits<NodeAlloc>::deallocate(nodeAlloc, nw, 1);
      throw;
    }
    nw->prev = it.node->prev;
    nw->next = it.node;
    it.node->prev->next = nw;
    it.node->prev = nw;
    ++sz_;
  }

  void push_back(const T& value) { insert(this->end(), value); }
  void push_front(const T& value) { insert(this->begin(), value); }

  void erase(const_iterator it) {
    BaseNode* node = it.node;
    node->next->prev = node->prev;
    node->prev->next = node->next;

    std::allocator_traits<NodeAlloc>::destroy(nodeAlloc,
                                              reinterpret_cast<Node*>(it.node));
    std::allocator_traits<NodeAlloc>::deallocate(
        nodeAlloc, reinterpret_cast<Node*>(it.node), 1);
    --sz_;
  }
  void pop_back() { erase(--(this->end())); }
  void pop_front() { erase(this->begin()); }
  ~List() {
    BaseNode* ptr = fakeNode.next;
    for (size_t j = 0; j < sz_; ++j) {
      BaseNode* next_ptr = ptr->next;
      std::allocator_traits<NodeAlloc>::destroy(nodeAlloc,
                                                reinterpret_cast<Node*>(ptr));
      std::allocator_traits<NodeAlloc>::deallocate(
          nodeAlloc, reinterpret_cast<Node*>(ptr), 1);
      ptr = next_ptr;
    }
    sz_ = 0;
  }
};
