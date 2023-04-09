#ifndef PARSERLIB_LEFTRECURSIONACCEPTED_HPP
#define PARSERLIB_LEFTRECURSIONACCEPTED_HPP


namespace parserlib {


    /**
     * Exception used as a goto to indicate that a left recursion was accepted and that
     * parsing can continue normally.
     */
    class LeftRecursionAccepted {
    };


} //namespace parserlib


#endif //PARSERLIB_LEFTRECURSIONACCEPTED_HPP
