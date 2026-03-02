#ifndef PARSERLIB_PARSE_NODE_HPP
#define PARSERLIB_PARSE_NODE_HPP


#include <string>


namespace parserlib {


    template <class ParseContext>
    class parse_node {
    public:
        const std::string& get_name() const {
            return m_name;
        }

        void set_name(const std::string& name) {
            m_name = name;
        }

        virtual bool parse(ParseContext& pc) const = 0;

    protected:
        virtual ~parse_node() {
        }

    private:
        std::string m_name;
    };


} //namespace parserlib


#endif //PARSERLIB_PARSE_NODE_HPP
