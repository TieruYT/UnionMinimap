# Gothic 2 Union Plugin: Tieru Minimap

![Gothic 2](https://img.shields.io/badge/Game-Gothic%202%20NoTR-blue) ![Union](https://img.shields.io/badge/Platform-Union%201.0m%2B-green) ![Version](https://img.shields.io/badge/Version-2.0-orange)

## 📖 Opis / Description

**(PL)** Zaawansowany plugin do gry Gothic 2 Noc Kruka (Union), dodający funkcjonalną minimapę z wieloma opcjami konfiguracji. Wyświetla pozycje NPC, wrogów, skrzyń, przedmiotów i roślin. Obsługuje rotację mapy zgodnie z kierunkiem postaci oraz zoom klawiszami.

**(EN)** An advanced Gothic 2 Night of the Raven (Union) plugin that adds a functional minimap with many configuration options. Displays positions of NPCs, enemies, chests, items, and plants. Supports map rotation according to character direction and keyboard zoom.

### ⚠️ Uwaga o rozwoju / Development Notice

**(PL)** Ten mod został stworzony przy wsparciu narzędzi AI (Gemini 3 Pro oraz Claude 4.5 Opus). Kod może zawierać błędy lub nieoptymalne rozwiązania. Wszelkie zgłoszenia błędów i propozycje ulepszeń są mile widziane w sekcji [Issues](https://github.com/TieruYT/UnionMinimap/issues).

**(EN)** This mod was created with the assistance of AI tools (Gemini 3 Pro and Claude 4.5 Opus). The code may contain bugs or non-optimal solutions. All bug reports and improvement suggestions are welcome in the [Issues](https://github.com/TieruYT/UnionMinimap/issues) section.

## 🌟 Funkcje / Features

### Minimapa / Minimap
*   **Rotacja mapy** - Mapa obraca się zgodnie z kierunkiem postaci (domyślnie włączone)
*   **Zoom klawiszami** - Przybliżaj (`.` lub `'`) i oddalaj (`,` lub `;`) mapę w czasie gry
*   **Strzałka gracza** - Wyraźny wskaźnik pozycji i kierunku gracza (tekstura O.TGA)

### Wskaźniki / Markers
| Kolor | Znaczenie (PL) | Meaning (EN) |
|-------|----------------|--------------|
| 🟡 Żółty | Gracz (strzałka) | Player (arrow) |
| 🔴 Czerwony | Wrogowie (zawsze widoczni) | Enemies (always visible) |
| 🟠 Pomarańczowy | NPC z questem | Quest NPCs |
| ⚪ Biały | Neutralni NPC | Neutral NPCs |
| 🔵 Niebieski | Przedmioty | Items |
| 🟢 Zielony | Rośliny i zioła | Plants and herbs |
| 🟢 Zielony (skrzynia) | Otwarta skrzynia | Open chest |
| 🟡 Żółty (skrzynia) | Skrzynia na wytrych | Picklock chest |
| 🟦 Cyjan | Skrzynia na klucz | Key chest |

### Opcje konfiguracji / Configuration Options
Menu dostępne przez: `Opcje → Union → Tieru Plugins → Ustawienia Minimapy`

| Opcja | Opis (PL) | Description (EN) |
|-------|-----------|------------------|
| Pokaż Minimapę | Włącz/wyłącz minimapę | Enable/disable minimap |
| Pokaż Skrzynie | Wyświetlaj skrzynie | Show chests |
| Ukryj zamknięte na klucz | Ukryj skrzynie wymagające klucza | Hide key-locked chests |
| Wszyscy NPC na biało | NPC bez kolorów (wrogowie nadal czerwoni) | NPCs white (enemies still red) |
| Pokaż Przedmioty | Wyświetlaj przedmioty (niebieskie) | Show items (blue) |
| Pokaż Rośliny | Wyświetlaj rośliny (zielone) | Show plants (green) |
| Pokaż Wrogów | Wyświetlaj wrogów (czerwone) | Show enemies (red) |
| Pokaż Nazwy NPC | Nazwy NPC nad kropkami | NPC names above dots |
| Pokaż Nazwy Przedmiotów | Nazwy przedmiotów | Item names |
| Pokaż Nazwy Roślin | Nazwy roślin i ziół | Plant names |
| Obracanie Mapy | Mapa obraca się z postacią | Map rotates with character |
| Zasięg Nazw NPC | Dystans wyświetlania nazw (1000-3500) | Name display range |
| Zoom Mapy | Przybliżenie mapy (1500-4000) | Map zoom level |

## ⌨️ Skróty klawiszowe / Keyboard Shortcuts

| Klawisz | Akcja |
|---------|-------|
| `;` lub `,` | Oddal mapę (zoom out) |
| `'` lub `.` | Przybliż mapę (zoom in) |

## ⚠️ Wymagania / Requirements

1.  **Gra:** Gothic 2 Noc Kruka (Night of the Raven)
2.  **Silnik:** Union SDK 1.0m lub nowszy
3.  **Rozdzielczość:** Testowano na 2560x1440 (2K)
4.  **Wymagane pluginy:**
    *   `zParserExtender`
    *   `Union_Menu`

## 📥 Instalacja / Installation

**(PL)**
1.  Pobierz najnowszy plik `.vdf` z zakładki [Releases](https://github.com/TieruYT/UnionMinimap/releases)
2.  Umieść plik w folderze `Data/` w katalogu gry Gothic 2
3.  Uruchom grę - minimapa pojawi się automatycznie

**(EN)**
1.  Download the latest `.vdf` file from [Releases](https://github.com/TieruYT/UnionMinimap/releases)
2.  Place the file in the `Data/` folder in your Gothic 2 directory
3.  Launch the game - minimap will appear automatically

## 🛠️ Kompilacja / Build

Wymagania:
- Visual Studio 2019+ (toolset v142)
- Union SDK 1.0m

```bash
1. Otwórz TieruPlugins.sln w Visual Studio
2. Wybierz konfigurację "G2A MD Release"
3. Build Solution (F7)
4. Pliki w folderze Bin/
```

## ⚙️ Ręczna konfiguracja / Manual Configuration

Edytuj `Gothic.ini`:
```ini
[QUESTHELPER_V2]
ShowMinimap=1
ShowChests=1
HideLockedChests=0
AllNpcWhite=1
ShowItems=0
ShowPlants=0
ShowEnemies=1
ShowNpcNames=0
ShowItemNames=0
ShowPlantNames=0
RotateMap=1
NpcNameRange=0
MapZoom=3
```

## 📝 Historia zmian / Changelog

### v2.0 (2024-12)
- ✨ Rotacja mapy zgodnie z kierunkiem postaci
- ✨ Zoom mapy klawiszami (`;'` lub `,.`)
- ✨ Wyświetlanie przedmiotów i roślin na mapie
- ✨ Wyświetlanie nazw NPC, przedmiotów i roślin
- ✨ Konfigurowalne zasięgi nazw NPC
- ✨ Strzałka gracza jako tekstura (O.TGA)
- ✨ Wrogowie zawsze czerwoni (nawet przy "Wszyscy NPC na biało")
- ✨ 13 opcji konfiguracyjnych w menu
- 🔧 Optymalizacja renderowania

### v1.0
- Podstawowa minimapa
- Wyświetlanie NPC i skrzyń
- 4 opcje konfiguracyjne

## 🤝 Wkład / Contributing

Zgłoszenia błędów, propozycje funkcji i pull requesty są mile widziane!
- [Issues](https://github.com/TieruYT/UnionMinimap/issues)
- [Pull Requests](https://github.com/TieruYT/UnionMinimap/pulls)

## 📜 Licencja / License

Open source - swobodna modyfikacja i dystrybucja zgodnie z zasadami społeczności Gothic modding.

---

**Autor / Author:** Tieru
**GitHub:** https://github.com/TieruYT/
**Repozytorium / Repository:** https://github.com/TieruYT/UnionMinimap
