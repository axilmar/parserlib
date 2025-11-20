#ifndef PARSERLIB_INFINITE_RECURSION_EXCEPTION_HPP
#define PARSERLIB_INFINITE_RECURSION_EXCEPTION_HPP


#include <string>
#include <stdexcept>


namespace parserlib {


    /**
     * Exception thrown when there is an infinite recursion.
     */
    class infinite_recursion_exception : public std::runtime_error {
    public:
        infinite_recursion_exception(const std::string& source, const std::string& position)
            : std::runtime_error("")
            , m_source(source)
            , m_position(position)
            , m_what("Infinite recursion exception at " + m_position + " from " + m_source)
        {
        }

        /**
         * Returns the 'what' of the exception.
         * @return the 'what' of the exception.
         */
        const char* what() const override {
            return m_what.c_str();
        }

    private:
        const std::string m_source;
        const std::string m_position;
        const std::string m_what;
    };


} //namespace parserlib


#endif //PARSERLIB_INFINITE_RECURSION_EXCEPTION_HPP
