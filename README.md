# Gothic 2 Union Plugin: Tieru Minimap

![Gothic 2](https://img.shields.io/badge/Game-Gothic%202%20NoTR-blue) ![Union](https://img.shields.io/badge/Platform-Union%201.0m%2B-green)

## 📖 Opis / Description

**(PL)** Plugin do gry Gothic 2 Noc Kruka (Union), dodający funkcjonalną minimapę z oznaczeniem zadań, wrogów i skrzyń. Wyświetla się w prawym górnym rogu ekranu i pozwala na łatwą nawigację oraz śledzenie ważnych obiektów w grze.

**(EN)** A Gothic 2 Night of the Raven (Union) plugin that adds a functional minimap with markers for quests, enemies, and chests. Displays in the top-right corner of the screen and allows for easy navigation and tracking of important objects in the game.

### ⚠️ Uwaga o rozwoju / Development Notice

**(PL)** Ten mod został stworzony przy wsparciu narzędzi AI (w tym Claude AI). Kod może zawierać błędy lub nieoptymalne rozwiązania. Wszelkie zgłoszenia błędów i propozycje ulepszeń są mile widziane w sekcji [Issues](https://github.com/TieruYT/UnionMinimap/issues).

**(EN)** This mod was created with the assistance of AI tools (including Claude AI). The code may contain bugs or non-optimal solutions. All bug reports and improvement suggestions are welcome in the [Issues](https://github.com/TieruYT/UnionMinimap/issues) section.

## 🌟 Funkcje / Features

*   **Minimapa (Radar):** Wyświetla pozycję gracza i otoczenie w prawym górnym rogu ekranu.
*   **Wskaźniki (Dots):**
    *   🔵 **Niebieski:** Gracz (Player).
    *   🔴 **Czerwony:** Wrogowie (Enemies).
    *   🟠 **Pomarańczowy:** NPC z aktywnym dialogiem zadania (Quest NPCs).
    *   ⚪ **Biały:** Neutralni NPC.
    *   🟢 **Zielony:** Otwarte/Dostępne skrzynie (Unlocked/Open chests).
    *   🟡 **Żółty:** Skrzynie na wytrych (Picklock chests).
    *   🟦 **Cyjan:** Skrzynie na klucz (Key chests).
    *   🔴 **Czerwony (Skrzynia):** Zamknięte (Locked/Unknown).
*   **Menu Konfiguracyjne:** Pełna integracja z menu gry ("Opcje -> Union -> Tieru Plugins").
    *   Możliwość włączenia/wyłączenia minimapy.
    *   Filtrowanie skrzyń (ukrywanie zamkniętych).
    *   Opcja wyłączenia kolorów (tylko białe kropki).

## ⚠️ Wymagania i Uwagi / Requirements & Notes

1.  **Platforma:** Gothic 2 Noc Kruka (Night of the Raven).
2.  **Silnik:** Union SDK (testowano na najnowszej wersji 1.0m).
3.  **Rozdzielczość:** Plugin był testowany i optymalizowany pod rozdzielczość **2560x1440 (2K)**. Na innych rozdzielczościach pozycja lub rozmiar mapy mogą wymagać skalowania interfejsu w SystemPack.ini.
4.  **Wymagane Pluginy:**
    *   `zParserExtender` (do obsługi menu).
    *   `Union_Menu` (jeśli korzystasz z gotowych paczek Union).

## 📥 Instalacja / Installation

**(PL)**
1.  Pobierz najnowszy plik `.vdf` z zakładki [Releases](https://github.com/TieruYT/UnionMinimap/releases).
2.  Umieść plik w folderze `Data/` lub `Data/Plugins/` w głównym katalogu gry Gothic 2.
3.  Uruchom grę. Minimapa powinna się pojawić automatycznie po włączeniu w menu.

**(EN)**
1.  Download the latest `.vdf` file from the [Releases](https://github.com/TieruYT/UnionMinimap/releases) tab.
2.  Place the file in the `Data/` or `Data/Plugins/` folder in your Gothic 2 main directory.
3.  Launch the game. The minimap should appear automatically after enabling it in the menu.

## 🛠️ Kompilacja (Dla Deweloperów) / Build (For Developers)

**(PL)** Projekt wymaga **Visual Studio 2019** (lub nowszego z toolsetem v142) oraz zainstalowanego **Union SDK 1.0m**.

1.  Sklonuj repozytorium lub pobierz kod źródłowy.
2.  Otwórz `TieruPlugins.sln` w Visual Studio.
3.  Wybierz konfigurację `G2A MT Release`.
4.  Zbuduj rozwiązanie (Build Solution - F7).
5.  Skompilowane pliki znajdą się w folderze `Bin/`.
6.  Zapakuj pliki `.dll` oraz `.d` do archiwum VDF, zachowując strukturę: `System\Autorun\TieruMinimap.*`

**(EN)** The project requires **Visual Studio 2019** (or newer with v142 toolset) and **Union SDK 1.0m** installed.

1.  Clone the repository or download the source code.
2.  Open `TieruPlugins.sln` in Visual Studio.
3.  Select `G2A MT Release` configuration.
4.  Build the solution (Build Solution - F7).
5.  Compiled files will be in the `Bin/` folder.
6.  Pack the `.dll` and `.d` files into a VDF archive, maintaining structure: `System\Autorun\TieruMinimap.*`

## ⚙️ Konfiguracja / Configuration

**(PL)** Możesz zarządzać wszystkimi ustawieniami bezpośrednio w grze poprzez menu:
```
Menu → Opcje → Union → Tieru Plugins → Ustawienia Minimapy
```

Dostępne opcje:
- **Pokaz Minimape** - Włącza/wyłącza wyświetlanie minimapy
- **Pokaz Skrzynie** - Pokazuje/ukrywa skrzynie na minimapie
- **Ukrój zamknięte na klucz** - Ukrywa skrzynie wymagające klucza
- **Wszyscy NPC na biało** - Wyłącza kolorowe oznaczenia NPC (wszyscy będą biali)

**(EN)** You can manage all settings directly in the game via the menu:
```
Menu → Options → Union → Tieru Plugins → Minimap Settings
```

Available options:
- **Show Minimap** - Enables/disables minimap display
- **Show Chests** - Shows/hides chests on the minimap
- **Hide Key-locked Chests** - Hides chests that require a key
- **All NPCs White** - Disables colored NPC markers (all will be white)

### Ręczna konfiguracja / Manual Configuration

Alternatywnie możesz edytować plik `Gothic.ini`:
```ini
[QUESTHELPER_V2]
ShowMinimap=1          ; 1 = włączona, 0 = wyłączona
ShowChests=1           ; 1 = pokazuj skrzynie
HideLockedChests=0     ; 1 = ukryj skrzynie na klucz
AllNpcWhite=0          ; 1 = wszyscy NPC biali
```

## 📸 Screenshot

![Minimap Settings](screenshot.png)

## 🤝 Wkład / Contributing

**(PL)** Zgłoszenia błędów, propozycje funkcji i pull requesty są mile widziane! Otwórz [Issue](https://github.com/TieruYT/UnionMinimap/issues) lub prześlij PR.

**(EN)** Bug reports, feature requests, and pull requests are welcome! Open an [Issue](https://github.com/TieruYT/UnionMinimap/issues) or submit a PR.

## 📜 Licencja / License

Ten projekt jest udostępniony jako open source. Możesz swobodnie modyfikować i dystrybuować zgodnie z zasadami społeczności Gothic modding.

This project is available as open source. You can freely modify and distribute it according to Gothic modding community guidelines.

---

**Autor / Author:** Tieru
**GitHub:** https://github.com/TieruYT/
**Repozytorium / Repository:** https://github.com/TieruYT/UnionMinimap
