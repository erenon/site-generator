Weboldal generátor
==================
A program lehetővé teszi, hogy nyers szöveges fájlokból formázott HTML oldalakat készítsünk; egységes megjelenéssel, menüvel és linkekkel.

A program egy előre meghatározott tartalomjegyzékből olvassa be a fájlok neveit. (AFAIK nincs portable ansi dir list)
Ezután a feldolgozandó fájlok is beolvasásra kerülnek, és a kiterjesztésük alapján megtörténik a feldolgozásuk.
A lehetséges típusok a következőek:

 * Page:
A honlap egy oldalát reprezentálja. Lehetőség van a tartalom formázására, a program képes a következő jelölők felismerésére és HTML kóddá alakítására:
        
    ** _dőlt_
    ** *vastag*
    ** [link:oldal_neve]
    ** [img:kep_neve.jpg]
    
A link célja amennyiben megegyezik a beolvasandó .page fájlok egyikének nevével, akkor lokális link lesz, ellenkező esetben az oldalon kívülre mutató. A képeket egy előre definiált útvonalról lehet beilleszteni, tetszőleges mélységből.

_Név formátuma:_ oldal_neve.page
        
 * Widget:
A page-hez hasonlóan formázott, akár HTML kódokat tartalmazó szöveges fájl, mely beilleszthető az oldal sablonjának (layout) tetszőleges pontjára.

_Név formátuma:_ widget_neve.widget
        
 * Layout:
    Egy általános séma, mely biztosítja az egységes megjelenést, és a HTML szabványnak megfelelő keretet ad minden oldalnak. A program definiál alapértelmezett sablont, de lehetőség van saját meghatározására is. Tartalmazhat helyörzőket (palceholder), melyek a következőek:
    
    ** {navigation}:
    Felsorolás formájában kilistázza az összes beolvasott page nevét, linkelve azokat a megfelelő oldalra.
    
    ** {content}:
    Beilleszti az aktuális oldalt.
    
    ** {widget:widget_neve}:
    Beilleszti a megnevezett widget tartalmát.
    
_Név formátuma:_ layout
    
Feladatkiírás: Egyszerű HTML alapú tartalommenedzser program
---------------------------------------------

Készíts programot, amely egy adott honlap tartalommal való feltöltését segíti oly módon, hogy
egy a programhoz kifejlesztett script-nyelv segítségével a felhasználónak csupán az oldalon megjelenítendő
adatokat, szövegeket, menüpontokat kell megadnia, a megjelenítéssel és a HTML-nyelvű kóddal nem kell
törődnie -- azokat a program automatikusan generálja a script alapján.
