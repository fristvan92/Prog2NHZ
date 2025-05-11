#ifndef TABLE_H_INCLUDED
#define TABLE_H_INCLUDED

#include <iostream>
#include <fstream>
#include "memtrace.h"
#include "iexpressions.h"
#include "string5.h"

/*
* 
*/
class Cell{
    bool format;    //true: cell contains string, false: cell contains expression (both/neither not allowed)
    int len;    //len of string
    char* str;  //string value
    Expression* exp;    //pointer to expression
public:
    /*
    * default constructor
    * can be initialized with const char*
    */
    Cell(const char* str="");
    
    /*
    * constructor
    * initialization with expression
    */
    Cell(Expression* exp);
    
    /*
    * copy constructor
    */
    Cell(const Cell& c);
    
    /*
    * returns format of cell
    * @return true: cell contains string, false: cell contains expression
    */
    bool getformat() const;
    
    /*
    * @return value of contained expression
    */
    double evaluate() const;    ///NEM JÓ, ÁTGONDOLNI!!! (mi van ha stringet tartalmaz épp?)
    
    /*
    * changes the cell to contain string
    * @param s the string value to be set in the cell
    * @return const reference to cell
    */
    const Cell& operator=(const char* s);
    
    /*
    * changes the cell to be equal to another (deep copy)
    * @return const reference to cell
    */
    const Cell& operator=(const Cell& cell);
    
    /*
    * writes the data contained by the cell to @param os
    */
    friend std::ostream& operator<<(std::ostream& os, const Cell& c);
    
    /*
    * destructor
    */
    ~Cell();
};

/*
* expression that refers to an other cell
*/
class Reference: public Expression{
    const Cell* cell;   //pointer to cell
public:
    /*
    * constructor
    * must be initialized with a pointer to a cell that contains an expression
    * throws const char* if @param cell contains string
    */
    Reference(const Cell* cell);
    
    /*
    * @return the value of the expression contained by the cell
    */
    double evaluate() const;
    
    /*
    * @return : dinamically allocated copy of object, caller must free the allocated memory
    */
    Expression* clone() const;
};


/*
* Table class that contains Cell-s
*/
class Table{
    int rows, columns;  //number of rows and columns
    Cell** table;       //two dimension dinamically allocated Cell array
    
    /*
    * @return an expression built from @param command
    */
    Expression* BuildExpressionFromCommand(const String& command);
public:
    /*
    * constructor
    * must be initialized with no. of rows (@param rows) and columns (@param columns)
    */
    Table(int rows, int columns);
    
    /*
    * constructor
    * to be initialized with a file
    * !!!NOT YET DEFINED!!!
    */
    Table(std::fstream& file);
    
    /*
    * @return reference to the cell in @param i row and @param j column
    */
    Cell& operator()(int i, int j);
    
    /*
    * @return constant reference to the cell in @param i row and @param j column
    */
    const Cell& operator()(int i, int j) const;
    
    /*
    * creates a cell according to @param command
    * @return Cell
    */
    Cell CreateCellFromCommand(const String& command);
    
    /*
    * prints the table to std::cout
    */
    void print() const;

    ~Table();
};

/*
* writes the data contained by the cell to @param os
*/
std::ostream& operator<<(std::ostream& os, const Cell& c);

/*
* returns a String that that is between @param first and @param last characters in @param command
* counts '(' and ')' chars, throws const char* exeption if they aren't paired (e.g: "(()")
* e.g.: command="(hello)", first='(', last=')' ==> returns "hello"
* @param lastidx : if not -1, index of char before @param last in @param command
*/
String CommandCut(const String& command, char first, char last, int* lastidx=NULL);

#endif // TABLE_H_INCLUDED
