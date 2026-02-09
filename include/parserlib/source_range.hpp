#ifndef PARSERLIB_SOURCE_RANGE_HPP
#define PARSERLIB_SOURCE_RANGE_HPP


#include <vector>


namespace parserlib {


    /**
     * Base class for source range.
     * @param Id type of id for this source range.
     * @param Iterator iterator type for this source range.
     */ 
    template <class Id, class Iterator>
    class source_range {
    public:
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
        const Iterator& begin() {
            return m_begin;
        }

        /**
         * Returns the begin iterator.
         * @return the begin iterator.
         */ 
        const Iterator& get_begin() {
            return m_begin;
        }
        /**
         * Sets the begin iterator.
         * @param it the begin iterator.
         */ 
        void set_begin(const Iterator& it) {
            m_begin = begin;
        }

        /**
         * Returns the end iterator.
         * @return the end iterator.
         */ 
        const Iterator& end() {
            return m_end;
        }

        /**
         * Returns the end iterator.
         * @return the end iterator.
         */ 
        const Iterator& get_end() {
            return m_end;
        }
        /**
         * Sets the end iterator.
         * @param it the end iterator.
         */ 
        void set_end(const Iterator& it) {
            m_end = end;
        }

    private:
        Id m_id;
        Iterator m_begin;
        Iterator m_end;
    };


    /**
     * A source range that represents a match.
     * @param Id the type of the match id.
     * @param Iterator the iterator the match source range.
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


    /**
     * A source range that represents an error.
     * @param Id the type of the error id.
     * @param Iterator the iterator the error source range.
     */ 
    template <class Id, class Iterator>
    class error : public source_range<Id, Iterator> {
    public:
        using source_range<Id, Iterator>::source_range;
    };


} //namespace parserlib


#endif //PARSERLIB_SOURCE_RANGE_HPP
