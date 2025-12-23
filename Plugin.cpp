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

  // Zoom mapy - wartości: 0=1500, 1=2000, 2=2500, 3=3000, 4=3500, 5=4000
  int opt_MapZoom = 3;        // Domyślnie 3000 (indeks 3)
  float currentMapRange = 3000.0f;

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
      opt_MapZoom = zoptions->ReadInt("QUESTHELPER_V2", "MapZoom", 3);

      // NpcNameRange - menu zapisuje indeks (0-4), konwertuj na wartość
      int rangeIndex = zoptions->ReadInt("QUESTHELPER_V2", "NpcNameRange", 0); // domyślnie 0 = 1000
      const int rangeValues[] = { 1000, 1500, 2000, 2500, 3500 };
      if (rangeIndex >= 0 && rangeIndex <= 4) {
          opt_NpcNameRange = rangeValues[rangeIndex];
      } else {
          opt_NpcNameRange = 1000; // fallback
      }

      // MapZoom - konwertuj indeks na wartość zasięgu
      const float zoomValues[] = { 1500.0f, 2000.0f, 2500.0f, 3000.0f, 3500.0f, 4000.0f };
      if (opt_MapZoom >= 0 && opt_MapZoom <= 5) {
          currentMapRange = zoomValues[opt_MapZoom];
      } else {
          currentMapRange = 3000.0f;
      }
  }

  // Obsługa klawiszy zoom (wywoływana w Game_Loop)
  void HandleZoomKeys() {
      if (!zinput || !zoptions) return;

      static bool keyPressed = false;

      // Klawisz ; (średnik) - oddal (zwiększ zasięg)
      // Klawisz ' (apostrof) - przybliż (zmniejsz zasięg)
      bool zoomOut = zinput->KeyPressed(KEY_SEMICOLON) || zinput->KeyPressed(KEY_COMMA);
      bool zoomIn = zinput->KeyPressed(KEY_APOSTROPHE) || zinput->KeyPressed(KEY_PERIOD);

      if ((zoomIn || zoomOut) && !keyPressed) {
          keyPressed = true;

          if (zoomIn && opt_MapZoom > 0) {
              opt_MapZoom--;
              zoptions->WriteInt("QUESTHELPER_V2", "MapZoom", opt_MapZoom, TRUE);
          }
          else if (zoomOut && opt_MapZoom < 5) {
              opt_MapZoom++;
              zoptions->WriteInt("QUESTHELPER_V2", "MapZoom", opt_MapZoom, TRUE);
          }

          const float zoomValues[] = { 1500.0f, 2000.0f, 2500.0f, 3000.0f, 3500.0f, 4000.0f };
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

      DWORD currentTime = GetTickCount();

      // Klawisz ] - otwórz/zamknij menu nawigacji (VK_OEM_6 = ])
      if (GetAsyncKeyState(VK_OEM_6) & 0x8000) {
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

  void DrawMinimap() {
      // Setup Views with textures to ensure visibility
      if (!pMinimap) {
          pMinimap = new zCView(MAP_X, MAP_Y, MAP_X + MAP_SIZE, MAP_Y + MAP_SIZE);
          pMinimap->InsertBack("DIA_HIGHL.TGA"); // Standard dialog texture (semi-transparent)
          pMinimap->SetAlphaBlendFunc(zRND_ALPHA_FUNC_BLEND);
          pMinimap->SetTransparency(150); // Ensure alpha
          screen->InsertItem(pMinimap);

          int bThick = 25; // Slightly thicker
          
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

      // Cleanup if disabled
      if (!opt_ShowMinimap) {
          if (pMinimap) {
             screen->RemoveItem(pMinimap); delete pMinimap; pMinimap = nullptr;
             screen->RemoveItem(pBorderTop); delete pBorderTop; pBorderTop = nullptr;
             screen->RemoveItem(pBorderBottom); delete pBorderBottom; pBorderBottom = nullptr;
             screen->RemoveItem(pBorderLeft); delete pBorderLeft; pBorderLeft = nullptr;
             screen->RemoveItem(pBorderRight); delete pBorderRight; pBorderRight = nullptr;
          }
          if (pPlayerArrow) {
             screen->RemoveItem(pPlayerArrow); delete pPlayerArrow; pPlayerArrow = nullptr;
          }
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
      HandleZoomKeys();  // Obsługa klawiszy zoom: ; (oddal) i ' (przybliż)
      HandleNavKeys();   // Obsługa klawiszy nawigacji: ] (otwórz menu)

      DrawMinimap();
      DrawNavMenu();     // Rysowanie menu wyboru NPC (jeśli otwarte)
  }

  TSaveLoadGameInfo& SaveLoadGameInfo = UnionCore::SaveLoadGameInfo;

  void Game_Entry() {}
  void Game_Init() { LoadOptions(); }
  void Game_Exit() {}
  void Game_PreLoop() {}
  void Game_PostLoop() {}
  void Game_MenuLoop() {}
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
