// This file added in headers queue
// File: "Sources.h"
#include "resource.h"
#include <string>
#include <vector>
#include <algorithm>

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

      DrawMinimap();
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
