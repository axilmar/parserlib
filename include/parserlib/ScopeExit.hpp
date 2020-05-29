#ifndef PARSERLIB_SCOPEEXIT_HPP
#define PARSERLIB_SCOPEEXIT_HPP


namespace parserlib
{


    /**
        Scope exit action.
        @param T type of function to invoke at scope exit.
     */
    template <typename T> class ScopeExitAction
    {
    public:
        /**
            Constructor.
            @param action action to invoke on scope exit.
         */
        ScopeExitAction(T&& action) : m_action(std::move(action))
        {
        }

        /**
            Destructor.
            Invokes the action.
         */
        ~ScopeExitAction()
        {
            m_action();
        }

    private:
        T m_action;
    };


    /**
        Helper function to create a scope exit action.
        @param action action to invoke on scope exit.
        @return a scope exit action object.
     */
    template <typename T> ScopeExitAction<T> onExit(T&& action)
    {
        return { std::forward<T>(action) };
    }


} //namespace parserlib


#endif //PARSERLIB_SCOPEEXIT_HPP
