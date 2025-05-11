#include <iostream>             // Kiíratáshoz
#include <cstring>              // Sztringműveletekhez
#include <cctype>

#include "memtrace.h"           // a standard headerek után kell lennie
#include "string5.h"
/// Konstruktorok: egy char karakterből (createString)
///                egy nullával lezárt char sorozatból (createString)
String::String(char c):pData(new char[2]), len(1){
    pData[0]=c;
    pData[1]='\0';
}

String::String(const char* st):pData(new char [strlen(st)+1]), len(strlen(st)){
    for (size_t i=0; i<len; ++i){pData[i]=st[i];}
    pData[len]='\0';
}

/// Másoló konstruktor: String-ből készít (createString)
String::String(const String& st):pData(new char[1]),len(0){
    *this=st;
}

/// Destruktor (disposeString)
String::~String(){
    delete[] pData;
}

/// operator=
const String& String::operator=(const String& st){
    if(this!=&st){
        delete[] pData;
        len=st.len;
        pData = new char[len+1];
        strcpy(pData, st.pData);
        pData[len]='\0';
    }
    return *this;
}
/// [] operátorok: egy megadott indexű elem REFERENCIÁJÁVAL térnek vissza (charAtString)
/// indexhiba esetén const char * kivételt dob!
char& String::operator[](size_t i){
    if (i<0 || i>=len) throw "idx";
    return pData[i];
}
const char& String::operator[](size_t i) const{
    if (i<0 || i>=len) throw "idx";
    return pData[i];
}

/// + operátorok:
///                 String-hez jobbról karaktert ad (addString)
///                 String-hez String-et ad (addString)
String String::operator+(char c) const{
    String res;
    res.len=len+1;
    delete[] res.pData;
    res.pData=new char[res.len+1];
    strcpy(res.pData, pData);
    res.pData[len]=c;
    res.pData[res.len]='\0';
    return res;
}

String operator+(char c, const String& st){
    return String(c)+st;
}

String String::operator+(const String& st) const{
    String res;
    res.len=len+st.len;
    delete[] res.pData;
    res.pData=new char[res.len+1];
    strcpy(res.pData, pData);
    strcat(res.pData, st.pData);
    return res;
}

/// << operator, ami kiír az ostream-re
std::ostream& operator<<(std::ostream& os, const String& st){
    os<<st.pData;
    return os;
}

/// >> operátor, ami beolvas az istream-ről egy szót


std::istream& operator>>(std::istream& is, String& st){
    is.unsetf(std::ios::skipws);
    char c;
    bool b=false, a=true;
    while(a && is>>c){
        if(!isspace(c)){
            st=st+c;
            b=true;
        } else if(b) a=false;
    }
    is.setf(std::ios::skipws);
    return is;
}

/*
char* rekbeolvas(std::istream& is, int n=1){
    int c;
    char* str=NULL;
    is>>c;
    if(isspace(c) || c=='\0'){
        str = new char[n];
        str[n-1]='\0';
        return str;
    }
    std::cout<<"["<<(char)c<<"]\n";
    str=rekbeolvas(is, n+1);
    str[n-1]=c;
    return str;
}

std::istream& operator>>(std::istream& is, String& st){
    is.unsetf(std::ios::skipws);
    char c;
    is>>c;
    while(isspace(c)){
        is>>c;
    }
    is.unget();
    char* str = rekbeolvas(is);
    st= st + String(str);
    delete[] str;
    is.setf(std::ios::skipws);
    return is;
}*/


