#pragma once 

#include <type_traits>

#include <iterable/define.h>
#include <iterable/iterator.h>

namespace Iterable {

/**
 * @brief A CRTP-based template class that provides iteration capabilities.
 * 
 * This class template is designed to be inherited from, enabling the derived
 * class `D` to be iterable using C++ range-based for loops.
 * 
 * Iteration is enabled either through a public member container `data_`, or by
 * using a custom `Iterator` provided in `iterable/iterator.h`.
 * 
 * @tparam D The derived class type inheriting from this template.
 * @tparam T The tag type used to customize the iterator behavior (default: `Default`).
 */
template <typename D, Tag T = Default>
class For {
  /**
   * @brief Helper struct to detect if a type has a public member `data_`.
   * 
   * @tparam C The type to inspect.
   * @tparam Dummy Used for SFINAE.
   */
  template <typename C, typename = std::void_t<>>
  struct HasPubContainerS {
    static constexpr bool Value = false; ///< Indicates `data_` is not present.
  };

  /**
   * @brief Specialization for types that have a `data_` member.
   */
  template <typename C>
  struct HasPubContainerS<C, std::void_t<decltype(std::declval<C>().data_)>> {
    static constexpr bool Value = true; ///< Indicates `data_` is present.
  };

  /// Compile-time flag indicating if the derived class has a public `data_` member.
  static constexpr bool HasPubContainer = HasPubContainerS<D>::Value;

  /// @brief Returns a pointer to the derived class (non-const).
  D *This() {
    return static_cast<D *>(this);
  }

  /// @brief Returns a pointer to the derived class (const).
  const D *This() const {
    return static_cast<const D *>(this);
  }

 public:
  /**
   * @brief Returns an iterator to the beginning of the range (non-const).
   * @return Iterator or container's `begin()` depending on presence of `data_`.
   */
  auto begin() {
    if constexpr (HasPubContainer) {
      return This()->data_.begin();
    } else {
      return Iterator<D, T>(This(), 0);
    }
  }

  /**
   * @brief Returns a const iterator to the beginning of the range.
   * @return Const iterator or container's `begin()` depending on presence of `data_`.
   */
  auto begin() const {
    if constexpr (HasPubContainer) {
      return This()->data_.begin();
    } else {
      return Iterator<const D, T>(This(), 0);
    }
  }

  /**
   * @brief Returns a const iterator to the beginning of the range.
   * Equivalent to `begin()` for const objects.
   */
  auto cbegin() const {
    return begin();
  }

  /**
   * @brief Returns an iterator to the end of the range (non-const).
   * @return Iterator or container's `end()` depending on presence of `data_`.
   */
  auto end() {
    if constexpr (HasPubContainer) {
      return This()->data_.end();
    } else {
      return Iterator<D, T>(This(), This()->Length());
    }
  }

  /**
   * @brief Returns a const iterator to the end of the range.
   * @return Const iterator or container's `end()` depending on presence of `data_`.
   */
  auto end() const {
    if constexpr (HasPubContainer) {
      return This()->data_.end();
    } else {
      return Iterator<const D, T>(This(), This()->Length());
    }
  }

  /**
   * @brief Returns a const iterator to the end of the range.
   * Equivalent to `end()` for const objects.
   */
  auto cend() const {
    return end();
  }
};

};
