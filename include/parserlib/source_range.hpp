#ifndef PARSERLIB_SOURCE_RANGE_HPP
#define PARSERLIB_SOURCE_RANGE_HPP


#include <vector>
#include <string>


namespace parserlib {


    template <class Id, class Iterator> class source_range;
    template <class Id, class Iterator> class match;
    template <class Id, class Iterator> class error;


    template <class T> struct source_type {
        using type = std::vector<T>;
    };


    template <> struct source_type<signed char> {
        using type = std::basic_string<signed char>;
    };


    template <> struct source_type<unsigned char> {
        using type = std::basic_string<unsigned char>;
    };


    template <> struct source_type<char> {
        using type = std::basic_string<char>;
    };


    #ifdef char8_t
    template <> struct source_type<char8_t> {
        using type = std::basic_string<char8_t>;
    };
    #endif


    template <> struct source_type<char16_t> {
        using type = std::basic_string<char16_t>;
    };


    template <> struct source_type<char32_t> {
        using type = std::basic_string<char32_t>;
    };


    template <> struct source_type<wchar_t> {
        using type = std::basic_string<wchar_t>;
    };


    template <class Id, class Iterator> struct source_type<source_range<Id, Iterator>> {
        using type = typename source_type<typename Iterator::value_type>::type;
    };


    template <class Id, class Iterator> struct source_type<match<Id, Iterator>> {
        using type = typename source_type<source_range<Id, Iterator>>::type;
    };


    template <class Id, class Iterator> struct source_type<error<Id, Iterator>> {
        using type = typename source_type<source_range<Id, Iterator>>::type;
    };


    /**
     * Base class for source range.
     * @param Id type of id for this source range.
     * @param Iterator iterator type for this source range.
     */ 
    template <class Id, class Iterator>
    class source_range {
    public:
        /** The id */ 
        using id = Id;

        /** The iterator */
        using iterator = Iterator;

        /** The value type */
        using value = typename Iterator::value_type;

        /**
         * The constructor.
         * @param id id.
         * @param begin the begin iterator.
         * @param end the end iterator.
         */ 
        source_range(const Id& id = Id(), const Iterator& begin = Iterator(), const Iterator& end = Iterator())
            : m_id(id)
            , m_begin(begin)
            , m_end(end)
        {
        }

        /**
         * Returns the id.
         * @return the id.
         */ 
        const Id& get_id() const {
            return m_id;
        }

        /**
         * Sets the id.
         * @param id the new id.
         */ 
        void set_id(const Id& id) {
            m_id = id;
        }

        /**
         * Returns the begin iterator.
         * @return the begin iterator.
         */ 
        const Iterator& begin() const {
            return m_begin;
        }

        /**
         * Returns the end iterator.
         * @return the end iterator.
         */ 
        const Iterator& end() const {
            return m_end;
        }

        /**
         * Returns the source that corresponds to this source range.
         * @return the source that corresponds to this source range.
         */ 
        auto get_source() const {
            return typename source_type<value>::type{ m_begin, m_end };
        }

        operator Id () const {
            return m_id;
        }

    private:
        Id m_id;
        Iterator m_begin;
        Iterator m_end;
    };


    /**
     * A source range that represents a match.
     * @param Id the type of the match id.
     * @param Iterator the iterator for the match source range.
     */ 
    template <class Id, class Iterator>
    class match : public source_range<Id, Iterator> {
    public:
        /** container of matches. */
        using match_container = std::vector<match<Id, Iterator>>;

        /**
         * The constructor.
         * @param id id.
         * @param begin the begin iterator.
         * @param end the end iterator.
         * @param children children.
         */ 
        match(const Id& id = Id(), const Iterator& begin = Iterator(), const Iterator& end = Iterator(), match_container&& children = {})
            : source_range<Id, Iterator>(id, begin, end)
            , m_children(std::move(children))
        {
        }

        /**
         * Returns the children.
         * @return the children.
         */ 
        const match_container& get_children() const {
            return m_children;
        }

    private:
        match_container m_children;
    };


} //namespace parserlib


#endif //PARSERLIB_SOURCE_RANGE_HPP
