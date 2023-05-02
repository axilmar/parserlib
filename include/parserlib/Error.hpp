#ifndef PARSERLIB_ERROR_HPP
#define PARSERLIB_ERROR_HPP


#include <string>
#include <vector>


namespace parserlib {


    /**
     * Basic enumeration for error types.
     */
    enum class ErrorType {
        /**
         * The one and only error type recognized by this library.
         */
        SyntaxError,

        /**
         * First error type available to external libraries.
         */
        User
    };


    /**
     * Definition of error.
     * @param SourcePositionType type of source position.
     */
    template <class SourcePositionType> class Error {
    public:
        /**
         * The default constructor.
         */
        Error() {
        }

        /**
         * Constructor from parameters.
         * @param type error type; stored as 'int' internally.
         * @param pos source position type.
         * @param msg error message.
         */
        template <class ErrorType> Error(ErrorType type, const SourcePositionType& pos, std::string&& msg)
            : m_type(static_cast<int>(type)), m_position(pos), m_message(std::move(msg))
        {
        }

        /**
         * Returns the error type.
         * @return the error type.
         */
        int type() const {
            return m_type;
        }

        /**
         * Returns the error position.
         * @return the error position.
         */
        const SourcePositionType& position() const {
            return m_position;
        }

        /**
         * Returns the error message.
         * @return the error message.
         */
        const std::string& message() const {
            return m_message;
        }

    private:
        int m_type;
        SourcePositionType m_position;
        std::string m_message;
    };


    /**
     * Type of error container.
     * @param SourcePositionType type of source position.
     */
    template <class SourcePositionType> using ErrorContainer = std::vector<Error<SourcePositionType>>;


    /**
     * Helper function for creating an error instance.
     * @param type error type; stored as 'int' internally.
     * @param pos source position type.
     * @param msg error message.
     * @return an error instance.
     */
    template <class ErrorType, class SourcePositionType> Error<SourcePositionType> makeError(ErrorType type, const SourcePositionType& pos, std::string&& msg) {
        return { type, pos, std::move(msg) };
    }


} //namespace parserlib


#endif //PARSERLIB_ERROR_HPP
