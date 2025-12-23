#ifndef PARSERLIB_LEFT_RECURSION_STATUS_HPP
#define PARSERLIB_LEFT_RECURSION_STATUS_HPP


namespace parserlib {


    /**
     * Left recursion status.
     */
    enum class left_recursion_status {
        /** No left recursion detected yet. */
        no_left_recursion,

        /** Reject the left recursion to allow non-left recursive parts to parse. */
        reject_left_recursion,

        /** Accept the left recursion without parsing to allow parsing the continuation of the left recursion. */
        accept_left_recursion
    };


} //namespace parserlib


#endif //PARSERLIB_LEFT_RECURSION_STATUS_HPP

