#ifndef PARSERLIB_MATCH_HPP
#define PARSERLIB_MATCH_HPP


#include <vector>
#include "source_partition.hpp"


namespace parserlib {


    template <class Iterator, class Id>
    class match : public source_partition<Iterator, Id> {
    public:
        match(const Id& id = {}, const Iterator& begin = {}, const Iterator& end = {}, std::vector<match>&& children = {})
            : source_partition<Iterator, Id>(id, begin, end)
            , m_children(std::move(children))
        {
        }

        operator int () const {
            return static_cast<int>(this->get_id());
        }

        const std::vector<match>& get_children() const {
            return m_children;
        }

    private:
        std::vector<match> m_children;
    };


    template <class T> std::basic_string<T> fix_source(const std::basic_string<T>& str) {
        std::basic_string<T> result;
        char prev_c = '\0';
        for (size_t i = 0; i < str.size(); ++i) {
            const char c = str[i];
            if (c == '\n') {
                result.push_back(' ');
            }
            else if (!std::isspace(c) || !std::isspace(prev_c)) {
                result.push_back(c);
            }
            prev_c = c;
        }
        return result;
    }


    template <class OutputStream, class Iterator, class Id> 
    void print_match(OutputStream& stream, const match<Iterator, Id>& m, size_t tabSize = 4, size_t depth = 0) {
        stream << std::string(depth * tabSize, ' ') << id_to_string(m.get_id());
        stream << ": \"" << fix_source(m.get_source()) << "\"\n";
        for (const match<Iterator, Id>& child_match : m.get_children()) {
            print_match(stream, child_match, tabSize, depth + 1);
        }
    }


    template <class OutputStream, class Iterator, class Id> 
    void print_matches(OutputStream& stream, const std::vector<match<Iterator, Id>>& matches, size_t tabSize = 4, size_t depth = 0) {
        for (const match<Iterator, Id>& match : matches) {
            print_match(stream, match, tabSize, depth);
        }
    }


} //namespace parserlib


#endif //PARSERLIB_MATCH_HPP
