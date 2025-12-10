#ifndef PARSERLIB_CHOICE_PARSE_NODE_HPP
#define PARSERLIB_CHOICE_PARSE_NODE_HPP


#include "vector_concatenation.hpp"
#include "parse_node.hpp"


namespace parserlib {


    class choice_parse_node : public interface::parse_node {
    public:
        using container = std::vector<parserlib::parse_node>;

        choice_parse_node(container&& children) : m_children(std::move(children)) {
        }

        bool parse(interface::parse_context& pc) const override {
            for (const auto& pn : m_children) {
                if (parse_child(pc, pn)) {
                    return true;
                }
            }
            return false;
        }

        const container& children() const {
            return m_children;
        }

    private:
        const container m_children;

        bool parse_child(interface::parse_context& pc, const parserlib::parse_node& child) const {
            pc.push_state();
            try {
                if (child.parse(pc)) {
                    return true;
                }
            }
            catch (...) {
                pc.pop_state();
                throw;
            }
            pc.pop_state();
            return false;
        }
    };


    inline parse_node operator | (const parse_node& left, const parse_node& right) {
        const choice_parse_node* left_choice = dynamic_cast<const choice_parse_node*>(left.get().get());
        const choice_parse_node* right_choice = dynamic_cast<const choice_parse_node*>(right.get().get());
        if (left_choice && right_choice) {
            return interface::create_parse_node<choice_parse_node>(concatenate_vectors(left_choice->children(), right_choice->children()));
        }
        else if (left_choice) {
            return interface::create_parse_node<choice_parse_node>(concatenate_vector_with_element(left_choice->children(), right));
        }
        else if (right_choice) {
            return interface::create_parse_node<choice_parse_node>(concatenate_element_with_vector(left, right_choice->children()));
        }
        else {
            return interface::create_parse_node<choice_parse_node>(choice_parse_node::container{left, right});
        }
    }


} //namespace parserlib


#endif //PARSERLIB_CHOICE_PARSE_NODE_HPP
