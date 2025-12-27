// This file added in headers queue
// File: "Sources.h"
#include "resource.h"
#include <string>
#include <vector>
#include <algorithm>
#include <windows.h>

namespace GOTHIC_ENGINE {

  // ========================================================================
  // SETTINGS & GLOBALS
  // ========================================================================
  #ifndef M_PI
  #define M_PI 3.14159265358979323846
  #endif

  zCView* pMinimap = nullptr;
  zCView* pBorderTop = nullptr;
  zCView* pBorderBottom = nullptr;
  zCView* pBorderLeft = nullptr;
  zCView* pBorderRight = nullptr;
  zCView* pCircularBorder = nullptr;  // Okrągła ramka minimapy
  zCView* pPlayerArrow = nullptr;  // Strzałka gracza z tekstury O.TGA

  const int MAP_SIZE = 1500;
  const int MAP_X = 6500;
  const int MAP_Y = 200;
  const float MAP_RANGE = 3000.0f;

  int opt_ShowMinimap = 0; // Default OFF to test if menu enables it
  int opt_ShowChests = 1;
  int opt_HideLockedChests = 0;
  int opt_AllNpcWhite = 1;    // Domyślnie włączone (quest NPC nie działa idealnie)
  int opt_ShowItems = 0;      // Pokaż przedmioty na mapie
  int opt_ShowPlants = 0;     // Pokaż rośliny na mapie
  int opt_ShowEnemies = 1;    // Pokaż wrogów na mapie
  int opt_ShowNpcNames = 0;   // Pokaż nazwy NPC nad kropkami
  int opt_ShowItemNames = 0;  // Pokaż nazwy przedmiotów
  int opt_ShowPlantNames = 0; // Pokaż nazwy roślin
  int opt_RotateMap = 1;      // Obracaj mapę zgodnie z kierunkiem gracza (domyślnie włączone)
  int opt_NpcNameRange = 500; // Zasięg wyświetlania nazw NPC (w jednostkach gry, ~100 = 1m)
  int opt_ShowCompass = 1;    // Pokaż kompas (N/S/E/W) na ramce minimapy
  int opt_CircularMap = 0;    // 0 = kwadratowa, 1 = okrągła minimapa
  int opt_ShowMapTexture = 1; // Pokaż teksturę tła mapy (domyślnie wł. dla kwadratu, wył. dla okręgu)

  // Zoom mapy - wartości: 0=1500, 1=2000, 2=2500, 3=3000, 4=3500, 5=4000
  int opt_MapZoom = 3;        // Domyślnie 3000 (indeks 3)
  float currentMapRange = 3000.0f;

  // Klawisze skrótów (kody VK_ wczytywane z INI)
  int key_ZoomIn = 0xBE;      // '.' (VK_OEM_PERIOD)
  int key_ZoomOut = 0xBC;     // ',' (VK_OEM_COMMA)
  int key_Legend = 0xBF;      // '/' (VK_OEM_2)
  int key_NpcSearch = 0x50;   // 'P'

  // ========================================================================
  // NAVIGATION SYSTEM
  // ========================================================================
  oCNpc* navTarget = nullptr;           // Aktualny cel nawigacji
  bool showNavMenu = false;             // Czy pokazać menu wyboru NPC
  std::vector<oCNpc*> navNpcList;       // Lista NPC do wyboru
  int navSelectedIndex = 0;             // Zaznaczony NPC w menu
  std::string navSearchBuffer = "";     // Bufor wyszukiwania
  zCView* pNavMenu = nullptr;           // Widok menu nawigacji
  bool navTypingMode = false;           // Czy jesteśmy w trybie wpisywania
  DWORD navLastInputTime = 0;           // Czas ostatniego inputu

  // ========================================================================
  // KEY BINDING SYSTEM - "Press any key to assign"
  // ========================================================================
  int keyBindMode = 0;                  // 0=off, 1=ZoomIn, 2=ZoomOut, 3=Legend, 4=NpcSearch
  zCView* pKeyBindOverlay = nullptr;    // Overlay "Press any key..."
  DWORD keyBindStartTime = 0;           // Czas rozpoczęcia trybu bindowania
  const DWORD KEY_BIND_TIMEOUT = 10000; // Timeout 10 sekund

  // ========================================================================
  // LEGEND & COMPASS
  // ========================================================================
  bool showLegend = false;              // Czy pokazać legendę
  zCView* pLegend = nullptr;            // Widok legendy
  DWORD legendLastInputTime = 0;        // Czas ostatniego inputu legendy

  // Flush all key states (Windows API)
  void FlushKeys() {
      for (int i = 0; i < 256; i++) GetAsyncKeyState(i);
  }      

  // ========================================================================
  // HELPERS
  // ========================================================================

  std::string ToStdString(const zSTRING& s) {
      if (s.IsEmpty()) return "";
      return std::string(s.ToChar());
  }

  // Zwraca pełną ścieżkę do pliku INI gry (sprawdza różne nazwy)
  zSTRING GetGameIniPath() {
      zSTRING basePath = zoptions->GetDirString(DIR_SYSTEM);

      // Upewnij się że ścieżka kończy się backslashem
      int len = basePath.Length();
      if (len > 0) {
          char lastChar = basePath.ToChar()[len - 1];
          if (lastChar != '\\' && lastChar != '/') {
              basePath = basePath + zSTRING("\\");
          }
      }

      // Lista możliwych nazw pliku INI
      const char* iniNames[] = { "Gothic.ini", "Gothic2.ini", "GOTHIC.INI", "gothic.ini" };

      for (int i = 0; i < 4; i++) {
          zSTRING path = basePath + zSTRING(iniNames[i]);
          if (GetFileAttributesA(path.ToChar()) != INVALID_FILE_ATTRIBUTES) {
              return path;
          }
      }

      // Fallback
      return basePath + zSTRING("Gothic.ini");
  }

  // Sprawdza czy punkt (x, y) jest wewnątrz okręgu minimapy
  // centerX, centerY = środek (4096, 4096), radius = promień (4096)
  bool IsInCircularBounds(int x, int y, int centerX, int centerY, int radius) {
      int dx = x - centerX;
      int dy = y - centerY;
      return (dx * dx + dy * dy) <= (radius * radius);
  }

  // ========================================================================
  // KEY NAME PARSER - parsuje nazwę klawisza na kod VK
  // ========================================================================
  // Obsługiwane formaty: "F1"-"F12", "A"-"Z", "0"-"9", "NUMPAD0"-"NUMPAD9",
  // "PERIOD", "COMMA", "SLASH", "BACKSLASH", "MINUS", "PLUS", "TAB", "SPACE",
  // "INSERT", "DELETE", "HOME", "END", "PAGEUP", "PAGEDOWN", etc.

  int ParseKeyName(const std::string& keyName) {
      if (keyName.empty()) return 0;

      // Konwertuj na uppercase
      std::string key = keyName;
      for (auto& c : key) c = toupper(c);

      // Pojedyncze litery A-Z
      if (key.length() == 1 && key[0] >= 'A' && key[0] <= 'Z') {
          return key[0];  // VK_A = 0x41 = 'A'
      }

      // Pojedyncze cyfry 0-9
      if (key.length() == 1 && key[0] >= '0' && key[0] <= '9') {
          return key[0];  // VK_0 = 0x30 = '0'
      }

      // Klawisze funkcyjne F1-F12
      if (key[0] == 'F' && key.length() >= 2 && key.length() <= 3) {
          int fNum = atoi(key.c_str() + 1);
          if (fNum >= 1 && fNum <= 12) return 0x6F + fNum;  // VK_F1 = 0x70
      }

      // Numpad 0-9
      if (key.substr(0, 6) == "NUMPAD" && key.length() == 7) {
          int num = key[6] - '0';
          if (num >= 0 && num <= 9) return 0x60 + num;  // VK_NUMPAD0 = 0x60
      }

      // Numpad operatory
      if (key == "NUMPADMUL" || key == "MULTIPLY") return 0x6A;  // VK_MULTIPLY
      if (key == "NUMPADADD" || key == "ADD") return 0x6B;       // VK_ADD
      if (key == "NUMPADSUB" || key == "SUBTRACT") return 0x6D;  // VK_SUBTRACT
      if (key == "NUMPADDEC" || key == "DECIMAL") return 0x6E;   // VK_DECIMAL
      if (key == "NUMPADDIV" || key == "DIVIDE") return 0x6F;    // VK_DIVIDE

      // Klawisze specjalne
      if (key == "SPACE" || key == "SPACEBAR") return VK_SPACE;
      if (key == "TAB") return VK_TAB;
      if (key == "ENTER" || key == "RETURN") return VK_RETURN;
      if (key == "ESCAPE" || key == "ESC") return VK_ESCAPE;
      if (key == "BACKSPACE" || key == "BACK") return VK_BACK;

      if (key == "INSERT" || key == "INS") return VK_INSERT;
      if (key == "DELETE" || key == "DEL") return VK_DELETE;
      if (key == "HOME") return VK_HOME;
      if (key == "END") return VK_END;
      if (key == "PAGEUP" || key == "PGUP" || key == "PRIOR") return VK_PRIOR;
      if (key == "PAGEDOWN" || key == "PGDN" || key == "NEXT") return VK_NEXT;

      if (key == "UP" || key == "ARROWUP") return VK_UP;
      if (key == "DOWN" || key == "ARROWDOWN") return VK_DOWN;
      if (key == "LEFT" || key == "ARROWLEFT") return VK_LEFT;
      if (key == "RIGHT" || key == "ARROWRIGHT") return VK_RIGHT;

      // Klawisze OEM (znaki specjalne)
      if (key == "PERIOD" || key == "DOT" || key == ".") return 0xBE;      // VK_OEM_PERIOD
      if (key == "COMMA" || key == ",") return 0xBC;                        // VK_OEM_COMMA
      if (key == "SLASH" || key == "/") return 0xBF;                        // VK_OEM_2
      if (key == "BACKSLASH" || key == "\\") return 0xDC;                   // VK_OEM_5
      if (key == "MINUS" || key == "-") return 0xBD;                        // VK_OEM_MINUS
      if (key == "PLUS" || key == "EQUALS" || key == "=") return 0xBB;      // VK_OEM_PLUS
      if (key == "SEMICOLON" || key == ";") return 0xBA;                    // VK_OEM_1
      if (key == "QUOTE" || key == "APOSTROPHE" || key == "'") return 0xDE; // VK_OEM_7
      if (key == "LBRACKET" || key == "[") return 0xDB;                     // VK_OEM_4
      if (key == "RBRACKET" || key == "]") return 0xDD;                     // VK_OEM_6
      if (key == "TILDE" || key == "GRAVE" || key == "`") return 0xC0;      // VK_OEM_3

      // Domyślne kody hex (np. "0xBE" lub "190")
      if (key.substr(0, 2) == "0X") {
          return strtol(key.c_str(), nullptr, 16);
      }
      if (key[0] >= '0' && key[0] <= '9') {
          return atoi(key.c_str());
      }

      return 0;  // Nieznany klawisz
  }

  // Konwertuje kod VK na nazwę (do zapisu w INI)
  std::string KeyCodeToName(int vk) {
      if (vk >= 'A' && vk <= 'Z') return std::string(1, (char)vk);
      if (vk >= '0' && vk <= '9') return std::string(1, (char)vk);
      if (vk >= 0x70 && vk <= 0x7B) return "F" + std::to_string(vk - 0x6F);
      if (vk >= 0x60 && vk <= 0x69) return "NUMPAD" + std::to_string(vk - 0x60);

      switch (vk) {
          case 0xBE: return "PERIOD";
          case 0xBC: return "COMMA";
          case 0xBF: return "SLASH";
          case 0xDC: return "BACKSLASH";
          case 0xBD: return "MINUS";
          case 0xBB: return "PLUS";
          case 0xBA: return "SEMICOLON";
          case 0xDE: return "QUOTE";
          case 0xDB: return "LBRACKET";
          case 0xDD: return "RBRACKET";
          case 0x6A: return "MULTIPLY";
          case 0x6B: return "ADD";
          case 0x6D: return "SUBTRACT";
          case VK_SPACE: return "SPACE";
          case VK_TAB: return "TAB";
          case VK_RETURN: return "ENTER";
          case VK_INSERT: return "INSERT";
          case VK_DELETE: return "DELETE";
          case VK_HOME: return "HOME";
          case VK_END: return "END";
          case VK_PRIOR: return "PAGEUP";
          case VK_NEXT: return "PAGEDOWN";
          default: return "0x" + std::to_string(vk);
      }
  }

  // Konwertuje kod VK na ładną nazwę do wyświetlania (ze znakami specjalnymi w cudzysłowie)
  std::string KeyCodeToDisplayName(int vk) {
      if (vk >= 'A' && vk <= 'Z') return std::string(1, (char)vk);
      if (vk >= '0' && vk <= '9') return std::string(1, (char)vk);
      if (vk >= 0x70 && vk <= 0x7B) return "F" + std::to_string(vk - 0x6F);
      if (vk >= 0x60 && vk <= 0x69) return "Num" + std::to_string(vk - 0x60);

      switch (vk) {
          case 0xBE: return "\".\"";      // Period
          case 0xBC: return "\",\"";      // Comma
          case 0xBF: return "\"/\"";      // Slash
          case 0xDC: return "\"\\\"";     // Backslash
          case 0xBD: return "\"-\"";      // Minus
          case 0xBB: return "\"+\"";      // Plus
          case 0xBA: return "\";\"";      // Semicolon
          case 0xDE: return "\"'\"";      // Quote
          case 0xDB: return "\"[\"";      // Left bracket
          case 0xDD: return "\"]\"";      // Right bracket
          case 0x6A: return "Num*";
          case 0x6B: return "Num+";
          case 0x6D: return "Num-";
          case VK_SPACE: return "Space";
          case VK_TAB: return "Tab";
          case VK_RETURN: return "Enter";
          case VK_INSERT: return "Ins";
          case VK_DELETE: return "Del";
          case VK_HOME: return "Home";
          case VK_END: return "End";
          case VK_PRIOR: return "PgUp";
          case VK_NEXT: return "PgDn";
          default: return "?";
      }
  }

  // ========================================================================
  // KEY BINDING FUNCTIONS - "Press any key to assign"
  // ========================================================================

  // Wykrywa wciśnięty klawisz (skanuje wszystkie VK codes)
  int DetectPressedKey() {
      // Pomijamy klawisze systemowe/modyfikujące i nawigacji menu
      const int skipKeys[] = {
          VK_LSHIFT, VK_RSHIFT, VK_SHIFT,
          VK_LCONTROL, VK_RCONTROL, VK_CONTROL,
          VK_LMENU, VK_RMENU, VK_MENU,  // Alt keys
          VK_LWIN, VK_RWIN,
          VK_CAPITAL, VK_NUMLOCK, VK_SCROLL,
          VK_PAUSE, VK_SNAPSHOT,  // PrintScreen
          VK_RETURN,  // Enter - używany w menu
          VK_UP, VK_DOWN, VK_LEFT, VK_RIGHT,  // Strzałki - nawigacja menu
          VK_LBUTTON, VK_RBUTTON, VK_MBUTTON,  // Przyciski myszy
          0
      };

      for (int vk = 1; vk < 256; vk++) {
          // Sprawdź czy to klawisz do pominięcia
          bool skip = false;
          for (int i = 0; skipKeys[i] != 0; i++) {
              if (vk == skipKeys[i]) { skip = true; break; }
          }
          if (skip) continue;

          // Sprawdź czy klawisz jest wciśnięty
          if (GetAsyncKeyState(vk) & 0x8000) {
              return vk;
          }
      }
      return 0;  // Żaden klawisz nie wciśnięty
  }

  // Pobiera nazwę klawisza do wyświetlenia (ładniejsza wersja)
  const char* GetKeyDisplayName(int bindMode) {
      switch (bindMode) {
          case 1: return "Zoom In";
          case 2: return "Zoom Out";
          case 3: return "Legend";
          case 4: return "NPC Search";
          default: return "Unknown";
      }
  }

  // Rysuje overlay "Press any key..."
  void DrawKeyBindOverlay() {
      if (keyBindMode == 0) {
          if (pKeyBindOverlay) {
              screen->RemoveItem(pKeyBindOverlay);
              delete pKeyBindOverlay;
              pKeyBindOverlay = nullptr;
          }
          return;
      }

      // Utwórz overlay jeśli nie istnieje
      if (!pKeyBindOverlay) {
          pKeyBindOverlay = new zCView(2000, 3000, 6000, 5000);
          pKeyBindOverlay->InsertBack("DEFAULT.TGA");
          pKeyBindOverlay->SetColor(zCOLOR(0, 0, 0, 240));
          pKeyBindOverlay->SetAlphaBlendFunc(zRND_ALPHA_FUNC_BLEND);
          pKeyBindOverlay->SetTransparency(240);
          screen->InsertItem(pKeyBindOverlay);
      }

      pKeyBindOverlay->ClrPrintwin();
      pKeyBindOverlay->SetFont("FONT_Default.tga");

      // Tytuł
      pKeyBindOverlay->SetFontColor(zCOLOR(255, 255, 0));
      zSTRING title = "=== ASSIGN KEY: ";
      title += GetKeyDisplayName(keyBindMode);
      title += " ===";
      pKeyBindOverlay->Print(200, 500, title);

      // Instrukcja
      pKeyBindOverlay->SetFontColor(zCOLOR(255, 255, 255));
      pKeyBindOverlay->Print(200, 2000, "Press any key to assign...");

      // Timeout info
      DWORD elapsed = GetTickCount() - keyBindStartTime;
      int remaining = (KEY_BIND_TIMEOUT - elapsed) / 1000;
      if (remaining < 0) remaining = 0;

      pKeyBindOverlay->SetFontColor(zCOLOR(150, 150, 150));
      zSTRING timeoutStr = "Timeout: ";
      timeoutStr += Z remaining;
      timeoutStr += "s | ESC = cancel";
      pKeyBindOverlay->Print(200, 3500, timeoutStr);
  }

  // Obsługa trybu przypisywania klawiszy
  void HandleKeyBindMode() {
      if (keyBindMode == 0) return;

      DWORD elapsed = GetTickCount() - keyBindStartTime;

      // Sprawdź timeout
      if (elapsed > KEY_BIND_TIMEOUT) {
          keyBindMode = 0;
          FlushKeys();
          return;
      }

      // Opóźnienie startowe 500ms - daj czas na puszczenie klawiszy menu
      if (elapsed < 500) {
          FlushKeys();
          return;
      }

      // ESC anuluje
      if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
          keyBindMode = 0;
          FlushKeys();
          return;
      }

      // Wykryj wciśnięty klawisz
      int detectedKey = DetectPressedKey();
      if (detectedKey != 0 && detectedKey != VK_ESCAPE) {
          std::string keyName = KeyCodeToName(detectedKey);

          // Zapisz klawisz do odpowiedniej opcji
          switch (keyBindMode) {
              case 1:  // ZoomIn
                  zoptions->WriteString("QUESTHELPER_V2", "KeyZoomIn", keyName.c_str(), TRUE);
                  key_ZoomIn = detectedKey;
                  break;
              case 2:  // ZoomOut
                  zoptions->WriteString("QUESTHELPER_V2", "KeyZoomOut", keyName.c_str(), TRUE);
                  key_ZoomOut = detectedKey;
                  break;
              case 3:  // Legend
                  zoptions->WriteString("QUESTHELPER_V2", "KeyLegend", keyName.c_str(), TRUE);
                  key_Legend = detectedKey;
                  break;
              case 4:  // NpcSearch
                  zoptions->WriteString("QUESTHELPER_V2", "KeyNpcSearch", keyName.c_str(), TRUE);
                  key_NpcSearch = detectedKey;
                  break;
          }

          // Wymuszony zapis do pliku INI (bezpośrednio przez Windows API)
          zSTRING iniPath = GetGameIniPath();
          WritePrivateProfileStringA("QUESTHELPER_V2", "KeyZoomIn", KeyCodeToName(key_ZoomIn).c_str(), iniPath.ToChar());
          WritePrivateProfileStringA("QUESTHELPER_V2", "KeyZoomOut", KeyCodeToName(key_ZoomOut).c_str(), iniPath.ToChar());
          WritePrivateProfileStringA("QUESTHELPER_V2", "KeyLegend", KeyCodeToName(key_Legend).c_str(), iniPath.ToChar());
          WritePrivateProfileStringA("QUESTHELPER_V2", "KeyNpcSearch", KeyCodeToName(key_NpcSearch).c_str(), iniPath.ToChar());

          // Zakończ tryb bindowania
          keyBindMode = 0;
          FlushKeys();

          // Dodatkowe opóźnienie aby nowy klawisz nie aktywował funkcji od razu
          keyBindStartTime = GetTickCount();  // Użyj jako znacznik czasu zakończenia
      }
  }

  // Sprawdza czy minęło wystarczająco czasu od ostatniego bindowania
  bool IsKeyBindCooldownActive() {
      // 500ms cooldown po zakończeniu bindowania
      return (GetTickCount() - keyBindStartTime) < 500;
  }

  // Sprawdza czy menu aktywowało tryb bindowania
  void CheckKeyBindTrigger() {
      if (keyBindMode != 0) return;  // Już w trybie bindowania

      // Ścieżka do INI (do resetu)
      zSTRING iniPath = GetGameIniPath();

      // Czytaj triggery z zoptions (tam menu zapisuje), resetuj w obu miejscach
      if (zoptions->ReadInt("QUESTHELPER_V2", "KeyBindZoomIn", 0) == 1) {
          keyBindMode = 1;
          zoptions->WriteInt("QUESTHELPER_V2", "KeyBindZoomIn", 0, TRUE);
          WritePrivateProfileStringA("QUESTHELPER_V2", "KeyBindZoomIn", "0", iniPath.ToChar());
      }
      else if (zoptions->ReadInt("QUESTHELPER_V2", "KeyBindZoomOut", 0) == 1) {
          keyBindMode = 2;
          zoptions->WriteInt("QUESTHELPER_V2", "KeyBindZoomOut", 0, TRUE);
          WritePrivateProfileStringA("QUESTHELPER_V2", "KeyBindZoomOut", "0", iniPath.ToChar());
      }
      else if (zoptions->ReadInt("QUESTHELPER_V2", "KeyBindLegend", 0) == 1) {
          keyBindMode = 3;
          zoptions->WriteInt("QUESTHELPER_V2", "KeyBindLegend", 0, TRUE);
          WritePrivateProfileStringA("QUESTHELPER_V2", "KeyBindLegend", "0", iniPath.ToChar());
      }
      else if (zoptions->ReadInt("QUESTHELPER_V2", "KeyBindNpcSearch", 0) == 1) {
          keyBindMode = 4;
          zoptions->WriteInt("QUESTHELPER_V2", "KeyBindNpcSearch", 0, TRUE);
          WritePrivateProfileStringA("QUESTHELPER_V2", "KeyBindNpcSearch", "0", iniPath.ToChar());
      }

      if (keyBindMode != 0) {
          keyBindStartTime = GetTickCount();
          FlushKeys();
          if (zinput) zinput->ClearKeyBuffer();
      }
  }

  void LoadOptions() {
      if (!zoptions) return;
      // Force read every frame
      opt_ShowMinimap = zoptions->ReadInt("QUESTHELPER_V2", "ShowMinimap", 0);
      opt_ShowChests = zoptions->ReadInt("QUESTHELPER_V2", "ShowChests", 1);
      opt_HideLockedChests = zoptions->ReadInt("QUESTHELPER_V2", "HideLockedChests", 0);
      opt_AllNpcWhite = zoptions->ReadInt("QUESTHELPER_V2", "AllNpcWhite", 0);
      opt_ShowItems = zoptions->ReadInt("QUESTHELPER_V2", "ShowItems", 0);
      opt_ShowPlants = zoptions->ReadInt("QUESTHELPER_V2", "ShowPlants", 0);
      opt_ShowEnemies = zoptions->ReadInt("QUESTHELPER_V2", "ShowEnemies", 1);
      opt_ShowNpcNames = zoptions->ReadInt("QUESTHELPER_V2", "ShowNpcNames", 0);
      opt_ShowItemNames = zoptions->ReadInt("QUESTHELPER_V2", "ShowItemNames", 0);
      opt_ShowPlantNames = zoptions->ReadInt("QUESTHELPER_V2", "ShowPlantNames", 0);
      opt_RotateMap = zoptions->ReadInt("QUESTHELPER_V2", "RotateMap", 1);
      opt_ShowCompass = zoptions->ReadInt("QUESTHELPER_V2", "ShowCompass", 1);
      opt_CircularMap = zoptions->ReadInt("QUESTHELPER_V2", "CircularMap", 0);

      // Tekstura mapy - domyślnie wł. dla kwadratu, wył. dla okręgu
      int defaultTexture = opt_CircularMap ? 0 : 1;
      opt_ShowMapTexture = zoptions->ReadInt("QUESTHELPER_V2", "ShowMapTexture", defaultTexture);

      opt_MapZoom = zoptions->ReadInt("QUESTHELPER_V2", "MapZoom", 3);

      // ========================================================================
      // KLAWISZE SKRÓTÓW - czytaj bezpośrednio z pliku INI (nie z cache)
      // ========================================================================
      // Obsługiwane: A-Z, 0-9, F1-F12, NUMPAD0-9, PERIOD, COMMA, SLASH, BACKSLASH,
      //              MINUS, PLUS, SEMICOLON, QUOTE, LBRACKET, RBRACKET, SPACE, TAB,
      //              INSERT, DELETE, HOME, END, PAGEUP, PAGEDOWN, kody hex (0xBE)

      // Buduj ścieżkę do pliku INI gry
      zSTRING iniPath = GetGameIniPath();

      // Czytaj bezpośrednio z pliku INI przez Windows API
      char keyBuffer[64];

      GetPrivateProfileStringA("QUESTHELPER_V2", "KeyZoomIn", "", keyBuffer, sizeof(keyBuffer), iniPath.ToChar());
      std::string strZoomIn = keyBuffer;

      GetPrivateProfileStringA("QUESTHELPER_V2", "KeyZoomOut", "", keyBuffer, sizeof(keyBuffer), iniPath.ToChar());
      std::string strZoomOut = keyBuffer;

      GetPrivateProfileStringA("QUESTHELPER_V2", "KeyLegend", "", keyBuffer, sizeof(keyBuffer), iniPath.ToChar());
      std::string strLegend = keyBuffer;

      GetPrivateProfileStringA("QUESTHELPER_V2", "KeyNpcSearch", "", keyBuffer, sizeof(keyBuffer), iniPath.ToChar());
      std::string strNpcSearch = keyBuffer;

      int parsedZoomIn = ParseKeyName(strZoomIn);
      int parsedZoomOut = ParseKeyName(strZoomOut);
      int parsedLegend = ParseKeyName(strLegend);
      int parsedNpcSearch = ParseKeyName(strNpcSearch);

      key_ZoomIn = parsedZoomIn ? parsedZoomIn : 0xBE;      // domyślnie '.'
      key_ZoomOut = parsedZoomOut ? parsedZoomOut : 0xBC;   // domyślnie ','
      key_Legend = parsedLegend ? parsedLegend : 0xBF;      // domyślnie '/'
      key_NpcSearch = parsedNpcSearch ? parsedNpcSearch : 0x50; // domyślnie 'P'

      // Zapisz domyślne wartości do INI jeśli puste
      if (strZoomIn.empty()) WritePrivateProfileStringA("QUESTHELPER_V2", "KeyZoomIn", "PERIOD", iniPath.ToChar());
      if (strZoomOut.empty()) WritePrivateProfileStringA("QUESTHELPER_V2", "KeyZoomOut", "COMMA", iniPath.ToChar());
      if (strLegend.empty()) WritePrivateProfileStringA("QUESTHELPER_V2", "KeyLegend", "SLASH", iniPath.ToChar());
      if (strNpcSearch.empty()) WritePrivateProfileStringA("QUESTHELPER_V2", "KeyNpcSearch", "P", iniPath.ToChar());

      // NpcNameRange - menu zapisuje indeks (0-4), konwertuj na wartość
      int rangeIndex = zoptions->ReadInt("QUESTHELPER_V2", "NpcNameRange", 0); // domyślnie 0 = 1000
      const int rangeValues[] = { 1000, 1500, 2000, 2500, 3500 };
      if (rangeIndex >= 0 && rangeIndex <= 4) {
          opt_NpcNameRange = rangeValues[rangeIndex];
      } else {
          opt_NpcNameRange = 1000; // fallback
      }

      // MapZoom - konwertuj indeks na wartość zasięgu (0=500, 1=1000, 2=1500, ...)
      const float zoomValues[] = { 500.0f, 1000.0f, 1500.0f, 2000.0f, 2500.0f, 3000.0f, 3500.0f, 4000.0f };
      if (opt_MapZoom >= 0 && opt_MapZoom <= 7) {
          currentMapRange = zoomValues[opt_MapZoom];
      } else {
          currentMapRange = 3000.0f;
      }
  }

  // Obsługa klawiszy zoom (wywoływana w Game_Loop)
  void HandleZoomKeys() {
      if (!zoptions) return;
      if (IsKeyBindCooldownActive()) return;  // Cooldown po bindowaniu

      static bool keyPressed = false;

      // Używa konfigurowalnych klawiszy z INI
      bool zoomOut = (GetAsyncKeyState(key_ZoomOut) & 0x8000) != 0;
      bool zoomIn = (GetAsyncKeyState(key_ZoomIn) & 0x8000) != 0;

      if ((zoomIn || zoomOut) && !keyPressed) {
          keyPressed = true;

          if (zoomIn && opt_MapZoom > 0) {
              opt_MapZoom--;
              zoptions->WriteInt("QUESTHELPER_V2", "MapZoom", opt_MapZoom, TRUE);
          }
          else if (zoomOut && opt_MapZoom < 7) {
              opt_MapZoom++;
              zoptions->WriteInt("QUESTHELPER_V2", "MapZoom", opt_MapZoom, TRUE);
          }

          const float zoomValues[] = { 500.0f, 1000.0f, 1500.0f, 2000.0f, 2500.0f, 3000.0f, 3500.0f, 4000.0f };
          currentMapRange = zoomValues[opt_MapZoom];
      }

      if (!zoomIn && !zoomOut) {
          keyPressed = false;
      }
  }

  // Funkcja do rotacji punktu względem środka mapy (rotacja zgodna z ruchem wskazówek zegara)
  void RotatePoint(float& relX, float& relZ, float angle) {
      float cosA = cos(angle);
      float sinA = sin(angle);
      // Standardowa rotacja 2D (clockwise)
      float newX = relX * cosA + relZ * sinA;
      float newZ = -relX * sinA + relZ * cosA;
      relX = newX;
      relZ = newZ;
  }

  zSTRING HasQuestDialogue(oCNpc* npc) {
      if (!npc || !player || !ogame) return "";
      oCInfoManager* im = ogame->GetInfoManager();
      if (!im) return "";

      zCParser* parser = zCParser::GetParser();
      if (parser) {
          parser->SetInstance("SELF", npc);
          parser->SetInstance("self", npc);
          parser->SetInstance("OTHER", player);
          parser->SetInstance("other", player);
          parser->SetInstance("HERO", player);
          parser->SetInstance("hero", player);
          parser->SetInstance("PC_HERO", player);
      }

      int npcInstance = npc->GetInstance();
      zCListSort<oCInfo>* list = &im->infoList;
      while (list) {
          oCInfo* info = list->GetData();
          if (info) {
              if (info->pd.npc == npcInstance) {
                  if (!info->pd.permanent && !info->pd.trade) {
                      if (info->WasTold()) {
                          list = list->next;
                          continue;
                      }
                      zSTRING desc = info->pd.description;
                      if (!info->pd.important && desc.IsEmpty()) {
                           list = list->next;
                           continue;
                      }
                      if (!desc.IsEmpty()) {
                          desc.Upper(); 
                          if (desc.HasWord("KONIEC") || desc.HasWord("ENDE") || desc.HasWord("BACK") || desc.HasWord("WR\xD3\x86")) { 
                              list = list->next;
                              continue;
                          }
                      }
                      if (info->DoCheck()) return info->pd.description; 
                  }
              }
          }
          list = list->next;
      }
      return "";
  }

  // ========================================================================
  // PLANT DETECTION
  // ========================================================================

  bool IsPlantItem(oCItem* item) {
      if (!item) return false;

      // Sprawdzamy po nazwie instancji - typowe rośliny w Gothic 2
      zSTRING name = item->GetInstanceName();
      name.Upper();

      // Lista typowych roślin w Gothic 2 Noc Kruka
      if (name.HasWord("PLANT") ||
          name.HasWord("HERB") ||
          name.HasWord("MUSHROOM") ||
          name.HasWord("KRAUT") ||       // niemieckie zioła
          name.HasWord("PILZ") ||        // niemieckie grzyby
          name.HasWord("ITMIPLANT") ||   // prefix dla roślin
          name.HasWord("ITPL_") ||       // alternatywny prefix
          name.HasWord("ITMI_PLANT") ||
          name.HasWord("ITFO_PLANT") ||
          name.HasWord("ITMIPILZ") ||    // grzyby
          name.HasWord("ITMI_MUSHROOM") ||
          name.HasWord("APPLE") ||
          name.HasWord("APFEL") ||
          name.HasWord("BERRY") ||
          name.HasWord("BEERE")) {
          return true;
      }

      return false;
  }

  // ========================================================================
  // COMPASS & LEGEND FUNCTIONS
  // ========================================================================

  // Rysuje kompas NA RAMCE minimapy (w koordynatach ekranu)
  void DrawCompass(float mapRotation) {
      if (!screen || !opt_ShowCompass) return;

      // Środek minimapy w koordynatach ekranu
      int centerX = MAP_X + MAP_SIZE / 2;  // 7250
      int centerY = MAP_Y + MAP_SIZE / 2;  // 950

      // Promień kompasu - na zewnątrz ramki
      int compassRadius = MAP_SIZE / 2 + 80;  // Trochę poza ramką

      // Kierunki: N, E, S, W
      float angles[4] = { 0.0f, (float)M_PI/2, (float)M_PI, (float)(-M_PI/2) };
      const char* letters[4] = { "N", "E", "S", "W" };
      zCOLOR colors[4] = {
          zCOLOR(255, 80, 80),   // N - czerwony
          zCOLOR(220, 220, 220), // E - jasny szary
          zCOLOR(220, 220, 220), // S - jasny szary
          zCOLOR(220, 220, 220)  // W - jasny szary
      };

      // Mała czcionka dla kompasu
      screen->SetFont("FONT_OLD_10_WHITE.TGA");

      for (int i = 0; i < 4; i++) {
          float angle = angles[i] + mapRotation;
          int px = centerX + (int)(compassRadius * sin(angle));
          int py = centerY - (int)(compassRadius * cos(angle));

          // Korekta pozycji dla centrowania liter
          px -= 30;  // Przesunięcie w lewo (połowa szerokości litery)
          py -= 40;  // Przesunięcie w górę

          screen->SetFontColor(colors[i]);
          screen->Print(px, py, letters[i]);
      }
  }

  // Rysuje legendę kolorów
  void DrawLegend() {
      if (!showLegend) {
          if (pLegend) {
              screen->RemoveItem(pLegend);
              delete pLegend;
              pLegend = nullptr;
          }
          return;
      }

      // Utwórz widok legendy jeśli nie istnieje (powiększona dla skrótów)
      if (!pLegend) {
          pLegend = new zCView(500, 800, 3800, 7200);
          pLegend->InsertBack("DEFAULT.TGA");
          pLegend->SetColor(zCOLOR(0, 0, 0, 230));
          pLegend->SetAlphaBlendFunc(zRND_ALPHA_FUNC_BLEND);
          pLegend->SetTransparency(230);
          screen->InsertItem(pLegend);
      }

      pLegend->ClrPrintwin();
      pLegend->SetFont("FONT_Default.tga");

      // Tytuł
      pLegend->SetFontColor(zCOLOR(255, 255, 0));
      pLegend->Print(100, 150, "=== LEGENDA MINIMAPY ===");

      int yPos = 600;
      int step = 380;

      // Lista kolorów i opisów
      struct LegendItem { zCOLOR color; const char* desc; };
      LegendItem items[] = {
          { zCOLOR(255, 255, 0),   "Gracz (strzalka)" },
          { zCOLOR(255, 0, 0),     "Wrogowie" },
          { zCOLOR(255, 128, 0),   "Wazny NPC" },
          { zCOLOR(255, 255, 255), "Neutralny NPC" },
          { zCOLOR(100, 150, 255), "Przedmioty" },
          { zCOLOR(0, 200, 50),    "Rosliny / Ziola" },
          { zCOLOR(0, 255, 0),     "Otwarta skrzynia" },
          { zCOLOR(255, 255, 0),   "Skrzynia na wytrych" },
          { zCOLOR(0, 255, 255),   "Skrzynia na klucz" },
          { zCOLOR(255, 200, 0),   "Cel nawigacji" }
      };

      for (int i = 0; i < 10; i++) {
          pLegend->SetFontColor(items[i].color);
          pLegend->Print(150, yPos, "o");
          pLegend->SetFontColor(zCOLOR(220, 220, 220));
          pLegend->Print(400, yPos, items[i].desc);
          yPos += step;
      }

      // Sekcja skrótów klawiszowych
      yPos += 200;
      pLegend->SetFontColor(zCOLOR(255, 255, 0));
      pLegend->Print(100, yPos, "=== SKROTY ===");
      yPos += 400;

      pLegend->SetFontColor(zCOLOR(180, 180, 180));
      zSTRING keyLine;

      keyLine = "Zoom +: "; keyLine += KeyCodeToDisplayName(key_ZoomIn).c_str();
      pLegend->Print(150, yPos, keyLine); yPos += step;

      keyLine = "Zoom -: "; keyLine += KeyCodeToDisplayName(key_ZoomOut).c_str();
      pLegend->Print(150, yPos, keyLine); yPos += step;

      keyLine = "Legenda: "; keyLine += KeyCodeToDisplayName(key_Legend).c_str();
      pLegend->Print(150, yPos, keyLine); yPos += step;

      keyLine = "Szukaj NPC: "; keyLine += KeyCodeToDisplayName(key_NpcSearch).c_str();
      pLegend->Print(150, yPos, keyLine); yPos += step;

      // Instrukcja zamknięcia
      pLegend->SetFontColor(zCOLOR(100, 100, 100));
      keyLine = "("; keyLine += KeyCodeToDisplayName(key_Legend).c_str(); keyLine += " = zamknij)";
      pLegend->Print(100, 7600, keyLine);
  }

  // Obsługa klawisza legendy
  void HandleLegendKey() {
      if (IsKeyBindCooldownActive()) return;  // Cooldown po bindowaniu

      DWORD currentTime = GetTickCount();

      // Używa konfigurowalnego klawisza z INI (domyślnie '/')
      if (GetAsyncKeyState(key_Legend) & 0x8000) {
          if (currentTime - legendLastInputTime > 300) {
              showLegend = !showLegend;
              legendLastInputTime = currentTime;
          }
      }
  }

  // ========================================================================
  // NAVIGATION FUNCTIONS
  // ========================================================================

  // Buduje listę wszystkich żywych NPC w świecie gry
  void BuildNpcList() {
      navNpcList.clear();
      if (!ogame || !ogame->GetGameWorld()) return;

      zCListSort<oCNpc>* list = ogame->GetGameWorld()->voblist_npcs;
      while (list) {
          oCNpc* npc = list->GetData();
          list = list->next;

          if (!npc || npc == player) continue;
          if (npc->attribute[0] <= 0) continue;  // Pomiń martwych

          // Sprawdź czy koordynaty nie są 0,0,0
          zVEC3 pos = npc->GetPositionWorld();
          if (pos[0] == 0.0f && pos[1] == 0.0f && pos[2] == 0.0f) continue;

          // Jeśli jest filtr wyszukiwania - sprawdź nazwę
          if (!navSearchBuffer.empty()) {
              std::string npcName = ToStdString(npc->GetName(0));
              std::string search = navSearchBuffer;
              // Konwertuj na małe litery do porównania
              std::transform(npcName.begin(), npcName.end(), npcName.begin(), ::tolower);
              std::transform(search.begin(), search.end(), search.begin(), ::tolower);
              if (npcName.find(search) == std::string::npos) continue;
          }

          navNpcList.push_back(npc);
      }

      // Sortuj alfabetycznie
      std::sort(navNpcList.begin(), navNpcList.end(), [](oCNpc* a, oCNpc* b) {
          return ToStdString(a->GetName(0)) < ToStdString(b->GetName(0));
      });

      // Resetuj indeks jeśli wyszedł poza zakres
      if (navSelectedIndex >= (int)navNpcList.size()) {
          navSelectedIndex = navNpcList.size() > 0 ? (int)navNpcList.size() - 1 : 0;
      }
  }

  // Rysuje menu wyboru NPC
  void DrawNavMenu() {
      if (!showNavMenu) {
          if (pNavMenu) {
              screen->RemoveItem(pNavMenu);
              delete pNavMenu;
              pNavMenu = nullptr;
          }
          return;
      }

      if (!pNavMenu) return;

      pNavMenu->ClrPrintwin();
      pNavMenu->SetFont("FONT_Default.tga");

      // Tytuł - zależy od trybu
      zSTRING title;
      if (navTypingMode) {
          pNavMenu->SetFontColor(zCOLOR(100, 255, 100));
          title = "SZUKAJ: ";
          title += navSearchBuffer.c_str();
          title += "_";
      } else if (!navSearchBuffer.empty()) {
          pNavMenu->SetFontColor(zCOLOR(255, 255, 0));
          title = "FILTR: ";
          title += navSearchBuffer.c_str();
          title += " (F = szukaj)";
      } else {
          pNavMenu->SetFontColor(zCOLOR(255, 255, 0));
          title = "=== NAWIGACJA DO NPC ===";
      }
      pNavMenu->Print(100, 100, title);

      // Instrukcje
      pNavMenu->SetFontColor(zCOLOR(180, 180, 180));
      if (navTypingMode) {
          pNavMenu->Print(100, 400, "Wpisz nazwe NPC | Enter: wybierz | Esc: anuluj");
      } else {
          pNavMenu->Print(100, 400, "Strzalki: wybierz | Enter: nawiguj | F: szukaj");
          pNavMenu->Print(100, 650, "Backspace: usun cel | Esc: zamknij");
      }

      // Lista NPC (max 10 widocznych)
      int startIdx = navSelectedIndex - 5;
      if (startIdx < 0) startIdx = 0;
      int endIdx = startIdx + 10;
      if (endIdx > (int)navNpcList.size()) endIdx = (int)navNpcList.size();

      int yPos = 1300;
      for (int i = startIdx; i < endIdx; i++) {
          oCNpc* npc = navNpcList[i];
          if (!npc) continue;

          // Oblicz dystans
          float dist = npc->GetDistanceToVob(*player);
          int distMeters = (int)(dist / 100.0f);  // ~100 jednostek = 1m

          // Zbuduj tekst
          zSTRING line = "";
          if (i == navSelectedIndex) {
              line = "> ";
              pNavMenu->SetFontColor(zCOLOR(255, 255, 0));  // Żółty dla zaznaczonego
          } else {
              line = "  ";
              pNavMenu->SetFontColor(zCOLOR(255, 255, 255));  // Biały
          }

          line += npc->GetName(0);
          line += " (";
          line += Z distMeters;
          line += "m)";

          pNavMenu->Print(100, yPos, line);
          yPos += 400;
      }

      // Informacja o liczbie wyników
      pNavMenu->SetFontColor(zCOLOR(150, 150, 150));
      zSTRING countInfo = "Znaleziono: ";
      countInfo += Z (int)navNpcList.size();
      countInfo += " NPC";
      pNavMenu->Print(100, 7000, countInfo);
  }

  // Otwiera menu nawigacji
  void OpenNavMenu() {
      // Utwórz widok menu jeśli nie istnieje
      if (!pNavMenu) {
          pNavMenu = new zCView(1500, 500, 6500, 7500);
          pNavMenu->InsertBack("DEFAULT.TGA");
          pNavMenu->SetColor(zCOLOR(0, 0, 0, 220));
          pNavMenu->SetAlphaBlendFunc(zRND_ALPHA_FUNC_BLEND);
          pNavMenu->SetTransparency(220);
      }
      screen->InsertItem(pNavMenu);

      showNavMenu = true;
      navTypingMode = false;
      navSearchBuffer = "";
      navSelectedIndex = 0;
      BuildNpcList();
      zinput->ClearKeyBuffer();
      FlushKeys();
  }

  // Zamyka menu nawigacji
  void CloseNavMenu() {
      if (pNavMenu) {
          screen->RemoveItem(pNavMenu);
      }
      showNavMenu = false;
      navTypingMode = false;
  }

  // Obsługa klawiszy dla menu nawigacji (Windows API)
  void HandleNavKeys() {
      if (!screen || !player) return;
      if (IsKeyBindCooldownActive()) return;  // Cooldown po bindowaniu

      DWORD currentTime = GetTickCount();

      // Używa konfigurowalnego klawisza z INI (domyślnie '\')
      if (GetAsyncKeyState(key_NpcSearch) & 0x8000) {
          if (currentTime - navLastInputTime > 300) {
              if (!showNavMenu) OpenNavMenu();
              else CloseNavMenu();
              navLastInputTime = currentTime;
          }
      }

      if (showNavMenu && pNavMenu) {
          // --- TRYB WPISYWANIA ---
          if (navTypingMode) {
              bool updated = false;

              // Nawigacja strzałkami (działa też w trybie wpisywania)
              if (currentTime - navLastInputTime > 100) {
                  if (GetAsyncKeyState(VK_UP) & 0x8000) {
                      if (navSelectedIndex > 0) navSelectedIndex--;
                      navLastInputTime = currentTime;
                  }
                  if (GetAsyncKeyState(VK_DOWN) & 0x8000) {
                      if (navSelectedIndex < (int)navNpcList.size() - 1) navSelectedIndex++;
                      navLastInputTime = currentTime;
                  }
                  if (GetAsyncKeyState(VK_PRIOR) & 0x8000) { navSelectedIndex -= 10; navLastInputTime = currentTime + 100; }
                  if (GetAsyncKeyState(VK_NEXT) & 0x8000) { navSelectedIndex += 10; navLastInputTime = currentTime + 100; }
              }

              // Litery A-Z (& 1 = single press)
              for (int key = 'A'; key <= 'Z'; key++) {
                  if (GetAsyncKeyState(key) & 1) {
                      navSearchBuffer += (char)key;
                      updated = true;
                  }
              }
              // Cyfry 0-9
              for (int key = '0'; key <= '9'; key++) {
                  if (GetAsyncKeyState(key) & 1) {
                      navSearchBuffer += (char)key;
                      updated = true;
                  }
              }
              // Spacja
              if (GetAsyncKeyState(VK_SPACE) & 1) {
                  navSearchBuffer += ' ';
                  updated = true;
              }
              // Backspace - usuń znak
              if (GetAsyncKeyState(VK_BACK) & 1) {
                  if (!navSearchBuffer.empty()) {
                      navSearchBuffer.pop_back();
                      updated = true;
                  }
              }
              // Enter - zakończ wpisywanie i wybierz NPC
              if (GetAsyncKeyState(VK_RETURN) & 1) {
                  if (!navNpcList.empty() && navSelectedIndex >= 0 && navSelectedIndex < (int)navNpcList.size()) {
                      navTarget = navNpcList[navSelectedIndex];
                      CloseNavMenu();
                      navLastInputTime = currentTime + 300;
                  } else {
                      navTypingMode = false;
                  }
              }
              // Escape - anuluj wyszukiwanie
              if (GetAsyncKeyState(VK_ESCAPE) & 1) {
                  navTypingMode = false;
                  navSearchBuffer = "";
                  updated = true;
              }

              if (updated) BuildNpcList();
          }
          // --- TRYB NAWIGACJI ---
          else {
              if (currentTime - navLastInputTime > 100) {
                  if (GetAsyncKeyState(VK_UP) & 0x8000) {
                      if (navSelectedIndex > 0) navSelectedIndex--;
                      navLastInputTime = currentTime;
                  }
                  if (GetAsyncKeyState(VK_DOWN) & 0x8000) {
                      if (navSelectedIndex < (int)navNpcList.size() - 1) navSelectedIndex++;
                      navLastInputTime = currentTime;
                  }
                  if (GetAsyncKeyState(VK_PRIOR) & 0x8000) { navSelectedIndex -= 10; navLastInputTime = currentTime + 100; }
                  if (GetAsyncKeyState(VK_NEXT) & 0x8000) { navSelectedIndex += 10; navLastInputTime = currentTime + 100; }

                  // Enter - wybierz NPC jako cel
                  if (GetAsyncKeyState(VK_RETURN) & 0x8000) {
                      if (!navNpcList.empty() && navSelectedIndex >= 0 && navSelectedIndex < (int)navNpcList.size()) {
                          navTarget = navNpcList[navSelectedIndex];
                          CloseNavMenu();
                      }
                      navLastInputTime = currentTime + 500;
                  }
                  // Escape - zamknij menu
                  if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
                      CloseNavMenu();
                      navLastInputTime = currentTime + 200;
                  }
                  // F - włącz tryb wpisywania
                  if (GetAsyncKeyState('F') & 0x8000) {
                      navTypingMode = true;
                      navLastInputTime = currentTime + 300;
                      FlushKeys();
                  }
                  // Backspace - wyczyść cel nawigacji
                  if (GetAsyncKeyState(VK_BACK) & 0x8000) {
                      navTarget = nullptr;
                      navLastInputTime = currentTime + 300;
                  }
              }
          }

          // Clamp index
          if (navSelectedIndex < 0) navSelectedIndex = (int)navNpcList.size() - 1;
          if (navSelectedIndex >= (int)navNpcList.size()) navSelectedIndex = 0;

          // Blokuj input gry
          zinput->ClearKeyBuffer();
          FlushKeys();
      }
  }

  // Rysuje linię nawigacyjną od gracza do celu
  void DrawNavLine(zCView* view, int centerX, int centerY, float scale, float mapRotation) {
      if (!navTarget || !player) return;

      // Sprawdź czy cel jest żywy
      if (navTarget->attribute[0] <= 0) {
          navTarget = nullptr;
          return;
      }

      // Sprawdź czy gracz dotarł do celu (dystans < 300 jednostek = ~3m)
      float distToTarget = navTarget->GetDistanceToVob(*player);
      if (distToTarget < 300.0f) {
          navTarget = nullptr;  // Cel osiągnięty - wyczyść nawigację
          return;
      }

      // Pobierz pozycję celu
      zVEC3 targetPos = navTarget->GetPositionWorld();

      // Sprawdź czy koordynaty są prawidłowe (nie 0,0,0)
      if (targetPos[0] == 0.0f && targetPos[1] == 0.0f && targetPos[2] == 0.0f) {
          return;
      }

      zVEC3 plrPos = player->GetPositionWorld();
      float relX = targetPos[0] - plrPos[0];
      float relZ = targetPos[2] - plrPos[2];

      // Rotacja mapy jeśli włączona
      if (opt_RotateMap) {
          RotatePoint(relX, relZ, mapRotation);
      }

      int targetMapX = centerX + (int)(relX * scale);
      int targetMapY = centerY - (int)(relZ * scale);

      // Ogranicz pozycję do granic mapy
      bool targetVisible = true;
      if (targetMapX < 0) { targetMapX = 100; targetVisible = false; }
      if (targetMapX > 8192) { targetMapX = 8092; targetVisible = false; }
      if (targetMapY < 0) { targetMapY = 100; targetVisible = false; }
      if (targetMapY > 8192) { targetMapY = 8092; targetVisible = false; }

      // Rysuj linię od środka (gracza) do celu
      view->SetFontColor(zCOLOR(255, 200, 0));  // Złota linia nawigacji

      // Rysuj linię jako serię kropek
      float dx = (float)(targetMapX - centerX);
      float dy = (float)(targetMapY - centerY);
      float dist = sqrt(dx * dx + dy * dy);
      int steps = (int)(dist / 100.0f);  // Co ~100 jednostek
      if (steps < 5) steps = 5;
      if (steps > 100) steps = 100;

      for (int i = 0; i <= steps; i++) {
          float t = (float)i / (float)steps;
          int px = centerX + (int)(dx * t);
          int py = centerY + (int)(dy * t);

          if (px >= 0 && px <= 8192 && py >= 0 && py <= 8192) {
              view->Print(px, py, ".");
          }
      }

      // Narysuj marker celu (większa kropka/kwadrat)
      view->SetFontColor(zCOLOR(255, 100, 0));  // Pomarańczowy marker celu
      for (int dx = -50; dx <= 50; dx += 50) {
          for (int dy = -50; dy <= 50; dy += 50) {
              int px = targetMapX + dx;
              int py = targetMapY + dy;
              if (px >= 0 && px <= 8192 && py >= 0 && py <= 8192) {
                  view->Print(px, py, ".");
              }
          }
      }

      // Wyświetl nazwę celu i dystans nad markerem
      int distMeters = (int)(distToTarget / 100.0f);

      zSTRING targetLabel = navTarget->GetName(0);
      targetLabel += " (";
      targetLabel += Z distMeters;
      targetLabel += "m)";

      view->SetFont("FONT_Default.tga");
      view->SetFontColor(zCOLOR(255, 200, 0));
      int labelY = targetMapY - 150;
      if (labelY < 100) labelY = targetMapY + 100;
      view->Print(targetMapX - 100, labelY, targetLabel);
  }

  // ========================================================================
  // MINIMAP LOGIC
  // ========================================================================

  void DrawDot(zCView* view, int x, int y, zCOLOR color) {
      if (x < 0 || x > 8192 || y < 0 || y > 8192) return;
      // Dla okrągłej minimapy - sprawdź czy punkt jest w kole
      if (opt_CircularMap && !IsInCircularBounds(x, y, 4096, 4096, 4000)) return;
      view->SetFontColor(color);
      view->Print(x, y, ".");
  }

  // Rysuje strzałkę gracza wskazującą kierunek patrzenia
  void DrawPlayerArrow(zCView* view, int centerX, int centerY, float angle, zCOLOR color) {
      // Rozmiar strzałki (zmniejszona 2x)
      const int arrowLength = 200;  // Długość strzałki
      const int arrowWidth = 100;   // Szerokość podstawy

      // Oblicz punkty strzałki (trójkąt)
      // Czubek strzałki (przód)
      float tipX = centerX + arrowLength * sin(angle);
      float tipY = centerY - arrowLength * cos(angle);

      // Lewy róg podstawy
      float leftX = centerX + arrowWidth * sin(angle - M_PI * 0.75);
      float leftY = centerY - arrowWidth * cos(angle - M_PI * 0.75);

      // Prawy róg podstawy
      float rightX = centerX + arrowWidth * sin(angle + M_PI * 0.75);
      float rightY = centerY - arrowWidth * cos(angle + M_PI * 0.75);

      // Środek (dla lepszego wyglądu - strzałka z wycięciem)
      float backX = centerX - (arrowLength * 0.3f) * sin(angle);
      float backY = centerY + (arrowLength * 0.3f) * cos(angle);

      view->SetFontColor(color);

      // Rysuj strzałkę jako serię kropek (linia od czubka do lewego rogu)
      for (float t = 0; t <= 1.0f; t += 0.05f) {
          int px = (int)(tipX + t * (leftX - tipX));
          int py = (int)(tipY + t * (leftY - tipY));
          if (px >= 0 && px <= 8192 && py >= 0 && py <= 8192)
              view->Print(px, py, ".");
      }

      // Linia od czubka do prawego rogu
      for (float t = 0; t <= 1.0f; t += 0.05f) {
          int px = (int)(tipX + t * (rightX - tipX));
          int py = (int)(tipY + t * (rightY - tipY));
          if (px >= 0 && px <= 8192 && py >= 0 && py <= 8192)
              view->Print(px, py, ".");
      }

      // Linia od lewego rogu do środka tyłu
      for (float t = 0; t <= 1.0f; t += 0.05f) {
          int px = (int)(leftX + t * (backX - leftX));
          int py = (int)(leftY + t * (backY - leftY));
          if (px >= 0 && px <= 8192 && py >= 0 && py <= 8192)
              view->Print(px, py, ".");
      }

      // Linia od prawego rogu do środka tyłu
      for (float t = 0; t <= 1.0f; t += 0.05f) {
          int px = (int)(rightX + t * (backX - rightX));
          int py = (int)(rightY + t * (backY - rightY));
          if (px >= 0 && px <= 8192 && py >= 0 && py <= 8192)
              view->Print(px, py, ".");
      }
  }

  // Rysuje okrągłą ramkę minimapy (gruba ramka z wielu okręgów)
  void DrawCircularBorder() {
      if (!screen || !opt_CircularMap) return;

      // Środek minimapy w koordynatach ekranu
      int centerX = MAP_X + MAP_SIZE / 2;
      int centerY = MAP_Y + MAP_SIZE / 2;
      int baseRadius = MAP_SIZE / 2;

      screen->SetFont("FONT_OLD_10_WHITE.TGA");

      // Rysuj zewnętrzną ramkę - grubą (kilka okręgów obok siebie)
      const int numPoints = 90;  // Więcej punktów = gładszy okrąg

      // Zewnętrzna ramka - biała, gruba (3 warstwy)
      for (int layer = 0; layer < 3; layer++) {
          int radius = baseRadius - 5 - layer * 8;

          // Kolor: zewnętrzna warstwa jaśniejsza
          if (layer == 0) {
              screen->SetFontColor(zCOLOR(200, 200, 200));  // Jasny szary
          } else if (layer == 1) {
              screen->SetFontColor(zCOLOR(150, 150, 150));  // Średni szary
          } else {
              screen->SetFontColor(zCOLOR(100, 100, 100));  // Ciemny szary
          }

          for (int i = 0; i < numPoints; i++) {
              float angle = (float)i / numPoints * 2.0f * (float)M_PI;
              int px = centerX + (int)(radius * cos(angle));
              int py = centerY + (int)(radius * sin(angle));
              screen->Print(px - 10, py - 20, ".");
          }
      }

      // Wewnętrzna linia - cienka, ciemniejsza
      screen->SetFontColor(zCOLOR(60, 60, 60));
      int innerRadius = baseRadius - 35;
      for (int i = 0; i < numPoints; i++) {
          float angle = (float)i / numPoints * 2.0f * (float)M_PI;
          int px = centerX + (int)(innerRadius * cos(angle));
          int py = centerY + (int)(innerRadius * sin(angle));
          screen->Print(px - 10, py - 20, ".");
      }
  }

  void DrawMinimap() {
      static int lastCircularMode = -1;   // Śledzenie zmiany trybu kształtu
      static int lastTextureMode = -1;    // Śledzenie zmiany trybu tekstury

      // Sprawdź czy zmienił się tryb (kwadratowy/okrągły lub tekstura)
      if ((lastCircularMode != opt_CircularMap || lastTextureMode != opt_ShowMapTexture) && pMinimap) {
          // Usuń wszystkie widoki i utwórz je od nowa
          screen->RemoveItem(pMinimap); delete pMinimap; pMinimap = nullptr;
          if (pBorderTop) { screen->RemoveItem(pBorderTop); delete pBorderTop; pBorderTop = nullptr; }
          if (pBorderBottom) { screen->RemoveItem(pBorderBottom); delete pBorderBottom; pBorderBottom = nullptr; }
          if (pBorderLeft) { screen->RemoveItem(pBorderLeft); delete pBorderLeft; pBorderLeft = nullptr; }
          if (pBorderRight) { screen->RemoveItem(pBorderRight); delete pBorderRight; pBorderRight = nullptr; }
          if (pPlayerArrow) { screen->RemoveItem(pPlayerArrow); delete pPlayerArrow; pPlayerArrow = nullptr; }
      }
      lastCircularMode = opt_CircularMap;
      lastTextureMode = opt_ShowMapTexture;

      // Setup Views with textures to ensure visibility
      if (!pMinimap) {
          pMinimap = new zCView(MAP_X, MAP_Y, MAP_X + MAP_SIZE, MAP_Y + MAP_SIZE);

          // Tło zależy od trybu mapy
          if (opt_CircularMap) {
              // Okrągła mapa - całkowicie przezroczyste tło
              pMinimap->InsertBack("DEFAULT.TGA");
              pMinimap->SetAlphaBlendFunc(zRND_ALPHA_FUNC_BLEND);
              pMinimap->SetTransparency(0);  // W pełni przezroczyste
          } else {
              // Kwadratowa - tekstura TIERUMENU.TGA
              pMinimap->InsertBack("TIERUMENU.TGA");
              pMinimap->SetAlphaBlendFunc(zRND_ALPHA_FUNC_BLEND);
              pMinimap->SetTransparency(180);
          }
          screen->InsertItem(pMinimap);

          // Ramki dekoracyjne - tylko dla trybu kwadratowego
          if (!opt_CircularMap) {
              int bThick = 25;

              pBorderTop = new zCView(MAP_X, MAP_Y, MAP_X + MAP_SIZE, MAP_Y + bThick);
              pBorderTop->InsertBack("Default.tga"); pBorderTop->SetColor(zCOLOR(255, 255, 255, 255));
              screen->InsertItem(pBorderTop);

              pBorderBottom = new zCView(MAP_X, MAP_Y + MAP_SIZE - bThick, MAP_X + MAP_SIZE, MAP_Y + MAP_SIZE);
              pBorderBottom->InsertBack("Default.tga"); pBorderBottom->SetColor(zCOLOR(255, 255, 255, 255));
              screen->InsertItem(pBorderBottom);

              pBorderLeft = new zCView(MAP_X, MAP_Y, MAP_X + bThick, MAP_Y + MAP_SIZE);
              pBorderLeft->InsertBack("Default.tga"); pBorderLeft->SetColor(zCOLOR(255, 255, 255, 255));
              screen->InsertItem(pBorderLeft);

              pBorderRight = new zCView(MAP_X + MAP_SIZE - bThick, MAP_Y, MAP_X + MAP_SIZE, MAP_Y + MAP_SIZE);
              pBorderRight->InsertBack("Default.tga"); pBorderRight->SetColor(zCOLOR(255, 255, 255, 255));
              screen->InsertItem(pBorderRight);
          }
      }

      // Cleanup if disabled
      if (!opt_ShowMinimap) {
          if (pMinimap) { screen->RemoveItem(pMinimap); delete pMinimap; pMinimap = nullptr; }
          if (pBorderTop) { screen->RemoveItem(pBorderTop); delete pBorderTop; pBorderTop = nullptr; }
          if (pBorderBottom) { screen->RemoveItem(pBorderBottom); delete pBorderBottom; pBorderBottom = nullptr; }
          if (pBorderLeft) { screen->RemoveItem(pBorderLeft); delete pBorderLeft; pBorderLeft = nullptr; }
          if (pBorderRight) { screen->RemoveItem(pBorderRight); delete pBorderRight; pBorderRight = nullptr; }
          if (pPlayerArrow) { screen->RemoveItem(pPlayerArrow); delete pPlayerArrow; pPlayerArrow = nullptr; }
          return;
      }

      pMinimap->ClrPrintwin();

      if (!player) return;

      float scale = 8192.0f / (currentMapRange * 2.0f);
      int centerX = 4096;
      int centerY = 4096;

      // Pobierz kąt obrotu gracza z macierzy transformacji (azymut postaci, nie kamery)
      zMAT4& trafo = player->trafoObjToWorld;
      float playerAngle = atan2(trafo[0][2], trafo[2][2]);

      // Kąt rotacji mapy (jeśli włączone obracanie)
      float mapRotation = opt_RotateMap ? -playerAngle : 0.0f;

      // Kąt strzałki gracza (obliczamy teraz, rysujemy na końcu)
      float arrowAngle = opt_RotateMap ? 0.0f : playerAngle;

      if (ogame->GetGameWorld()) {
          // --- NPCs ---
          zCListSort<oCNpc>* list = ogame->GetGameWorld()->voblist_npcs;
          while (list) {
              oCNpc* npc = list->GetData();
              list = list->next;

              if (!npc || npc == player) continue;
              if (npc->attribute[0] <= 0) continue;

              float dist = npc->GetDistanceToVob(*player);
              if (dist > currentMapRange) continue;

              zVEC3 npcPos = npc->GetPositionWorld();
              zVEC3 plrPos = player->GetPositionWorld();
              float relX = npcPos[0] - plrPos[0];
              float relZ = npcPos[2] - plrPos[2];

              // Rotacja mapy jeśli włączona
              if (opt_RotateMap) {
                  RotatePoint(relX, relZ, mapRotation);
              }

              int mapX = centerX + (int)(relX * scale);
              int mapY = centerY - (int)(relZ * scale);

              zCOLOR dotColor = zCOLOR(255, 255, 255); // Default White
              bool isEnemy = (npc->GetAttitude(player) == NPC_ATT_HOSTILE || npc->GetAttitude(player) == NPC_ATT_ANGRY);

              // Jeśli to wróg a opcja ShowEnemies jest wyłączona - pomiń
              if (isEnemy && !opt_ShowEnemies) continue;

              // Wrogowie zawsze czerwoni (nawet przy AllNpcWhite)
              if (isEnemy) {
                  dotColor = zCOLOR(255, 0, 0); // Red - wrogowie
              }
              else if (!opt_AllNpcWhite) {
                  zSTRING qDesc = HasQuestDialogue(npc);
                  if (!qDesc.IsEmpty()) {
                      dotColor = zCOLOR(255, 128, 0); // Orange - quest NPC
                  }
              }

              DrawDot(pMinimap, mapX, mapY, dotColor);

              // Wyświetl nazwę NPC jeśli opcja włączona i NPC jest w zasięgu nazw
              if (opt_ShowNpcNames && dist <= (float)opt_NpcNameRange) {
                  zSTRING npcName = npc->GetName(0);
                  if (!npcName.IsEmpty()) {
                      pMinimap->SetFont("FONT_Default.tga");
                      pMinimap->SetFontColor(dotColor);
                      int nameY = mapY - 80;
                      if (nameY < 0) nameY = mapY + 40;
                      pMinimap->Print(mapX - 30, nameY, npcName);
                  }
              }
          }

          // --- CHESTS ---
          if (opt_ShowChests) {
              zCListSort<zCVob>* vobList = ogame->GetGameWorld()->voblist;
              while (vobList) {
                  zCVob* vob = vobList->GetData();
                  vobList = vobList->next;
                  
                  if (!vob) continue;
                  if (vob->GetVobType() != zVOB_TYPE_MOB) continue; 

                  oCMobContainer* mob = zDYNAMIC_CAST<oCMobContainer>(vob);
                  if (mob) {
                      float dist = vob->GetDistanceToVob(*player);
                      if (dist > currentMapRange) continue;

                      zCOLOR chestColor = zCOLOR(0, 0, 0, 0); 

                      if (mob->locked) {
                          if (!mob->keyInstance.IsEmpty()) {
                              if (opt_HideLockedChests) { chestColor.alpha = 0; } 
                              else { chestColor = zCOLOR(0, 255, 255); } // Cyan (Key)
                          }
                          else if (!mob->pickLockStr.IsEmpty()) {
                              chestColor = zCOLOR(255, 255, 0); // Yellow (Picklock)
                          }
                          else {
                              chestColor = zCOLOR(255, 0, 0); // Red (Locked unknown)
                          }
                      } else if (mob->containList.GetNumInList() > 0) {
                          chestColor = zCOLOR(0, 255, 0); // Green (Open)
                      }

                      if (chestColor.alpha != 0) {
                          zVEC3 plrPos = player->GetPositionWorld();
                          zVEC3 vobPos = vob->GetPositionWorld();
                          float relX = vobPos[0] - plrPos[0];
                          float relZ = vobPos[2] - plrPos[2];

                          // Rotacja mapy jeśli włączona
                          if (opt_RotateMap) {
                              RotatePoint(relX, relZ, mapRotation);
                          }

                          int mapX = centerX + (int)(relX * scale);
                          int mapY = centerY - (int)(relZ * scale);

                          DrawDot(pMinimap, mapX, mapY, chestColor);
                      }
                  }
              }
          }

          // --- ITEMS & PLANTS ---
          if (opt_ShowItems || opt_ShowPlants) {
              zCListSort<zCVob>* vobList = ogame->GetGameWorld()->voblist;
              while (vobList) {
                  zCVob* vob = vobList->GetData();
                  vobList = vobList->next;

                  if (!vob) continue;
                  if (vob->GetVobType() != zVOB_TYPE_ITEM) continue;

                  oCItem* item = zDYNAMIC_CAST<oCItem>(vob);
                  if (!item) continue;

                  float dist = vob->GetDistanceToVob(*player);
                  if (dist > MAP_RANGE) continue;

                  bool isPlant = IsPlantItem(item);

                  // Jeśli to roślina - sprawdź czy ShowPlants jest włączone
                  // Jeśli to inny przedmiot - sprawdź czy ShowItems jest włączone
                  if (isPlant && !opt_ShowPlants) continue;
                  if (!isPlant && !opt_ShowItems) continue;

                  zVEC3 plrPos = player->GetPositionWorld();
                  zVEC3 vobPos = vob->GetPositionWorld();
                  float relX = vobPos[0] - plrPos[0];
                  float relZ = vobPos[2] - plrPos[2];

                  // Rotacja mapy jeśli włączona
                  if (opt_RotateMap) {
                      RotatePoint(relX, relZ, mapRotation);
                  }

                  int mapX = centerX + (int)(relX * scale);
                  int mapY = centerY - (int)(relZ * scale);

                  zCOLOR itemColor;
                  if (isPlant) {
                      itemColor = zCOLOR(0, 200, 50); // Zielony dla roślin
                  } else {
                      itemColor = zCOLOR(100, 150, 255); // Niebieski dla przedmiotów
                  }

                  DrawDot(pMinimap, mapX, mapY, itemColor);

                  // Wyświetl nazwę - osobno dla roślin i przedmiotów
                  bool showName = (isPlant && opt_ShowPlantNames) || (!isPlant && opt_ShowItemNames);
                  if (showName) {
                      zSTRING itemName = item->GetName(0);
                      if (!itemName.IsEmpty()) {
                          pMinimap->SetFont("FONT_Default.tga");
                          pMinimap->SetFontColor(itemColor);
                          int nameY = mapY - 80;
                          if (nameY < 0) nameY = mapY + 40;
                          pMinimap->Print(mapX - 30, nameY, itemName);
                      }
                  }
              }
          }

          // Rysuj linię nawigacyjną do celu (przed strzałką gracza)
          DrawNavLine(pMinimap, centerX, centerY, scale, mapRotation);

          // Rysuj kompas na obramowaniu
          DrawCompass(mapRotation);

          // Rysuj okrągłą ramkę jeśli włączony tryb okrągły
          DrawCircularBorder();
      }

      // Strzałka gracza z tekstury O.TGA - na wierzchu wszystkich elementów
      // Rozmiar strzałki w koordynatach ekranu (proporcjonalnie do minimapy)
      const int ARROW_SIZE = 50;  // Rozmiar strzałki (zmniejszona 6x)
      const int ARROW_OFFSET_Y = 75;  // Przesunięcie w dół dla lepszej koordynacji
      int arrowX = MAP_X + MAP_SIZE / 2 - ARROW_SIZE / 2;
      int arrowY = MAP_Y + MAP_SIZE / 2 - ARROW_SIZE / 2 + ARROW_OFFSET_Y;

      if (!pPlayerArrow) {
          pPlayerArrow = new zCView(arrowX, arrowY, arrowX + ARROW_SIZE, arrowY + ARROW_SIZE);
          pPlayerArrow->InsertBack("O.TGA");
          pPlayerArrow->SetAlphaBlendFunc(zRND_ALPHA_FUNC_BLEND);
          pPlayerArrow->SetTransparency(255);
          screen->InsertItem(pPlayerArrow);
      }
      // Strzałka ma stałą pozycję - nie aktualizujemy jej co klatkę
  }

  // ========================================================================
  // MAIN
  // ========================================================================

  void Game_Loop() {
      if (!ogame || !player || !screen) return;

      LoadOptions();

      // Zawsze rysuj minimapę (niezależnie od key binding)
      HandleZoomKeys();      // Obsługa klawiszy zoom
      HandleLegendKey();     // Obsługa klawisza legendy
      DrawMinimap();         // Rysuje minimapę (radar)
      DrawLegend();          // Rysowanie legendy

      // Key binding overlay na wierzchu (jeśli aktywny)
      CheckKeyBindTrigger();
      if (keyBindMode != 0) {
          HandleKeyBindMode();
          DrawKeyBindOverlay();
      } else {
          // Nawigacja NPC tylko gdy nie bindujemy
          HandleNavKeys();
          DrawNavMenu();
      }
  }

  TSaveLoadGameInfo& SaveLoadGameInfo = UnionCore::SaveLoadGameInfo;

  void Game_Entry() {}

  void Game_Init() {
      // Wyczyść triggery key binding przy starcie gry (na wszelki wypadek)
      if (zoptions) {
          zSTRING iniPath = GetGameIniPath();
          // Resetuj w pliku INI
          WritePrivateProfileStringA("QUESTHELPER_V2", "KeyBindZoomIn", "0", iniPath.ToChar());
          WritePrivateProfileStringA("QUESTHELPER_V2", "KeyBindZoomOut", "0", iniPath.ToChar());
          WritePrivateProfileStringA("QUESTHELPER_V2", "KeyBindLegend", "0", iniPath.ToChar());
          WritePrivateProfileStringA("QUESTHELPER_V2", "KeyBindNpcSearch", "0", iniPath.ToChar());
          // Resetuj też w zoptions (cache) - CheckKeyBindTrigger czyta stąd!
          zoptions->WriteInt("QUESTHELPER_V2", "KeyBindZoomIn", 0, TRUE);
          zoptions->WriteInt("QUESTHELPER_V2", "KeyBindZoomOut", 0, TRUE);
          zoptions->WriteInt("QUESTHELPER_V2", "KeyBindLegend", 0, TRUE);
          zoptions->WriteInt("QUESTHELPER_V2", "KeyBindNpcSearch", 0, TRUE);
      }
      LoadOptions();
  }
  void Game_Exit() {}
  void Game_PreLoop() {}
  void Game_PostLoop() {}
  void Game_MenuLoop() {
      if (!screen) return;

      CheckKeyBindTrigger(); // Sprawdź czy menu aktywowało key binding

      if (keyBindMode != 0) {
          HandleKeyBindMode();
          DrawKeyBindOverlay();
      }
  }
  void Game_SaveBegin() {}
  void Game_SaveEnd() {}
  void LoadBegin() {}
  void LoadEnd() {}
  void Game_LoadBegin_NewGame() { LoadBegin(); }
  void Game_LoadEnd_NewGame() { LoadEnd(); }
  void Game_LoadBegin_SaveGame() { LoadBegin(); }
  void Game_LoadEnd_SaveGame() { LoadEnd(); }
  void Game_LoadBegin_ChangeLevel() { LoadBegin(); }
  void Game_LoadEnd_ChangeLevel() { LoadEnd(); }
  void Game_LoadBegin_Trigger() {}
  void Game_LoadEnd_Trigger() {}
  void Game_Pause() {}
  void Game_Unpause() {}
  void Game_DefineExternals() {}
  void Game_ApplyOptions() { LoadOptions(); }

#define AppDefault True
  CApplication* lpApplication = !CHECK_THIS_ENGINE ? Null : CApplication::CreateRefApplication(
    Enabled( AppDefault ) Game_Entry,
    Enabled( AppDefault ) Game_Init,
    Enabled( AppDefault ) Game_Exit,
    Enabled( AppDefault ) Game_PreLoop,
    Enabled( AppDefault ) Game_Loop,
    Enabled( AppDefault ) Game_PostLoop,
    Enabled( AppDefault ) Game_MenuLoop,
    Enabled( AppDefault ) Game_SaveBegin,
    Enabled( AppDefault ) Game_SaveEnd,
    Enabled( AppDefault ) Game_LoadBegin_NewGame,
    Enabled( AppDefault ) Game_LoadEnd_NewGame,
    Enabled( AppDefault ) Game_LoadBegin_SaveGame,
    Enabled( AppDefault ) Game_LoadEnd_SaveGame,
    Enabled( AppDefault ) Game_LoadBegin_ChangeLevel,
    Enabled( AppDefault ) Game_LoadEnd_ChangeLevel,
    Enabled( AppDefault ) Game_LoadBegin_Trigger,
    Enabled( AppDefault ) Game_LoadEnd_Trigger,
    Enabled( AppDefault ) Game_Pause,
    Enabled( AppDefault ) Game_Unpause,
    Enabled( AppDefault ) Game_DefineExternals,
    Enabled( AppDefault ) Game_ApplyOptions
  );
}
