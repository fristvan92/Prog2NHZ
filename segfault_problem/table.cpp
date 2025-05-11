#include <iostream>
#include <cstring>
#include <cmath>
#include <iomanip>
#include <cstdlib>
#include <cstring>

#include "memtrace.h"
#include "table.h"
#include "string5.h"
#include "expressions.h"
#include "CommandIdentifiers.h"

std::string nextDelimiter(const std::string& str, size_t& starti, const char delim){
    size_t i=starti;
    int bpcs =0;
    while(i<str.length() && !(bpcs==0 && str[i]==delim)){
        if(str[i]==OBRACKET) ++bpcs;
        if(str[i]==CBRACKET) --bpcs;
        ++i;
    }
    std::string ret;
    if(i<str.length()){
        ret =str.substr(starti, i-starti);
        starti=i+1;
    }
    else if(i==starti){
        starti = str.length()+1;
        ret.clear();
    }
    else if(str[i-1]==CBRACKET){
        ret =str.substr(starti, i-starti-1);
        starti=i;
    }
    else{
        ret = str.substr(starti, i-starti);
        starti=i;
    }
    return ret;
}

char* RecursiveGetLine(std::istream& is, size_t s=0){
    char c;
    char* ret;
    if(!is.get(c)){
        if(is.eof() && s>0){
            ret = new char[s+1];
            ret[s]='\0';
            return ret;
        }
        return NULL;
    }

    if(c=='\n'){
        ret = new char[s+1];
        ret[s]='\0';
        return ret;
    }

    ret = RecursiveGetLine(is, s+1);
    ret[s]=c;
    return ret;
}

///=====================================================================================
/**Cell class definition**/

Cell::Cell(const std::string& str):format(true), str(str), exp(NULL){}

Cell::Cell(Expression* exp):format(false), exp(exp){}

Cell::Cell(const Cell& c):format(true), str(""), exp(NULL){
    *this=c;
}

bool Cell::getformat() const{return this->format;}

Cell& Cell::operator=(const std::string& s){
    if(!format) delete exp;
    format=true;
    str=s;
    return *this;
}

Cell& Cell::operator=(const Cell& cell){
    if(!format) delete exp;
    format=cell.format;
    if(format){
        str=cell.str;
        exp=NULL;
    }
    else{
        exp=cell.exp->clone();
        str="";
    }
    return *this;
}

void Cell::printvalue(std::ostream& os) const{
    if(format){
        os<<std::setw(7)<<str.substr(0, 8);
    }
    else{
        double res;
        try{
            res = exp->evaluate();
            os<<std::setprecision(6)<<res;
        }
        catch(const char* exp){
            os<<std::setw(7)<<"INVALID";
        }
    }
}
void Cell::printformula(std::ostream& os) const{
    if(format){
        os<<str;
    }
    else{
        os<<'=';
        exp->print(os);
    }
}

void Cell::fill(const std::string& command, const Table& table){
    if(command.empty() || command[0]!='='){
        (*this)=command;
    }
    else (*this) = Cell(BuildExpressionFromCommand(command.substr(1), table));
}

Expression* Cell::BuildExpressionFromCommand(const std::string& command, const Table& table){
    if(command.length()==0) throw std::string("invalid expression")+command;
    Function* exp;
    size_t i, j;
    char* separator;
    switch(command[0]){
    case CONSTANT:
        return new Constant(strtod(command.c_str()+1, nullptr));
        break;
    case REFERENCE:
        i = strtol(command.c_str()+1, &separator, 10);
        if(*separator!=REFERENCE) throw "invalid expression: invalid reference";
        j = strtol(separator+1, nullptr, 10);
        return new Reference(table, i, j);
        break;
    case NEGATIVE:
        return new Negative(BuildExpressionFromCommand(command.substr(1), table));
    }

    struct FunctionIdentifier{
        double (*fptr) (Expression**, size_t);
        char id;
    };

    FunctionIdentifier arr[] = {{sum, SUM},{product, PRODUCT},{average, AVERAGE},{NULL, '\0'}};

    for(size_t i=0; arr[i].fptr!=NULL; ++i){
        if(command[0]==arr[i].id){
            exp=new Function(arr[i].fptr, arr[i].id);
            FillFunctionWithOperands(exp, command.substr(2, command.length()-1), table);
            return exp;
        }
    }
    throw std::string("invalid expression")+command;
}

void Cell::FillFunctionWithOperands(Function*& func, const std::string& command, const Table& table){
    size_t starti=0;
    std::string next;
    while(starti!=command.length()){
        next = nextDelimiter(command, starti, COMMANDSEPARATOR);
        func->add(BuildExpressionFromCommand(next, table));
    }
}

Cell::~Cell(){
    if(!format) delete exp;
}

///======================================================================================
/**Reference class definition**/

Reference::Reference(const Table& table, size_t i, size_t j):table(table), i(i),j(j), detectRecursion(false){
    if(table(i, j).getformat()) throw "invalid expression: reference to string cell";
}

double Reference::evaluate() const{
    if (detectRecursion) throw "recursive cell reference detected";
    const Cell& cell = table(i,j);
    if(cell.getformat()) throw "invalid expression: reference to string cell";   ///SEGFAULT
    detectRecursion=true;
    double ret= cell.exp->evaluate();    ///SEGFAULT
    detectRecursion=false;
    return ret;
}

Expression* Reference::clone() const{
    return new Reference(table, i, j);
}

void Reference::print(std::ostream& os) const{
    os<<':'<<i<<':'<<j;
}

///======================================================================================
/**Table class definition**/

Table::Table(size_t rows, size_t columns):rows(rows), columns(columns), table(new Cell*[rows]){
    for(size_t i=0; i<rows; ++i){
        table[i]=new Cell[columns];
    }
}

void Table::SetColumnsAndRows(std::ifstream& file){
    std::streampos location = file.tellg(); //Elmentjünk a pozíciót
    std::ifstream::fmtflags flags = file.flags(); //Elmentjük a flag-eket

    ///Kiszámoljuk, hogy hány sor, hány oszlop van
    char c;
    size_t row=0, col=0;
    file.setf(std::ios::skipws);    //kellenek a ws-ek a '\n'-ekhez
    while(file.get(c)){
        switch (c){
        case CSVSEP: ++col; break;
        case '\n':
            ++row; ++col;
            columns = col > columns ? col : columns;
            col = 0;
            break;
        default:
            break;
        }
    }
    ++col;  //az utolsó cella
    columns = col > columns ? col : columns;
    rows = row;
    //megvannak a sorok, oszlopok, vissza a file elejére
    file.clear();
    file.seekg(location);
    file.setf(flags);
}

Table::Table(std::ifstream& file):rows(0), columns(0), table(NULL){
    SetColumnsAndRows(file);

    std::string line;
    table=new Cell*[rows];
    for(size_t i=0; i<rows; ++i){
        table[i]=new Cell[columns];
        std::getline(file, line);
        std::stringstream linestream(line);
        std::string command;
//        size_t starti=0;
        size_t j=0;
        while(std::getline(linestream, command, CSVSEP)){
            std::cout<<command<<std::endl;
            try{
                table[i][j++].fill(command, *this);
            }
            catch(const char* str){
                std::cout<<str<<std::endl;
            }
            catch(const std::string& str){
                std::cout<<str<<std::endl;
            }
        }
    }
}

Table::Table(const Table& t):Table(){
    *this=t;
}

Cell& Table::operator()(size_t i, size_t j){
    if(i>=rows || j>=columns) throw "inv idx";
    Cell& ret = table[i][j];
    return ret;
//    return table[i][j];
}

const Cell& Table::operator()(size_t i, size_t j) const{
    if(i>=rows || j>=columns) throw "inv idx";
    const Cell* cellrow= this->table[i];
    const Cell& ret = cellrow[j];
    return ret;
//    return table[i][j];
}

Table& Table::operator=(const Table& t){
    clear();
    rows=t.rows;
    columns=t.columns;
    table=new Cell*[rows];
    for(size_t i=0; i<rows; ++i){
        table[i]=new Cell[columns];
        for(size_t j=0; j<columns; ++j) table[i][j] = t.table[i][j];
    }
    return *this;
}

void Table::print(){
    using namespace std;
    size_t fcw = (size_t) log10(rows)+2;  //first column width

    /*draws the "first" row...*/
    cout<<setw(fcw+1)<<right<<"|";
    for(size_t i=0; i<columns; ++i){
        cout<<setw(7)<<right<<i<<"|";
    }
    cout<<endl;

    /*... and the separator between rows*/
    for(size_t i=0; i<fcw; ++i) cout<<'-';
    cout<<'+';
    for(size_t i=0; i<columns; ++i){
        cout<<"-------+";
    }
    cout<<endl;

    /*draws the table*/
    for(size_t i=0; i<rows; ++i){
        /*draws the "first" column*/
        cout<<setw(fcw)<<i<<"|";

        /*draws the cells in the row*/
        for(size_t j=0; j<columns; ++j){
            cout<<setw(7);
            (*this)(i,j).printvalue(cout);
            cout<<"|";
        }
        cout<<endl;

        /*draws the separator between rows*/
        for(size_t i=0; i<fcw; ++i) cout<<'-';
        cout<<'+';
        for(size_t i=0; i<columns; ++i){
            cout<<"-------+";
        }
        cout<<endl;
    }
}

void Table::save(std::ostream& fs) const{
    for(size_t i=0; i<rows; ++i){
        for(size_t j=0; j<columns; ++j){
            (*this)(i,j).printformula(fs);
            if(j<columns-1) fs<<CSVSEP;
        }
        if(i<rows-1) fs<<std::endl;
    }
}

void Table::clear(){
    for(size_t i=0; i<rows; ++i){
        delete[] table[i];
    }
    delete[] table;
}

Table::~Table(){
    clear();
}

///=====================================================================================
