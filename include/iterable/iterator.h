/**
 * @file
 * @brief Provides a flexible iterator adapter for custom container types.
 */
#pragma once 

#include <iterator>
#include <memory>
#include <type_traits>
#include <iterable/define.h>
#include <utility>

namespace Iterable {
/**
 * @brief Enumeration for iterator category tags.
 *
 * Allows compile-time selection of iterator capabilities:
 * - `Default`: Standard random access iterator
 * - `Contiguous`: Contiguous memory iterator (C++20 only)
 */
enum Tag {
  Default,    ///< Standard random access iterator
  Contiguous, ///< Contiguous memory iterator (requires C++20)
};
namespace Detail {

// Conditional iterator tag selection (C++20 activates contiguous support)
#if ITERABLE_CPP_20
template <Tag T>
using ConvertTag = std::conditional_t<T == Tag::Default, 
  std::random_access_iterator_tag, 
  std::contiguous_iterator_tag>;
#else
template <Tag T>
using ConvertTag = std::random_access_iterator_tag;
#endif

// Deduce return type of container's operator[]
template <typename D>
using OperatorReturn = decltype(std::declval<D>()[0]);

// Remove references/cv-qualifiers from element type
template <typename D>
using Stored = std::decay_t<OperatorReturn<D>>;

// Preserve constness when container is const
template <typename D>
using HandledReturn = std::conditional_t<std::is_const_v<D>,
  const Stored<D>, Stored<D>>; 

/**
 * @brief Provides standard iterator type aliases.
 * 
 * @tparam D Container type
 * @tparam T Iterable::Tag category
 */
template <typename D, Tag T>
struct IteratorCompat {
  using iterator_category = ConvertTag<T>;  ///< Iterator category tag
  using difference_type   = std::ptrdiff_t; ///< Difference type
  using value_type        = Stored<D>;      ///< Value type (decayed)
  using pointer           = Stored<D>*;     ///< Pointer type
  using reference         = Stored<D>&;     ///< Reference type
};

/**
 * @brief Core implementation of iterator functionality.
 *
 * Implements standard iterator operations and provides CRTP base.
 * 
 * @tparam D Container type
 * @tparam I Concrete iterator type (CRTP)
 */
template <typename D, typename I>
class IteratorCore {
 public:
  /// Default constructor
  IteratorCore() = default;

  /**
   * @brief Construct with container pointer and index.
   * 
   * @param data Pointer to container
   * @param current Starting index
   */
  IteratorCore(D *data, int current) 
    : data_(data), current_(current) {}

  /// Copy constructor
  IteratorCore(const IteratorCore &other) = default;
  /// Move constructor
  IteratorCore(IteratorCore &&other) = default;

  /// Copy assignment
  IteratorCore &operator=(const IteratorCore &other) noexcept = default;
  /// Move assignment
  IteratorCore &operator=(IteratorCore &&other) noexcept = default;

  /// Prefix increment
  I &operator++() noexcept {
    current_++; 
    return Reference();
  }

  /// Postfix increment
  I operator++(int) noexcept {
    auto temp = *this;
    current_++; 
    return temp.Reference();
  }

  // Comparison operators
  bool operator==(const IteratorCore &other) const noexcept {
    return current_ == other.current_;
  }
  bool operator!=(const IteratorCore &other) const noexcept {
    return current_ != other.current_;
  }
  bool operator<(const IteratorCore &other) const noexcept {
    return current_ < other.current_;
  }
  bool operator>(const IteratorCore &other) const noexcept {
    return current_ > other.current_;
  }
  bool operator<=(const IteratorCore &other) const noexcept {
    return current_ <= other.current_;
  }
  bool operator>=(const IteratorCore &other) const noexcept {
    return current_ >= other.current_;
  }

  /// Dereference operator
  HandledReturn<D> &operator*() const noexcept {
    return (*data_)[current_];
  }

  /// Member access operator
  HandledReturn<D> *operator->() const noexcept {
    return std::addressof(operator*());
  }

  /// Prefix decrement
  I &operator--() noexcept {
    current_--; 
    return this->Reference();
  }

  /// Postfix decrement
  I operator--(int) noexcept {
    auto temp = *this;
    current_--;
    return temp.Reference();
  }

  /// Addition operator (iterator + n)
  I operator+(std::ptrdiff_t n) const noexcept {
    return I(this->data_, this->current_ + n);
  }

  /// Subtraction operator (iterator - n)
  I operator-(std::ptrdiff_t n) const noexcept {
    return I(this->data_, this->current_ - n);
  }

  /// Difference between iterators
  std::ptrdiff_t operator-(const IteratorCore &other) const noexcept {
    return this->current_ - other.current_;
  }

  /// Compound addition assignment
  I &operator+=(std::ptrdiff_t n) noexcept {
    this->current_ += n;
    return this->Reference();
  }

  /// Compound subtraction assignment
  I &operator-=(std::ptrdiff_t n) noexcept {
    this->current_ -= n;
    return this->Reference();
  }

  /// Subscript operator
  HandledReturn<D> &operator[](std::ptrdiff_t n) const noexcept {
    return *(*this + n);
  }

 private:
  D *data_ = nullptr;    ///< Pointer to underlying container
  int current_ = 0;      ///< Current position index

  // CRTP helpers
  I *Pointer() noexcept {
    return static_cast<I *>(this);
  }
  const I *Pointer() const noexcept {
    return static_cast<const I *>(this);
  } 
  I &Reference() noexcept {
    return *Pointer();
  }
  const I &Reference() const noexcept {
    return *Pointer();
  }
};

/// Global operator for (n + iterator)
template <typename D, typename I>
I operator+(std::ptrdiff_t n, const IteratorCore<D, I> &i) {
  return i + n;
}

#if ITERABLE_CPP_20
/**
 * @brief C++20 specialization for contiguous iterators.
 *
 * Adds contiguous iterator requirements and conversion to raw pointer.
 * 
 * @tparam D Container type
 * @tparam I Concrete iterator type
 */
template <typename D, typename I>
struct ContiguousImpl : IteratorCore<D, I> {
  using IteratorCore<D, I>::IteratorCore;

  /// Conversion to raw pointer
  operator Detail::HandledReturn<D> *() noexcept {
    return this->operator->();
  }

  /// Obtain address of current element
  friend Detail::HandledReturn<D> *to_address(const ContiguousImpl &iterator) noexcept {
    return iterator;
  }
};
#endif

// Select implementation based on tag
template <typename D, typename I, Tag T>
struct SelectImplS {
  using Type = IteratorCore<D, I>; ///< Default implementation
};

#if ITERABLE_CPP_20
template <typename D, typename I>
struct SelectImplS<D, I, Tag::Contiguous> {
  using Type = ContiguousImpl<D, I>; ///< C++20 contiguous implementation
};
#endif

/// Alias for selected implementation type
template <typename D, typename I, Tag T>
using SelectImpl = typename SelectImplS<D, I, T>::Type;

} // namespace Detail

/**
 * @brief Public iterator interface for custom containers.
 *
 * Provides standard-compliant iterator using CRTP. Supports:
 * - Random access (default)
 * - Contiguous memory (C++20)
 *
 * @tparam D Container type (must implement `operator[]`)
 * @tparam T Iterator category tag (default: Tag::Default)
 */
template <typename D, Tag T = Default>
struct Iterator 
: Detail::SelectImpl<D, Iterator<D, T>, T>, Detail::IteratorCompat<D, T> {
  using Impl = Detail::SelectImpl<D, Iterator, T>; ///< Selected implementation
  using Impl::Impl; ///< Inherit constructors
};
} // namespace Iterable
