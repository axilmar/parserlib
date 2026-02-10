#ifndef PARSERLIB_VECTOR_HPP
#define PARSERLIB_VECTOR_HPP


#include <vector>


namespace parserlib {


    template <class T> 
    std::vector<T> vector_cat(const std::vector<T>& v1, const std::vector<T>& v2) {
        std::vector<T> result;
        result.insert(result.end(), v1.begin(), v1.end());
        result.insert(result.end(), v2.begin(), v2.end());
        return result;
    }


    template <class T> 
    std::vector<T> vector_cat(const std::vector<T>& v1, const T& v2) {
        std::vector<T> result;
        result.insert(result.end(), v1.begin(), v1.end());
        result.insert(result.end(), v2);
        return result;
    }


    template <class T> 
    std::vector<T> vector_cat(const T& v1, const std::vector<T>& v2) {
        std::vector<T> result;
        result.insert(result.end(), v1);
        result.insert(result.end(), v2.begin(), v2.end());
        return result;
    }


} //namespace parserlib


#endif //PARSERLIB_VECTOR_HPP
