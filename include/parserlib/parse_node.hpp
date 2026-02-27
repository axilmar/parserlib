#ifndef PARSERLIB_PARSE_NODE_HPP
#define PARSERLIB_PARSE_NODE_HPP


namespace parserlib {


    template <class ParseContext>
    class parse_node {
    public:
        virtual bool parse(ParseContext& pc) const = 0;

    protected:
        virtual ~parse_node() {
        }
    };


} //namespace parserlib


#endif //PARSERLIB_PARSE_NODE_HPP
