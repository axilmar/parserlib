#ifndef PARSERLIB_FUNCTION_PARSE_NODE_HPP
#define PARSERLIB_FUNCTION_PARSE_NODE_HPP


#include "parse_node.hpp"
#include "is_parse_functor.hpp"


namespace parserlib {


    /**
     * A parse node that allows a function to participate in the parsing process.
     * @param F type of function/functor to invoke.
     */
    template <class F> class function_parse_node : public parse_node<function_parse_node<F>> {
    public:
        /**
         * The constructor.
         * @param f the function/functor to invoke.
         */
        function_parse_node(const F& f) : m_function(f) {
        }

        /**
         * Invokes the supplied function or functor with the given parse context.
         * @param pc the current parse context.
         * @return true on success, false on error.
         */
        template <class ParseContext>
        bool parse(ParseContext& pc) const {
            return m_function(pc);
        }

        #ifndef NDEBUG
        std::string text() const {
            return "function";
        }
        #endif

    private:
        const F m_function;
    };


    template <class Source, class MatchId, class ErrorId, class TextPosition, class SymbolComparator, class... Extensions> class parse_context;


    /**
     * Converts a parse function to a function parse node.
     * @param f the function.
     * @return the function parse node.
     */
    template <class Source, class MatchId, class TextPosition, class SymbolComparator, class... Extensions>
    auto make_parse_node(bool (*f)(parse_context<Source, MatchId, TextPosition, SymbolComparator, Extensions...>&)) {
        return function_parse_node(f);
    }


    /**
     * Converts a parse functor to a function parse node.
     * @param f the functor.
     * @return the function parse node.
     */
    template <class T, std::enable_if_t<is_parse_functor_v<T>, bool> = true>
    auto make_parse_node(const T& f) {
        return function_parse_node(f);
    }


    /**
     * Converts a function to a function parse node.
     * @return f the function or functor to convert to a function parse node.
     * @return the function parse node.
     */
    template <class F> auto function(F&& f) {
        return function_parse_node(std::forward<F>(f));
    }


} //namespace parserlib


#endif //PARSERLIB_FUNCTION_PARSE_NODE_HPP
