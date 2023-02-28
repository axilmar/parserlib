#ifndef PARSERLIB__INPUT_VIEW__HPP
#define PARSERLIB__INPUT_VIEW__HPP


#include <string>


namespace parserlib {


    /**
     * Newline traits basic implementation.
     * Checks against '\n' character, which is skipped if found.
     */
    class newline_traits {
    public:
        /**
         * Operator or checking if a character is newline.
         * @param it iterator.
         * @return true if newline, false otherwise.
         */
        template <class It> bool operator ()(const It& it) const {
            return *it == '\n';
        }

        /**
         * Trait for skipping new lines.
         * @param it iterator.
         */
        template <class It> void skip(It& it) {
            ++it;
        }
    };


    /**
     * Input view iterator.
     * @param It input iterator type.
     * @param NewlineTraits newline traits.
     */
    template <class It, class NewlineTraits = newline_traits> class input_view_iterator {
    public:
        /**
         * The default constructor.
         * No value is initialized.
         */
        input_view_iterator() {
        }

        /**
         * Constructor from iterator.
         */
        input_view_iterator(const It& it) : m_it(it) {
        }

        /**
         * Pre-increment.
         */
        input_view_iterator<It>& operator ++() {
            increment();
            return *this;
        }

        /**
         * Post-increment.
         */
        input_view_iterator<It> operator ++(int i) {
            auto self = *this;
            increment();
            return self;
        }

        /**
         * Allows construction of input ranges (const version).
         */
        operator const It& () const {
            return m_it;
        }

        /**
         * Allows construction of input ranges.
         */
        operator It () {
            return m_it;
        }

        /**
         * Equality test.
         */
        bool operator == (const input_view_iterator& other) const {
            return m_it == other.m_it;
        }

        /**
         * Difference test.
         */
        bool operator != (const input_view_iterator& other) const {
            return m_it != other.m_it;
        }

        /**
         * Less than test.
         */
        bool operator < (const input_view_iterator& other) const {
            return m_it < other.m_it;
        }

        /**
         * Greater than test.
         */
        bool operator > (const input_view_iterator& other) const {
            return m_it > other.m_it;
        }

        /**
         * Less than or equal test.
         */
        bool operator <= (const input_view_iterator& other) const {
            return m_it <= other.m_it;
        }

        /**
         * Greater than or equal test.
         */
        bool operator >= (const input_view_iterator& other) const {
            return m_it >= other.m_it;
        }

        /**
         * The dereference operator.
         */
        auto operator *() const {
            return *m_it;
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
        It m_it;
        int m_line = 1;
        int m_column = 1;

        void increment() {
            if (!NewlineTraits()(m_it)) {
                ++m_column;
                ++m_it;
            }
            else {
                ++m_line;
                m_column = 1;
                NewlineTraits().skip(m_it);
            }
        }
    };


    /**
     * Defines a view over a container.
     * The view contains an iterator class that defines a position (line and column).
     * @param T any stl-like container that can be used as input.
     * @param It input view iterator.
     */
    template <class T = std::string, class It = input_view_iterator<typename T::const_iterator>> class input_view {
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
        input_view(const T& input) : m_input(input) {
        }

        /**
         * Begins iteration.
         */
        const_iterator begin() const {
            return const_iterator(m_input.begin());
        }

        /**
         * Ends iteration.
         */
        const_iterator end() const {
            return const_iterator(m_input.end());
        }

    private:
        const T& m_input;
    };


} //namespace parserlib


#endif //PARSERLIB__INPUT_VIEW__HPP

