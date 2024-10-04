#ifndef PARSERLIB_CORE_LEFTRECURSION_HPP
#define PARSERLIB_CORE_LEFTRECURSION_HPP


namespace parserlib::core::LeftRecursion {


    /**
     * Left recursion rule state.
     */
    enum class State {
        /**
         * In possible left recursion.
         */
        Init,

        /**
         * Left recursion found; parsing is rejected.
         */
        Reject,

        /**
         * Left recursion resolved; continue parsing.
         */
        Accept
    };


    /**
     * Exception thrown for the init state, when a left recursion is first recognized.
     */
    class Init {
    public:
    };


    /**
     * Exception thrown for the reject state.
     */
    class Reject {
    public:
    };


    /**
     * Exception thrown for the accept phase.
     */
    class Accept {
    public:
    };


} //namespace parserlib::core::LeftRecursion


#endif //PARSERLIB_CORE_LEFTRECURSION_HPP
