#ifndef PARSERLIB_DEBUG_PARSE_CONTEXT_HPP
#define PARSERLIB_DEBUG_PARSE_CONTEXT_HPP


#include <ostream>
#include <iostream>
#include "parse_context.hpp"


namespace parserlib {


    /**
     * An enhanced parse context class that allows printing of annotations.
     * Provided for debugging purposes.
     *
     * @param Source the source type; it can be any STL-like container.
     * @param MatchId type of id for matches; it can be any custom enumeration or any other type.
     * @param ErrorId type of id for errors; it can be any custom enumeration or any other type.
     * @param TextPosition position in text; it is optionally used for providing line and column information.
     * @param SymbolComparator class used for comparing symbols; it can be used for case-insensitive comparisons.
     */
    template <
        class Source = std::string,
        class MatchId = int,
        class ErrorId = int,
        class TextPosition = default_text_position,
        class SymbolComparator = default_symbol_comparator
    >
    class debug_parse_context : public parse_context<Source, MatchId, ErrorId, TextPosition, SymbolComparator> {
    public:
        /** Base parse context type. */
        using parse_context_type = parse_context<Source, MatchId, ErrorId, TextPosition, SymbolComparator>;

        /** Reusing constructors from base class. */
        using parse_context_type::parse_context;

        /** 
         * Returns the output stream associated with this context.
         * By default, debug information is written to std::cout.
         * @return the output stream associated with this context.
         */
        std::ostream* output_stream() const {
            return m_output_stream;
        }

        /**
         * Sets the output stream associated with this context.
         * @param stream the stream to set.
         */
        void set_output_stream(std::ostream* stream) {
            m_output_stream = stream;
        }

        /**
         * Returns the indentation size.
         * By default, it returns 4 (characters).
         * @return the indentation size.
         */
        size_t indentation_size() const {
            return m_indentation_size;
        }

        /**
         * Sets the indentation size.
         * @param size number of characters for indentation.
         */
        void set_indentation_size(size_t size) {
            m_indentation_size = size;
        }

        /**
         * Parses an annotation.
         * If an output stream is specified, then it outputs parsing information
         * before and after calling the given parse node to parse.
         * @param parse_node the parse node to invoke for parsing.
         * @param annotation the annotation to write to the output stream.
         * @return the parsing result.
         */
        template <class ParseNode, class Annotation>
        bool parse_annotation(const ParseNode& parse_node, const Annotation& annotation) {
            if (m_output_stream) {
                *m_output_stream << std::string(m_indentation_level * m_indentation_size, ' ') << "Parsing " << annotation << " at " << this->parse_position().to_string(this->begin_iterator()) << std::endl;
                ++m_indentation_level;
            }
            const bool result = parse_node.parse(*this);
            if (m_output_stream) {
                --m_indentation_level;
                if (result) {
                    *m_output_stream << std::string(m_indentation_level * m_indentation_size, ' ') << "Successfully parsed " << annotation << std::endl;
                }
                else {
                    *m_output_stream << std::string(m_indentation_level * m_indentation_size, ' ') << "Failed to parse " << annotation << std::endl;
                }
            }
            return result;
        }

    private:
        std::ostream* m_output_stream{ &std::cout };
        size_t m_indentation_level{ 0 };
        size_t m_indentation_size{ 4 };
    };


} //namespace parserlib


#endif //PARSERLIB_DEBUG_PARSE_CONTEXT_HPP
