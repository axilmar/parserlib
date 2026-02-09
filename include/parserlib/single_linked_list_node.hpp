#ifndef PARSERLIB_SINGLE_LINKED_LIST_NODE_HPP
#define PARSERLIB_SINGLE_LINKED_LIST_NODE_HPP


#include <memory>


namespace parserlib {


    /**
     * Base class for single-linked list nodes.
     * Nodes are shared via reference counting.
     * @param Derived the subclass of this node.
     */
    template <class Derived>
    class single_linked_list_node : public std::enable_shared_from_this<Derived> {
    public:
        /**
         * Returns the next node.
         * @return the next node.
         */
        const std::shared_ptr<Derived>& get_next() const {
            return m_next;
        }

        /**
         * Sets the next node.
         * @param next the new next node.
         */
        void set_next(const std::shared_ptr<Derived>& next) {
            m_next = next;
        }

        /**
         * Returns the last node for the single-linked list that starts with this node.
         * @return the last node.
         */
        std::shared_ptr<Derived> get_last() const {
            std::shared_ptr<Derived> node = const_cast<Derived*>(static_cast<const Derived*>(this))->shared_from_this();
            for (; node->m_next; node = node->m_next) {
            }
            return node;
        }

        /**
         * Sets the last node.
         * It appends the given node as next to the last node of this list.
         * @param last the last node.
         */
        void set_last(const std::shared_ptr<Derived>& last) {
            if (m_next) {
                m_next->set_last(last);
            }
            else {
                m_next = last;
            }
        }

    private:
        std::shared_ptr<Derived> m_next;
    };


} //namespace parserlib


#endif //PARSERLIB_SINGLE_LINKED_LIST_NODE_HPP
