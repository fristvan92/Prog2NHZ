#ifndef EXPRESSIONS_H_INCLUDED
#define EXPRESSIONS_H_INCLUDED

#include <iostream>
#include <fstream>
#include "memtrace.h"
#include "CommandIdentifiers.h"

/*
* Interface for Expression classes
*/
struct Expression{
    /*
    * @return is the value of the expression
    */
    virtual double evaluate() const =0;

    /*
    * @return is a dinamically allocated copy of the expression, caller must free the memory
    */
    virtual Expression* clone() const =0;

    virtual void print(std::ostream& os) const=0;

    /*
    * destruktor
    */
    virtual ~Expression(){};
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

    void print(std::ostream& os) const;
};

class Negative: public Expression{
    Expression* exp;
public:
    /*
    * constructor
    * must be initialized with a value
    */
    Negative(Expression* exp);

    /*
    * @return constant value
    */
    double evaluate() const;

    /*
    * @return : dinamically allocated copy of object, caller must free the allocated memory
    */
    Expression* clone() const;

    void print(std::ostream& os) const;

    ~Negative();
};

///===================================================================
class Function: public Expression{
    double (*fptr) (Expression**, size_t);
    char id;
    Expression** exps;
    size_t pcs;
public:
    Function(double (*fptr) (Expression**, size_t), char id):fptr(fptr),id(id),exps(NULL),pcs(0){};

    void add(Expression* exp){
        Expression** tmp = new Expression*[pcs+1];
        for(size_t i=0; i<pcs; ++i){
            tmp[i]=exps[i];
        }
        if(exps!=NULL) delete[] exps;
        tmp[pcs++]=exp;
        exps=tmp;
    }

    double evaluate() const{
        return fptr(exps, pcs);
    }

    Expression* clone() const{
        Function* res = new Function(fptr, id);
        for(size_t i=0; i<pcs; ++i){
            res->add(exps[i]->clone());
        }
        return res;
    }

    void print(std::ostream& os) const{
        os<<id<<OBRACKET;
        for(size_t i=0; i<pcs; ++i){
            exps[i]->print(os);
            if(i<pcs-1) os<<COMMANDSEPARATOR;
        }
        os<<CBRACKET;
    }

    ~Function(){
        for(size_t i=0; i<pcs; ++i){
            delete exps[i];
        }
        delete[] exps;
    }
};

///===================================================================
double sum(Expression** arr, size_t pcs);
double product(Expression** arr, size_t pcs);
double average(Expression** arr, size_t pcs);





#endif // EXPRESSIONS_H_INCLUDED
