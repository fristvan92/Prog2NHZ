#include <iostream>
#include <cstring>
#include <cmath>
#include <iomanip>
#include <cstdlib>

#include "memtrace.h"
#include "table.h"
#include "string5.h"
#include "expressions.h"

///=====================================================================================
/**Cell class definition**/

Cell::Cell(const char* str): format(true), len(strlen(str)), str(new char[len+1]), exp(NULL){
    for(int i=0; i<len; ++i){
        this->str[i]=str[i];
    }
    this->str[len]='\0';
}

Cell::Cell(Expression* exp):format(false), len(0), str(NULL), exp(exp){}

Cell::Cell(const Cell& c){
    *this=c;
}

bool Cell::getformat() const{return format;}

double Cell::evaluate() const{return exp->evaluate();}

const Cell& Cell::operator=(const char* s){
    if(format) delete[] str;
    else delete exp;
    format=true;
    len=strlen(s);
    str=new char[len+1];
    strcpy(str, s);
    return *this;
}

const Cell& Cell::operator=(const Cell& cell){
    if(format) delete[] str;
    else delete exp;
    format=cell.format;
    if(format){
        len=cell.len;
        str=new char[len+1];
        strcpy(str, cell.str);
        str[len]='\0';
        exp=NULL;
    }
    else{
        exp=cell.exp->clone();
        len=0;
        str=NULL;
    }
    return *this;
}

Cell::~Cell(){
    if(format) delete[] str;
    else delete exp;
}

///======================================================================================
/**Reference class definition**/

Reference::Reference(const Cell* cell){
    if(cell->getformat()) throw "invalid expression: reference to string cell";
    this->cell=cell;
}

double Reference::evaluate() const{
    return cell->evaluate();
}

Expression* Reference::clone() const{
    return new Reference(cell);
}

///======================================================================================
/**Table class definition**/

Table::Table(int rows, int columns):rows(rows), columns(columns), table(new Cell*[rows]){
    for(int i=0; i<rows; ++i){
        table[i]=new Cell[columns];
    }
}

Cell& Table::operator()(int i, int j){
    if(i>rows || i<0 || j>columns || j<0) throw "inv idx";
    return table[i][j];
}

const Cell& Table::operator()(int i, int j) const{
    if(i>rows || i<0 || j>columns || j<0) throw "inv idx";
    return table[i][j];
}

void Table::print() const{
    using namespace std;
    int fcw = (int) log10(rows)+2;  //first column width
    
    /*draws the "first" row...*/
    cout<<setw(fcw+1)<<right<<"|";
    for(int i=0; i<columns; ++i){
        cout<<setw(7)<<right<<i<<"|";
    }
    cout<<endl;
    
    /*... and the separator between rows*/
    for(int i=0; i<fcw; ++i) cout<<'-';
    cout<<'+';
    for(int i=0; i<columns; ++i){
        cout<<"-------+";
    }
    cout<<endl;
    
    /*draws the table*/
    for(int i=0; i<rows; ++i){
        /*draws the "first" column*/
        cout<<setw(fcw)<<i<<"|";
        
        /*draws the cells in the row*/
        for(int j=0; j<columns; ++j){
            cout<<setw(7)<<(*this)(i,j)<<"|";
        }
        cout<<endl;
        
        /*draws the separator between rows*/
        for(int i=0; i<fcw; ++i) cout<<'-';
        cout<<'+';
        for(int i=0; i<columns; ++i){
            cout<<"-------+";
        }
        cout<<endl;
    }
}

Cell Table::CreateCellFromCommand(const String& command){
    if(command[0]!='='){
        return Cell(command.c_str());   //if the command is just a string, not an expression
    }
    /*build an expression out of string - Create a string out of it - from the command without'='*/
    return Cell(BuildExpressionFromCommand(String(command.c_str()+1)));

}

Expression* Table::BuildExpressionFromCommand(const String& command){
    String comm;            //for the extracted commands
    Expression* lv, *rv;    //pointer for left and right operands
    char* separator;        //for checking reference validity and easier command extraction
    int i=0, j=0, endidx=0;       //indexes for reference building, index for easier command extraction
    switch(command[0]){
    case 's':
        comm = CommandCut(String(command.c_str()+1), '(', ',', &endidx);
        if(command[endidx+1]!=',') throw "invalid expression: sum";
        lv = BuildExpressionFromCommand(comm);
        
        comm = CommandCut(String(command.c_str()+endidx), ',', ')');
        rv = BuildExpressionFromCommand(comm);
        
        return new Sum(lv, rv);
        break;
    case 'p':
        comm = CommandCut(String(command.c_str()+1), '(', ',', &endidx);
        if(command[endidx+1]!=',') throw "invalid expression: product first operand";
        lv = BuildExpressionFromCommand(comm);
        
        comm = CommandCut(String(command.c_str()+endidx), ',', ')');
        rv = BuildExpressionFromCommand(comm);
        
        return new Product(lv, rv);
        break;
    case '$':
        return new Constant(strtod(command.c_str()+1, nullptr));
        break;
    case ':':
        i = strtol(command.c_str()+1, &separator, 10);
        if(*separator!=':') throw "invalid expression: invalid reference";
        j = strtol(separator+1, nullptr, 10);
        return new Reference(&(*this)(i, j));
        break;
    default:
        throw "invalid expression";
        break;
    }
}

Table::~Table(){
    for(int i=0; i<rows; ++i){
        delete[] table[i];
    }
    delete[] table;
}

///=====================================================================================
/**Global function definitions**/

std::ostream& operator<<(std::ostream& os, const Cell& c){
    if(c.format){
        char ostr[7+1];
        strncpy(ostr, c.str, 8);
        ostr[7]='\0';
        os<<std::setw(7)<<ostr;
    }
    else{
        os<<std::setprecision(6)<<c.exp->evaluate();
    }
    return os;
}

String CommandCut(const String& command, char first, char last, int* lastidx){
    int firsti=0;
    while(command[firsti]!=first) ++firsti;
    int endi=firsti+1;
    int brackets=0;
    while(command[endi]!='\0' && !(command[endi]==last && brackets==0)){
        if(command[endi]=='(') ++brackets;
        else if(command[endi]==')') --brackets;
        ++endi;
    }

    if(command[endi]!=last) throw "invalid expression: invalid brackets";
    if(lastidx != NULL) *lastidx=endi;

    int len = endi-firsti-1;
    char* str= new char[len+1];
    for(int i=0; i<len; ++i){
        str[i]=command[i+firsti+1];
    }
    str[len]='\0';
    String res = str;
    delete[] str;
    return res;
}






