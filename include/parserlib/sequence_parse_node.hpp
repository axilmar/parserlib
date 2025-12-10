#ifndef PARSERLIB_SEQUENCE_PARSE_NODE_HPP
#define PARSERLIB_SEQUENCE_PARSE_NODE_HPP


#include "vector_concatenation.hpp"
#include "parse_node.hpp"


namespace parserlib {


    class sequence_parse_node : public interface::parse_node {
    public:
        using container = std::vector<parserlib::parse_node>;

        sequence_parse_node(container&& children) : m_children(std::move(children)) {
        }

        bool parse(interface::parse_context& pc) const override {
            pc.push_state();
            for (const auto& pn : m_children) {
                try {
                    if (!pn.parse(pc)) {
                        pc.pop_state();
                        return false;
                    }
                }
                catch (...) {
                    pc.pop_state();
                    throw;
                }
            }
            return true;
        }

        const container& children() const {
            return m_children;
        }

    private:
        const container m_children;
    };


    inline parse_node operator >> (const parse_node& left, const parse_node& right) {
        const sequence_parse_node* left_sequence = dynamic_cast<const sequence_parse_node*>(left.get().get());
        const sequence_parse_node* right_sequence = dynamic_cast<const sequence_parse_node*>(right.get().get());
        if (left_sequence && right_sequence) {
            return interface::create_parse_node<sequence_parse_node>(concatenate_vectors(left_sequence->children(), right_sequence->children()));
        }
        else if (left_sequence) {
            return interface::create_parse_node<sequence_parse_node>(concatenate_vector_with_element(left_sequence->children(), right));
        }
        else if (right_sequence) {
            return interface::create_parse_node<sequence_parse_node>(concatenate_element_with_vector(left, right_sequence->children()));
        }
        else {
            return interface::create_parse_node<sequence_parse_node>(sequence_parse_node::container{left, right});
        }
    }


} //namespace parserlib


#endif //PARSERLIB_SEQUENCE_PARSE_NODE_HPP
