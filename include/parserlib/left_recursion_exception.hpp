#ifndef PARSERLIB_LEFT_RECURSION_EXCEPTION_HPP
#define PARSERLIB_LEFT_RECURSION_EXCEPTION_HPP


namespace parserlib {


    /**
     * An exception thrown when a left recursion is found.
     * It contains the address of the parse node that caused the exception.
     * (Only the address of a parse node is required for handling a left recursion.)
     */
    class left_recursion_exception {
    public:
        /**
         * The constructor.
         * @param pn the parse node for which the exception is for.
         */
        left_recursion_exception(const void* pn)
            : m_parse_node(pn)
        {
        }

        /**
         * Returns the parse node that the exception is for.
         * @return the parse node that the exception is for.
         */
        const void* get_parse_node() const noexcept {
            return m_parse_node;
        }

    private:
        const void* m_parse_node;
    };


} //namespace parserlib


#endif //PARSERLIB_LEFT_RECURSION_EXCEPTION_HPP
