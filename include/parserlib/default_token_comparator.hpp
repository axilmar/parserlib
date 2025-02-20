#ifndef PARSERLIB_DEFAULT_TOKEN_COMPARATOR_HPP
#define PARSERLIB_DEFAULT_TOKEN_COMPARATOR_HPP


namespace parserlib {


    class default_token_comparator {
    public:
        template <class L, class R>
        auto operator ()(const L& l, const R& r) const noexcept {
            return l - r;
        }
    };


} //namespace parserlib


#endif //PARSERLIB_DEFAULT_TOKEN_COMPARATOR_HPP
