#ifndef STRING_H
#define STRING_H

#include <iostream>
#include "memtrace.h"

///Ez a laborfeladatként elkészített String osztály

class String {
    char *pData;        ///< pointer az adatra
    size_t len;         ///< hossz lezáró nulla nélkül
public:
/// Kiírunk egy Stringet (debug célokra)
/// Ezt a tagfüggvényt elkészítettük, hogy használja a hibák felderítéséhez.
/// Igény szerint módosítható
/// @param txt - nullával lezárt szövegre mutató pointer.
///              Ezt a szöveget írjuk ki a debug információ előtt.
    void printDbg(const char *txt = "") const {
        std::cout << txt << "[" << len << "], "
                  << (pData ? pData : "(NULL)") << '|' << std::endl;
    }

/// 1  A paraméter nélkül hívható konstruktora üres sztringet hozzon létre!
/// Van olyan tagfüggvénye ( size() ), ami visszaadja a sztring hosszát.
    /// Paraméter nélküli konstruktor:
    String() :pData(new char[1]), len(0) {
        pData[0]='\0';
    }

    /// Sztring hosszát adja vissza.
    /// @return sztring tényleges hossza (lezáró nulla nélkül).
    size_t size() const { return len; }

/// 2. Van olyan tagfüggvénye ( c_str() ), ami C-sztringgel, azaz nullával lezárt
///    karaktersorozatra mutató pointerrel (const char *) tér vissza.
    /// C-sztringet ad vissza
    /// @return pointer a tárolt, vagy azzal azonos tartalmú nullával lezárt sztring-re.
    const char* c_str() const { return pData;}

/// 3. Van olyan konstruktora, ami karakterből hoz létre sztringet.
    String(char c);
/// 4. Van olyan konstruktora, ami C-sztringből (const char*) hoz létre sztringet.
    String(const char* st);
/// 5. Az osztályból létrehozott objektum legyen átadható értékparaméterként!
    String(const String& st);

    ///Destruktor
    ~String();
/// 6. Támogassa a többszörös értékadást (b = a = a)!
    const String& operator=(const String& st);

/// 7. Legyenek olyan operátorai (operator+), amivel a sztring végéhez sztringet
///    és karaktert is lehet fűzni!
    String operator+(char c) const;
    String operator+(const String&) const;
/// 8. Lehessen karakterhez is sztringet fűzni a + operátorral!
    friend String operator+(char c, const String& st);
/// 9. A tárolt a karakterek legyenek elérhetőek a szokásos módon indexeléssel!
///    Az indexeléssel elért elem legyen használható balértékként is!
///    A konstans objektumok is legyenek indexelhetők.
///    Az indexelési hiba esetén dobjon const char * kivételt!
    char& operator[](size_t);
    const char& operator[](size_t) const;
/// 10. Készítsen olyan << operátort, amivel egy sztring kiírható egy ostream típusú
///    objektumra!
    friend std::ostream& operator<<(std::ostream& os, const String& st);
    friend std::istream& operator>>(std::istream& is, String& st);



}; /// Itt az osztály deklarációjának vége

/// Ide kerülnek a globális operátorok deklarációi.
/// ...

String operator+(char c, const String& st);

std::ostream& operator<<(std::ostream& os, const String& st);
std::istream& operator>>(std::istream& is, String& st);



#endif
