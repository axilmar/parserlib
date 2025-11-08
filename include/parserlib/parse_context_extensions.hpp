#ifndef PARSERLIB_PARSE_CONTEXT_EXTENSIONS_HPP
#define PARSERLIB_PARSE_CONTEXT_EXTENSIONS_HPP


#include <ostream>
#include <iostream>


namespace parserlib {


    /**
     * Trait class for obtaining the default output stream for the given output stream type.
     * @param OutputStream output stream to get the default value of.
     */
    template <class OutputStream> struct default_output_stream;


    /**
     * Specialization for std::ostream.
     * It returns std::cout as the default value.
     */
    template <> struct default_output_stream<std::ostream> {
        static std::ostream& get() {
            return std::cout;
        }
    };


    /**
     * Specialization for std::wostream.
     * It returns std::wcout as the default value.
     */
    template <> struct default_output_stream<std::wostream> {
        static std::wostream& get() {
            return std::wcout;
        }
    };


    /**
     * A debug annotations extension that is empty.
     */
    class default_debug_annotations_extension {
    public:
        /**
         * Returns the output stream associated with this context.
         * @return nullptr.
         */
        void* output_stream() const {
            return nullptr;
        }

        /**
         * Sets the output stream associated with this context.
         * Does nothing.
         * @param stream the stream to set.
         */
        void set_output_stream(void* stream) {
        }

        /**
         * Returns the indentation size.
         * It returns 0.
         * @return the indentation size.
         */
        size_t indentation_size() const {
            return 0;
        }

        /**
         * Sets the indentation size.
         * Does nothing.
         * @param size number of characters for indentation.
         */
        void set_indentation_size(size_t size) {
        }

        /**
         * Parses an annotation.
         * It only calls the `parse(pc)` method of the given parse node.
         * @param pc the current parse context.
         * @param parse_node the parse node to invoke for parsing.
         * @param annotation the annotation to write to the output stream.
         * @return the parsing result.
         */
        template <class ParseContext, class ParseNode, class Annotation>
        bool parse_annotation(ParseContext& pc, const ParseNode& parse_node, const Annotation& annotation) {
            return parse_node.parse(pc);
        }
    };


    /**
     * The default debug annotations extension type.
     */
    using default_debug_annotations_extension_type = default_debug_annotations_extension;


    /**
     * A parse context extension that allows 
     * printing of annotations to an output stream,
     * while parsing.
     * 
     * @param OutputStream type of output stream to put the information on;
     *  by default, it is std::cout, for std::ostream, and std::wcout for std::wostream.
     */
    template <class OutputStream = std::ostream>
    class debug_annotations_extension {
    public:
        /** Output stream type. */
        using output_stream_type = std::ostream;

        /**
         * Returns the output stream associated with this context.
         * By default, debug information is written to std::cout.
         * @return the output stream associated with this context.
         */
        output_stream_type* output_stream() const {
            return m_output_stream;
        }

        /**
         * Sets the output stream associated with this context.
         * @param stream the stream to set.
         */
        void set_output_stream(output_stream_type* stream) {
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
         * @param pc the current parse context.
         * @param parse_node the parse node to invoke for parsing.
         * @param annotation the annotation to write to the output stream.
         * @return the parsing result.
         */
        template <class ParseContext, class ParseNode, class Annotation>
        bool parse_annotation(ParseContext& pc, const ParseNode& parse_node, const Annotation& annotation) {
            if (m_output_stream) {
                *m_output_stream << std::string(m_indentation_level * m_indentation_size, ' ') << "Parsing " << annotation << " at " << pc.parse_position().to_string(pc.begin_iterator()) << std::endl;
                ++m_indentation_level;
            }
            const bool result = parse_node.parse(pc);
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
        output_stream_type* m_output_stream{ &default_output_stream<output_stream_type>::get() };
        size_t m_indentation_level{ 0 };
        size_t m_indentation_size{ 4 };
    };


} //namespace parserlib


#endif //PARSERLIB_PARSE_CONTEXT_EXTENSIONS_HPP
