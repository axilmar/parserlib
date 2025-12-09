#ifndef PARSERLIB_VECTOR_CONCATENATION_HPP
#define PARSERLIB_VECTOR_CONCATENATION_HPP


#include <vector>
#include <iterator>


namespace parserlib {


    template <class T, class Alloc1, class Alloc2>
    std::vector<T> concatenate_vectors(const std::vector<T, Alloc1>& vec, const std::vector<T, Alloc2>& vec2) {
        std::vector<T> result;
        result.insert(result.end(), vec.begin(), vec.end());
        result.insert(result.end(), vec2.begin(), vec2.end());
        return result;
    }


    template <class T, class Alloc1, class Alloc2>
    std::vector<T> concatenate_vectors(std::vector<T, Alloc1>&& vec, std::vector<T, Alloc2>&& vec2) {
        std::vector<T> result;
        result.insert(result.end(), std::make_move_iterator(vec.begin()), std::make_move_iterator(vec.end()));
        result.insert(result.end(), std::make_move_iterator(vec2.begin()), std::make_move_iterator(vec2.end()));
        return result;
    }


    template <class T, class Alloc>
    std::vector<T> concatenate_vector_with_element(const std::vector<T, Alloc>& vec, const T& elem) {
        std::vector<T> result;
        result.insert(result.end(), vec.begin(), vec.end());
        result.push_back(elem);
        return result;
    }


    template <class T, class Alloc>
    std::vector<T> concatenate_vector_with_element(std::vector<T, Alloc>&& vec, T&& elem) {
        std::vector<T> result;
        result.insert(result.end(), std::make_move_iterator(vec.begin()), std::make_move_iterator(vec.end()));
        result.push_back(std::forward<T>(elem));
        return result;
    }


    template <class T, class Alloc>
    std::vector<T> concatenate_element_with_vector(const T& elem, const std::vector<T, Alloc>& vec) {
        std::vector<T> result;
        result.push_back(elem);
        result.insert(result.end(), vec.begin(), vec.end());
        return result;
    }


    template <class T, class Alloc>
    std::vector<T> concatenate_element_with_vector(T&& elem, std::vector<T, Alloc>&& vec) {
        std::vector<T> result;
        result.push_back(std::forward<T>(elem));
        result.insert(result.end(), std::make_move_iterator(vec.begin()), std::make_move_iterator(vec.end()));
        return result;
    }


} //namespace parserlib


#endif //PARSERLIB_VECTOR_CONCATENATION_HPP

