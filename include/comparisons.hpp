
/// Defines the operators '==' and '<' for aggregates in the namespace att::operators.

#pragma once

#include <core.hpp>

namespace att {

    /// Operator '=='

    /// Check if equality is detected for a type.

    namespace detail {
        namespace impl {
            template <class T>
            using equality_expr = decltype(
                std::declval<T const&>() == std::declval<T const&>()
            );
        }
        template <class T>
        constexpr bool has_equality = is_detected<impl::equality_expr, T>;
    }

    /// Test equality implementation.

    namespace impl {
        
        template <class T>
        bool test_equality(T const& lhs, T const& rhs);

        template <int I, class...Ts>
        bool test_equality_tuple(
                std::tuple<Ts...> const& lhs,
                std::tuple<Ts...> const& rhs,
                detail::value_tag<int, I>)
        {
            if constexpr (I == sizeof...(Ts)) {
                return true;
            }
            else {
                return test_equality(std::get<I>(lhs), std::get<I>(rhs)) &&
                       test_equality_tuple(lhs, rhs, detail::value_tag<int, I + 1>{});
            }
        }
        
        template <class T>
        bool test_equality(T const& lhs, T const& rhs) {
            
            static_assert(detail::has_equality<T> || is_aggregate<T>,
                "T must be an aggregate, or operator==(T const&, T const&) must be implemented");

            if constexpr (detail::has_equality<T>) {
                return lhs == rhs;
            }
            else { // T is aggregate
                return test_equality_tuple(
                    att::as_tuple(lhs),
                    att::as_tuple(rhs),
                    detail::value_tag<int, 0>{});
            }
        }
    }

    /// Public operators.

    namespace operators {
        template <class T>
        bool operator==(T const& lhs, T const& rhs) {
            return impl::test_equality(lhs, rhs);
        }
        template <class T>
        bool operator!=(T const& lhs, T const& rhs) {
            return !impl::test_equality(lhs, rhs);
        }
    }

    /// Operator '<'

    /// Check if inferior operator is detected for a type.

    namespace detail {
        namespace impl {
            template <class T>
            using less_expr = decltype(
                std::declval<T const&>() < std::declval<T const&>()
            );
        }
        template <class T>
        constexpr bool has_less = is_detected<impl::less_expr, T>;
    }

    /// Test less of tuples recursively.

    namespace impl {
        
        template <class T>
        bool test_less(T const& lhs, T const& rhs);

        template <int I, class...Ts>
        bool test_less_tuple(
                std::tuple<Ts...> const& lhs,
                std::tuple<Ts...> const& rhs,
                detail::value_tag<int, I>)
        {
            if constexpr (I == sizeof...(Ts)) {
                return false;
            }
            else {
                auto& lv = std::get<I>(lhs);
                auto& rv = std::get<I>(rhs);
                if (test_less(lv, rv)) return true;
                if (test_less(rv, lv)) return false;
                
                return test_less_tuple(lhs, rhs, detail::value_tag<int, I + 1>{});
            }
        }

        template <class T>
        bool test_less(T const& lhs, T const& rhs) {

            static_assert(detail::has_less<T> || is_aggregate<T>,
                "T must be an aggregate, or operator<(T const&, T const&) must be implemented");

            if constexpr (detail::has_less<T>) {
                return lhs < rhs;
            }
            else { // T is aggregate
                return test_less_tuple(
                    as_tuple(lhs),
                    as_tuple(rhs),
                    detail::value_tag<int, 0>{});
            }
        }
    }

    /// The public operators.

    namespace operators {
        template <class T>
        bool operator<(T const& lhs, T const& rhs) {
            return impl::test_less(lhs, rhs);
        }
        template <class T>
        bool operator<=(T const& lhs, T const& rhs) {
            return !impl::test_less(rhs, lhs);
        }
        template <class T>
        bool operator>(T const& lhs, T const& rhs) {
            return impl::test_less(rhs, lhs);
        }
        template <class T>
        bool operator>=(T const& lhs, T const& rhs) {
            return !impl::test_less(lhs, rhs);
        }
    }

}
