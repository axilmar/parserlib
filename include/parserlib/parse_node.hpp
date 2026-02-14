#ifndef PARSERLIB_PARSE_NODE_HPP
#define PARSERLIB_PARSE_NODE_HPP


#include <string>


namespace parserlib {


    /**
     * Generic base class for parse nodes.
     */ 
    class parse_node {
    public:
        /**
         * The destructor.
         * Virtual due to polymorphism.
         */ 
        virtual ~parse_node() {
        }

        /**
            * Returns the name of this parse node.
            * @return the name of this parse node.
            */ 
        const std::string& get_name() const {
            return m_name;
        }

        /**
            * Sets the name of this parse node.
            * @param name the name of this parse node.
            */  
        void set_name(const std::string& name) {
            m_name = name;
        }

    private:
        std::string m_name;
    };


} //namespace parserlib


#endif //PARSERLIB_PARSE_NODE_HPP
