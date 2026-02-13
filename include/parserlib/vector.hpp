#ifndef PARSERLIB_VECTOR_HPP
#define PARSERLIB_VECTOR_HPP


#include <vector>


namespace parserlib {


    /**
     * Concatenates two vectors.
     * @param v1 the first vector.
     * @param v2 the second vector.
     * @return a vector which contains all the elements.
     */ 
    template <class T> 
    std::vector<T> vector_cat(const std::vector<T>& v1, const std::vector<T>& v2) {
        std::vector<T> result;
        result.insert(result.end(), v1.begin(), v1.end());
        result.insert(result.end(), v2.begin(), v2.end());
        return result;
    }


    /**
     * Concatenates a vector with a value.
     * @param v1 the vector.
     * @param v2 the value.
     * @return a vector which contains all the elements.
     */ 
    template <class T> 
    std::vector<T> vector_cat(const std::vector<T>& v1, const T& v2) {
        std::vector<T> result;
        result.insert(result.end(), v1.begin(), v1.end());
        result.insert(result.end(), v2);
        return result;
    }


    /**
     * Concatenates a value with a vector.
     * @param v1 the value.
     * @param v2 the vector.
     * @return a vector which contains all the elements.
     */ 
    template <class T> 
    std::vector<T> vector_cat(const T& v1, const std::vector<T>& v2) {
        std::vector<T> result;
        result.insert(result.end(), v1);
        result.insert(result.end(), v2.begin(), v2.end());
        return result;
    }


    /**
     * Concatenates two values into a vector.
     * @param v1 the first value.
     * @param v2 the second value.
     * @return a vector which contains all the elements.
     */ 
    template <class T> 
    std::vector<T> vector_cat(const T& v1, const T& v2) {
        return std::vector<T>({v1, v2});
    }


} //namespace parserlib


#endif //PARSERLIB_VECTOR_HPP
