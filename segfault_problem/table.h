#ifndef TABLE_H_INCLUDED
#define TABLE_H_INCLUDED

#include <iostream>
#include <fstream>
#include "memtrace.h"

class Table;

#include "expressions.h"


/*
* expression that refers to an other cell
*/
class Reference: public Expression{
    const Table& table;   //reference to table
    size_t i, j;           //index of cell
    mutable bool detectRecursion; //bool for detecting recursive cell reference
public:
    /*
    * constructor
    * must be initialized with a pointer to a cell that contains an expression
    * throws const char* if @param cell contains string
    */
    Reference(const Table& table, size_t i, size_t j);

    /*
    * @return the value of the expression contained by the cell
    */
    double evaluate() const;

    /*
    * @return : dinamically allocated copy of object, caller must free the allocated memory
    */
    Expression* clone() const;

    void print(std::ostream& os) const;
};

/*
*
*/
class Cell{
    bool format;    //true: cell contains string, false: cell contains expression (both/neither not allowed)
    std::string str;
    Expression* exp;    //pointer to expression

    void FillFunctionWithOperands(Function*& func, const std::string& command, const Table& table);

    Expression* BuildExpressionFromCommand(const std::string& command, const Table& table);
public:

    Cell(const std::string& str="");

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
    friend double Reference::evaluate() const;

    Cell& operator=(const std::string& s);

    /*
    * changes the cell to be equal to another (deep copy)
    * @return reference to cell
    */
    Cell& operator=(const Cell& cell);

    void printvalue(std::ostream& os) const;

    void printformula(std::ostream& os) const;

    void fill(const std::string& command, const Table& table);

    /*
    * destructor
    */
    ~Cell();
};

/*
* Table class that contains Cell-s
*/
class Table{
    size_t rows, columns;  //number of rows and columns
    Cell** table;       //two dimension dinamically allocated Cell array

    void SetColumnsAndRows(std::ifstream& file);

    void clear();

public:
    /*
    * constructor
    * must be initialized with no. of rows (@param rows) and columns (@param columns)
    */
    Table(size_t rows = 0, size_t columns = 0);

    /*
    * constructor
    * to be initialized with a file
    * !!!NOT YET DEFINED!!!
    */
    Table(std::ifstream& file);

    Table(const Table&);

    /*
    * @return reference to the cell in @param i row and @param j column
    */
    Cell& operator()(size_t i, size_t j);

    /*
    * @return constant reference to the cell in @param i row and @param j column
    */
    const Cell& operator()(size_t i, size_t j) const;

    Table& operator=(const Table&);

    /*
    * prints the table to std::cout
    */
    void print();

    void save(std::ostream& fs) const;

    ~Table();
};



#endif // TABLE_H_INCLUDED
