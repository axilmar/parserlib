#ifndef PARSERLIB_SOURCE_RANGE_HPP
#define PARSERLIB_SOURCE_RANGE_HPP


#include <vector>
#include <string>


namespace parserlib {


    template <class Id, class Iterator> class source_range;
    template <class Id, class Iterator> class match;
    template <class Id, class Iterator> class error;


    /**
     * Class that provides the appropriate container for type T.
     * @param T type to define the container of.
     */ 
    template <class T> struct source_container_type {
        /** The container type. */ 
        using type = std::vector<T>;
    };


    /**
     * Specialization for the given character type.
     * The container type is an std::basic_string instance for the given type.
     */ 
    template <> struct source_container_type<signed char> {
        /** the container type. */
        using type = std::basic_string<signed char>;
    };


    /**
     * Specialization for the given character type.
     * The container type is an std::basic_string instance for the given type.
     */ 
    template <> struct source_container_type<unsigned char> {
        /** the container type. */
        using type = std::basic_string<unsigned char>;
    };


    /**
     * Specialization for the given character type.
     * The container type is an std::basic_string instance for the given type.
     */ 
    template <> struct source_container_type<char> {
        /** the container type. */
        using type = std::basic_string<char>;
    };


    #ifdef char8_t
    /**
     * Specialization for the given character type.
     * The container type is an std::basic_string instance for the given type.
     */ 
    template <> struct source_container_type<char8_t> {
        /** the container type. */
        using type = std::basic_string<char8_t>;
    };
    #endif


    /**
     * Specialization for the given character type.
     * The container type is an std::basic_string instance for the given type.
     */ 
    template <> struct source_container_type<char16_t> {
        /** the container type. */
        using type = std::basic_string<char16_t>;
    };


    /**
     * Specialization for the given character type.
     * The container type is an std::basic_string instance for the given type.
     */ 
    template <> struct source_container_type<char32_t> {
        /** the container type. */
        using type = std::basic_string<char32_t>;
    };


    /**
     * Specialization for the given character type.
     * The container type is an std::basic_string instance for the given type.
     */ 
    template <> struct source_container_type<wchar_t> {
        /** the container type. */
        using type = std::basic_string<wchar_t>;
    };


    /**
     * Specialization for source range.
     * The container type for the source of the source range is defined by its iterator type value.
     */ 
    template <class Id, class Iterator> struct source_container_type<source_range<Id, Iterator>> {
        using type = typename source_container_type<typename Iterator::value_type>::type;
    };


    /**
     * Specialization for match.
     * Same container as for the `source_range` class.
     */ 
    template <class Id, class Iterator> struct source_container_type<match<Id, Iterator>> {
        using type = typename source_container_type<source_range<Id, Iterator>>::type;
    };


    /**
     * Base class for source range.
     * @param Id type of id for this source range.
     * @param Iterator iterator type for this source range.
     */ 
    template <class Id, class Iterator>
    class source_range {
    public:
        /** The id type. */ 
        using id_type = Id;

        /** The iterator type. */
        using iterator_type = Iterator;

        /** The value type. */
        using value_type = typename Iterator::value_type;

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
            return typename source_container_type<value_type>::type{ m_begin, m_end };
        }

        /**
         * Auto conversion to id type.
         * @return the id type.
         */ 
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
        using match_container_type = std::vector<match<Id, Iterator>>;

        /**
         * The constructor.
         * @param id id.
         * @param begin the begin iterator.
         * @param end the end iterator.
         * @param children children.
         */ 
        match(const Id& id = Id(), const Iterator& begin = Iterator(), const Iterator& end = Iterator(), match_container_type&& children = {})
            : source_range<Id, Iterator>(id, begin, end)
            , m_children(std::move(children))
        {
        }

        /**
         * Returns the children.
         * @return the children.
         */ 
        const match_container_type& get_children() const {
            return m_children;
        }

    private:
        match_container_type m_children;
    };


} //namespace parserlib


#endif //PARSERLIB_SOURCE_RANGE_HPP
