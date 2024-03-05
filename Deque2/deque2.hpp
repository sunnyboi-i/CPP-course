#include <stddef.h>

#include <cmath>
#include <cstdio>
#include <exception>
#include <iostream>

const size_t kKinf = 2147483648 / 2;
const size_t kKeight = 8;
const size_t kSeven = 7;

template <typename T, typename Allocator = std::allocator<T>>
class Deque {
 public:
  template <bool IsConst>
  class base_iterator;

  using allocator_type = Allocator;
  using value_type = T;
  using internal_alloc_traits = std::allocator_traits<Allocator>;
  using external_alloc =
      typename internal_alloc_traits::template rebind_alloc<T*>;
  using external_alloc_traits =
      typename internal_alloc_traits::template rebind_traits<T*>;
  Deque()
      : need_{1},
        externall_{nullptr},
        begin_x_{0},
        begin_y_{0},
        end_x_{0},
        end_y_{1},
        current_size_{0},
        ex_alloc_{},
        int_alloc_{} {
    externall_ = external_alloc_traits::allocate(ex_alloc_, need_);
    size_t allocated = 0;
    try {
      allocate(externall_, 0, need_, allocated);
    } catch (...) {
      deallocate_w(externall_, allocated);
      throw;
    }
  }

  Deque(const Allocator& alloc)
      : need_{1},
        externall_{nullptr},
        begin_x_{0},
        begin_y_{0},
        end_x_{0},
        end_y_{1},
        current_size_{0},
        ex_alloc_{alloc},
        int_alloc_{alloc} {
    externall_ = external_alloc_traits::allocate(ex_alloc_, need_);
    size_t allocated = 0;
    try {
      allocate(externall_, 0, need_, allocated);
    } catch (...) {
      deallocate_w(externall_, allocated);
      throw;
    }
  }

  Allocator get_allocator() const { return int_alloc_; }

  Deque(size_t count, const Allocator& alloc = Allocator()) : Deque() {
    ex_alloc_ = alloc;
    int_alloc_ = alloc;
    size_t allocated;
    try {
      allocated = 0;
      for (size_t i = 0; i < count; ++i) {
        emplace_back();  // fixed
        allocated++;
      }
    } catch (...) {
      for (size_t i = 0; i < allocated; ++i) {
        pop_back();
      }
      throw;
    }
  }

  Deque(size_t count, const T& value, const Allocator& alloc = Allocator())
      : Deque() {
    ex_alloc_ = alloc;
    int_alloc_ = alloc;
    size_t allocated = 0;
    try {
      for (size_t i = 0; i < count; ++i) {
        push_back(value);
        allocated++;
      }
    } catch (...) {
      for (size_t i = 0; i < allocated; ++i) {
        pop_back();
      }
      throw;
    }
  }
  Deque(std::initializer_list<T> init, const Allocator& alloc = Allocator())
      : Deque() {
    ex_alloc_ = alloc;
    int_alloc_ = alloc;
    size_t allocated = 0;
    try {
      for (auto& value : init) {
        push_back(value);
        ++allocated;
      }
    } catch (...) {
      for (size_t i = 0; i < allocated; ++i) {
        pop_back();
      }
      throw;
    }
  }

  Deque(const Deque& k_other)
      : need_{k_other.need_},
        current_size_{k_other.current_size_},
        externall_{nullptr},
        begin_x_{k_other.begin_x_},
        begin_y_{k_other.begin_y_},
        end_x_{k_other.end_x_},
        end_y_{k_other.end_y_},
        ex_alloc_{external_alloc_traits ::select_on_container_copy_construction(
            k_other.ex_alloc_)},
        int_alloc_{
            internal_alloc_traits ::select_on_container_copy_construction(
                k_other.int_alloc_)} {
    size_t allocated = 0;
    size_t allocated_i = 0;
    size_t allocated_j = 0;
    size_t pos_i = 0;
    size_t pos_j = 0;
    try {
      externall_ = external_alloc_traits::allocate(ex_alloc_, need_);
      allocate(externall_, 0, need_, allocated);
      pos_i = begin_x_ + (begin_y_ + 1) / kKeight;
      pos_j = (begin_y_ + 1) % kKeight;
      allocated_i = pos_i;
      allocated_j = pos_j;
      while (pos_i != end_x_ || pos_j != end_y_) {
        internal_alloc_traits::construct(int_alloc_,
                                         (externall_[pos_i] + pos_j),
                                         k_other.externall_[pos_i][pos_j]);
        ++pos_j;
        ++allocated_j;
        if (pos_j == kKeight) {
          ++pos_i;
          ++allocated_i;
          pos_j = 0;
          allocated_j = 0;
        }
      }
    } catch (...) {
      for (size_t i = 0; i < allocated; ++i) {
        pop_back();
      }
      deallocate_w(externall_, allocated);
      external_alloc_traits::deallocate(ex_alloc_, externall_, need_);
      throw;
    }
  }
  Deque(Deque&& k_other) noexcept
      : need_{k_other.need_},
        current_size_{k_other.current_size_},
        externall_{k_other.externall_},
        begin_x_{k_other.begin_x_},
        begin_y_{k_other.begin_y_},
        end_x_{k_other.end_x_},
        end_y_{k_other.end_y_},
        ex_alloc_{external_alloc_traits ::select_on_container_copy_construction(
            k_other.get_allocator())},
        int_alloc_{
            internal_alloc_traits ::select_on_container_copy_construction(
                k_other.get_allocator())} {
    k_other.need_ = 0;
    k_other.current_size_ = 0;
    k_other.externall_ = nullptr;
    k_other.begin_x_ = 0;
    k_other.begin_y_ = 0;
    k_other.end_x_ = 0;
    k_other.end_y_ = 1;  // fixed
  }

  bool operator!=(const Deque& other) {
    return (externall_ != other.externall_);
  }

  Deque& operator=(const Deque& k_other) {
    if (*this != k_other) {
      Deque tmp = k_other;
      tmp.int_alloc_ = int_alloc_;
      tmp.ex_alloc_ = ex_alloc_;
      if (internal_alloc_traits::propagate_on_container_copy_assignment::
              value) {
        int_alloc_ = k_other.get_allocator();
        ex_alloc_ = k_other.get_allocator();
      }
      deck_swap(tmp);
    }
    return *this;
  }

  Deque& push_back(const T& value) {
    if (end_y_ == 0 && end_x_ == need_) {
      T** new_external = external_alloc_traits::allocate(ex_alloc_, 3 * need_);
      size_t allocated1 = 0;
      size_t allocated2 = 0;
      try {
        allocate(new_external, 0, need_, allocated1);
        allocate(new_external, 2 * need_, 3 * need_, allocated2);
      } catch (...) {
        deallocate(new_external, 0, allocated1);
        deallocate(new_external, 2 * need_, (2 * need_ + allocated2));
        external_alloc_traits::deallocate(ex_alloc_, new_external, need_);
        throw;
      }
      medium_set(new_external, need_);
      external_alloc_traits::deallocate(ex_alloc_, externall_, need_);
      externall_ = new_external;
      begin_x_ += need_;
      end_x_ = 2 * need_;
      need_ = 3 * need_;
    }
    internal_alloc_traits::construct(int_alloc_, (externall_[end_x_] + end_y_),
                                     value);
    end_y_++;
    if (end_y_ == kKeight) {
      end_y_ = 0;
      end_x_++;
    }
    current_size_++;
    return *this;
  }

  void push_back(T&& value) { emplace_back(std::move(value)); }

  template <typename... Args>
  Deque& emplace_back(Args&&... arguments) {
    if (end_y_ == 0 && end_x_ == need_) {
      T** new_external = external_alloc_traits::allocate(ex_alloc_, 3 * need_);
      size_t allocated1 = 0;
      size_t allocated2 = 0;
      try {
        allocate(new_external, 0, need_, allocated1);
        allocate(new_external, 2 * need_, 3 * need_, allocated2);
      } catch (...) {
        deallocate(new_external, 0, allocated1);
        deallocate(new_external, 2 * need_, (2 * need_ + allocated2));
        external_alloc_traits::deallocate(ex_alloc_, new_external, need_);
        throw;
      }
      medium_set(new_external, need_);
      external_alloc_traits::deallocate(ex_alloc_, externall_, need_);
      externall_ = new_external;
      begin_x_ += need_;
      end_x_ = 2 * need_;
      need_ = 3 * need_;
    }
    internal_alloc_traits::construct(int_alloc_, (externall_[end_x_] + end_y_),
                                     std::forward<Args>(arguments)...);
    end_y_++;
    if (end_y_ == kKeight) {
      end_y_ = 0;
      end_x_++;
    }
    current_size_++;
    return *this;
  }

  Deque& pop_back() {
    if (end_y_ == 0) {
      end_y_ = kSeven;
      end_x_--;
    } else {
      end_y_--;
    }
    internal_alloc_traits::destroy(int_alloc_, (externall_[end_x_] + end_y_));
    current_size_--;
    return *this;
  }

  Deque& push_front(const T& value) {
    int begin_x = static_cast<int>(begin_x_);
    if (begin_x == -1 && begin_y_ == kSeven) {
      T** new_external = external_alloc_traits::allocate(ex_alloc_, 3 * need_);
      size_t allocated1 = 0;
      size_t allocated2 = 0;
      try {
        allocate(new_external, 0, need_, allocated1);
        allocate(new_external, 2 * need_, 3 * need_, allocated2);
      } catch (...) {
        deallocate(new_external, 0, allocated1);
        deallocate(new_external, 2 * need_, (2 * need_ + allocated2));
        external_alloc_traits::deallocate(ex_alloc_, new_external, need_);
        throw;
      }
      medium_set(new_external, need_);
      external_alloc_traits::deallocate(ex_alloc_, externall_, need_);
      externall_ = new_external;
      begin_x_ += need_;
      end_x_ += need_;
      need_ = 3 * need_;
    }
    internal_alloc_traits::construct(int_alloc_,
                                     (externall_[begin_x_] + begin_y_), value);
    if (begin_y_ == 0) {
      begin_x_--;
      begin_y_ = kSeven;
    } else {
      begin_y_--;
    }
    current_size_++;
    return *this;
  }

  void push_front(T&& value) { emplace_front(std::move(value)); }
  template <typename... Args>
  Deque& emplace_front(Args&&... arguments) {
    int begin_x = static_cast<int>(begin_x_);
    if (begin_x == -1 && begin_y_ == kSeven) {
      T** new_external = external_alloc_traits::allocate(ex_alloc_, 3 * need_);
      size_t allocated1 = 0;
      size_t allocated2 = 0;
      try {
        allocate(new_external, 0, need_, allocated1);
        allocate(new_external, 2 * need_, 3 * need_, allocated2);
      } catch (...) {
        deallocate(new_external, 0, allocated1);
        deallocate(new_external, 2 * need_, (2 * need_ + allocated2));
        external_alloc_traits::deallocate(ex_alloc_, new_external, need_);
        throw;
      }
      medium_set(new_external, need_);
      external_alloc_traits::deallocate(ex_alloc_, externall_, need_);
      externall_ = new_external;
      begin_x_ += need_;
      end_x_ += need_;
      need_ = 3 * need_;
    }
    internal_alloc_traits::construct(int_alloc_,
                                     (externall_[begin_x_] + begin_y_),
                                     std::forward<Args>(arguments)...);
    if (begin_y_ == 0) {
      begin_x_--;
      begin_y_ = kSeven;
    } else {
      begin_y_--;
    }
    current_size_++;
    return *this;
  }

  Deque& pop_front() {
    internal_alloc_traits::destroy(int_alloc_,
                                   (externall_[begin_x_] + begin_y_));
    if (begin_y_ == kSeven) {
      begin_x_++;
      begin_y_ = 0;
    } else {
      begin_y_++;
    }
    current_size_--;
    return *this;
  }

  size_t size() const { return current_size_; }

  bool empty() { return (current_size_ == 0); }

  const T& operator[](size_t position) const {
    position++;
    return externall_[begin_x_ + (position + begin_y_) / kKeight]
                     [(begin_y_ + position) % kKeight];
  }

  T& operator[](size_t position) {
    position++;
    return externall_[begin_x_ + (position + begin_y_) / kKeight]
                     [(begin_y_ + position) % kKeight];
  }

  T& at(size_t position) {
    if (position >= size()) {
      throw std::out_of_range{"OUT OF RANGE\n"};
    }
    return (*this)[position];
  }

  const T& at(size_t position) const {
    position++;
    if (position >= size()) {
      throw std::out_of_range{"OUT OF RANGE\n"};
    }
    return (*this)[position];
  }

  void show_deque() {
    size_t pos_i = begin_x_;
    size_t pos_j = begin_y_;
    std::cout << pos_i << ' ' << pos_j << std::endl;
    do {
      if (pos_j == kKArrL) {
        ++pos_i;
        pos_j = 0;
      }
      std::cout << externall_[pos_i][pos_j] << " ";
      ++pos_j;
    } while (pos_i != end_x_ || pos_j != end_y_);
    std::cout << "\n";
  }

  ~Deque() {
    size_t pos_i = begin_x_;
    size_t pos_j = begin_y_ + 1;
    if (pos_j == kKeight) {
      pos_i++;
      pos_j = 0;
    }
    while (pos_i != end_x_ || pos_j != end_y_) {
      internal_alloc_traits::destroy(int_alloc_, (externall_[pos_i] + pos_j));
      ++pos_j;
      if (pos_j == kKeight) {
        ++pos_i;
        pos_j = 0;
      }
    }

    for (size_t i = 0; i < need_; ++i) {
      internal_alloc_traits ::deallocate(int_alloc_, externall_[i], kKArrL);
    }
    external_alloc_traits::deallocate(ex_alloc_, externall_, need_);
  }

  template <bool IsConst>
  class base_iterator {
   public:
    using iterator_category = std::random_access_iterator_tag;
    using value_type = std::conditional_t<IsConst, const T, T>;
    using pointer = value_type*;
    using reference = value_type&;
    using difference_type = std::ptrdiff_t;
    base_iterator() : external_{nullptr}, position_x_{0}, position_y_{0} {}

    base_iterator(size_t number_x, size_t number_y, pointer* exter)
        : position_x_{number_x}, position_y_{number_y}, external_{exter} {}

    base_iterator(const base_iterator& k_other)
        : external_{k_other.external_},
          position_x_{k_other.position_x_},
          position_y_{k_other.position_y_} {}

    base_iterator& operator=(base_iterator k_other) {
      std::swap(external_, k_other.external_);
      std::swap(position_x_, k_other.position_x_);
      std::swap(position_y_, k_other.position_y_);
      return *this;
    }

    base_iterator& operator++() {
      ++position_y_;
      if ((position_y_ % kKeight) == 0) {
        ++position_x_;
        position_y_ = 0;
      }
      return *this;
    }

    base_iterator operator++(int) {
      base_iterator temp = *this;
      ++(*this);
      return temp;
    }

    base_iterator& operator--() {
      if (position_y_ == 0) {
        --position_x_;
        position_y_ = kSeven;
      } else {
        --position_y_;
      }
      return *this;
    }

    base_iterator operator--(int) {
      base_iterator temp = *this;
      --(*this);
      return temp;
    }

    base_iterator operator-(difference_type num) const {
      base_iterator tmp(*this);
      tmp.position_x_ -= ((kSeven + num) - tmp.position_y_) / kKeight;
      tmp.position_y_ = ((kKinf + tmp.position_y_) - num) % kKeight;

      return tmp;
    }

    base_iterator operator-=(size_t num) {
      position_x_ -= ((kSeven + num) - position_y_) / kKeight;
      position_y_ = ((kKinf + position_y_) - num) % kKeight;

      return *this;
    }

    base_iterator operator+(size_t num) const {
      base_iterator tmp(*this);
      tmp.position_x_ += (num + tmp.position_y_) / kKeight;
      tmp.position_y_ = (tmp.position_y_ + num) % kKeight;

      return tmp;
    }

    base_iterator& operator+=(size_t num) {
      position_x_ += (num + position_y_) / kKeight;
      position_y_ = (position_y_ + num) % kKeight;

      return *this;
    }

    difference_type operator-(const base_iterator& k_other) const {
      return (position_x_ - k_other.position_x_) * kKeight -
             k_other.position_y_ + position_y_;
    }

    reference operator*() const {
      return (external_[position_x_][position_y_]);
    }

    pointer operator->() const { return &(*(*this)); }

    bool operator==(base_iterator k_other) const {
      return (position_x_ == k_other.position_x_) &&
             (position_y_ == k_other.position_y_) &&
             (external_ == k_other.external_);
    }

    bool operator!=(base_iterator k_other) const { return !(*this == k_other); }

    bool operator>(base_iterator k_other) const {
      return (position_x_ > k_other.position_x_) ||
             ((position_x_ == k_other.position_x_) &&
              (position_y_ > k_other.position_y_));
    }

    bool operator<(base_iterator k_other) const {
      return (position_x_ < k_other.position_x_) ||
             ((position_x_ == k_other.position_x_) &&
              (position_y_ < k_other.position_y_));
    }

    bool operator<=(base_iterator k_other) const { return !(*this > k_other); }

    bool operator>=(base_iterator k_other) const { return !(*this < k_other); }

   private:
    size_t position_x_;
    size_t position_y_;
    pointer* external_;
  };

  using iterator = base_iterator<false>;
  using const_iterator = base_iterator<true>;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  iterator begin() {
    return iterator(begin_x_ + (begin_y_ + 1) / kKeight,
                    (begin_y_ + 1) % kKeight, externall_);
  }

  const_iterator begin() const {
    return const_iterator(begin_x_ + (begin_y_ + 1) / kKeight,
                          (begin_y_ + 1) % kKeight,
                          const_cast<const T**>(externall_));
  }

  const_iterator cbegin() const {
    return const_iterator(begin_x_ + (begin_y_ + 1) / kKeight,
                          (begin_y_ + 1) % kKeight,
                          const_cast<const T**>(externall_));
  }

  reverse_iterator rbegin() { return reverse_iterator(end()); }

  const_reverse_iterator rbegin() const {
    return const_reverse_iterator(end());
  }

  const_reverse_iterator crbegin() const {
    return const_reverse_iterator(end());
  }

  iterator end() { return iterator(end_x_, end_y_, externall_); }

  const_iterator end() const {
    return const_iterator(end_x_, end_y_, const_cast<const T**>(externall_));
  }

  const_iterator cend() const {
    return const_iterator(end_x_, end_y_, const_cast<const T**>(externall_));
  }

  reverse_iterator rend() { return reverse_iterator(begin()); }

  const_reverse_iterator rend() const {
    return const_reverse_iterator(begin());
  }

  const_reverse_iterator crend() const {
    return const_reverse_iterator(begin());
  }

  void insert(iterator pos, const T& value) { emplace(pos, value); }

  template <typename... Args>
  void emplace(iterator pos, Args&&... arguments) {
    if (pos == end()) {
      push_back(std::forward<Args>(arguments)...);
      return;
    }
    T tmp = *(--end());
    for (auto it = end() - 1; it != pos; --it) {
      *it = *(it - 1);
    }
    *pos = T(std::forward<Args>(arguments)...);
    push_back(tmp);
  }

  void erase(iterator pos) {
    for (auto it = pos; it != end() - 1; ++it) {
      *it = *(it + 1);
    }
    pop_back();
  }

 private:
  size_t need_;
  static const size_t kKArrL = 8;
  T** externall_;
  size_t current_size_ = 0;
  size_t begin_x_ = 0;
  size_t begin_y_ = 0;
  size_t end_x_ = 0;
  size_t end_y_ = 0;
  Allocator int_alloc_ = {};
  external_alloc ex_alloc_ = {};
  int need() { return need_; }

  void allocate(T** externall, size_t begin, size_t end, size_t& allocated) {
    for (size_t i = begin; i < end; ++i) {
      externall[i] = internal_alloc_traits::allocate(int_alloc_, kKArrL);
      allocated++;
    }
  }

  void deallocate(T** external, size_t begin, size_t end) {
    for (size_t i = begin; i < end; ++i) {
      internal_alloc_traits ::deallocate(int_alloc_, external[i], kKArrL);
    }
  }

  void deallocate_w(T** externall, size_t allocated) {
    while (allocated > 0) {
      internal_alloc_traits ::deallocate(int_alloc_, externall[allocated - 1],
                                         kKArrL);
      allocated--;
    }
  }

  void medium_set(T** external, size_t begin) {
    for (size_t i = begin; i < 2 * begin; ++i) {
      external[i] = externall_[i - begin];
    }
  }

  void deck_swap(Deque& other) {
    std::swap(need_, other.need_);
    std::swap(externall_, other.externall_);
    std::swap(begin_x_, other.begin_x_);
    std::swap(end_x_, other.end_x_);
    std::swap(begin_y_, other.begin_y_);
    std::swap(end_y_, other.end_y_);
    std::swap(current_size_, other.current_size_);
  }
};

