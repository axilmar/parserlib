#include <sstream>
#include "character_parser.hpp"


using namespace parserlib;


namespace java
{


    static const char* const TAG_DOUBLE_BACKSLASH    = "double_backslash"   ;
    static const char* const TAG_UNICODE_ESCAPE      = "unicode_escape"     ;
    static const char* const TAG_LINE_TERMINATOR     = "line_terminator"    ;
    static const char* const TAG_RAW_INPUT_CHARACTER = "raw_input_character";


    static const auto hex_digit
        = one_of(u"0123456789abcdefABCDEF");


    static const auto double_backslash
        = terminal(u"\\\\");


    static const auto unicode_escape
        = u'\\' >> +terminal(u'u') >> ~(hex_digit >> hex_digit >> hex_digit >> hex_digit);


    static const auto line_terminator
        = terminal(u"\r\n")
        | u'\n'
        | u'\r';


    static const auto raw_input_character
        = range(char16_t(0), std::numeric_limits<char16_t>::max());


    static const auto unicode_input_character
        = double_backslash        == TAG_DOUBLE_BACKSLASH
        | unicode_escape          == TAG_UNICODE_ESCAPE
        | line_terminator         == TAG_LINE_TERMINATOR
        | raw_input_character     == TAG_RAW_INPUT_CHARACTER;


    static const auto ignored_character
        = terminal((char16_t)0x1a);


    static const auto character_grammar
        = *unicode_input_character >> !ignored_character;


    //parse unicode escape
    static std::u16string parse_unicode_escape(const std::u16string& input)
    {
        std::u16string result;

        //first character must be a backslash
        if (input[0] != u'\\')
        {
            throw std::logic_error("invalid unicode escape");
        }

        //find range of 'u'
        auto start = input.begin() + 1;
        auto end = start;
        for (; *end == u'u'; ++end) {}
        const auto u_length = end - start;

        //if u_length is 1, then convert the input to an actual character
        if (u_length == 1)
        {
            std::stringstream stream;
            const std::basic_string_view<char16_t> sv(&*end, input.end() - end);
            for (char16_t c : sv)
            {
                stream << (char)c;
            }
            int c;
            stream >> std::hex >> c;
            const char16_t ch = (char16_t)c;
            result.push_back(c);
        }

        //else if u_length is greater than 1, then put one less 'u'
        else if (u_length > 1)
        {
            std::basic_stringstream<char16_t> stream;
            stream << u'\\';
            stream << std::u16string(u_length - 1, u'u');
            const std::basic_string_view<char16_t> sv(&*end, input.end() - end);
            stream << sv;
            result += stream.str();
        }

        //else if u_length is zero, then there is an error
        else
        {
            throw std::logic_error("invalid unicode escape syntax");
        }

        return std::move(result);
    }


    //creates the appropriate maches
    static void create_matches(parse_context<std::u16string>& pc, java_string& result, int& line, int& column)
    {
        for (const auto& m : pc.matches)
        {
            if (m.tag == TAG_DOUBLE_BACKSLASH)
            {
                result.push_back('\\');
                result.push_back('\\');
                column += 2;
            }

            else if (m.tag == TAG_UNICODE_ESCAPE)
            {
                const auto s = parse_unicode_escape(m);
                result.insert(result.end(), s.begin(), s.end());
                column += result.size();
            }

            else if (m.tag == TAG_LINE_TERMINATOR)
            {
                ++line;
                column = 0;
                result.push_back(u'\n');
            }

            else
            {
                const auto s = m.input();
                result.insert(result.end(), s.begin(), s.end());
                column += s.size();
            }
        }

        pc.matches.clear();
    }


    //parse input into a java string
    java_string parse_characters(const std::u16string& input, std::vector<Error>& errors)
    {
        auto pc = parse_context(input);

        java_string result;

        int line = 0, column = 0;

        while (pc.valid())
        {
            bool success;

            try
            {
                success = parse(character_grammar, pc);
            }
            catch (const parse_error<parse_context<std::u16string>>& error)
            {
                success = false;
                create_matches(pc, result, line, column);
                
                if (error.tag() == TAG_UNICODE_ESCAPE)
                {
                    errors.emplace_back(line, column, "invalid unicode escape");
                }
                
                pc.position = pc.furthest_position;                
                continue;
            }

            create_matches(pc, result, line, column);

            if (success) break;

            errors.emplace_back(line, column, "syntax error");
            pc.position = pc.furthest_position;                
        }

        return std::move(result);
    }


} //namespace java
