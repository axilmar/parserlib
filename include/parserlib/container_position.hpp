#ifndef PARSERLIB_CONTAINER_POSITION_HPP
#define PARSERLIB_CONTAINER_POSITION_HPP


namespace parserlib {


    class container_position {
    public:
        size_t get_index() const {
            return m_index;
        }

        bool operator < (const container_position& other) const {
            return m_index < other.m_index;
        }

        container_position& operator ++() {
            ++m_index;
            return *this;
        }

        container_position& operator += (size_t count) {
            m_index += count;
            return *this;
        }

    private:
        size_t m_index{ 0 };
    };


} //namespace parserlib


#endif //PARSERLIB_CONTAINER_POSITION_HPP
