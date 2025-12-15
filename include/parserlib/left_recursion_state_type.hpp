#ifndef PARSERLIB_LEFT_RECURSION_STATE_TYPE_HPP
#define PARSERLIB_LEFT_RECURSION_STATE_TYPE_HPP


namespace parserlib {


    /**
     * State types for left recursion.
     */
    enum class left_recursion_state_type {
        /** No left recursion detected. */
        no_left_recursion,

        /** Left recursion must be rejected. */
        reject_left_recursion,

        /** Left recursion must be accepted. */
        accept_left_recursion,

        /** Left recursion accepted. */
        accepted_left_recursion
    };


} //namespace parserlib


#endif //PARSERLIB_LEFT_RECURSION_STATE_TYPE_HPP
