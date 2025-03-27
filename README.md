# Prog2NHZ
## egyszerű számolótábla implementáció
### használat
1. `make`, aztán `make run` fordítja és indítja a programot \
2. "Type in the text: " után az alábbi formátumok jöhetnek szóba:\
  - = -> formula\
  - ' '-> szöveg\
  - p(lv, rv) -> szorzat\
  - s(lv, rv) -> összeg\
  - $double -> konstans\
  - :int:int -> cella hivatkozás\
  - pl.: "=p(:0:0,s($5,$6.4)" -> "=táblázat[0][0] * (5+6.4)", valamint "szoveg" esetén "szoveg" jelenik meg a cellában\
