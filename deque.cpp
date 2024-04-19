#include <iostream>

template <typename T, bool IsConst, size_t in_sz>
class DequeIt {
 private:
  using Type = typename std::conditional<IsConst, T, const T>::type;
  T** ptr;
  int place;

 public:
  using iterator_category = std::random_access_iterator_tag;
  using difference_type = int;
  using value_type = Type;
  using reference = Type&;
  using pointer = Type*;

  template <typename K, bool Const, size_t sz>
  friend bool operator==(const DequeIt<K, Const, sz>& it1,
                         const DequeIt<K, Const, sz>& it2);

  template <typename K, bool Const, size_t sz>
  friend bool operator!=(const DequeIt<K, Const, sz>& it1,
                         const DequeIt<K, Const, sz>& it2);
  template <typename K, bool Const, size_t sz>
  friend int operator<=>(const DequeIt<K, Const, sz>& it1,
                         const DequeIt<K, Const, sz>& it2);

  template <typename K, bool Const, size_t sz>
  friend bool operator>(const DequeIt<K, Const, sz>& it1,
                        const DequeIt<K, Const, sz>& it2);

  template <typename K, bool Const, size_t sz>
  friend bool operator<(const DequeIt<K, Const, sz>& it1,
                        const DequeIt<K, Const, sz>& it2);

  template <typename K, bool Const, size_t sz>
  friend bool operator>=(const DequeIt<K, Const, sz>& it1,
                         const DequeIt<K, Const, sz>& it2);

  template <typename K, bool Const, size_t sz>
  friend bool operator<=(const DequeIt<K, Const, sz>& it1,
                         const DequeIt<K, Const, sz>& it2);

  template <typename K, bool Const, size_t sz>
  friend int operator-(const DequeIt<K, Const, sz>& it1,
                       const DequeIt<K, Const, sz>& it2);

  DequeIt(const DequeIt<T, IsConst, in_sz>& other)
      : ptr(other.ptr), place(other.place){};
  DequeIt(T** other_ptr, size_t other_place) {
    ptr = other_ptr;
    place = other_place;
  }
  DequeIt() : ptr(nullptr), place(0){};
  DequeIt<T, IsConst, in_sz>& operator=(
      const DequeIt<T, IsConst, in_sz>& other) {
    ptr = other.ptr;
    place = other.place;
    return *this;
  }
  Type& operator*() { return *(*(ptr) + place); };
  Type* operator->() { return *(ptr) + place; }

  Type& operator[](int n) { return *(*this + n); }

  DequeIt<T, IsConst, in_sz>& operator+=(int t) {
    if (t >= 0) {
      ptr += (place + t) / in_sz;
      place = (place + t) % in_sz;
    } else {
      if (place + t < 0) {
        ptr -= (-t - place + in_sz - 1) / in_sz;
        place =
            (in_sz - (-t - place) % in_sz == 16 ? 0
                                                : in_sz - (-t - place) % in_sz);
      } else {
        place += t;
      }
    }
    return *this;
  }

  DequeIt<T, IsConst, in_sz>& operator-=(int t) {
    (*this) += -1 * t;
    return *this;
  }
  DequeIt<T, IsConst, in_sz>& operator++() {
    *this += 1;
    return *this;
  }
  DequeIt<T, IsConst, in_sz> operator++(int) {
    DequeIt<T, IsConst, in_sz> temp = *this;
    ++(*this);
    return temp;
  }
  DequeIt<T, IsConst, in_sz>& operator--() {
    *this -= 1;
    return *this;
  }
  DequeIt<T, IsConst, in_sz> operator--(int) {
    DequeIt<T, IsConst, in_sz> temp = *this;
    --(*this);
    return temp;
  }
  operator DequeIt<T, false, in_sz>() {
    return DequeIt<T, false, in_sz>(ptr, place);
  }
};

template <typename T, bool IsConst, size_t in_sz>
bool operator==(const DequeIt<T, IsConst, in_sz>& it1,
                const DequeIt<T, IsConst, in_sz>& it2) {
  return ((it1 - it2) == 0);
}

template <typename T, bool IsConst, size_t in_sz>
bool operator!=(const DequeIt<T, IsConst, in_sz>& it1,
                const DequeIt<T, IsConst, in_sz>& it2) {
  return !(it1 == it2);
}

template <typename T, bool IsConst, size_t in_sz>
int operator<=>(const DequeIt<T, IsConst, in_sz>& it1,
                const DequeIt<T, IsConst, in_sz>& it2) {
  if (it1.ptr > it2.ptr) {
    return 1;
  }
  if (it1.ptr == it2.ptr) {
    if (it1.place > it2.place) {
      return 1;
    }
    if (it1.place == it2.place) {
      return 0;
    }
  }
  return -1;
  return false;
}

template <typename T, bool IsConst, size_t in_sz>
bool operator>(const DequeIt<T, IsConst, in_sz>& it1,
               const DequeIt<T, IsConst, in_sz>& it2) {
  return it1 - it2 > 0;
}

template <typename T, bool IsConst, size_t in_sz>
bool operator<(const DequeIt<T, IsConst, in_sz>& it1,
               const DequeIt<T, IsConst, in_sz>& it2) {
  return it2 > it1;
}

template <typename T, bool IsConst, size_t in_sz>
bool operator>=(const DequeIt<T, IsConst, in_sz>& it1,
                const DequeIt<T, IsConst, in_sz>& it2) {
  return !(it1 < it2);
}

template <typename T, bool IsConst, size_t in_sz>
bool operator<=(const DequeIt<T, IsConst, in_sz>& it1,
                const DequeIt<T, IsConst, in_sz>& it2) {
  return !(it1 > it2);
}

template <typename T, bool IsConst, size_t in_sz>
DequeIt<T, IsConst, in_sz> operator+(DequeIt<T, IsConst, in_sz> it, int t) {
  it += t;
  return it;
}

template <typename T, bool IsConst, size_t in_sz>
DequeIt<T, IsConst, in_sz> operator-(DequeIt<T, IsConst, in_sz> it, int t) {
  it -= t;
  return it;
}

template <typename T, bool IsConst, size_t in_sz>
DequeIt<T, IsConst, in_sz> operator+(int t, DequeIt<T, IsConst, in_sz> it) {
  it += t;
  return it;
}
template <typename T, bool IsConst, size_t in_sz>
int operator-(const DequeIt<T, IsConst, in_sz>& it1,
              const DequeIt<T, IsConst, in_sz>& it2) {
  return (it1.ptr - it2.ptr) * in_sz + it1.place - it2.place;
}

template <typename T, bool IsConst, size_t in_sz>
class DequeRevIt {
 private:
  using Type = typename std::conditional<IsConst, T, const T>::type;
  T** ptr;
  int place;

 public:
  using iterator_category = std::random_access_iterator_tag;
  using difference_type = int;
  using value_type = Type;
  using reference = Type&;
  using pointer = Type*;

  template <typename K, bool Const, size_t sz>
  friend bool operator==(const DequeRevIt<K, Const, sz>& it1,
                         const DequeRevIt<K, Const, sz>& it2);

  template <typename K, bool Const, size_t sz>
  friend bool operator!=(const DequeRevIt<K, Const, sz>& it1,
                         const DequeRevIt<K, Const, sz>& it2);
  template <typename K, bool Const, size_t sz>
  friend int operator<=>(const DequeRevIt<K, Const, sz>& it1,
                         const DequeRevIt<K, Const, sz>& it2);

  template <typename K, bool Const, size_t sz>
  friend bool operator>(const DequeRevIt<K, Const, sz>& it1,
                        const DequeRevIt<K, Const, sz>& it2);

  template <typename K, bool Const, size_t sz>
  friend bool operator<(const DequeRevIt<K, Const, sz>& it1,
                        const DequeRevIt<K, Const, sz>& it2);

  template <typename K, bool Const, size_t sz>
  friend bool operator>=(const DequeRevIt<K, Const, sz>& it1,
                         const DequeRevIt<K, Const, sz>& it2);

  template <typename K, bool Const, size_t sz>
  friend bool operator<=(const DequeRevIt<K, Const, sz>& it1,
                         const DequeRevIt<K, Const, sz>& it2);

  template <typename K, bool Const, size_t sz>
  friend int operator-(const DequeRevIt<K, Const, sz>& it1,
                       const DequeRevIt<K, Const, sz>& it2);

  DequeRevIt(const DequeRevIt<T, IsConst, in_sz>& other)
      : ptr(other.ptr), place(other.place){};
  DequeRevIt(T** other_ptr, size_t other_place) {
    ptr = other_ptr;
    place = other_place;
  }
  DequeRevIt() : ptr(nullptr), place(0){};
  DequeRevIt<T, IsConst, in_sz>& operator=(
      const DequeRevIt<T, IsConst, in_sz>& other) {
    ptr = other.ptr;
    place = other.place;
    return *this;
  }
  Type& operator*() { return *(*(ptr) + place); };
  Type* operator->() { return *(ptr) + place; }

  Type& operator[](int n) { return *(*this + n); }

  DequeRevIt<T, IsConst, in_sz>& operator-=(int t) {
    if (t >= 0) {
      ptr += (place + t) / in_sz;
      place = (place + t) % in_sz;
    } else {
      if (place + t < 0) {
        ptr -= (-t - place + in_sz - 1) / in_sz;
        place =
            (in_sz - (-t - place) % in_sz == 16 ? 0
                                                : in_sz - (-t - place) % in_sz);
      } else {
        place += t;
      }
    }
    return *this;
  }

  DequeRevIt<T, IsConst, in_sz>& operator+=(int t) {
    (*this) -= -1 * t;
    return *this;
  }
  DequeRevIt<T, IsConst, in_sz>& operator++() {
    *this += 1;
    return *this;
  }
  DequeRevIt<T, IsConst, in_sz> operator++(int) {
    DequeRevIt<T, IsConst, in_sz> temp = *this;
    ++(*this);
    return temp;
  }
  DequeRevIt<T, IsConst, in_sz>& operator--() {
    *this -= 1;
    return *this;
  }
  DequeRevIt<T, IsConst, in_sz> operator--(int) {
    DequeRevIt<T, IsConst, in_sz> temp = *this;
    --(*this);
    return temp;
  }
  operator DequeRevIt<T, false, in_sz>() {
    return DequeRevIt<T, false, in_sz>(ptr, place);
  }
};

template <typename T, bool IsConst, size_t in_sz>
bool operator==(const DequeRevIt<T, IsConst, in_sz>& it1,
                const DequeRevIt<T, IsConst, in_sz>& it2) {
  return ((it1 - it2) == 0);
}

template <typename T, bool IsConst, size_t in_sz>
bool operator!=(const DequeRevIt<T, IsConst, in_sz>& it1,
                const DequeRevIt<T, IsConst, in_sz>& it2) {
  return !(it1 == it2);
}

template <typename T, bool IsConst, size_t in_sz>
int operator<=>(const DequeRevIt<T, IsConst, in_sz>& it1,
                const DequeRevIt<T, IsConst, in_sz>& it2) {
  if (it1.ptr > it2.ptr) {
    return 1;
  }
  if (it1.ptr == it2.ptr) {
    if (it1.place > it2.place) {
      return 1;
    }
    if (it1.place == it2.place) {
      return 0;
    }
  }
  return -1;
  return false;
}

template <typename T, bool IsConst, size_t in_sz>
bool operator>(const DequeRevIt<T, IsConst, in_sz>& it1,
               const DequeRevIt<T, IsConst, in_sz>& it2) {
  return it1 - it2 > 0;
}

template <typename T, bool IsConst, size_t in_sz>
bool operator<(const DequeRevIt<T, IsConst, in_sz>& it1,
               const DequeRevIt<T, IsConst, in_sz>& it2) {
  return it2 > it1;
}

template <typename T, bool IsConst, size_t in_sz>
bool operator>=(const DequeRevIt<T, IsConst, in_sz>& it1,
                const DequeRevIt<T, IsConst, in_sz>& it2) {
  return !(it1 < it2);
}

template <typename T, bool IsConst, size_t in_sz>
bool operator<=(const DequeRevIt<T, IsConst, in_sz>& it1,
                const DequeRevIt<T, IsConst, in_sz>& it2) {
  return !(it1 > it2);
}

template <typename T, bool IsConst, size_t in_sz>
DequeRevIt<T, IsConst, in_sz> operator+(DequeRevIt<T, IsConst, in_sz> it,
                                        int t) {
  it += t;
  return it;
}

template <typename T, bool IsConst, size_t in_sz>
DequeRevIt<T, IsConst, in_sz> operator-(DequeRevIt<T, IsConst, in_sz> it,
                                        int t) {
  it -= t;
  return it;
}

template <typename T, bool IsConst, size_t in_sz>
DequeRevIt<T, IsConst, in_sz> operator+(int t,
                                        DequeRevIt<T, IsConst, in_sz> it) {
  it += t;
  return it;
}
template <typename T, bool IsConst, size_t in_sz>
int operator-(const DequeRevIt<T, IsConst, in_sz>& it1,
              const DequeRevIt<T, IsConst, in_sz>& it2) {
  return -1 * ((it1.ptr - it2.ptr) * in_sz + it1.place - it2.place);
}

template <typename T>
class Deque {
 private:
  T** arr;
  size_t out_sz;
  static constexpr size_t in_sz = 16;
  size_t start;
  size_t sz;

  void delete_out(T** arr, size_t i) {
    for (size_t j = 0; j < i; ++j) {
      delete[] reinterpret_cast<char*>(*(arr + j));
    }
    delete[] reinterpret_cast<char*>(arr);
  }

  void delete_in(T** arr, size_t start, size_t sz, size_t out_sz) {
    for (size_t j = 0; j < sz; ++j) {
      (*(arr + (start + j) / in_sz) + (start + j) % in_sz)->~T();
    }
    for (size_t j = 0; j < out_sz; ++j) {
      delete[] reinterpret_cast<char*>(*(arr + j));
    }
    delete[] reinterpret_cast<char*>(arr);
  }

 public:
  using iterator = DequeIt<T, true, in_sz>;
  using const_iterator = DequeIt<T, false, in_sz>;

  iterator begin() { return iterator((arr + start / in_sz), start % in_sz); }

  const_iterator begin() const {
    return const_iterator((arr + start / in_sz), start % in_sz);
  }

  const_iterator cbegin() {
    return const_iterator((arr + start / in_sz), start % in_sz);
  }
  iterator end() {
    return iterator((arr + (start + sz) / in_sz), (start + sz) % in_sz);
  }
  const_iterator end() const {
    return const_iterator((arr + (start + sz) / in_sz), (start + sz) % in_sz);
  }

  const_iterator cend() {
    return const_iterator((arr + (start + sz) / in_sz), (start + sz) % in_sz);
  }
  using reverse_iterator = DequeRevIt<T, true, in_sz>;
  using const_reverse_iterator = DequeRevIt<T, false, in_sz>;

  reverse_iterator rend() {
    return reverse_iterator((arr + (start - 1) / in_sz), (start - 1) % in_sz);
  }
  const_reverse_iterator rend() const {
    return const_reverse_iterator((arr + (start - 1) / in_sz),
                                  (start - 1) % in_sz);
  }
  const_reverse_iterator crend() {
    return const_reverse_iterator((arr + (start - 1) / in_sz),
                                  (start - 1) % in_sz);
  }
  reverse_iterator rbegin() {
    return reverse_iterator((arr + (start - 1 + sz) / in_sz),
                            (start + sz - 1) % in_sz);
  }
  const_reverse_iterator rbegin() const {
    return const_reverse_iterator((arr + (start - 1 + sz) / in_sz),
                                  (start + sz - 1) % in_sz);
  }

  const_reverse_iterator crbegin() {
    return const_reverse_iterator((arr + (start + sz - 1) / in_sz),
                                  (start + sz - 1) % in_sz);
  }

  Deque(size_t count, const T& value)
      : arr(reinterpret_cast<T**>(
            new char[sizeof(T*) * (count + in_sz - 1) / in_sz * 3])),
        out_sz((count + in_sz - 1) / in_sz * 3),
        start((count + in_sz - 1) / in_sz * in_sz),
        sz(count) {
    size_t i = 0;
    try {
      for (; i < out_sz; ++i) {
        new (arr + i) T*(reinterpret_cast<T*>(new char[in_sz * sizeof(T)]));
      }
    } catch (...) {
      delete_out(arr, i);
      throw;
    }

    i = 0;
    try {
      for (; i < count; ++i) {
        new (*(arr + (start + i) / in_sz) + (start + i) % in_sz) T(value);
      }
    } catch (...) {
      delete_in(arr, start, i, out_sz);
      throw;
    }
  }

  Deque(const Deque<T>& other)
      : arr(reinterpret_cast<T**>(new char[sizeof(T*) * other.out_sz])),
        out_sz(other.out_sz),
        start(other.start),
        sz(other.sz) {
    size_t i = 0;
    try {
      for (; i < out_sz; ++i) {
        new (arr + i) T*(reinterpret_cast<T*>(new char[in_sz * sizeof(T)]));
      }
    } catch (...) {
      delete_out(arr, i);
      throw;
    }

    i = 0;
    try {
      for (; i < sz; ++i) {
        new (*(arr + (start + i) / in_sz) + (start + i) % in_sz) T(other[i]);
      }
    } catch (...) {
      delete_in(arr, start, i, out_sz);
      throw;
    }
  }
  Deque(size_t count)
      : arr(reinterpret_cast<T**>(
            new char[sizeof(T*) * (count + in_sz - 1) / in_sz * 3])),
        out_sz((count + in_sz - 1) / in_sz * 3),
        start((count + in_sz - 1) / in_sz * in_sz),
        sz(count) {
    size_t i = 0;
    try {
      for (; i < out_sz; ++i) {
        new (arr + i) T*(reinterpret_cast<T*>(new char[in_sz * sizeof(T)]));
      }
    } catch (...) {
      delete_out(arr, i);
      throw;
    }

    i = 0;
    try {
      for (; i < count; ++i) {
        new (*(arr + (start + i) / in_sz) + (start + i) % in_sz) T(T());
      }
    } catch (...) {
      delete_in(arr, start, i, out_sz);
      throw;
    }
  }
  Deque() : arr(nullptr), out_sz(0), start(0), sz(0) {}

  Deque<T>& operator=(const Deque<T>& other) {
    T** newarr = reinterpret_cast<T**>(new char[sizeof(T*) * other.out_sz]);
    size_t i = 0;
    try {
      for (; i < other.out_sz; ++i) {
        new (newarr + i) T*(reinterpret_cast<T*>(new char[in_sz * sizeof(T)]));
      }
    } catch (...) {
      delete_out(newarr, i);
      throw;
    }
    i = 0;
    try {
      for (; i < other.sz; ++i) {
        new (*(newarr + (other.start + i) / in_sz) + (other.start + i) % in_sz)
            T(other[i]);
      }
    } catch (...) {
      delete_in(newarr, other.start, i, other.out_sz);
      throw;
    }
    delete_in(arr, start, sz, out_sz);
    out_sz = other.out_sz;
    start = other.start;
    sz = other.sz;
    arr = newarr;
    return *this;
  }
  size_t size() { return sz; }
  size_t size() const { return sz; }

  T& operator[](size_t index) {
    return *(*(arr + (start + index) / in_sz) + (start + index) % in_sz);
  }
  const T& operator[](size_t index) const {
    return *(*(arr + (start + index) / in_sz) + (start + index) % in_sz);
  }

  T& at(size_t index) {
    if (index >= sz) {
      throw std::out_of_range("");
    }
    return (*this)[index];
  }
  const T& at(size_t index) const {
    if (index >= sz) {
      throw std::out_of_range("");
    }
    return (*this)[index];
  }

  void push_back(const T& value) {
    if (start + sz == out_sz * in_sz) {
      size_t n_out_sz = out_sz * 3 + 1;
      size_t n_start = out_sz * in_sz + start;
      T** newarr = reinterpret_cast<T**>(new char[sizeof(T*) * n_out_sz]);
      size_t i = 0;
      try {
        for (; i < out_sz; ++i) {
          new (newarr + i)
              T*(reinterpret_cast<T*>(new char[sizeof(T) * in_sz]));
        }
        for (size_t j = 0; j < out_sz; ++j, ++i) {
          new (newarr + i) T*(*(arr + j));
        }
        for (; i < n_out_sz; ++i) {
          new (newarr + i)
              T*(reinterpret_cast<T*>(new char[sizeof(T) * in_sz]));
        }
      } catch (...) {
        for (size_t j = 0; j < i && j < out_sz; ++j) {
          delete[] reinterpret_cast<char*>(*(newarr + j));
        }
        for (size_t j = 2 * out_sz; j < i; ++i) {
          delete[] reinterpret_cast<char*>(*(newarr + j));
        }
        delete[] reinterpret_cast<char*>(newarr);

        throw;
      }

      delete[] reinterpret_cast<char*>(arr);
      arr = newarr;
      start = n_start;
      out_sz = n_out_sz;
    }
    new (*(arr + (start + sz) / in_sz) + (start + sz) % in_sz) T(value);
    ++sz;
  }
  void push_front(const T& value) {
    if (start % in_sz == 0) {
      if (start == 0) {
        if (out_sz == 0) {
          this->push_back(value);
          return;
        }
        size_t n_out_sz = out_sz * 3 + 1;
        size_t n_start = out_sz * in_sz + start;
        T** newarr = reinterpret_cast<T**>(new char[sizeof(T*) * n_out_sz]);
        size_t i = 0;
        try {
          for (; i < out_sz; ++i) {
            new (newarr + i)
                T*(reinterpret_cast<T*>(new char[sizeof(T) * in_sz]));
          }
          for (size_t j = 0; j < out_sz; ++j, ++i) {
            new (newarr + i) T*(*(arr + j));
          }
          for (; i < n_out_sz; ++i) {
            new (newarr + i)
                T*(reinterpret_cast<T*>(new char[sizeof(T) * in_sz]));
          }
        } catch (...) {
          for (size_t j = 0; j < i && j < out_sz; ++j) {
            delete[] reinterpret_cast<char*>(*(newarr + j));
          }
          for (size_t j = 2 * out_sz; j < i; ++i) {
            delete[] reinterpret_cast<char*>(*(newarr + j));
          }
          delete[] reinterpret_cast<char*>(newarr);
          throw;
        }

        delete[] reinterpret_cast<char*>(arr);
        arr = newarr;
        start = n_start;
        out_sz = n_out_sz;
      }
      new (*(arr + start / in_sz - 1) + in_sz - 1) T(value);
      --start;
      ++sz;
      return;
    }

    new (*(arr + (start - 1) / in_sz) + (start - 1) % in_sz) T(value);
    ++sz;
    --start;
  }

  void pop_back() {
    --sz;
    (*(arr + (start + sz) / in_sz) + (start + sz) % in_sz)->~T();
  }

  void pop_front() {
    (*(arr + start / in_sz) + start % in_sz)->~T();
    ++start;
    --sz;
  }

  void insert(iterator it, const T& value) {
    if (it > this->end() || it < this->begin()) {
      return;
    }
    if (it == this->end()) {
      this->push_back(value);
      return;
    }
    if (it == this->begin()) {
      this->push_front(value);
      return;
    }
    int index = it - this->begin();
    this->push_back((*this)[sz - 1]);
    for (int i = sz - 1; i > index; --i) {
      (*this)[i] = (*this)[i - 1];
    }
    (*this)[index] = value;
  }

  void erase(iterator it) {
    if (it >= this->end() || it < this->begin()) {
      return;
    }
    if (it == this->begin()) {
      this->pop_front();
      return;
    }
    size_t index = it - this->begin();
    for (size_t i = index; i < sz - 1; ++i) {
      (*this)[i] = (*this)[i + 1];
    }
    (*(arr + (start + sz) / in_sz) + (start + sz) % in_sz)->~T();
    --sz;
  }

  ~Deque() { delete_in(arr, start, sz, out_sz); }
};
