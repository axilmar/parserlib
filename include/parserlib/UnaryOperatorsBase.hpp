#ifndef PARSERLIB_UNARYOPERATORSBASE_HPP
#define PARSERLIB_UNARYOPERATORSBASE_HPP


namespace parserlib
{


    template <typename ExpressionType> class ZeroOrMore;
    template <typename ExpressionType> class OneOrMore;
    template <typename ExpressionType> class Optional;
    template <typename ExpressionType> class LogicalAnd;
    template <typename ExpressionType> class LogicalNot;


    /**
        Class that provides unary operations to its base classes.
        @param DerivedClass type of derived class.
     */
    template <typename DerivedClass> class UnaryOperatorsBase
    {
    public:
        /**
            Returns a zero-or-more expression out of this.
            @return a zero-or-more expression out of this.
         */
        ZeroOrMore<DerivedClass> operator *() const
        {
            return { *static_cast<const DerivedClass *>(this) };
        }

        /**
            Returns an one-or-more expression out of this.
            @return an one-or-more expression out of this.
         */
        OneOrMore<DerivedClass> operator +() const
        {
            return { *static_cast<const DerivedClass *>(this) };
        }

        /**
            Returns an optional expression out of this.
            @return an optional expression out of this.
         */
        Optional<DerivedClass> operator -() const
        {
            return { *static_cast<const DerivedClass *>(this) };
        }

        /**
            Returns a logical AND expression out of this.
            @return a logical AND expression out of this.
         */
        LogicalAnd<DerivedClass> operator &() const
        {
            return { *static_cast<const DerivedClass *>(this) };
        }

        /**
            Returns a logical NOT expression out of this.
            @return a logical NOT expression out of this.
         */
        LogicalNot<DerivedClass> operator !() const
        {
            return { *static_cast<const DerivedClass *>(this) };
        }
    };


} //namespace parserlib


#endif //PARSERLIB_UNARYOPERATORSBASE_HPP
