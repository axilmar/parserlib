#ifndef PARSERLIB_LEXER_HPP
#define PARSERLIB_LEXER_HPP


#include "parser_engine.hpp"


namespace parserlib {


    /**
     * A lexer.
     * 
     * A class that can be used to convert an input to a series of tokens.
     * 
     * @param SourceT type of source; must be an STL-compatible container.
     * @param MatchIdT type of id for matches; it defaults to 'int', if a special enumeration is not provided.
     */
    template <typename SourceT = std::string, typename MatchIdT = int> class lexer {
    public:
        /**
         * Type of this class.
         */
        using class_type = parser_engine<SourceT, MatchIdT>;

        /**
         * Type of source.
         */
        using source_type = SourceT;

        /**
         * Type of iterator over the source.
         */
        using iterator_type = typename source_type::const_iterator;

        /**
         * Type of token id.
         */
        using match_id_type = MatchIdT;

        /**
         * Type of character.
         */
        using character_type = typename SourceT::value_type;

        /**
         * Type of parser engine to use for parsing.
         */
        using parse_engine_type = parser_engine<SourceT, MatchIdT>;

        /**
         * Source position.
         */
        class source_position {
        public:
            /**
             * The default initializing constructor.
             */
            source_position() {
            }

            /**
             * Constructor.
             * @param iterator iterator into the source.
             * @param line line in the source.
             * @param column column in the source.
             */
            source_position(const iterator_type& iterator, size_t line, size_t column)
                : m_iterator(iterator)
                , m_line(line)
                , m_column(column)
            {
            }

            /**
             * Returns the iterator for this source position.
             * @return the iterator for this source position.
             */
            const iterator_type& get_iterator() const {
                return m_iterator;
            }

            /**
             * Returns the line of this position into the source.
             * Lines start counting from 1.
             * @return the line of this position into the source.
             */
            size_t get_line() const {
                return m_line;
            }

            /**
             * Returns the column of this position into the source.
             * Columns start counting from 1.
             * @return the column of this position into the source.
             */
            size_t get_column() const {
                return m_column;
            }

        private:
            iterator_type m_iterator;
            size_t m_line;
            size_t m_column;

            friend class_type;
        };

        /**
         * A token.
         * 
         * It is the result of tokenizing the input.
         */
        class token {
        public:
            /**
             * The default initializing constructor.
             */
            token() {
            }

            /**
             * The constructor.
             * @param id id of token.
             * @param start_position start position of token.
             * @param end_position end position of token.
             */
            token(MatchIdT id, const source_position& start_position, const source_position& end_position)
                : m_id(id)
                , m_start_position(start_position)
                , m_end_position(end_position)
            {
            }

            /**
             * Returns the id of the token.
             * @return the id of the token.
             */
            match_id_type get_id() const {
                return m_id;
            }

            /**
             * Returns the start position of the token.
             * @return the start position of the token.
             */
            const source_position& get_start_position() const {
                return m_start_position;
            }

            /**
             * Returns the end position of the token.
             * @return the end position of the token.
             */
            const source_position& get_end_position() const {
                return m_end_position;
            }

            /**
             * Returns a view of the source that corresponds to this token.
             * @return a view of the source that corresponds to this token.
             */
            std::basic_string_view<character_type> get_source() const {
                const character_type* start = &*m_start_position.get_iterator();
                const size_t size = m_end_position.get_iterator() - m_start_position.get_iterator();
                return { start, size };
            }

            /**
             * Compares the token id to the given id.
             * @param id id to compare to the id of this token.
             * @return true if the ids are equal, false otherwise.
             */
            bool operator == (MatchIdT id) const {
                return m_id == id;
            }

            /**
             * Compares the token id to the given id.
             * @param id id to compare to the id of this token.
             * @return true if the ids are different, false otherwise.
             */
            bool operator != (MatchIdT id) const {
                return m_id != id;
            }

            /**
             * Compares a token's id to the given id.
             * @param id id to compare to the id of this token.
             * @param tk the token to compare the id of.
             * @return true if the ids are equal, false otherwise.
             */
            friend bool operator == (MatchIdT id, const token& tk) {
                return id == tk.m_id;
            }

            /**
             * Compares a token's id to the given id.
             * @param id id to compare to the id of this token.
             * @param tk the token to compare the id of.
             * @return true if the ids are different, false otherwise.
             */
            friend bool operator != (MatchIdT id, const token& tk) {
                return id != tk.m_id;
            }

        private:
            MatchIdT m_id;
            source_position m_start_position;
            source_position m_end_position;
        };

        /**
         * Type of token container.
         */
        using token_container_type = std::vector<token>;

        /**
         * Utility function that helps parsing an input to a series of tokens.
         * @param input the source.
         * @param grammar the root of the grammar to use for parsing.
         * @param newLineParser parser to be used for counting lines and columns.
         * @return a tuple of the following: success flag, tokens created, iterator where parsing stopped.
         */
        template <typename GrammarParserT, typename NewLineParserT>
        static std::tuple<bool, token_container_type, iterator_type> parse(SourceT& input, GrammarParserT&& grammar, NewLineParserT&& newLineParser) {
            //parse
            auto [success, ast, it] = parse_engine_type::parse(input, grammar);

            //create tokens
            token_container_type tokens;
            typename parser_engine<SourceT>::parse_context pc(input);
            source_position_container_type source_positions;
            last_position_data last_position{1, 1};
            for (const auto& an : ast) {
                source_position start_position = get_line_and_column(input.begin(), input.end(), an->get_start_position(), std::forward<NewLineParserT>(newLineParser), pc, source_positions, last_position);
                source_position end_position = get_line_and_column(input.begin(), input.end(), an->get_end_position(), std::forward<NewLineParserT>(newLineParser), pc, source_positions, last_position);
                tokens.emplace_back(an->get_id(), start_position, end_position);
            }

            //return result
            return std::make_tuple(success, std::move(tokens), it);
        }

        /**
         * Utility function that helps parsing an input to a series of tokens.
         * The newline parser is the terminal '\n'.
         * @param input the source.
         * @param grammar the root of the grammar to use for parsing.
         * @return a tuple of the following: success flag, tokens created, iterator where parsing stopped.
         */
        template <typename GrammarParserT>
        static std::tuple<bool, token_container_type, iterator_type> parse(SourceT& input, GrammarParserT&& grammar) {
            return parse(input, std::forward<GrammarParserT>(grammar), parser_engine<SourceT>::terminal('\n'));
        }

    private:
        using source_position_container_type = std::vector<source_position>;

        struct last_position_data {
            size_t line;
            size_t column;
        };

        template <typename NewLineParserT>
        static source_position get_line_and_column(
            const iterator_type& begin,
            const iterator_type& end,
            const iterator_type& curr_pos,
            NewLineParserT&& newLineParser,
            typename parser_engine<SourceT>::parse_context& pc,
            source_position_container_type& source_positions, 
            last_position_data& last_position)
        {
            //find distance between start and current position
            size_t index = (size_t)std::distance(begin, curr_pos);

            //if line and column have been identified for the given position, return it
            if (index < source_positions.size()) {
                return source_positions[index];
            }

            //compute line and column
            while (pc.get_current_position() < curr_pos) {
                iterator_type start = pc.get_current_position();

                //found non-newline
                if (newLineParser.parse(pc) == parser_engine<SourceT>::parse_result::failure) {
                    source_positions.emplace_back(pc.get_current_position(), last_position.line, last_position.column);
                    pc.increment_position();
                    ++last_position.column;
                }

                //found newline
                else {
                    size_t length = std::distance(start, pc.get_current_position());
                    for (size_t i = 0; i < length; ++i) {
                        source_positions.emplace_back(pc.get_current_position(), last_position.line, last_position.column);
                    }
                    ++last_position.line;
                    last_position.column = 1;

                }
            }

            //return position
            return source_position(pc.get_current_position(), last_position.line, last_position.column);
        }
    };


} //namespace parserlib


#endif //PARSERLIB_LEXER_HPP
