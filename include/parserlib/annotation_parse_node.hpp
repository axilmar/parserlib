#ifndef PARSERLIB_ANNOTATION_PARSE_NODE_HPP
#define PARSERLIB_ANNOTATION_PARSE_NODE_HPP


#include "parse_node.hpp"


namespace parserlib {


    /**
     * A parse node that adds an object as an annotation to another parse node.
     * @param ParseNode the type of the parse node to invoke.
     * @param Annotation the annotation object.
     */
    template <class ParseNode, class Annotation>
    class annotation_parse_node : public parse_node<annotation_parse_node<ParseNode, Annotation>> {
    public:
        /**
         * The constructor.
         * @param parse_node the parse node to use for parsing.
         * @param annotation the annotation object.
         */
        annotation_parse_node(const ParseNode& parse_node, const Annotation& annotation)
            : m_parse_node(parse_node)
            , m_annotation(annotation)
        {
        }

        /**
         * Tells the given parse context to parse this annotation.
         * @param pc the parse context to parse this annotation.
         * @return true on success, false on failure.
         */
        template <class ParseContext>
        bool parse(ParseContext& pc) const {
            return pc.parse_annotation(m_parse_node, m_annotation);
        }

    private:
        const ParseNode m_parse_node;
        const Annotation m_annotation;
    };


    template <class T, class Annotation>
    auto make_annotation_parse_node(T&& object, const Annotation& annotation) {
        return annotation_parse_node(make_parse_node(object), annotation);
    }


    template <class T, class Annotation>
    auto make_annotation_parse_node(T&& object, const Annotation* annotation) {
        return annotation_parse_node(make_parse_node(object), std::basic_string(annotation));
    }


} //namespace parserlib


#endif //PARSERLIB_ANNOTATION_PARSE_NODE_HPP
