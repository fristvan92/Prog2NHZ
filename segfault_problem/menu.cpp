#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include <limits>
#include "menu.hpp"

//====================================================================================
//  GLOBAL FUNCTION
//====================================================================================

void getFileName(std::string& filename){
    bool validformat;
    do{
        validformat=true;
        std::cout<<"Please enter the filename (*.csv): ";
        std::cin>>filename;

        if (filename.length()<5 || filename.substr(filename.length()-4) != std::string(".csv")){
            std::cout<<"Invalid filename\n";
            validformat=false;
        }
    }while(!validformat);
}

//====================================================================================
//  EXIT
//====================================================================================

Exit::Exit(MenuStates to):to(to){}

MenuStates Exit::action(Table& table){ return to; }

//====================================================================================
//  TABLEFROMFILE
//====================================================================================

TableFromFile::TableFromFile(std::string& f): filename(f){}

MenuStates TableFromFile::action(Table& table){
    getFileName(filename);
    std::ifstream file(filename);
    if(!file.is_open()){
        std::cout<<"Couldn't open file.\n";
        return MenuStates::mainmenu;
    }
    table=Table(file);
    return MenuStates::editing;
}

//====================================================================================
//  NEWTABLE
//====================================================================================

MenuStates NewTable::action(Table& table){
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
    table = Table(rows, columns);
    return MenuStates::editing;
}

//====================================================================================
//  CHANGECELL
//====================================================================================

MenuStates ChangeCell::action(Table& table){
    int ri, ci;
    char c;
    bool validformat;
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

    std::cout<<"Type in the formula: ";
    std::string s;
    std::getline(std::cin, s);
    try{
        table(ri, ci).fill(s, table);
    }
    catch(const char* err){
        std::cout<<"Error: "<<err<<std::endl;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout<<std::endl;
    }
    return MenuStates::editing;
}

//====================================================================================
//  SHOWCELL
//====================================================================================

MenuStates ShowCell::action(Table& table){
    int ri, ci;
    char c;
    bool validformat;
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

    table(ri, ci).printformula(std::cout);
    std::cout<<std::endl;
    return MenuStates::editing;
}

//====================================================================================
//  SAVETOSAME
//====================================================================================

SaveToSame::SaveToSame(const std::string& f):filename(f){}

MenuStates SaveToSame::action(Table& table){
    std::ofstream file(filename);
    if(!file.is_open()){
        std::cout<<"Couldn't open file.\n";
    }
    else table.save(file);
    return MenuStates::saving;
}

//====================================================================================
//  SAVETOOTHER
//====================================================================================

MenuStates SaveToOther::action(Table& table){
    std::string filename;
    getFileName(filename);
    std::ofstream file(filename);
    if(!file.is_open()){
        std::cout<<"Couldn't open file.\n";
    }
    else table.save(file);
    return MenuStates::saving;
}

//====================================================================================
//  MAINMENU
//====================================================================================

MainMenuInput MainMenu::input(int n){
    switch(n){
    case 0: return MainMenuInput::Exit; break;
    case 1: return MainMenuInput::blankTable; break;
    case 2: return MainMenuInput::openFile; break;
    default: return MainMenuInput::other; break;
    }
}

void MainMenu::showOptions(){
    std::cout<<"1. Create blank table"<<std::endl;
    std::cout<<"2. Open file"<<std::endl;
    std::cout<<"0. Exit"<<std::endl;
}

MainMenu::MainMenu(std::string& f):filename(f){
    tab[MainMenuInput::blankTable] = new NewTable();
    tab[MainMenuInput::openFile] = new TableFromFile(filename);
    tab[MainMenuInput::Exit] = new Exit(MenuStates::exit);
}

MenuStates MainMenu::action(Table& table){
    std::cout<<"MAIN MENU"<<std::endl;
    int inp;
    MainMenuInput choice;
    do{
        showOptions();
        std::cout<<"Choice: ";
        std::cin>>inp;
        choice=input(inp);
    }while(choice==MainMenuInput::other);
    return tab[choice]->action(table);
}

MainMenu::~MainMenu(){
    std::map<MainMenuInput, MenuItem*>::iterator i1= tab.begin();
    while(i1!=tab.end()){
        delete i1->second;
        ++i1;
    }
}

//====================================================================================
//  EDITMENU
//====================================================================================

EditMenuInput EditMenu::input(int n){
    switch(n){
    case 0: return EditMenuInput::exit; break;
    case 1: return EditMenuInput::change; break;
    case 2: return EditMenuInput::show; break;
    case 3: return EditMenuInput::save; break;
    case 4: return EditMenuInput::exitToMain; break;
    default: return EditMenuInput::other; break;
    }
}

void EditMenu::showOptions(){
    std::cout<<"1. Edit cell"<<std::endl;
    std::cout<<"2. Show cell content"<<std::endl;
    std::cout<<"3. Save table"<<std::endl;
    std::cout<<"4. Exit to main menu"<<std::endl;
    std::cout<<"0. Exit"<<std::endl;
}

EditMenu::EditMenu(){
    tab[EditMenuInput::change] = new ChangeCell();
    tab[EditMenuInput::show] = new ShowCell();
    tab[EditMenuInput::save] = new Exit(MenuStates::saving);
    tab[EditMenuInput::exitToMain] = new Exit(MenuStates::mainmenu);
    tab[EditMenuInput::exit] = new Exit(MenuStates::exit);
}

MenuStates EditMenu::action(Table& table){
    table.print();
    int inp;
    EditMenuInput choice;
    do{
        showOptions();
        std::cout<<"Choice: ";
        std::cin>>inp;
        choice=input(inp);
    }while(choice==EditMenuInput::other);
    return tab[choice]->action(table);
}

EditMenu::~EditMenu(){
    std::map<EditMenuInput, MenuItem*>::iterator i1= tab.begin();
    while(i1!=tab.end()){
        delete i1->second;
        ++i1;
    }
}

//====================================================================================
//  SAVEMENU
//====================================================================================

SaveMenuInput SaveMenu::input(int n){
    switch(n){
    case 0: return SaveMenuInput::exit; break;
    case 1: return SaveMenuInput::same; break;
    case 2: return SaveMenuInput::another; break;
    case 3: return SaveMenuInput::keepEditing; break;
    default: return SaveMenuInput::other; break;
    }
}

void SaveMenu::showOptions(){
    if(!filename.empty()) std::cout<<"1. Save to same file"<<std::endl;
    std::cout<<"2. Save to new file"<<std::endl;
    std::cout<<"3. Keep editing"<<std::endl;
    std::cout<<"0. Exit"<<std::endl;
}

SaveMenu::SaveMenu(const std::string& f):filename(f){
    tab[SaveMenuInput::same] = new SaveToSame(filename);
    tab[SaveMenuInput::another] = new SaveToOther();
    tab[SaveMenuInput::keepEditing] = new Exit(MenuStates::editing);
    tab[SaveMenuInput::exit] = new Exit(MenuStates::exit);
}

MenuStates SaveMenu::action(Table& table){
    table.print();
    int inp;
    SaveMenuInput choice;
    do{
        showOptions();
        std::cout<<"Choice: ";
        std::cin>>inp;
        choice=input(inp);
    }while(choice==SaveMenuInput::other);
    return tab[choice]->action(table);
}

SaveMenu::~SaveMenu(){
    std::map<SaveMenuInput, MenuItem*>::iterator i1= tab.begin();
    while(i1!=tab.end()){
        delete i1->second;
        ++i1;
    }
}



