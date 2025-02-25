#ifndef PARSERLIB_UTF8_STRING_HPP
#define PARSERLIB_UTF8_STRING_HPP


#include <string>
#include <cstdint>
#include <stdexcept>


namespace parserlib {


    /**
     * An extension of std::string which provides a const iterator that knows how to decode UTF8 into unicode code points.
     * Useful for parsing utf8 strings; it does not provide other utf8 functionality.
     * @param String string class to derive from.
     */
    template <class String = std::string>
    class utf8_string : public String {
    public:
        /**
         * Uses the standard string constructors.
         */
        using std::string::string;

        /**
         * An iterator class that knows how to read unicode code points from a UTF8 encoding.
         */
        class const_iterator : public std::string::const_iterator {
        public:
            /**
             * It uses the constructors of the standard string iterator.
             */
            using std::string::const_iterator::const_iterator;

            /**
             * Returns the unicode codepoint at the specific position.
             * It synthesizes the code point value from 1, 2, 3 or 4 byte values 
             * present at the current iterator position.
             * @return current unicode codepoint.
             * @exception std::runtime_error if the unicode code point cannot be decoded due to invalid utf.
             */
            uint32_t operator *() const {
                const uint32_t leader_byte = (unsigned char)*it();

                //single byte (range 0-127)
                if (leader_byte < 128) {
                    return leader_byte;
                }

                if (leader_byte < 192) {
                    goto ERROR;
                }

                //two bytes (range 192-223)
                if (leader_byte < 224) {
                    const uint32_t trailing_byte_1 = (unsigned char) *(it() + 1);
                    const uint32_t result = ((leader_byte - 192) * 64) + (trailing_byte_1 - 128);
                    return result;
                }

                //three bytes (range 224-239)
                if (leader_byte < 240) {
                    const uint32_t trailing_byte_1 = (unsigned char) *(it() + 1);
                    const uint32_t trailing_byte_2 = (unsigned char) *(it() + 2);
                    const uint32_t result = ((leader_byte - 224) * 4096) + ((trailing_byte_1 - 128) * 64) + (trailing_byte_2 - 128);
                    return result;
                }

                //four bytes (range 240-247)
                if (leader_byte < 248) {
                    const uint32_t trailing_byte_1 = (unsigned char) *(it() + 1);
                    const uint32_t trailing_byte_2 = (unsigned char) *(it() + 2);
                    const uint32_t trailing_byte_3 = (unsigned char) *(it() + 3);
                    const uint32_t result = ((leader_byte - 240) * 262144) + ((trailing_byte_1 - 128) * 4096) + ((trailing_byte_2 - 128) * 64) + (trailing_byte_3 - 128);
                    return result;
                }

                ERROR:
                throw std::runtime_error("invalid UTF-8 string");
            }

            /**
             * Increments the iterator by 1, 2, 3 or 4 bytes, depending on the utf8 decoding.
             * @return reference to this.
             * @exception std::runtime_error if the unicode code point cannot be decoded due to invalid utf.
             */
            const_iterator& operator ++() {
                const uint32_t leader_byte = (unsigned char)*it();

                //single byte (range 0-127)
                if (leader_byte < 128) {
                    ++it();
                    return *this;
                }

                if (leader_byte < 192) {
                    goto ERROR;
                }

                //two bytes (range 192-223)
                if (leader_byte < 224) {
                    it() += 2;
                    return *this;
                }

                //three bytes (range 224-239)
                if (leader_byte < 240) {
                    it() += 3;
                    return *this;
                }

                //four bytes (range 240-247)
                if (leader_byte < 248) {
                    it() += 4;
                    return *this;
                }

                return *this;

                ERROR:
                throw std::runtime_error("invalid UTF-8 string");
            }

            /**
             * Checks equality with given iterator.
             * @param it the iterator to compare to this iterator.
             * @return true if they are equal, false otherwise.
             */
            bool operator == (const const_iterator& it) const {
                return this->it() == it.it();
            }

            /**
             * Checks difference with given iterator.
             * @param it the iterator to compare to this iterator.
             * @return true if they are different, false otherwise.
             */
            bool operator != (const const_iterator& it) const {
                return this->it() != it.it();
            }

            /**
             * Checks if this iterator is less than the given iterator.
             * @param it the iterator to compare to this iterator.
             * @return true if they are equal, false otherwise.
             */
            bool operator < (const const_iterator& it) const {
                return this->it() < it.it();
            }

            /**
             * Checks if this iterator is less than or equal to the given iterator.
             * @param it the iterator to compare to this iterator.
             * @return true if they are equal, false otherwise.
             */
            bool operator <= (const const_iterator& it) const {
                return this->it() <= it.it();
            }

            /**
             * Checks if this iterator is greater than the given iterator.
             * @param it the iterator to compare to this iterator.
             * @return true if they are equal, false otherwise.
             */
            bool operator > (const const_iterator& it) const {
                return this->it() > it.it();
            }

            /**
             * Checks if this iterator is greater than or equal to the given iterator.
             * @param it the iterator to compare to this iterator.
             * @return true if they are equal, false otherwise.
             */
            bool operator >= (const const_iterator& it) const {
                return this->it() >= it.it();
            }

        private:
            const_iterator(const std::string::const_iterator& it) :std::string::const_iterator(it) {
            }

            const std::string::const_iterator& it() const {
                return *this;
            }

            std::string::const_iterator& it() {
                return *this;
            }

            friend class utf8_string;
        };

        /**
         * Returns an iterator that points to the first element of the sequence.
         * @return an iterator that points to the first element of the sequence.
         */
        const_iterator begin() const {
            return std::string::begin();
        }

        /**
         * Returns an iterator that points to the end of the sequence.
         * @return an iterator that points to the end of the sequence.
         */
        const const_iterator end() const {
            return std::string::end();
        }
    };


} //namespace parserlib


#endif //PARSERLIB_UTF8_STRING_HPP
