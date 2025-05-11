#include <iostream>
#include <fstream>

#include "memtrace.h"
#include "expressions.h"

///======================================================================
/**Constant class definition**/

Constant::Constant(double c):c(c){};

double Constant::evaluate() const{return c;}

Expression* Constant::clone() const{
    return new Constant(c);
}

void Constant::print(std::ostream& os) const{
    os<<CONSTANT<<c;
}

///======================================================================
/**Negative class definition**/

Negative::Negative(Expression* exp):exp(exp){};

double Negative::evaluate() const{return (-1)*exp->evaluate();}

Expression* Negative::clone() const{
    return new Negative(exp->clone());
}

void Negative::print(std::ostream& os) const{
    os<<NEGATIVE;
    exp->print(os);
}

Negative::~Negative(){
    delete exp;
}

///========================================================================



double sum(Expression** arr, size_t pcs){
    double res=arr[0]->evaluate();
    for(size_t i=1; i<pcs; ++i){
        res+=arr[i]->evaluate();
    }
    return res;
}

double product(Expression** arr, size_t pcs){
    double res = arr[0]->evaluate();
    for(size_t i=1; i<pcs; ++i){
        res*=arr[i]->evaluate();
    }
    return res;
}

double average(Expression** arr, size_t pcs){
    return (double) sum(arr, pcs)/pcs;
}

