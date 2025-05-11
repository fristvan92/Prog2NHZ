#include "memtrace.h"
#include "expressions.h"

///======================================================================
/**BinaryExpression abstract class definition**/

BinaryExpression::BinaryExpression(const Expression* lv, const Expression* rv):lv(lv), rv(rv){}

BinaryExpression::~BinaryExpression(){
    delete lv;
    delete rv;
}


///======================================================================
/**Sum class definition**/

Sum::Sum(const Expression* lv, const Expression* rv):BinaryExpression(lv, rv){}

double Sum::evaluate() const{
    return lv->evaluate()+rv->evaluate();
}

Expression* Sum::clone() const{
    return new Sum(lv->clone(), rv->clone());
}

///======================================================================
/**Product class definition**/

Product::Product(const Expression* lv, const Expression* rv):BinaryExpression(lv, rv){}

double Product::evaluate() const{
    return lv->evaluate() * rv->evaluate();
}

Expression* Product::clone() const{
    return new Product(lv->clone(), rv->clone());
}

///======================================================================
/**Constant class definition**/

Constant::Constant(double c):c(c){};

double Constant::evaluate() const{return c;}

Expression* Constant::clone() const{
    return new Constant(c);
}

