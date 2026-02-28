#ifndef PARSERLIB_TEXT_ITERATOR_HPP
#define PARSERLIB_TEXT_ITERATOR_HPP


#include <string>


namespace parserlib {


    template <class Iterator = std::string::const_iterator>
    class text_iterator : public Iterator {
    public:
        text_iterator() {
        }

        text_iterator(const Iterator& iterator)
            : Iterator(iterator)
        {
        }

        text_iterator& operator ++() {
            Iterator::operator ++();
            ++m_column;
            return *this;
        }

        text_iterator& operator += (size_t count) {
            Iterator::operator += (count);
            m_column += count;
            return *this;
        }

        size_t get_column() const {
            return m_column;
        }

        size_t get_line() const {
            return m_line;
        }

        void increment_line() {
            ++m_line;
            m_column = 1;
        }

    private:
        size_t m_column{ 1 };
        size_t m_line{ 1 };
    };


    template <class Iterator> 
    void increment_line(text_iterator<Iterator>& it) {
        it.increment_line();
    }


} //namespace parserlib


#endif //PARSERLIB_TEXT_ITERATOR_HPP
