#ifndef PARSERLIB_NON_CONVERTING_SYMBOL_COMPARATOR_HPP
#define PARSERLIB_NON_CONVERTING_SYMBOL_COMPARATOR_HPP


namespace parserlib {


    class non_converting_symbol_comparator {
    public:
        template <class A, class B>
        int operator ()(const A& a, const B& b) const {
            return static_cast<int>(a) - static_cast<int>(b);
        }
    };


} //namespace parserlib


#endif //PARSERLIB_NON_CONVERTING_SYMBOL_COMPARATOR_HPP
