#ifndef EXPRESSIONS_H_INCLUDED
#define EXPRESSIONS_H_INCLUDED

#include "memtrace.h"
#include "iexpressions.h"

/*
* Interface for binary expressions
*/
class BinaryExpression: public Expression{
protected:
    const Expression* lv;   //pointer to expression on the left of "operator"
    
    const Expression* rv;   //pointer to expression on the right of "operator"
    
    /*
    * construktor
    * @param lv pointer to left expression
    * @param rv pointer to right expression
    */
    BinaryExpression(const Expression* lv, const Expression* rv);
    
    /*
    * destruktor
    */
    ~BinaryExpression();
};

/*
* Interface for unary expressions
*/
class UnaryExpression: public Expression{
protected:
    const Expression* exp;
};

/*
* Sum binary expression (lv + rv)
*/
class Sum: public BinaryExpression{
public:
    /*
    * construktor
    * @param lv pointer to left expression
    * @param rv pointer to right expression
    */
    Sum(const Expression* lv, const Expression* rv);
    
    /*
    * @return sum of left and right operand
    */
    double evaluate() const;
    
    /*
    * @return : dinamically allocated copy of object, caller must free the allocated memory
    */
    Expression* clone() const;
};

class Product: public BinaryExpression{
public:
    /*
    * construktor
    * @param lv pointer to left expression
    * @param rv pointer to right expression
    */
    Product(const Expression* lv, const Expression* rv);
    
    /*
    * @return product of left and right operand
    */
    double evaluate() const;
    
    /*
    * @return : dinamically allocated copy of object, caller must free the allocated memory
    */
    Expression* clone() const;
};

class Constant: public Expression{
    double c;   //constant value
public:
    /*
    * constructor
    * must be initialized with a value
    */
    Constant(double c);
    
    /*
    * @return constant value
    */
    double evaluate() const;
    
    /*
    * @return : dinamically allocated copy of object, caller must free the allocated memory
    */
    Expression* clone() const;
};




#endif // EXPRESSIONS_H_INCLUDED
