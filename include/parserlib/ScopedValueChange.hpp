#ifndef PARSERLIB_SCOPEDVALUECHANGE_HPP
#define PARSERLIB_SCOPEDVALUECHANGE_HPP


namespace parserlib
{


    /**
        A value change that is recalled on scope exit.
        @param T type of value.
     */
    template <typename T> class ScopedValueChange
    {
    public:
        /**
            Constructor.
            @param value value on scope enter.
            @param variable function to obtain the variable reference.
         */
        template <typename F>
        ScopedValueChange(const T& value, F&& variable)
            : m_value(value)
            , m_variable(*variable())
        {
        }

        /**
            Destructor.
            It restores the variable to the value at scope enter.
         */
        ~ScopedValueChange()
        {
            m_variable = m_value;
        }

    private:
        const T m_value;
        T& m_variable;
    };


    /**
        Helper function for creating a scoped value change.
        @param value value to set the variable to on scope exit.
        @param func function to invoke to obtain the variable.
        @return a scope value change object.
     */
    template <typename T, typename F> ScopedValueChange<T> scopedValueChange(const T& value, F&& variable)
    {
        return { value, std::forward<F>(variable) };
    }


} //namespace parserlib


#endif //PARSERLIB_SCOPEDVALUECHANGE_HPP
