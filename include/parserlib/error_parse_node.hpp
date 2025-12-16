#ifndef PARSERLIB_ERROR_PARSE_NODE_HPP
#define PARSERLIB_ERROR_PARSE_NODE_HPP


#include <type_traits>
#include "parent_parse_node.hpp"


namespace parserlib {


    /**
     * A parse node that adds a error to a parse context when its child node parses successfully.
     * @param ErrorId type of error id.
     * @param Child type of parse node to use as a child.
     */
    template <class ErrorId, class Child>
    class error_parse_node : public parent_parse_node<error_parse_node<ErrorId, Child>, Child> {
    public:
        /** The parent type. */
        using parent_type = parent_parse_node<error_parse_node<ErrorId, Child>, Child>;

        /**
         * The constructor.
         * @param id the id to put to the parse context as a error id.
         * @param child the parse node to use as a child.
         */
        error_parse_node(const ErrorId& id, const Child& child)
            : parent_type(child)
            , m_id(id)
        {
        }

        /**
         * Invokes the child parse node to parse.
         * If the child parses successfully, then it adds a error
         * to the given parse context.
         * @param pc the parse context to add a error to.
         * @return true on success, false on failure.
         */
        template <class ParseContext>
        bool parse(ParseContext& pc) const {
            if (parent_type::get_children().parse(pc)) {
                pc.add_error(static_cast<int>(m_id));
            }
            return false;
        }

    private:
        ErrorId m_id;
    };


    struct skip_error_parse_node_tag {
    };


    /**
     * A parse node that skips input until the child parse node parses successfully.
     * The parse position after this node parses is set to the last parse position
     * before the child parse node parsed successfully.
     * @param Child Type of the child parse node.
     */
    template <class Child>
    class skip_before_parse_node : public parent_parse_node<skip_before_parse_node<Child>, Child>, public skip_error_parse_node_tag {
    public:
        /** The parent parse node type. */
        using parent_type = parent_parse_node<skip_before_parse_node<Child>, Child>;

        /**
         * The constructor.
         * @param child the child.
         */
        skip_before_parse_node(const Child& child)
            : parent_type(child)
        {
        }

        /**
         * It invokes the child node to parse in a loop, 
         * until the child parse node parses successfully,
         * or the end of input is reached.
         * On return, the parse position is set to the parse position
         * before the successful parsing by the child parse node.
         * @param pc the context to pass to the child.
         * @return always true.
         */
        template <class ParseContext>
        bool parse(ParseContext& pc) const {
            while (pc.is_valid_parse_position()) {
                pc.save_parse_state();
                try {
                    const bool result = parent_type::get_children().parse(pc);
                    pc.reject_parse_state();
                    if (result) {
                        return true;
                    }
                }
                catch (...) {
                    pc.reject_parse_state();
                    throw;
                }
                pc.increment_parse_position();
            }
            return true;
        }
    };


    /**
     * A parse node that skips input until the child parse node parses successfully.
     * The parse position after this node parses is set to the parse position
     * after the child parse node parsed successfully.
     * @param Child Type of the child parse node.
     */
    template <class Child>
    class skip_after_parse_node : public parent_parse_node<skip_after_parse_node<Child>, Child>, public skip_error_parse_node_tag {
    public:
        /** The parent parse node type. */
        using parent_type = parent_parse_node<skip_after_parse_node<Child>, Child>;

        /**
         * The constructor.
         * @param child the child.
         */
        skip_after_parse_node(const Child& child)
            : parent_type(child)
        {
        }

        /**
         * It invokes the child node to parse in a loop, 
         * until the child parse node parses successfully,
         * or the end of input is reached.
         * On return, the parse position is set to the parse position
         * after the successful parsing by the child parse node.
         * @param pc the context to pass to the child.
         * @return always true.
         */
        template <class ParseContext>
        bool parse(ParseContext& pc) const {
            while (pc.is_valid_parse_position()) {
                pc.save_parse_state();
                try {
                    const bool result = parent_type::get_children().parse(pc);
                    if (result) {
                        pc.accept_parse_state();
                        return true;
                    }
                    pc.reject_parse_state();
                }
                catch (...) {
                    pc.reject_parse_state();
                    throw;
                }
                pc.increment_parse_position();
            }
            return true;
        }
    };


    /**
     * Creates an error parse node for the specific error id and child.
     * @param child the child parse node; it must be a skip error parse node.
     * @param id id of the error.
     * @return a error parse node for the given id and child.
     */
    template <class ErrorId, class Child, std::enable_if_t<std::is_base_of_v<skip_error_parse_node_tag, Child>, bool> = true>
    error_parse_node<ErrorId, Child> error(const ErrorId& id, const parse_node<Child>& child) {
        return error_parse_node<ErrorId, Child>(id, *child.get_impl());
    }


    /**
     * Creates a skip before parse node.
     * @param value value or parse node to create a skip before parse node.
     * @return a skip before parse node.
     */
    template <class T>
    auto skip_before(const T& value) {
        return skip_before_parse_node(make_parse_node(value));
    }


    /**
     * Creates a skip after parse node.
     * @param value value or parse node to create a skip after parse node.
     * @return a skip after parse node.
     */
    template <class T>
    auto skip_after(const T& value) {
        return skip_after_parse_node(make_parse_node(value));
    }


} //namespace parserlib


#endif //PARSERLIB_ERROR_PARSE_NODE_HPP
