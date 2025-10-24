#ifndef PARSERLIB_ANNOTATION_PARSER_NODE_HPP
#define PARSERLIB_ANNOTATION_PARSER_NODE_HPP


#include <string>
#include "parser_node.hpp"


namespace parserlib {


    template <class Parser> class annotation_parser_node : public parser_node<annotation_parser_node<Parser>> {
    public:
        annotation_parser_node(const Parser& parser, const std::string& annotation) 
            : m_parser(parser), m_annotation(annotation)
        {
        }

        template <class ParseContext> bool parse(ParseContext& pc) const {
            if constexpr (ParseContext::debug_info_enabled) {
                pc.add_debug_info("Started parsing ", m_annotation, " at position = ", pc.parse_position().to_string(pc.end()));
                pc.increase_debug_info_indentation_level();
            }
            const bool result = m_parser.parse(pc);
            if constexpr (ParseContext::debug_info_enabled) {
                pc.decrease_debug_info_indentation_level();
                pc.add_debug_info("Ended parsing ", m_annotation, " with result = ", result ? "true" : "false");
            }
            return result;
        }

    private:
        Parser m_parser;
        std::string m_annotation;
    };


    template <class Parser>
    annotation_parser_node<Parser> parser_node<Parser>::operator [] (const std::string& annotation) const {
        return annotation_parser_node<Parser>(*this_(), annotation);
    }


} //namespace parserlib


#endif //PARSERLIB_ANNOTATION_PARSER_NODE_HPP
