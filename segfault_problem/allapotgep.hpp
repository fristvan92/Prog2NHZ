/**
*    @file allapotgep.hpp
*    Nem kell beadni!
*/
#ifndef ALLAPOTGEP_HPP
#define ALLAPOTGEP_HPP

#include <map>
#include "memtrace.h"

/**
 * Allapotgep ősosztály
 * @tparam All az állapotokat kódoló típus
 * @tparam Inp az input csoportokat kódoló típus
 * @tparam T az input típusa
 */
template <typename All, typename Inp, typename T>
class Allapotgep {
public:
    /**
     * Nop osztály
     * Az állapotátmenetekhez tartozó következő állapotot tárolja,
     * valamint az állapot során meghívandó akciót
     */
    struct Nop {
        /// következő állapot
        All kov_allapot;
        Nop(All kov_all) :kov_allapot(kov_all) {}

        /// ha az állapotgép ebbe az állapotba ér, meghívja ezt a függvényt
        /// @param ch - erre az input értékre léptünk ide
        virtual void akcio(T ch) {}
        virtual ~Nop() {}
    };
    /**
     * Az állapotátmenetek kódolását megkönnyítő Állapot/Input tábla
     */
    struct AllTabla : public std::map<All, std::map<Inp, Nop*> > {
        ~AllTabla() {
            typename AllTabla::iterator i1 = this->begin();
            while (i1 != this->end()) {
                typename std::map<Inp, Nop*>::iterator i2 = i1->second.begin();
                while (i2 != i1->second.end()) {
                    delete i2->second;
                    ++i2;
                }
                ++i1;
            }
        };
    };
private:
    /// éppen melyik állapotban vagyunk
    All akt_allapot;
    /// referencia az állapotátmenet táblára
    AllTabla& tab;
public:
    /// beállítja a kezdő állapotot és az állapottábla referenciáját
    Allapotgep(All kezdo, AllTabla& tab) : akt_allapot(kezdo), tab(tab) {}
    /// az input átalakitását végző segédfüggvény
    /// @param ch a kapott input
    /// @return az inputnak megfelelő Inp-ben kódolt érték
    virtual Inp input(T ch) const = 0;
    /// Input feldolgozása
    /// @param ch aktuális input
    /// @return az átmenet után melyik állapotba értünk
    All operator()(T ch) {
        Inp akt_input = input(ch);
        tab[akt_allapot][akt_input]->akcio(ch);
        akt_allapot = tab[akt_allapot][akt_input]->kov_allapot;
        return akt_allapot;
    }
    virtual ~Allapotgep() {}
};

#endif
