#ifndef PARSERLIB_RULESTATE_HPP
#define PARSERLIB_RULESTATE_HPP


namespace parserlib {


    /**
     * A rule's state.
     * @param ParseContextType type of parse context.
     */
    template <class ParseContextType> class RuleState {
    public:
        /**
         * Status of the rule. 
         */
        enum Status {
            /**
             * Normal state.
             */
            Normal
        };
        
        /**
         * Constructor.
         * @param status initial status.
         * @param position initial position.
         */
        RuleState(Status status = Normal, const typename ParseContextType::PositionType& position = {}) 
            : m_status(status), m_position(position) {
        }

        /**
         * Returns the rule's current status.
         * @return the rule's current status.
         */
        Status status() const {
            return m_status;
        }

        /**
         * Sets the rule's current status.
         * @param status the new status of the rule.
         */
        void setStatus(Status status) {
            m_status = status;
        }

        /**
         * Returns the last position into source that the rule was called to parse.
         * @param returns the last known position for the rule.
         */
        const typename ParseContextType::PositionType& position() const {
            return m_position;
        }

        /**
         * Sets the parse position of the rule.
         * @param position the parse position.
         */
        void setPosition(const typename ParseContextType::PositionType& position) {
            m_position = position;
        }

    private:
        Status m_status;
        typename ParseContextType::PositionType m_position;
    };


} //namespace parserlib


#endif //PARSERLIB_RULESTATE_HPP
