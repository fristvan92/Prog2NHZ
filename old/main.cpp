#include <iostream>
#include <limits>
#include "memtrace.h"
#include "expressions.h"
#include "table.h"
#include "string5.h"

int main()
{
    int rows, columns;
    char c;
    bool validformat;
    do{
        validformat=true;
        std::cout<<"Give me the number of rows and columns (RxC): ";
        std::cin>>rows>>c>>columns;
        if(c!='x'|| std::cin.fail()){
            validformat=false;
            std::cout<<"Invalid format.\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }while(!validformat);
    Table table(rows, columns);
    table.print();

    bool end=false;
    do{
        int ri, ci;
        do{
            validformat=true;
            std::cout<<"Which cell do you want to change? (R:C): ";
            std::cin>>ri>>c>>ci;
            if(c!=':' || std::cin.fail()){
                validformat=false;
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout<<"Invalid format.\n";
            }
        }while(!validformat);

        std::cout<<"Type in the text: ";
        String s;
        std::cin>>s;
        bool error=false;
        try{
            table(ri, ci)=table.CreateCellFromCommand(s.c_str());
        }
        catch(const char* err){
            error=true;
            std::cout<<"Error: "<<err<<std::endl;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
        if(!error) table.print();

        std::cout<<"End? (y/n): ";
        char b;
        std::cin>>b;
        if(b=='y') end=true;
    }while(!end);
    return 0;
}
