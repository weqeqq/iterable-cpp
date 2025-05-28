
#include <type_traits>
#include <iterator>
#include <cstdint>

template <typename D>
class Iterable {
public:
  using Derived = D;

  template <typename S>
  class Iterator {
  public:
    using Self = S;

    using iterator_category = std::random_access_iterator_tag;
    using difference_type   = std::ptrdiff_t;
    using value_type        = std::decay_t<decltype(
      std::declval<Derived>().operator[](0)
    )>;
    using pointer   = value_type*;
    using reference = value_type&;

    explicit Iterator(
      Self &self, std::uint64_t current

    ) : self_(self), current_(current) {}

    decltype(auto) operator*() {
      return self_[current_];
    }
    Iterator &operator++() {
      current_++; return *this;
    }
    Iterator operator++(int) {
      auto temp = *this; operator++();
      return temp;
    }
    Iterator &operator--() {
      current_--; return *this;
    }
    Iterator operator--(int) {
      auto temp = *this; operator--();
      return temp;
    }
    Iterator operator+(std::ptrdiff_t value) {
      return Iterator(self_, current_ + value);
    }
    Iterator operator-(std::ptrdiff_t value) {
      return Iterator(self_, current_ - value);
    }
    Iterator &operator+=(std::ptrdiff_t value) {
      current_ += value; return *this;
    }
    Iterator &operator-=(std::ptrdiff_t value) {
      current_ -= value; return *this;
    }
    decltype(auto) operator[](std::ptrdiff_t index) {
      return self_[current_ + index];
    }
    bool operator==(const Iterator &other) const {
      return current_ == other.current_;
    }
    bool operator!=(const Iterator &other) const {
      return !operator==(other);
    }
    bool operator>(const Iterator &other) const {
      return current_ > other.current_;
    }
    bool operator<(const Iterator &other) const {
      return current_ < other.current_;
    }
    bool operator>=(const Iterator &other) const {
      return !operator<(other);
    }
    bool operator<=(const Iterator &other) const {
      return !operator>(other);
    }
  private:
    Self &self_; std::uint64_t current_;
  };

  auto begin() {
    return Iterator<Derived>(Cast(), 0);
  }
  auto begin() const {
    return Iterator<Derived>(Cast(), 0);
  }
  auto cbegin() const {
    return begin();
  }
  auto end() {
    return Iterator<Derived>(Cast(), Cast().GetLength());
  }
  auto end() const {
    return Iterator<Derived>(Cast(), Cast().GetLength());
  }
  auto cend() const {
    return end();
  }

private:
  Derived &Cast() {
    return *static_cast<Derived *>(this);
  }
  const Derived &Cast() const {
    return *static_cast<const Derived *>(this);
  }
};
