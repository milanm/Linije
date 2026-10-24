# Linije

Logička igra u stilu čuvene igre „Lines” za Windows, napisana u C++-u i MFC-u 2004. godine.

**Igrajte u browseru: [milanm.github.io/Linije](https://milanm.github.io/Linije/)**. Radi i na telefonu.

![Linije, partija u toku](screenshots/gameplay.png)

## Igranje u browseru

Web verzija na [milanm.github.io/Linije](https://milanm.github.io/Linije/) zadržava originalnu grafiku, zvuke, muziku, dijaloge i pravila. Kada telefon držite uspravno, desni panel se premešta ispod table, da bi polja bila dovoljno velika za prst. Igru možete dodati na početni ekran telefona, a posle prvog otvaranja radi i bez interneta.

Sačuvane partije i rezultati čuvaju se u browseru. Trenutna partija se čuva i automatski, pa se ne gubi kada zatvorite stranicu.

## Preuzimanje za Windows

Poslednju verziju preuzmite sa [stranice Releases](https://github.com/milanm/Linije/releases/latest):

- **`Linije-<verzija>-setup.exe`**: instaler. Dodaje prečicu u Start meni i, po želji, na desktop. Instalira se u vaš korisnički profil, pa ne traži administratorska prava.
- **`Linije-<verzija>-portable.zip`**: bez instalacije. Raspakujte ga bilo gde i pokrenite `Linije.exe`.

Radi na Windows 10 i 11. Ništa drugo ne treba instalirati.

Fajlovi nisu digitalno potpisani, pa Windows SmartScreen može da prikaže upozorenje „Windows protected your PC”. Kliknite **More info** (Više informacija), pa **Run anyway** (Ipak pokreni).

Ako koristite ZIP i pomoć (F1) se otvori prazna, kliknite desnim tasterom na ZIP, izaberite **Properties** (Svojstva), štiklirajte **Unblock** (Deblokiraj) i ponovo ga raspakujte.

## Kako se igra

- Tabla je 10×10, sa pet figura: kugla, kocka, romb, trougao i zvezda.
- Kliknite na figuru, pa na prazno polje. Figura se pomera samo ako do tog polja postoji slobodan put, koji se traži A* algoritmom. Put može da ide i dijagonalno, ali ne između dve zauzete figure.
- Poređajte 5 ili više istih figura vodoravno, uspravno ili dijagonalno da biste ih uklonili. Pet figura donosi 100 poena, a svaka sledeća još 50.
- Ako potez ne napravi liniju, na tabli se pojavljuju tri nove figure. Panel desno pokazuje koje dolaze. Dugme **SLEDEĆE** jednom po potezu menja te tri figure.
- Igra se završava kada se tabla popuni.
- U **Meni**-ju su nova igra, čuvanje i učitavanje partije, muzika, prepreke (zidići na tabli), rezultati i pomoć.

| Nova igra | Meni | Rezultati |
|---|---|---|
| ![Nova igra](screenshots/new-game.png) | ![Meni](screenshots/menu.png) | ![Rezultati](screenshots/high-scores.png) |

## Kompajliranje iz izvornog koda

Potreban je Visual Studio 2022 ili noviji, sa workload-om **Desktop development with C++** i komponentom **C++ MFC for latest build tools (x86 & x64)**.

Otvorite `Linije.sln` i izgradite konfiguraciju `Release | x86`, ili iz Developer Command Prompt-a pokrenite:

```
msbuild Linije.sln /p:Configuration=Release /p:Platform=x86
```

Igra učitava zvuke, muziku i pomoć iz foldera u kome se pokreće, pa iskopirajte `*.wav`, `Zvuk.mid` i `Linije.chm` pored `bin\Release\Linije.exe`.

GitHub Actions pravi instaler i ZIP pri svakom push-u. Push taga `v*` ih objavljuje kao novi release.

### Web verzija

Web verzija u folderu `web/` je običan HTML, CSS i JavaScript, bez build koraka. Folder možete servirati bilo kojim statičkim serverom, na primer `python -m http.server --directory web`. Svaki push na `master` koji menja `web/` objavljuje je na GitHub Pages.

`web/assets/` i `web/help/` prave se iz originalnih resursa skriptom `python tools/build_web_assets.py` (potreban je Pillow, a za stranice pomoći i Windows). Muzika je originalni `Zvuk.mid`, pretvoren u MP3 skriptom `tools/render_midi.py`.

## Autori

- Programiranje: [Milan Milanović](https://www.linkedin.com/in/milanmilanovic/)
- Grafika: [Uroš Pavlović](https://rs.linkedin.com/in/urospavlovic)
- Muzika u web verziji renderovana je soundfontom [GeneralUser GS](https://github.com/mrbumpy409/GeneralUser-GS) autora S. Christiana Collinsa.
