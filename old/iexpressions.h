#ifndef IEXPRESSIONS_H_INCLUDED
#define IEXPRESSIONS_H_INCLUDED

#include "memtrace.h"

/*
* Interface for Expression classes
*/
class Expression{
public:
    /*
    * @return is the value of the expression
    */
    virtual double evaluate() const =0;
    
    /*
    * @return is a dinamically allocated copy of the expression, caller must free the memory
    */
    virtual Expression* clone() const =0;
    
    /*
    * destruktor
    */
    virtual ~Expression(){};
};


#endif //IEXPRESSIONS_H_INCLUDED
