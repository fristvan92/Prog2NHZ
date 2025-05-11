#include <iostream>
#include <limits>
#include <fstream>
#include "gtest_lite.h"
#include "expressions.h"
#include "table.h"
#include "string5.h"
#include "memtrace.h"
#include "CommandIdentifiers.h"
#include "menu.hpp"
#include <map>

#define SPREADSHEETTEST 2
#define FROMFILE 2

int test_main();

int main()
{
    std::string filename;
    std::map<MenuStates, MenuItem*> menu{{MenuStates::mainmenu, new MainMenu(filename)},
                                         {MenuStates::editing, new EditMenu()},
                                         {MenuStates::saving, new SaveMenu(filename)}};
    Table table;
    MenuStates choice=MenuStates::mainmenu;
    while(choice!=MenuStates::exit) choice=menu[choice]->action(table);

    std::map<MenuStates, MenuItem*>::iterator i = menu.begin();
    while(i!=menu.end()){
        delete i->second;
        ++i;
    }
    test_main();

#if SPREADSHEETTEST==1
    test_main();
#endif // SPREADSHEETTEST

#if SPREADSHEETTEST == 0

#if FROMFILE == 0
    bool validformat;
    char c;
    int rows, columns;
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
    table(0,0)=Cell(Constant(5).clone());
    table.print();
#endif // FROMFILE

#if FROMFILE == 1
    std::ifstream infile("test.csv");
    if(!infile.is_open()){
            std::cout<<"Not open\n";
            return 1;
    }
    try{
        Table table(infile);
        table.print();
    }
    catch(const char* e){
        std::cout<<"Error: "<<e<<std::endl;
        return 1;
    }
#endif // FROMFILE

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
            table(ri, ci).fill(&s[0], table);
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
    std::ofstream file("testo.csv");
    if(file.is_open()){
        table.save(file);
        file.close();
    }
    return 0;
#endif // SPREADSHEETTEST
}

int test_main(){
    ///Expression class test
    //Constant class test
    TEST(Expression, Constant) {
        Constant a(5), b(3.15);
        EXPECT_EQ((double)5, a.evaluate());
        EXPECT_EQ(3.15, b.evaluate());
        Expression* c=a.clone();
        EXPECT_EQ((double)5, c->evaluate());
        std::stringstream stream;
        a.print(stream);
        EXPECT_STREQ("$5", stream.str().c_str());
        delete c;
    }ENDM

    Constant a(5), b(4), c(11);

    //Function class test with sum
    TEST(Expression, Function_Sum){
        Function s(sum, SUM);
        s.add(a.clone());
        s.add(b.clone());
        s.add(c.clone());
        EXPECT_EQ((double)20, s.evaluate());
        Expression* sclone = s.clone();
        EXPECT_EQ((double)20, sclone->evaluate());
        std::stringstream stream;
        s.print(stream);
        EXPECT_STREQ("s($5;$4;$11)", stream.str().c_str());
        s.add(sclone);
        EXPECT_EQ((double)40, s.evaluate());
        stream.clear();
        stream.str("");
        s.print(stream);
        EXPECT_STREQ("s($5;$4;$11;s($5;$4;$11))", stream.str().c_str());
    }ENDM

    //Function class test with product
    TEST(Expression, Function_Product){
        Function p(product, PRODUCT);
        p.add(a.clone());
        p.add(b.clone());
        p.add(c.clone());
        EXPECT_EQ((double)220, p.evaluate());
        Expression* pclone = p.clone();
        EXPECT_EQ((double)220, pclone->evaluate());
        std::stringstream stream;
        p.print(stream);
        EXPECT_STREQ("p($5;$4;$11)", stream.str().c_str());
        p.add(pclone);
        EXPECT_EQ((double)48400, p.evaluate());
        stream.clear();
        stream.str("");
        p.print(stream);
        EXPECT_STREQ("p($5;$4;$11;p($5;$4;$11))", stream.str().c_str());
    }ENDM

    //Function class test with average
    TEST(Expression, Function_Average){
        Function avg(average, AVERAGE);
        double calcavg = (5+4+11)/3.0;
        avg.add(a.clone());
        avg.add(b.clone());
        avg.add(c.clone());
        EXPECT_DOUBLE_EQ(calcavg, avg.evaluate());
        Expression* avgclone = avg.clone();
        EXPECT_DOUBLE_EQ(calcavg, avg.evaluate());
        std::stringstream stream;
        avg.print(stream);
        EXPECT_STREQ("a($5;$4;$11)", stream.str().c_str());
        avg.add(avgclone);
        EXPECT_DOUBLE_EQ(calcavg, avg.evaluate());
        stream.clear();
        stream.str("");
        avg.print(stream);
        EXPECT_STREQ("a($5;$4;$11;a($5;$4;$11))", stream.str().c_str());
    }ENDM

    TEST(Expression, Negative_with_Sum){
        Function s(sum, SUM);
        s.add(a.clone());
        s.add(b.clone());
        s.add(c.clone());
        Negative n(s.clone());
        EXPECT_EQ(-20.0, n.evaluate());
        Expression* np=n.clone();
        EXPECT_EQ(-20.0, np->evaluate());
        std::stringstream stream;
        n.print(stream);
        EXPECT_STREQ("-s($5;$4;$11)", stream.str().c_str());
        delete np;
    }ENDM

    TEST(Cell, Constructor_and_Format){
        Cell a;
        EXPECT_TRUE(a.getformat());
        Constant c(5);
        Cell b(c.clone());
        EXPECT_FALSE(b.getformat());
        Cell a1(a), b1(b);
        EXPECT_TRUE(a1.getformat());
        EXPECT_FALSE(b1.getformat());
    }END

    TEST(Cell, Content_and_save){
        Cell cell1("test");
        Cell cell2(Constant(5).clone());
        std::stringstream stream1, stream2;
        cell1.printvalue(stream1);
        cell2.printvalue(stream2);
        EXPECT_STREQ("   test", stream1.str().c_str());
        EXPECT_STREQ("5", stream2.str().c_str());
        stream1.clear(); stream1.str("");
        stream2.clear(); stream2.str("");
        cell1.printformula(stream1);
        cell2.printformula(stream2);
        EXPECT_STREQ("test", stream1.str().c_str());
        EXPECT_STREQ("=$5", stream2.str().c_str());
    }END

    TEST(Cell, assign){
        Cell cell1, cell2(Constant(5).clone()), cell3;
        cell1 = "test";
        cell3 = cell2;
        std::stringstream stream1, stream3;
        cell1.printvalue(stream1);
        cell3.printvalue(stream3);
        EXPECT_STREQ("   test", stream1.str().c_str());
        EXPECT_STREQ("5", stream3.str().c_str());
        stream1.clear(); stream1.str("");
        stream3.clear(); stream3.str("");
        cell1.printformula(stream1);
        cell3.printformula(stream3);
        EXPECT_STREQ("test", stream1.str().c_str());
        EXPECT_STREQ("=$5", stream3.str().c_str());
    }END

    TEST(Table, Constructor_and_op()){
        Table table1(4, 4);
        EXPECT_THROW(table1(4, 1), const char*);
        EXPECT_TRUE(table1(0,0).getformat());
        std::stringstream stream1;
        table1.save(stream1);
        EXPECT_STREQ(",,,\n,,,\n,,,\n,,,", stream1.str().c_str());
        std::ifstream file("test.csv");
        Table table2(file);
        std::stringstream stream2;
        EXPECT_THROW(table2(2, 6), const char*);
        table2.save(stream2);
        EXPECT_STREQ("=$25,=:0:0,,,,\n,,,,Haloka,\n,,,,,lalala", stream2.str().c_str());
    }END


    TEST(Table, Cell::assign_and_Cell::fill){
        Table table(2, 2);
        std::string c00str("test");
        std::string c01str("=s($2;$2)");
        table(0, 0).fill(c00str, table);
        table(0, 1).fill(c01str, table);
        std::stringstream stream1, stream2;
        table(0, 0).printvalue(stream1);
        table(0, 1).printvalue(stream2);
        EXPECT_STREQ("   test", stream1.str().c_str());
        EXPECT_STREQ("4", stream2.str().c_str());
        stream1.clear(); stream1.str("");
        stream2.clear(); stream2.str("");
        table(0, 0).printformula(stream1);
        table(0, 1).printformula(stream2);
        EXPECT_STREQ("test", stream1.str().c_str());
        EXPECT_STREQ("=s($2;$2)", stream2.str().c_str());
        Cell cell3;
        table(0, 0) = "test2";
        table(1, 1) = table(0, 1);
        std::stringstream stream3;
        stream1.clear(); stream1.str("");
        table(0, 0).printvalue(stream1);
        table(1, 1).printvalue(stream3);
        EXPECT_STREQ("  test2", stream1.str().c_str());
        EXPECT_STREQ("4", stream3.str().c_str());
        stream1.clear(); stream1.str("");
        stream2.clear(); stream3.str("");
        table(0, 0).printformula(stream1);
        table(1, 1).printformula(stream3);
        EXPECT_STREQ("test2", stream1.str().c_str());
        EXPECT_STREQ("=s($2;$2)", stream3.str().c_str());
    }END

    return 0;
}
