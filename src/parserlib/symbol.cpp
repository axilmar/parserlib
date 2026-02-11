#include "parserlib/symbol.hpp"


namespace parserlib {


    /**** class symbol ****/


    symbol::~symbol() {
    }


    symbol_value_type symbol::get_value() const {
        return m_value;
    }


    symbol::symbol(symbol_value_type value) 
        : m_value(value)
    {
    }



    /**** class symbol_string ****/


    symbol_string::~symbol_string() {
    }


    const std::vector<symbol_value_type>& symbol_string::get_value() const {
        return m_value;
    }


    symbol_string::symbol_string(const std::vector<symbol_value_type>& value)
        : m_value(value)
    {
    }


    /**** class symbol_set ****/


    symbol_set::~symbol_set() {
    }


    const std::vector<symbol_value_type>& symbol_set::get_value() const {
        return m_value;
    }


    symbol_set::symbol_set(const std::vector<symbol_value_type>& value)
        : m_value(value)
    {
        std::sort(m_value.begin(), m_value.end());
    }


    /**** class symbol_pair ****/


    symbol_pair::~symbol_pair() {
    }


    const std::pair<symbol_value_type, symbol_value_type>& symbol_pair::get_value() const {
        return m_value;
    }


    symbol_pair::symbol_pair(const std::pair<symbol_value_type, symbol_value_type>& value)
        : m_value(value)
    {
    }


} //namespace parserlib
