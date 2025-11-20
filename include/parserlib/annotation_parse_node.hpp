#ifndef PARSERLIB_ANNOTATION_PARSE_NODE_HPP
#define PARSERLIB_ANNOTATION_PARSE_NODE_HPP


#include <type_traits>
#include "parse_node.hpp"


namespace parserlib {


    /**
     * Trait for identifying if a class has the method `parse_annotation`.
     */
    template <class, class, class, class = void> 
    struct has_method_parse_annotation : std::false_type
    {
    };


    /**
     * True if the given method has the `parse_annotation` method.
     * @param ParseContext the parse context type to pass to the method.
     * @param ParseNode the parse node type to pass to the method.
     * @param Annotation the annotation type to pass to the method.
     */
    template <class ParseContext, class ParseNode, class Annotation>
    struct has_method_parse_annotation<ParseContext, ParseNode, Annotation, 
        std::void_t<decltype(std::declval<ParseContext>().parse_annotation(std::declval<ParseContext&>(), std::declval<ParseNode>(), std::declval<Annotation>()))>>
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
                return pc.parse(m_parse_node);
            }
        }

        /**
         * Converts the parse node to a textual description.
         * @return a string of this parse node as text.
         */
        std::string text() const override {
            std::stringstream stream;
            stream << "annotation(" << m_parse_node.text() << ", " << m_annotation << ")";
            return stream.str();
        }

        #ifndef NDEBUG
        void init_tree() const override {
            m_parse_node.init();
        }
        #endif

    private:
        const ParseNode m_parse_node;
        const Annotation m_annotation;
    };


    /**
     * Makes an annotaton parse node.
     * @param parse_node or expression to create an annotation for.
     * @param annotation the annotation object.
     * @return an annotation parse node.
     */
    template <class ParseNode, class Annotation>
    auto make_annotation_parse_node(ParseNode&& parse_node, const Annotation& annotation) {
        return annotation_parse_node(make_parse_node(parse_node), annotation);
    }


    /**
     * Makes an annotaton parse node from a string.
     * @param parse_node or expression to create an annotation for.
     * @param annotation the annotation string.
     * @return an annotation parse node.
     */
    template <class ParseNode, class Annotation>
    auto make_annotation_parse_node(ParseNode&& parse_node, const Annotation* annotation) {
        return annotation_parse_node(make_parse_node(parse_node), std::basic_string(annotation));
    }


} //namespace parserlib


#endif //PARSERLIB_ANNOTATION_PARSE_NODE_HPP
