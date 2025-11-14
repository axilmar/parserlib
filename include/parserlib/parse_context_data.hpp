#ifndef PARSERLIB_PARSE_CONTEXT_DATA_HPP
#define PARSERLIB_PARSE_CONTEXT_DATA_HPP


#include <cassert>
#include <string>
#include <sstream>
#include <algorithm>
#include <vector>
#include "is_char.hpp"


namespace parserlib {


    /**
     * Trait for getting the part of a content from a content type.
     * It must be overloaded for custom source types in order to retrieve
     * the content from them.
     * @param Source type of source.
     */
    template <class Source> struct content_type {
        /** The source type. */
        using source_type = Source;

        /** The iterator type. */
        using iterator_type = typename source_type::const_iterator;

        /**
         * Returns a copy of the given range.
         * @param begin begin of range.
         * @param end end of range.
         * @return a copy of the given range.
         */
        static Source get(const iterator_type& begin, const iterator_type& end) {
            return { begin, end };
        }
    };


    template <class Source, class MatchId, class TextPosition> class match;


    /**
     * Overload for vector of matches.
     */
    template <class Source, class MatchId, class TextPosition, class Alloc> struct content_type<std::vector<match<Source, MatchId, TextPosition>, Alloc>> {
        /** The source type is the vector of matches. */
        using source_type = std::vector<match<Source, MatchId, TextPosition>, Alloc>;

        /** The iterator type. */
        using iterator_type = typename source_type::const_iterator;

        /**
         * Returns a copy of the source of the given range.
         * @param begin begin of range.
         * @param end end of range.
         * @return a copy of the source of the given range.
         */
        static auto get(const iterator_type& begin, const iterator_type& end) {
            return content_type<Source>::get(begin->begin(), end->begin());
        }
    };


    /**
     * A parse position.
     * It combines an iterator and a text position.
     * @param Source the source type.
     * @param TextPositon text position type.
     */
    template <class Source, class TextPosition>
    class parse_position {
    public:
        /** The source type. */
        using source_type = Source;

        /** Iterator type. */
        using iterator_type = typename Source::const_iterator;

        /** The text position type. */
        using text_position_type = TextPosition;

        /**
         * The constructor.
         * @param it iterator.
         * @param tpos the text position.
         */
        parse_position(const iterator_type& it = iterator_type(), const text_position_type& tpos = text_position_type())
            : m_iterator(it)
            , m_text_position(tpos)
        {
        }

        /**
         * Allows the use of this parse position as a const iterator.
         * @return the iterator for this parse position.
         */
        operator const iterator_type& () const {
            return m_iterator;
        }

        /**
         * Returns the iterator.
         * @return the iterator.
         */
        const iterator_type& iterator() const {
            return m_iterator;
        }

        /**
         * Returns the text position.
         * @return the text position.
         */
        const text_position_type& text_position() const {
            return m_text_position;
        }

        /**
         * Increments the iterator and the text column by one.
         */
        void increment() {
            ++m_iterator;
            m_text_position.increment_column();
        }

        /**
         * Increments the iterator and the text column by a specific count.
         */
        void increment(size_t count) {
            m_iterator += count;
            m_text_position.increment_column(count);
        }

        /**
         * Increments the the text position line only by one.
         * The iterator is not affected.
         * The column is set to its initial value.
         */
        void increment_line() {
            m_text_position.increment_line();
        }

        /**
         * Converts the parse position to a string.
         * @param begin begin iterator; used in deriving the index of the parse position iterator,
         *  if the text position returns an empty string.
         * @return a string for this parse position.
         */
        std::string to_string(const iterator_type& begin) const {
            std::string result = m_text_position.to_string();
            if (result.empty()) {
                std::stringstream stream;
                const auto index = std::distance(begin, m_iterator);
                stream << "index " << index;
                result = stream.str();
            }
            return result;
        }

        /**
         * Converts the parse position to a string.
         * @return a string for this parse position.
         */
        std::string to_string() const {
            return m_text_position.to_string();
        }

    private:
        iterator_type m_iterator;
        text_position_type m_text_position;
    };


    /**
     * A partition of a source.
     * @param Source the source type.
     * @param Id error id type.
     * @param TextPosition text position type.
     */
    template <class Source, class Id, class TextPosition>
    class source_partition {
    public:
        /** Source type. */
        using source_type = Source;

        /** Symbol type. */
        using symbol_type = typename Source::value_type;

        /** Iterator type. */
        using iterator_type = typename Source::const_iterator;

        /** id type. */
        using id_type = Id;

        /** Text position type. */
        using text_position_type = TextPosition;

        /** Parse position type. */
        using parse_position_type = parse_position<source_type, text_position_type>;

        /**
         * The constructor.
         * @param id match id.
         * @param start_pos start position.
         * @param end_it end_iterator.
         */
        source_partition(
            const id_type& id = id_type(),
            const parse_position_type& start_pos = parse_position_type(),
            const iterator_type& end_it = iterator_type()
        )
            : m_id(id)
            , m_start_position(start_pos)
            , m_end_iterator(end_it)
        {
            assert(static_cast<intptr_t>(std::distance(m_start_position.iterator(), m_end_iterator)) >= 0);
        }

        /**
         * Returns the id.
         * @return the id.
         */
        const id_type& id() const {
            return m_id;
        }

        /**
         * Returns the start position.
         * @return the start position.
         */
        const parse_position_type& start_position() const {
            return m_start_position;
        }

        /**
         * Returns the end iterator.
         * @return the end iterator.
         */
        const iterator_type& end_iterator() const {
            return m_end_iterator;
        }

        /**
         * Returns the start iterator.
         * @return the start iterator.
         */
        const iterator_type& begin() const {
            return m_start_position.iterator();
        }

        /**
         * Returns the end iterator.
         * @return the end iterator.
         */
        const iterator_type& end() const {
            return m_end_iterator;
        }

        /**
         * Returns the part of the source that corresponds to this partition.
         * @return the part of the source that corresponds to this partition.
         */
        auto source() const {
            if constexpr (is_char_v<symbol_type>) {
                return std::basic_string<symbol_type>(m_start_position.iterator(), m_end_iterator);
            }
            else {
                return std::vector<symbol_type>(m_start_position.iterator(), m_end_iterator);
            }
        }

        /**
         * Returns the part of the content that corresponds to this partition.
         * The content is the initial text that was parsed.
         * @return the part of the content that corresponds to this partition.
         */
        auto content() const {
            return parserlib::content_type<Source>::get(m_start_position.iterator(), m_end_iterator);
        }

        /**
         * Allows for using the source partition as input to parsing.
         * @return the id of this source partition as an int.
         */
        operator int () const {
            return static_cast<int>(m_id);
        }

    private:
        id_type m_id;
        parse_position_type m_start_position;
        iterator_type m_end_iterator;
    };


    /**
     * A match represents a recognized portion of a source.
     * @param Source the source type.
     * @param MatchId match id type.
     * @param TextPosition text position type.
     */
    template <class Source, class MatchId, class TextPosition>
    class match : public source_partition<Source, MatchId, TextPosition> {
    public:
        /** The source partition type. */
        using source_partition_type = source_partition<Source, MatchId, TextPosition>;

        /** This class' type. */
        using match_type = match<Source, MatchId, TextPosition>;

        /** The match container type. */
        using match_container_type = std::vector<match_type>;

        /**
         * The constructor.
         * @param id match id.
         * @param start_pos start position.
         * @param end_it end_iterator.
         * @param children children matches.
         */
        match(
            const MatchId& id = MatchId(),
            const typename source_partition_type::parse_position_type& start_pos = typename source_partition_type::parse_position_type(),
            const typename source_partition_type::iterator_type& end_it = typename source_partition_type::iterator_type(),
            match_container_type&& children = match_container_type()
        )
            : source_partition_type(id, start_pos, end_it)
            , m_children(std::move(children))
        {
        }

        /**
         * Returns the child matches.
         * @return the child matches.
         */
        const match_container_type& children() const {
            return m_children;
        }

        /**
         * Returns the child matches.
         * @return the child matches.
         */
        const match_container_type& matches() const {
            return m_children;
        }

        /**
         * Visits this node and its children.
         * @param visitor the visitor function; must have the signature `(const T& object, size_t depth)`.
         * @param depth current tree depth.
         */
        template <class Visitor> void visit(const Visitor& visitor, size_t depth = 0) const {
            visitor(*this, depth);
            for (const auto& child : m_children) {
                child.visit(visitor, depth + 1);
            }
        }

    private:
        match_container_type m_children;
    };


    /**
     * An error.
     * @param Source the source type.
     * @param ErrorId error id type.
     * @param TextPosition text position type.
     */
    template <class Source, class ErrorId, class TextPosition>
    class parse_error : public source_partition<Source, ErrorId, TextPosition> {
    public:
        /** The source partition type. */
        using source_partition_type = source_partition<Source, ErrorId, TextPosition>;

        /** This class' type. */
        using error_type = parse_error<Source, ErrorId, TextPosition>;

        /** The error container type. */
        using error_container_type = std::vector<error_type>;

        /**
         * The constructor.
         * @param id match id.
         * @param start_pos start position.
         * @param end_it end_iterator.
         */
        parse_error(
            const ErrorId& id = ErrorId(),
            const typename source_partition_type::parse_position_type& start_pos = typename source_partition_type::parse_position_type(),
            const typename source_partition_type::iterator_type& end_it = typename source_partition_type::iterator_type()
        )
            : source_partition_type(id, start_pos, end_it)
        {
        }
    };


} //namespace parserlib


#endif //PARSERLIB_PARSE_CONTEXT_DATA_HPP
