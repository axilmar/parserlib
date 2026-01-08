#ifndef PARSERLIB_GENERIC_ITERATOR_HPP
#define PARSERLIB_GENERIC_ITERATOR_HPP


#include <cassert>
#include <cstdint>
#include <any>
#include <iterator>
#include <algorithm>
#include <utility>


namespace parserlib {


    /**
     * A wrapper over an iterator.
     * Useful when it is required that grammars shall have to be compiled 
     * before knowing the exact input type.
     */
    class generic_iterator {
    public:
        /** value type. */
        using value_type = uintptr_t;

        /**
         * The default constructor.
         * An empty iterator is created.
         */
        generic_iterator() 
            : m_vtable(nullptr)
        {
        }

        /**
         * The copy constructor.
         * @param src the source object.
         */
        generic_iterator(const generic_iterator& src) 
            : m_vtable(src.m_vtable)
            , m_iterator(src.m_iterator)
        {
        }

        /**
         * The move constructor.
         * @param src the source object.
         */
        generic_iterator(generic_iterator&& src)
            : m_vtable(src.m_vtable)
            , m_iterator(std::move(src.m_iterator))
        {
        }

        /**
         * Constructor from iterator.
         * @param iterator the iterator to wrap over.
         */
        template <class It>
        generic_iterator(const It& iterator)
            : m_vtable(get_vtable<It>())
            , m_iterator(iterator)
        {
        }

        /**
         * The destructor.
         */
        ~generic_iterator() {
        }

        /**
         * The copy assignment operator.
         * @param src the source object.
         * @return reference to this.
         */
        generic_iterator& operator = (const generic_iterator& src) {
            assert(m_vtable == src.m_vtable);
            m_iterator = src.m_iterator;
            return *this;
        }

        /**
         * The move assignment operator.
         * @param src the source object.
         * @return reference to this.
         */
        generic_iterator& operator = (generic_iterator&& src) {
            assert(m_vtable == src.m_vtable);
            m_iterator = std::move(src.m_iterator);
            return *this;
        }

        /**
         * Assigment from iterator.
         * @param iterator the iterator to wrap over.
         * @return reference to this.
         */
        template <class It>
        generic_iterator& operator = (const It& iterator) {
            m_vtable = get_vtable<It>();
            m_iterator = iterator;
            return *this;
        }

        /**
         * Returns the iterator.
         * @return the iterator.
         */
        const std::any& get_iterator() const noexcept {
            return m_iterator;
        }

        /**
         * Checks if this iterator points to the same element as another iterator.
         * @param other the other iterator to compare against this.
         * @return true if both iterators point to the same element, false otherwise.
         */
        bool operator == (const generic_iterator& other) const {
            assert(m_vtable == other.m_vtable);
            return m_vtable->compare_equal(m_iterator, other.m_iterator);
        }

        /**
         * Checks if this iterator points to a different element than another iterator.
         * @param other the other iterator to compare against this.
         * @return true if this points to a different element than the given iterator, false otherwise.
         */
        bool operator != (const generic_iterator& other) const {
            return !operator == (other);
        }

        /**
         * Dereferences the iterator.
         * @return the result of operator *() on the iterator, statically cast the generic value type.
         */
        value_type operator *() const {
            return m_vtable->dereference(m_iterator);
        }

        /**
         * Increments the iterator by one.
         * @return reference to this.
         */
        generic_iterator& operator ++() {
            m_vtable->increment(m_iterator);
            return *this;
        }

        /**
         * Increments the iterator by a specific amount.
         * @param count number of positions to increment the iterator.
         * @return reference to this.
         */
        generic_iterator& operator += (size_t count) {
            m_vtable->increase(m_iterator, count);
            return *this;
        }

    private:
        struct vtable {
            bool (*compare_equal)(const std::any& left, const std::any& right);
            value_type (*dereference)(const std::any& it);
            void (*increment)(std::any& it);
            void (*increase)(std::any& it, size_t count);
        };

        vtable* m_vtable;
        std::any m_iterator;

        template <class It>
        static const It& get(const std::any& a) {
            const It* it = std::any_cast<It>(&a);
            assert(it);
            return *it;
        }

        template <class It>
        static It& get(std::any& a) {
            It* it = std::any_cast<It>(&a);
            assert(it);
            return *it;
        }

        template <class It>
        struct vtable_impl {
            static bool compare_equal(const std::any& left, const std::any& right) {
                return get<It>(left) == get<It>(right);
            }

            static value_type dereference(const std::any& it) {
                return *get<It>(it);
            }

            static void increment(std::any& it) {
                ++get<It>(it);
            }

            static void increase(std::any& it, size_t count) {
                get<It>(it) = std::next(get<It>(it), count);
            }
        };

        template <class It>
        static vtable* get_vtable() {
            static vtable vt = {
                &vtable_impl<It>::compare_equal,
                &vtable_impl<It>::dereference,
                &vtable_impl<It>::increment,
                &vtable_impl<It>::increase
            };
            return &vt;
        }
    };


} //namespace parserlib


namespace std {


    template <>
    struct iterator_traits<parserlib::generic_iterator> {
        using difference_type = std::ptrdiff_t;
        using value_type = parserlib::generic_iterator::value_type;
        using pointer = value_type*;
        using reference = value_type&;
        using iterator_category = std::input_iterator_tag;
    };


} //namespace std


#endif //PARSERLIB_GENERIC_ITERATOR_HPP
