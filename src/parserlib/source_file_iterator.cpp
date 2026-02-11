#include "parserlib/source_file_iterator.hpp"


namespace parserlib {


    /**** class source_file_position ****/


    size_t source_file_position::get_line() const {
        return m_line;
    }


    size_t source_file_position::get_column() const {
        return m_column;
    }


    void source_file_position::increment_column() {
        ++m_column;
    }


    void source_file_position::increment_column(size_t count) {
        m_column += count;
    }


    void source_file_position::increment_line() {
        ++m_line;
        m_column = 1;
    }


} //namespace parserlib
