#include <sstream>
#include "parserlib.hpp"
#include "preprocessor.hpp"


using namespace parserlib;


namespace java
{


    //convert input to lines
    static std::vector<java_string> convert_to_lines(const java_string& input)
    {
        //a line terminator is carriage return/line feed/end of input
        const auto line_terminator = terminal("\r\n") | '\r' | '\n' | eof;

        //any character
        const auto any_char = range(java_char(0), std::numeric_limits<java_char>::max());

        //line content is a loop of any character minus the line terminator;
        //the '== 0' means to match the line content with tag 0.
        const auto line_content = *(!line_terminator >> any_char) == 0;

        //a line is a line content followed by a line terminator
        const auto line = line_content >> line_terminator;

        //grammar is a loop of lines
        const auto grammar = *line;

        //create the parse context
        parse_context<java_string, int> pc(input);

        //parse
        const bool parse_ok = parse(grammar, pc);

        //result shall always be true
        if (!parse_ok)
        {
            throw std::logic_error("preprocessor: internal error");
        }

        //create lines
        std::vector<java_string> result;
        for (const auto& match : pc.matches)
        {
            result.push_back(match);
        }

        return std::move(result);
    }


    //coverts a unicode escape to a unicode character
    static java_char convert_unicode_escape_to_character(const java_string& unicode_escape)
    {
        std::stringstream stream;
        stream << (char)unicode_escape[2];
        stream << (char)unicode_escape[3];
        stream << (char)unicode_escape[4];
        stream << (char)unicode_escape[5];
        int c;
        stream >> std::hex >> c;
        return (java_char)c;
    }


    //process a unicode escape
    static java_string process_unicode_escape(const java_string& unicode_escape)
    {
        java_string result;

        //count the number of 'u' after the initial backslash
        size_t u_count = 0;
        for (auto it = unicode_escape.begin() + 1; *it == u'u'; ++it, ++u_count)
        {
        }

        //if u count is 1, it means the escape shall be converted to character
        if (u_count == 1)
        {
            result += convert_unicode_escape_to_character(unicode_escape);
        }

        //otherwise the unicode escape must be 'lowered', i.e. one less 'u' must be present,
        //so remove the first 'u' after the initial backslash
        else
        {
            result = unicode_escape;
            result.erase(result.begin() + 1);
        }

        return std::move(result);
    }


    //process unicode escapes for all lines
    static std::vector<java_string> process_unicode_escapes(const std::vector<java_string>& input, std::vector<error>& errors)
    {
        //match tags
        enum
        {
            DOUBLE_BACKSLASH,
            UNICODE_ESCAPE,
            ANY_CHAR
        };

        //according to the java spec, a legit unicode escape must be preceeded by an even number
        //of backslashes, and therefore this expression is used to consume two backslashes before 
        //anything else, preventing non-legit unicode escapes to be parsed as legit unicode escapes.
        const auto double_backslash = terminal(u"\\\\");

        //hex digit
        const auto hex_digit = one_of(u"0123456789abcdefABCDEF");

        //a unicode escape; not providing 4 hex digits is an error.
        const auto unicode_escape = '\\' >> +terminal(u'u') >> ~(hex_digit >> hex_digit >> hex_digit >> hex_digit);

        //any character
        const auto any_char = range(java_char(0), std::numeric_limits<java_char>::max());

        //input character
        const auto input_char 
            = double_backslash == DOUBLE_BACKSLASH
            | unicode_escape   == UNICODE_ESCAPE
            | any_char         == ANY_CHAR;

        //grammar
        const auto grammar = *input_char;

        //process lines
        std::vector<java_string> result;
        for (size_t line_index = 0; line_index < input.size(); ++line_index)
        {
            const java_string& line = input[line_index];
            parse_context<java_string, int> pc(line);

            //parse loop
            while (pc.valid())
            {
                try
                {
                    const bool parse_ok = parse(grammar, pc);

                    //catch syntax error
                    if (!parse_ok)
                    {
                        errors.push_back(error{ line_index, (size_t)(pc.position - line.begin()), u"syntax error" });
                        pc.position = pc.furthest_position;
                    }
                }

                //catch unicode escape error
                catch (const parse_error<parse_context<java_string, int>>& pe)
                {
                    errors.push_back(error{ line_index, (size_t)(pe.error_position_start() - line.begin()), u"invalid unicode escape" });
                    pc.position = pe.error_position_end();
                }
            }

            //create legit characters
            result.push_back(java_string());
            for (const auto& match : pc.matches)
            {
                switch (match.tag)
                {
                    case DOUBLE_BACKSLASH:
                        result.back() += match;
                        break;

                    case UNICODE_ESCAPE:
                        result.back() += process_unicode_escape(match);
                        break;

                    case ANY_CHAR:
                        result.back() += match;
                        break;
                }
            }
        }

        return std::move(result);
    }


    //preprocesses input
    std::vector<java_string> preprocess(const java_string& input, std::vector<error>& errors)
    {
        std::vector<java_string> lines = convert_to_lines(input);
        lines = process_unicode_escapes(lines, errors);
        return std::move(lines);
    }


} //namespace java
