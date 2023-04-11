#ifndef PARSERLIB_SVO_HPP
#define PARSERLIB_SVO_HPP


namespace parserlib {


    /**
     * Scoped Variable Override.
     * @param T type of variable to restore at exit.
     */
    template <class T> class SVO {
    public:
        /**
         * Constructor.
         * @param var variable.
         * @param newVal new value of the variable.
         */
        template <class V>
        SVO(T& var, const V& newVal) : m_val(var), m_var(var) {
            var = newVal;
        }

        /**
         * Restores the variable to its stored value.
         */
        ~SVO() {
            m_var = m_val;
        }

    private:
        const T m_val;
        T& m_var;
    };


} //namespace parserlib


#endif //PARSERLIB_SVO_HPP
