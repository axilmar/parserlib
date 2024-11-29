#ifndef PARSERLIB_COMPARE_HPP
#define PARSERLIB_COMPARE_HPP


#include <type_traits>


namespace parserlib {


    /*
     * Comparison functions that are required to remove those signed/unsigned mismatches.
     * They cast the smaller value into the larger one.
     */


    template <typename Left, typename Right> 
    bool compare_equal(const Left& left, const Right& right) {
        if constexpr (std::is_integral_v<Left> && std::is_integral_v<Right>) {
            if constexpr (sizeof(Left) >= sizeof(Right)) {
                return left == (Left)right;
            }
            else {
                return (Right)left == right;
            }
        }
        else {
            return left == right;
        }
    }


    template <typename Left, typename Right>
    bool compare_less_than(const Left& left, const Right& right) {
        if constexpr (std::is_integral_v<Left> && std::is_integral_v<Right>) {
            if constexpr (sizeof(Left) >= sizeof(Right)) {
                return left < (Left)right;
            }
            else {
                return (Right)left < right;
            }
        }
        else {
            return left < right;
        }
    }


    template <typename Left, typename Right>
    bool compare_greater_than(const Left& left, const Right& right) {
        if constexpr (std::is_integral_v<Left> && std::is_integral_v<Right>) {
            if constexpr (sizeof(Left) >= sizeof(Right)) {
                return left > (Left)right;
            }
            else {
                return (Right)left > right;
            }
        }
        else {
            return left > right;
        }
    }


    template <typename Left, typename Right>
    bool compare_less_than_or_equal_to(const Left& left, const Right& right) {
        if constexpr (std::is_integral_v<Left> && std::is_integral_v<Right>) {
            if constexpr (sizeof(Left) >= sizeof(Right)) {
                return left <= (Left)right;
            }
            else {
                return (Right)left <= right;
            }
        }
        else {
            return left <= right;
        }
    }


    template <typename Left, typename Right>
    bool compare_greater_than_or_equal_to(const Left& left, const Right& right) {
        if constexpr (std::is_integral_v<Left> && std::is_integral_v<Right>) {
            if constexpr (sizeof(Left) >= sizeof(Right)) {
                return left >= (Left)right;
            }
            else {
                return (Right)left >= right;
            }
        }
        else {
            return left >= right;
        }
    }


    class less_than {
    public:
        template <typename Left, typename Right>
        bool operator ()(const Left& left, const Right& right) const {
            return compare_less_than(left, right);
        }
    };


    class greater_than {
    public:
        template <typename Left, typename Right>
        bool operator ()(const Left& left, const Right& right) const {
            return compare_greater_than(left, right);
        }
    };


    class less_than_or_equal_to {
    public:
        template <typename Left, typename Right>
        bool operator ()(const Left& left, const Right& right) const {
            return compare_less_than_or_equal_to(left, right);
        }
    };


    class greater_than_or_equal_to {
    public:
        template <typename Left, typename Right>
        bool operator ()(const Left& left, const Right& right) const {
            return compare_greater_than_or_equal_to(left, right);
        }
    };


} //namespace parserlib


#endif //PARSERLIB_COMPARE_HPP
