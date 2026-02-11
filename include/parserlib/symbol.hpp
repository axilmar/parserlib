#ifndef PARSERLIB_SYMBOL_HPP
#define PARSERLIB_SYMBOL_HPP


#include <memory>
#include <vector>
#include <string_view>
#include <utility>
#include <algorithm>
#include <initializer_list>


namespace parserlib {


    /**
     * The symbol value type.
     */
    using symbol_value_type = int;


    /**
     * Base class for symbols.
     * Used to provide a generic interface to typed symbols.
     */
    class symbol {
    public:
        /**
         * Virtual destructor due to inheritance.
         */
        virtual ~symbol();

        /**
         * Returns the symbol value as an 'symbol_value_type'.
         * @return the symbol value.
         */
        symbol_value_type get_value() const;

    protected:
        /**
         * The constructor.
         * It can only be instantiated through the `typed_symbol` class.
         * @param value the 'symbol_value_type' value of the symbol.
         */
        symbol(symbol_value_type value);

    private:
        symbol_value_type m_value;
    };


    /**
     * Symbol pointer type.
     * It is a unique pointer because symbols are not be shared between grammar nodes.
     */
    using symbol_ptr = std::unique_ptr<symbol>;


    /**
     * A typed symbol.
     * It can be used either for single characters or other values that can be statically converted to 'symbol_value_type'.
     * @param T type of symbol.
     */ 
    template <class T>
    class typed_symbol : public symbol {
    public:
        /**
         * The constructor.
         * @param value the value.
         */
        typed_symbol(const T& value) 
            : symbol(static_cast<symbol_value_type>(value))
            , m_value(value)
        {
        }

        /**
         * Returns the value.
         * @return the value.
         */ 
        const T& get_value() const {
            return m_value;
        }

    private:
        T m_value;
    };


    /**
     * Base class for symbol strings.
     * Used to provide a generic interface to typed symbol strings.
     */
    class symbol_string {
    public:
        /**
         * Virtual destructor due to inheritance.
         */
        virtual ~symbol_string();

        /**
         * Returns the string as a vector of 'symbol_value_type'.
         * @return the string as a vector of 'symbol_value_type'.
         */ 
        const std::vector<symbol_value_type>& get_value() const;

    protected:
        /**
         * The constructor.
         * It can only be instantiated through the `typed_symbol_string` class.
         * @param value the array of characters as of type 'symbol_value_type'.
         */
        symbol_string(const std::vector<symbol_value_type>& value);

    private:
        std::vector<symbol_value_type> m_value;
    };


    /**
     * Symbol string pointer type.
     * It is a unique pointer because symbols are not be shared between grammar nodes.
     */
    using symbol_string_ptr = std::unique_ptr<symbol_string>;


    /**
     * A typed symbol string.
     * @param T type of character.
     */
    template <class T> 
    class typed_symbol_string : public symbol_string {
    public:
        /**
         * The constructor.
         * @param value the string value.
         */
        typed_symbol_string(const std::basic_string_view<T>& value)
            : symbol_string(std::vector<symbol_value_type>(value.begin(), value.end()))
            , m_value(value)
        {
        }

        /**
         * Returns the string value.
         * @return the string value.
         */ 
        const std::basic_string_view<T>& get_value() const {
            return m_value;
        }

    private:
        std::basic_string_view<T> m_value;
    };


    /**
     * Base class for symbol sets.
     * Sets are represented as sorted vectors in order to use binary partioning when finding an element.
     * Used to provide a generic interface to typed symbol sets.
     */
    class symbol_set {
    public:
        /**
         * Virtual destructor due to inheritance.
         */
        virtual ~symbol_set();

        /**
         * Returns the set as a vector of 'symbol_value_type'.
         * @return the set as a vector of 'symbol_value_type'.
         */ 
        const std::vector<symbol_value_type>& get_value() const;

    protected:
        /**
         * The constructor.
         * It can only be instantiated through the `typed_symbol_set` class.
         * @param value the array of characters as of type 'symbol_value_type'; they are sorted.
         */
        symbol_set(const std::vector<symbol_value_type>& value);

    private:
        std::vector<symbol_value_type> m_value;
    };


    /**
     * Symbol set pointer type.
     * It is a unique pointer because symbols are not be shared between grammar nodes.
     */
    using symbol_set_ptr = std::unique_ptr<symbol_set>;


    /**
     * A typed symbol set.
     * @param T type of character.
     */
    template <class T> 
    class typed_symbol_set : public symbol_set {
    public:
        /**
         * Constructor from string.
         * @param value the set value.
         */
        typed_symbol_set(const std::basic_string_view<T>& value)
            : symbol_set(get_value_vector(value))
            , m_value(value.begin(), value.end())
        {
            std::sort(m_value.begin(), m_value.end());
        }

        /**
         * Constructor from initializer list.
         * @param value the set value.
         */
        typed_symbol_set(const std::initializer_list<T>& value)
            : symbol_set(get_value_vector(value))
            , m_value(value.begin(), value.end())
        {
            std::sort(m_value.begin(), m_value.end());
        }

        /**
         * Constructor from vector.
         * @param value the set value.
         */
        typed_symbol_set(const std::vector<T>& value)
            : symbol_set(get_value_vector(value))
            , m_value(value)
        {
            std::sort(m_value.begin(), m_value.end());
        }

        /**
         * Returns the set value.
         * @return the set value.
         */ 
        const std::vector<T>& get_value() const {
            return m_value;
        }

    private:
        std::vector<T> m_value;

        template <class Container>
        static std::vector<symbol_value_type> get_value_vector(const Container& container) {
            std::vector<symbol_value_type> result(container.size());
            for (size_t index = 0; index < container.size(); ++index) {
                result[index] = (static_cast<symbol_value_type>(container[index]));
            }
            return result;
        }
    };


    /**
     * Base class for a pair of symbols.
     * Used to provide a generic interface to typed symbol pairs.
     */
    class symbol_pair {
    public:
        /**
         * Virtual destructor due to inheritance.
         */ 
        virtual ~symbol_pair();

        /**
         * Returns the pair of symbols as pair of 'symbol_value_type'.
         * @return the pair of symbols as pair of 'symbol_value_type'.
         */ 
        const std::pair<symbol_value_type, symbol_value_type>& get_value() const;

    protected:
        /**
         * The constructor.
         * It can only be instantiated through the `typed_symbol_pair` class.
         * @param value the array of characters as of type 'symbol_value_type'.
         */
        symbol_pair(const std::pair<symbol_value_type, symbol_value_type>& value);

    private:
        std::pair<symbol_value_type, symbol_value_type> m_value;
    };


    /**
     * Symbol string pointer type.
     * It is a unique pointer because symbols are not be shared between grammar nodes.
     */
    using symbol_pair_ptr = std::unique_ptr<symbol_pair>;


    /**
     * A typed symbol pair.
     * Both types must be statically convertible to 'symbol_value_type'.
     * @param T1 type of first symbol.
     * @param T2 type of second symbol.
     */
    template <class T1, class T2>
    class typed_symbol_pair : public symbol_pair {
    public:
        /**
         * The constructor from single value.
         * @param value the pair value.
         */
        typed_symbol_pair(const std::pair<T1, T2>& value)
            : symbol_pair(std::make_pair(static_cast<symbol_value_type>(value.first), static_cast<symbol_value_type>(value.second)))
            , m_value(value)
        {
        }

        /**
         * The constructor from two values.
         * @param value1 the first value.
         * @param value2 the second value.
         */
        typed_symbol_pair(const T1& value1, const T2& value2)
            : typed_symbol_pair(std::make_pair(value1, value2))
        {
        }

        /**
         * Returns the pair of symbols.
         * @return the pair of symbols.
         */
        const std::pair<T1, T2>& get_value() const {
            return m_value;
        }

    private:
        std::pair<T1, T2> m_value;
    };


} //namespace parserlib


#endif //PARSERLIB_SYMBOL_HPP
