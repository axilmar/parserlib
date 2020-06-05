#ifndef PARSERLIB__PARSE_RESULT__HPP
#define PARSERLIB__PARSE_RESULT__HPP


namespace parserlib
{


    ///parse result.
    class parse_result
    {
    public:
        ///rejected result.
        static inline constexpr bool rejected = false;

        ///accepted result.
        static inline constexpr bool accepted = true;

        /**
            Constructor.
            @param v initial value.
         */
        parse_result(const bool v = rejected) : m_value(v)
        {
        }

        /**
            Automatic conversion to bool.
            @return rejected/accepted.
         */
        operator bool() const
        {
            return m_value;
        }

    private:
        //value
        bool m_value;
    };


} //namespace parserlib


#endif //PARSERLIB__PARSE_RESULT__HPP
