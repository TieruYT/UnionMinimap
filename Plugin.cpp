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

  const int MAP_SIZE = 1500;   
  const int MAP_X = 6500;      
  const int MAP_Y = 200;       
  const float MAP_RANGE = 3000.0f; 

  int opt_ShowMinimap = 0; // Default OFF to test if menu enables it
  int opt_ShowChests = 1;
  int opt_HideLockedChests = 0; 
  int opt_AllNpcWhite = 0;      

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
  // MINIMAP LOGIC
  // ========================================================================

  void DrawDot(zCView* view, int x, int y, zCOLOR color) {
      if (x < 0 || x > 8192 || y < 0 || y > 8192) return;
      view->SetFontColor(color);
      view->Print(x, y, "."); 
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
             // Hide instead of destroy to prevent flickering recreation
             // Or better, just don't draw dots. 
             // Ideally remove views.
             screen->RemoveItem(pMinimap); delete pMinimap; pMinimap = nullptr;
             screen->RemoveItem(pBorderTop); delete pBorderTop; pBorderTop = nullptr;
             screen->RemoveItem(pBorderBottom); delete pBorderBottom; pBorderBottom = nullptr;
             screen->RemoveItem(pBorderLeft); delete pBorderLeft; pBorderLeft = nullptr;
             screen->RemoveItem(pBorderRight); delete pBorderRight; pBorderRight = nullptr;
          }
          return;
      }

      pMinimap->ClrPrintwin(); 

      if (!player) return;

      float scale = 8192.0f / (MAP_RANGE * 2.0f);
      int centerX = 4096;
      int centerY = 4096;

      // Draw Player (Red Dot for visibility)
      DrawDot(pMinimap, centerX, centerY, zCOLOR(255, 0, 0)); 

      if (ogame->GetGameWorld()) {
          // --- NPCs ---
          zCListSort<oCNpc>* list = ogame->GetGameWorld()->voblist_npcs;
          while (list) {
              oCNpc* npc = list->GetData();
              list = list->next;
              
              if (!npc || npc == player) continue;
              if (npc->attribute[0] <= 0) continue; 

              float dist = npc->GetDistanceToVob(*player);
              if (dist > MAP_RANGE) continue;

              zVEC3 npcPos = npc->GetPositionWorld();
              zVEC3 plrPos = player->GetPositionWorld(); 
              float relX = npcPos[0] - plrPos[0];
              float relZ = npcPos[2] - plrPos[2];

              int mapX = centerX + (int)(relX * scale);
              int mapY = centerY - (int)(relZ * scale); 

              zCOLOR dotColor = zCOLOR(255, 255, 255); // Default White

              if (opt_AllNpcWhite) {
                  dotColor = zCOLOR(255, 255, 255);
              } else {
                  if (npc->GetAttitude(player) == NPC_ATT_HOSTILE || npc->GetAttitude(player) == NPC_ATT_ANGRY) {
                       dotColor = zCOLOR(255, 0, 0); // Red
                  }
                  else {
                      zSTRING qDesc = HasQuestDialogue(npc);
                      if (!qDesc.IsEmpty()) {
                          dotColor = zCOLOR(255, 128, 0); // Orange
                      }
                  }
              }

              DrawDot(pMinimap, mapX, mapY, dotColor);
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
                      if (dist > MAP_RANGE) continue;

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

                          int mapX = centerX + (int)(relX * scale);
                          int mapY = centerY - (int)(relZ * scale);

                          DrawDot(pMinimap, mapX, mapY, chestColor);
                      }
                  }
              }
          }
      }
  }

  // ========================================================================
  // MAIN
  // ========================================================================

  void Game_Loop() {
      if (!ogame || !player || !screen) return;
      
      LoadOptions();
      
      // DEBUG: Verify options are loaded
      // zSTRING dbg = "QuestHelper: " + zSTRING(opt_ShowMinimap);
      // screen->Print(100, 100, dbg);

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
