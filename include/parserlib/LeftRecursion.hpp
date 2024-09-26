#ifndef PARSERLIB_LEFTRECURSION_HPP
#define PARSERLIB_LEFTRECURSION_HPP


namespace parserlib::LeftRecursion {


    enum class State {
        Init,
        Parsing,
        LeftRecursive,
        Reject,
        Accept
    };


    class Init {
    public:
    };


    class Reject {
    public:
    };


    class Accept {
    public:
    };


} //namespace parserlib::LeftRecursion


#endif //PARSERLIB_LEFTRECURSION_HPP
