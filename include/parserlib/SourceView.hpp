#ifndef PARSERLIB_SOURCEVIEW_HPP
#define PARSERLIB_SOURCEVIEW_HPP


#include <string>


namespace parserlib {


    /**
     * Newline traits basic implementation.
     * Checks against '\n' character, which is skipped if found.
     */
    class NewlineTraits {
    public:
        /**
         * Operator or checking if a character is newline.
         * @param it iterator.
         * @param end end position.
         * @return true if newline, false otherwise.
         */
        template <class It> bool operator ()(const It& it, const It& end) const {
            return *it == '\n';
        }

        /**
         * Trait for skipping new lines.
         * It applies operator ++() over the given iterator.
         * @param it iterator.
         * @param end end position.
         */
        template <class It> void skip(It& it, const It& end) const {
            ++it;
        }
    };


    /**
     * Source view iterator.
     * @param It base input iterator type.
     * @param NL newline traits.
     */
    template <class It, class NL> class SourceViewIterator : public It {
    public:
        /**
         * The default constructor.
         * No value is initialized.
         */
        SourceViewIterator() {
        }

        /**
         * Constructor from iterator.
         * @param it initial position.
         * @param end end position.
         */
        SourceViewIterator(const It& it, const It& end) : It(it), m_end(end) {
        }

        /**
         * Pre-increment.
         */
        SourceViewIterator<It, NL>& operator ++() {
            increment();
            return *this;
        }

        /**
         * Post-increment.
         */
        SourceViewIterator<It, NL> operator ++(int i) {
            auto self = *this;
            increment();
            return self;
        }

        /**
         * Returns the line.
         */
        int line() const {
            return m_line;
        }

        /**
         * Returns the column.
         */
        int column() const {
            return m_column;
        }

    private:
        int m_line = 1;
        int m_column = 1;
        It m_end;

        //returns this as base iterator
        It& base() {
            return static_cast<It&>(*this);
        }

        void increment() {
            const NL nl;

            if (!nl(base(), m_end)) {
                ++m_column;
                It::operator++();
            }
            else {
                ++m_line;
                m_column = 1;
                nl.skip(base(), m_end);
            }
        }
    };


    /**
     * Defines a view over a container.
     * The view contains an iterator class that defines a position (line and column).
     * @param T any stl-like container that can be used as input.
     * @param NL newline traits.
     * @param It input view iterator.
     */
    template <class T = std::string, class NL = NewlineTraits, class It = SourceViewIterator<typename T::const_iterator, NL>> class SourceView {
    public:
        /**
         * Value type.
         */
        using value_type = typename T::value_type;

        /**
         * Const iterator is the given input.
         */
        using const_iterator = It;

        /**
         * The default constructor.
         * @param input input; must have lifetime as the view.
         */
        SourceView(const T& input) : m_input(input) {
        }

        /**
         * Returns the initial position.
         * @return the initial position.
         */
        const_iterator begin() const {
            return const_iterator(m_input.begin(), m_input.end());
        }

        /**
         * Returns the end position.
         * @return the end position.
         */
        const_iterator end() const {
            return const_iterator(m_input.end(), m_input.end());
        }

    private:
        const T& m_input;
    };


} //namespace parserlib


#endif //PARSERLIB_SOURCEVIEW_HPP
