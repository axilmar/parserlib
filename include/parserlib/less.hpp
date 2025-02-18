#ifndef PARSERLIB_LESS_HPP
#define PARSERLIB_LESS_HPP


namespace parserlib {


    template <class Comparator>
    class less {
    public:
        template <class L, class R>
        bool operator ()(const L& l, const R& r) const noexcept {
            return Comparator()(l, r) < 0;
        }
    };


} //namespace parserlib


#endif //PARSERLIB_LESS_HPP
