#ifndef PARSERLIB_DEFAULT_SOURCE_POSITION_HPP
#define PARSERLIB_DEFAULT_SOURCE_POSITION_HPP


#include <cstddef>


namespace parserlib {


    /**
     * An empty source position.
     */
    class default_source_position {
    public:
        /**
         * Does nothing.
         */
        void increment() {
        }

        /**
         * Does nothing.
         */
        void increment(std::size_t) {
        }

        /**
         * Does nothing.
         */
        void increment_line() {
        }
    };


} //namespace parserlib


#endif //PARSERLIB_DEFAULT_SOURCE_POSITION_HPP
