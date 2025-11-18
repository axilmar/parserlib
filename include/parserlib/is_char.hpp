#ifndef PARSERLIB_IS_CHAR_HPP
#define PARSERLIB_IS_CHAR_HPP


#include <cuchar>


namespace parserlib {


    /**
     * Implementation of 'is-character' trait for non-characters.
     * @param T type to check for being a character.
     */
    template <class T> struct is_char {
        /** the value flag; false. */
        static constexpr bool value = false;
    };

    /**
     * Implementation of is-character trait for 'unsigned char'.
     */
    template <> struct is_char<unsigned char> {
        /** the value flag; true. */
        static constexpr bool value = true;
    };


    /**
     * Implementation of is-character trait for 'signed char'.
     */
    template <> struct is_char<signed char> {
        /** the value flag; true. */
        static constexpr bool value = true;
    };


    /**
     * Implementation of is-character trait for 'char'.
     */
    template <> struct is_char<char> {
        /** the value flag; true. */
        static constexpr bool value = true;
    };


    /**
     * Implementation of is-character trait for 'wchar_t'.
     */
    template <> struct is_char<wchar_t> {
        /** the value flag; true. */
        static constexpr bool value = true;
    };


    #ifdef char8_t
    /**
     * Implementation of is-character trait for 'char8_t'.
     */
    template <> struct is_char<char8_t> {
        /** the value flag; true. */
        static constexpr bool value = true;
    };
    #endif


    /**
     * Implementation of is-character trait for 'char16_t'.
     */
    template <> struct is_char<char16_t> {
        /** the value flag; true. */
        static constexpr bool value = true;
    };


    /**
     * Implementation of is-character trait for 'char32_t'.
     */
    template <> struct is_char<char32_t> {
        /** the value flag; true. */
        static constexpr bool value = true;
    };


    /**
     * Helper for accessing the 'is-character' trait value.
     * @param T type of value.
     */
    template <class T>
    constexpr bool is_char_v = is_char<T>::value;


} //namespace parserlib


#endif //PARSERLIB_IS_CHAR_HPP
