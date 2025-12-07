#ifndef PARSERLIB_GENERIC_SOURCE_HPP
#define PARSERLIB_GENERIC_SOURCE_HPP


#include <cassert>
#include <vector>
#include <utility>
#include <iterator>
#include "invalid_generic_source_iterator.hpp"


namespace parserlib {


    template <size_t BufferSize = sizeof(std::vector<int>::const_iterator)>
    class generic_source {
    public:
        static constexpr size_t buffer_size = BufferSize;

        class const_iterator {
        public:
            using iterator_category = std::random_access_iterator_tag;
            using difference_type = std::ptrdiff_t;
            using value_type = int;
            using pointer = const value_type*;
            using reference = const value_type&;

            const_iterator() 
                : m_vtable(get_empty_vtable())
            {
            }

            const_iterator(const const_iterator& src) 
                : m_vtable(src.m_vtable)
            {
                m_vtable->copy_construct(m_buffer, src.m_buffer);
            }

            const_iterator(const_iterator&& src) 
                : m_vtable(src.m_vtable)
            {
                m_vtable->move_construct(m_buffer, src.m_buffer);
            }

            template <class Iterator>
            const_iterator(const Iterator& iterator) 
                : m_vtable(get_vtable<Iterator>())
            {
                m_vtable->copy_construct(m_buffer, reinterpret_cast<const char*>(&iterator));
            }

            ~const_iterator() {
                m_vtable->destruct(m_buffer);
            }

            const_iterator& operator = (const const_iterator& src) {
                m_vtable = src.m_vtable;
                m_vtable->copy_assign(m_buffer, src.m_buffer);
                return *this;
            }

            const_iterator& operator = (const_iterator&& src) {
                m_vtable = src.m_vtable;
                m_vtable->move_assign(m_buffer, src.m_buffer);
                return *this;
            }

            value_type operator *() const {
                return m_vtable->dereference(m_buffer);
            }

            const_iterator& operator ++() {
                m_vtable->increment_by_one(m_buffer);
                return *this;
            }

            const_iterator& operator += (size_t count) {
                m_vtable->increment_by_many(m_buffer, count);
                return *this;
            }

            bool operator == (const const_iterator& src) const {
                assert(m_vtable == src.m_vtable);
                return m_vtable->compare_equal(m_buffer, src.m_buffer);
            }

            bool operator != (const const_iterator& src) const {
                assert(m_vtable == src.m_vtable);
                return m_vtable->compare_different(m_buffer, src.m_buffer);
            }

        private:
            using buffer_type = char[BufferSize];

            struct vtable {
                void (*const copy_construct)(buffer_type dst, const buffer_type src);
                void (*const move_construct)(buffer_type dst, buffer_type src);
                void (*const destruct)(buffer_type dst);
                void (*const copy_assign)(buffer_type dst, const buffer_type src);
                void (*const move_assign)(buffer_type dst, buffer_type src);
                value_type (*const dereference)(const buffer_type dst);
                void (*const increment_by_one)(buffer_type dst);
                void (*const increment_by_many)(buffer_type dst, size_t count);
                bool (*const compare_equal)(const buffer_type dst, const buffer_type src);
                bool (*const compare_different)(const buffer_type dst, const buffer_type src);
            };

            const vtable* m_vtable;
            buffer_type m_buffer;

            template <class Iterator>
            class vtable_impl {
            public:
                static void copy_construct(buffer_type dst, const buffer_type src) {
                    new (dst) Iterator(*reinterpret_cast<const Iterator *>(src));
                }

                static void move_construct(buffer_type dst, buffer_type src) {
                    new (dst) Iterator(std::move(*reinterpret_cast<Iterator *>(src)));
                }

                static void destruct(buffer_type dst) {
                    reinterpret_cast<Iterator*>(dst)->~Iterator();
                }

                static void copy_assign(buffer_type dst, const buffer_type src) {
                    *reinterpret_cast<Iterator*>(dst) = *reinterpret_cast<const Iterator*>(src);
                }

                static void move_assign(buffer_type dst, buffer_type src) {
                    *reinterpret_cast<Iterator*>(dst) = std::move(*reinterpret_cast<const Iterator*>(src));
                }

                static value_type dereference(const buffer_type dst) {
                    return static_cast<value_type>(**reinterpret_cast<const Iterator*>(dst));
                }

                static void increment_by_one(buffer_type dst) {
                    ++*reinterpret_cast<Iterator*>(dst);
                }

                static void increment_by_many(buffer_type dst, size_t count) {
                    std::advance(*reinterpret_cast<Iterator*>(dst), count);
                }

                static bool compare_equal(const buffer_type dst, const buffer_type src) {
                    return *reinterpret_cast<const Iterator*>(dst) == *reinterpret_cast<const Iterator*>(src);
                }

                static bool compare_different(const buffer_type dst, const buffer_type src) {
                    return *reinterpret_cast<const Iterator*>(dst) != *reinterpret_cast<const Iterator*>(src);
                }
            };

            template <class Iterator>
            static const vtable* get_vtable() {
                static_assert(sizeof(Iterator) <= sizeof(buffer_type));
                static const vtable vt = {
                    &vtable_impl<Iterator>::copy_construct,
                    &vtable_impl<Iterator>::move_construct,
                    &vtable_impl<Iterator>::destruct,
                    &vtable_impl<Iterator>::copy_assign,
                    &vtable_impl<Iterator>::move_assign,
                    &vtable_impl<Iterator>::dereference,
                    &vtable_impl<Iterator>::increment_by_one,
                    &vtable_impl<Iterator>::increment_by_many,
                    &vtable_impl<Iterator>::compare_equal,
                    &vtable_impl<Iterator>::compare_different,
                };
                return &vt;
            }

            class empty_vtable_impl {
            public:
                static void copy_construct(buffer_type dst, const buffer_type src) {
                }

                static void move_construct(buffer_type dst, buffer_type src) {
                }

                static void destruct(buffer_type dst) {
                }

                static void copy_assign(buffer_type dst, const buffer_type src) {
                }

                static void move_assign(buffer_type dst, buffer_type src) {
                }

                static value_type dereference(const buffer_type dst) {
                    throw invalid_generic_source_iterator();
                }

                static void increment_by_one(buffer_type dst) {
                    throw invalid_generic_source_iterator();
                }

                static void increment_by_many(buffer_type dst, size_t count) {
                    throw invalid_generic_source_iterator();
                }

                static bool compare_equal(const buffer_type dst, const buffer_type src) {
                    throw invalid_generic_source_iterator();
                }

                static bool compare_different(const buffer_type dst, const buffer_type src) {
                    throw invalid_generic_source_iterator();
                }
            };

            static const vtable* get_empty_vtable() {
                static const vtable vt = {
                    &empty_vtable_impl::copy_construct,
                    &empty_vtable_impl::move_construct,
                    &empty_vtable_impl::destruct,
                    &empty_vtable_impl::copy_assign,
                    &empty_vtable_impl::move_assign,
                    &empty_vtable_impl::dereference,
                    &empty_vtable_impl::increment_by_one,
                    &empty_vtable_impl::increment_by_many,
                    &empty_vtable_impl::compare_equal,
                    &empty_vtable_impl::compare_different,
                };
                return &vt;
            }
        };

        template <class Iterator>
        generic_source(const Iterator& begin, const Iterator& end)
            : m_begin(begin)
            , m_end(end)
        {
        }

        template <class Container>
        generic_source(Container& container)
            : generic_source(container.begin(), container.end())
        {
        }

        const const_iterator& begin() const {
            return m_begin;
        }

        const const_iterator& end() const {
            return m_end;
        }

    private:
        const_iterator m_begin;
        const_iterator m_end;
    };


} //namespace parserlib


namespace std {


    template <size_t BufferSize>
    typename parserlib::generic_source<BufferSize>::const_iterator& next(typename parserlib::generic_source<BufferSize>::const_iterator& it, size_t count) {
        it += count;
        return it;
    }


} //namespace std


#endif //PARSERLIB_GENERIC_SOURCE_HPP
