#ifndef PARSERLIB_TO_STRING_HPP
#define PARSERLIB_TO_STRING_HPP


#include <algorithm>
#include "parse_iterator.hpp"
#include "match.hpp"
#include "ast.hpp"


namespace parserlib {


    /**
     * Class that allows the conversion of an id to a string.
     * @param Id type of id.
     */ 
    template <class Id>
    struct id_to_string {
        /**
         * Writes the given id to string.
         * @param stream the destination stream.
         * @param id the id to write to the stream.
         */
        template <class Stream>
        static void exec(Stream& stream, const Id& id) {
            stream << (int)id;
        }
    };


    struct to_string_options {
        size_t source_max_length = 40;
        size_t tab_size = 4;
    };


    template <class Stream, class Char>
    void char_to_string(Stream& stream, size_t length, Char ch) {
        for (; length > 0; --length) {
            stream << ch;
        }
    }


    template <class Stream>
    void spaces_to_string(Stream& stream, size_t length) {
        char_to_string(stream, length, ' ');
    }


    template <class Stream>
    void spaces_to_string(Stream& stream, const to_string_options& options, size_t tree_depth) {
        char_to_string(stream, options.tab_size * tree_depth, ' ');
    }


    template <class Iterator, class Value>
    struct source_to_string {
        template <class Stream>
        static void exec(Stream& stream, const Iterator& begin, const Iterator& end, const to_string_options& options) {
            size_t length = options.source_max_length;
            Iterator it = begin;
            for (; it != end && length > 0; ++it, --length) {
                if (*it != '\n') {
                    stream << *it;
                }
                else {
                    stream << "\\n";
                }
            }
            if (it != end) {
                stream << "...";
            }
        }
    };


    template <class Iterator, class TextPosition, class T>
    struct source_to_string<parse_iterator<Iterator, TextPosition>, T> {
        template <class Stream>
        static void exec(Stream& stream, const parse_iterator<Iterator, TextPosition>& begin, const parse_iterator<Iterator, TextPosition>& end, const to_string_options& options) {
            stream << begin.get_text_position().to_string();
            stream << ": ";
            source_to_string<Iterator, T>::exec(stream, begin.get_iterator(), end.get_iterator(), options);
        }
    };


    template <class Iterator, class Id, class MatchIterator>
    struct source_to_string<Iterator, match<Id, MatchIterator>> {
        template <class Stream>
        static void exec(Stream& stream, const Iterator& begin, const Iterator& end, const to_string_options& options) {
            source_to_string<MatchIterator, typename MatchIterator::value_type>::exec(stream, begin->begin(), std::prev(end)->end(), options);
        }
    };


    template <class Iterator, class Id, class NodeIterator>
    struct source_to_string<Iterator, std::shared_ptr<ast_node<Id, NodeIterator>>> {
        template <class Stream, class Iterator>
        static void exec(Stream& stream, const Iterator& begin, const Iterator& end, const to_string_options& options) {
            source_to_string<NodeIterator, typename NodeIterator::value_type>::exec(stream, (*begin)->begin(), (*std::prev(end))->end(), options);
        }
    };


    template <class Stream, class Id, class Iterator> 
    void source_range_to_string(Stream& stream, const source_range<Id, Iterator>& src_range, const to_string_options& options, size_t tree_depth) {
        spaces_to_string(stream, options, tree_depth);
        id_to_string<Id>::exec(stream, src_range.get_id());
        stream << " at ";
        source_to_string<Iterator, typename Iterator::value_type>::exec(stream, src_range.begin(), src_range.end(), options);
        stream << '\n';
    }


    template <class Stream, class Id, class Iterator> 
    void to_string(Stream& stream, const std::vector<match<Id, Iterator>>& matches, const to_string_options& options, size_t tree_depth);


    template <class Stream, class Id, class Iterator> 
    void to_string(Stream& stream, const match<Id, Iterator>& match, const to_string_options& options = to_string_options(), size_t tree_depth = 0) {
        parserlib::source_range_to_string(stream, match, options, tree_depth);
        parserlib::to_string(stream, match.get_children(), options, tree_depth + 1);
    }


    template <class Stream, class Id, class Iterator> 
    void to_string(Stream& stream, const std::vector<match<Id, Iterator>>& matches, const to_string_options& options = to_string_options(), size_t tree_depth = 0) {
        for (const match<Id, Iterator>& child_match : matches) {
            parserlib::to_string(stream, child_match, options, tree_depth);
        }
    }


    template <class Stream, class Id, class Iterator> 
    static void to_string(Stream& stream, const std::shared_ptr<ast_node<Id, Iterator>>& node_ptr, const to_string_options& options = to_string_options(), size_t tree_depth = 0) {
        parserlib::source_range_to_string(stream, *node_ptr, options, tree_depth);
        for (const std::shared_ptr<ast_node<Id, Iterator>>& child_ptr : node_ptr->get_children()) {
            parserlib::to_string(stream, child_ptr, options, tree_depth + 1);
        }
    }


    template <class Stream, class Id, class Iterator, class Alloc> 
    static void to_string(Stream& stream, const std::vector<std::shared_ptr<ast_node<Id, Iterator>>, Alloc>& nodes, const to_string_options& options = to_string_options(), size_t tree_depth = 0) {
        for (const std::shared_ptr<ast_node<Id, Iterator>>& node_ptr : nodes) {
            parserlib::to_string(stream, node_ptr, options, tree_depth);
        }
    }


} //namespace parserlib


#endif //PARSERLIB_TO_STRING_HPP
