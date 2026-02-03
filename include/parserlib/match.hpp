#ifndef PARSERLIB_MATCH_HPP
#define PARSERLIB_MATCH_HPP


#include <vector>
#include <utility>
#include "source_range.hpp"


namespace parserlib {


    /**
     * A class that represents a match.
     * @param Id id type.
     * @param Iterator iterator type.
     */
    template <class Id = int, class Iterator = typename std::string::const_iterator>
    class match : public source_range<Id, Iterator> {
    public:
        /** id type. */
        using id_type = Id;

        /** iterator type. */
        using iterator_type = Iterator;

        /** match type. */
        using match_type = match<id_type, iterator_type>;

        /** match container type. */
        using match_container_type = std::vector<match_type>;

        /**
         * The default constructor.
         */
        match() {
        }

        /**
         * The constructor.
         * @param id id of the range.
         * @param begin begin iterator.
         * @param end end iterator.
         * @param children children matches.
         */
        match(const id_type& id, const iterator_type& begin, const iterator_type& end, match_container_type&& children)
            : source_range<Id, Iterator>(id, begin, end)
            , m_children(std::move(children))
        {
        }

        /**
         * Returns the id of the match as an `int`.
         * @return the id of the match as an `int`.
         */
        operator int () const noexcept {
            return static_cast<int>(this->get_id());
        }

        /**
         * Returns the children matches.
         * @return the children matches.
         */
        const match_container_type& get_children() const noexcept {
            return m_children;
        }

        /**
         * Returns number of nodes in the whole tree.
         * @return number of nodes in the whole tree.
         */
        size_t get_tree_count() const {
            size_t result = 1;
            for (const auto& child : m_children) {
                result += child.get_tree_count();
            }
            return result;
        }

    private:
        match_container_type m_children;
    };


    template <class Id, class Iterator>
    struct has_begin_method<match<Id, Iterator>> {
        static constexpr bool value = true;
    };


    /**
     * Helper function which converts a match and its children to a string.
     * @param stream target stream.
     * @param match the match to convert to a stream.
     * @param tab_size number of characters for a tab.
     * @param max_length maximum length for source output.
     * @param depth tree depth.
     */
    template <class Stream, class Id, class Iterator>
    void to_string(Stream& stream, const match<Id, Iterator>& match, size_t tab_size = 4, size_t max_length = 10, size_t depth = 0) {
        for (size_t index = 0; index < depth * tab_size; ++index) {
            stream << ' ';
        }
        to_string(stream, match.get_id());
        stream << " at ";
        to_string(stream, match.begin(), match.end(), max_length);
        stream << '\n';
        for (const auto& child_match : match.get_children()) {
            to_string(stream, child_match, tab_size, max_length, depth + 1);
        }
    }


    /**
     * Helper function for converting a vector of matches to a string.
     * @param stream target stream.
     * @param matches the matches to convert to a string.
     * @param tab_size number of characters for a tab.
     * @param max_length maximum length for source output.
     * @param depth tree depth.
     */
    template <class Stream, class Id, class Iterator>
    void to_string(Stream& stream, const std::vector<match<Id, Iterator>>& matches, size_t tab_size = 4, size_t max_length = 10, size_t depth = 0) {
        for (const auto& match : matches) {
            to_string(stream, match, tab_size, max_length, depth);
        }
    }


} //namespace parserlib


#endif //PARSERLIB_MATCH_HPP
