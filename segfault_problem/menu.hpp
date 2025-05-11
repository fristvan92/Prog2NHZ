#ifndef MENU_HPP_INCLUDED
#define MENU_HPP_INCLUDED

#include "allapotgep.hpp"
#include "table.h"
#include <map>
#include <string>
#include <iostream>
#include <fstream>

enum class MenuStates{mainmenu, editing, saving, exit};
enum class MainMenuInput{blankTable, openFile, Exit, other};
enum class EditMenuInput{ change, show, save, exitToMain, exit, other};
enum class SaveMenuInput{ same, another, keepEditing, exit, other};

struct MenuItem{
    virtual MenuStates action(Table& table)=0;
    virtual ~MenuItem(){}
};

class Exit : public MenuItem{
    MenuStates to;
public:
    Exit(MenuStates to);
    MenuStates action(Table&);
};

class TableFromFile : public MenuItem{
    std::string& filename;
public:
    TableFromFile(std::string& f);
    MenuStates action(Table& table);
};

class NewTable : public MenuItem {
public:
    MenuStates action(Table& table);
};

class ChangeCell : public MenuItem{
public:
    MenuStates action(Table& table);
};

class ShowCell : public MenuItem{
public:
    MenuStates action(Table& table);
};

class SaveToSame : public MenuItem{
    const std::string& filename;
public:
    SaveToSame(const std::string& f);
    MenuStates action(Table& table);
};

class SaveToOther : public MenuItem{
public:
    MenuStates action(Table& table);
};

class MainMenu: public MenuItem{
    std::map<MainMenuInput, MenuItem*> tab;
    std::string& filename;
    MainMenuInput input(int n);
    void showOptions();
public:
    MainMenu(std::string& f);

    MenuStates action(Table& table);

    ~MainMenu();
};

class EditMenu : public MenuItem{
    std::map<EditMenuInput, MenuItem*> tab;
    EditMenuInput input(int n);
    void showOptions();
public:
    EditMenu();

    MenuStates action(Table& table);

    ~EditMenu();
};

class SaveMenu : public MenuItem{
    std::map<SaveMenuInput, MenuItem*> tab;
    const std::string& filename;
    SaveMenuInput input(int n);
    void showOptions();
public:
    SaveMenu(const std::string&);

    MenuStates action(Table& table);

    ~SaveMenu();
};

#endif // MENU_HPP_INCLUDED
