#ifndef PARSERLIB_ANNOTATION_PARSE_NODE_HPP
#define PARSERLIB_ANNOTATION_PARSE_NODE_HPP


#include <type_traits>
#include "parse_node.hpp"


namespace parserlib {


    template <class, class, class, class = void> 
    struct has_method_parse_annotation
        : std::false_type
    {
    };


    template <class ParseContext, class ParseNode, class Annotation>
    struct has_method_parse_annotation<
        ParseContext,
        ParseNode,
        Annotation,
        std::void_t<decltype(std::declval<ParseContext>().parse_annotation(std::declval<ParseContext&>(), std::declval<ParseNode>(), std::declval<Annotation>()))>
    >
        : std::true_type
    {
    };


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
            #ifndef NDEBUG
            , m_text(create_text())
            #endif
        {
        }

        /**
         * Tells the given parse context to parse this annotation.
         * @param pc the parse context to parse this annotation.
         * @return true on success, false on failure.
         */
        template <class ParseContext>
        bool parse(ParseContext& pc) const {
            if constexpr (has_method_parse_annotation<ParseContext, ParseNode, Annotation>::value) {
                return pc.parse_annotation(pc, m_parse_node, m_annotation);
            }
            else {
                return m_parse_node.parse(pc);
            }
        }

        #ifndef NDEBUG
        const std::string& text() const {
            return m_text;
        }
        #endif

    private:
        const ParseNode m_parse_node;
        const Annotation m_annotation;
        #ifndef NDEBUG
        const std::string m_text;
        #endif

        #ifndef NDEBUG
        std::string create_text() const {
            std::stringstream stream;
            stream << "annotation(" << m_parse_node.text() << ", " << m_annotation << ")";
            return stream.str();
        }
        #endif
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
