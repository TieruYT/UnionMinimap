META
{
  Parser = Menu;
  After = zUnionMenu.d;
  Namespace = QuestHelper;
};

// ------ Constants ------
const int Start_PY  = 1400;
const int Title_PY  = 450;
const int Menu_DY   = 550;
const int Text_PX   = 400;
const int Text_SX   = 8000;
const int Text_SY   = 750;
const int Text_DY   = 120;
const int Choice_PX = 6400;
const int Choice_SX = 1500;
const int Choice_SY = 350;
const int Choice_DY = 120;

// ================================================================================================
// LEVEL 1: MAIN ENTRY "TIERU PLUGINS" IN UNION MENU
// ================================================================================================

instance MenuItem_Union_Auto_Tieru(C_MENU_ITEM_UNION_DEF)
{
  text[0]          = "Tieru Plugins";
  text[1]          = "Tieru mod collection";
  onselaction[0]   = SEL_ACTION_STARTMENU;
  onselaction_s[0] = "QuestHelper:Menu_Tieru_Main";
};

// ================================================================================================
// LEVEL 2: MOD LIST (e.g. MINIMAP)
// ================================================================================================

instance Menu_Tieru_Main(C_MENU_DEF)
{
  backpic    = "";
  items[0]   = "";
  items[100] = "Union_menuitem_back";
  flags      = flags | MENU_SHOW_INFO;
  Menu_SearchItems("QuestHelper:MENUITEM_TIERU_LIST_*");
};

// Header
instance MenuItem_Tieru_List_Headline(C_MENU_ITEM_DEF)
{
  type    = MENU_ITEM_TEXT;
  posx    = 0;
  posy    = Title_PY;
  dimx    = 8100;
  flags   = flags & ~IT_SELECTABLE;
  flags   = flags | IT_TXT_CENTER;
  text[0] = "TIERU PLUGINS";
};

// Entry "Minimap"
instance MenuItem_Tieru_List_Minimap(C_MENU_ITEM_DEF)
{
  backpic        = "";
  posx           = Text_PX;
  posy           = Start_PY;
  dimx           = Text_SX;
  dimy           = Text_SY;
  flags          = flags | IT_EFFECTS_NEXT;
  onselaction[0] = SEL_ACTION_STARTMENU;
  onselaction_s[0] = "QuestHelper:Menu_Minimap_Settings";
  text[0]        = "Minimap";
  text[1]        = "Minimap configuration";
};

// ================================================================================================
// LEVEL 3: MINIMAP CONFIGURATION
// ================================================================================================

instance Menu_Minimap_Settings(C_MENU_DEF)
{
  backpic    = "";
  items[0]   = "";
  items[100] = "Union_menuitem_back";
  flags      = flags | MENU_SHOW_INFO;
  Menu_SearchItems("QuestHelper:MENUITEM_MM_OPT_*");
};

// --- Headline ---
instance MenuItem_MM_OPT_Headline(C_MENU_ITEM_DEF)
{
  type    = MENU_ITEM_TEXT;
  posx    = 0;
  posy    = Title_PY;
  dimx    = 8100;
  flags   = flags & ~IT_SELECTABLE;
  flags   = flags | IT_TXT_CENTER;
  text[0] = "MINIMAP SETTINGS";
};

// --- Show Minimap ---
instance MenuItem_MM_OPT_ShowMinimap(C_MENU_ITEM_DEF)
{
  backpic        = "";
  posx           = Text_PX;
  posy           = Start_PY;
  dimx           = Text_SX;
  dimy           = Text_SY;
  flags          = flags | IT_EFFECTS_NEXT;
  onselaction[0] = SEL_ACTION_UNDEF;
  text[0]        = "Show Minimap";
  text[1]        = "Enable or disable minimap";
};

instance MenuItem_MM_OPT_ShowMinimap_Choice(C_MENU_ITEM_DEF)
{
  backpic  = "MENU_CHOICE_BACK.TGA";
  type     = MENU_ITEM_CHOICEBOX;
  fontname = "font_old_10_white.tga";
  posx     = Choice_PX;
  posy     = Start_PY + Choice_DY;
  dimx     = Choice_SX;
  dimy     = Choice_SY;
  flags    = flags & ~IT_SELECTABLE;
  flags    = flags | IT_TXT_CENTER;

  onchgsetoption        = "ShowMinimap";
  onchgsetoptionsection = "QUESTHELPER_V2";
  text[0]               = "Off|On";
};

// --- Show Chests ---
instance MenuItem_MM_OPT_ShowChests(C_MENU_ITEM_DEF)
{
  backpic        = "";
  posx           = Text_PX;
  posy           = Start_PY + Menu_DY;
  dimx           = Text_SX;
  dimy           = Text_SY;
  flags          = flags | IT_EFFECTS_NEXT;
  onselaction[0] = SEL_ACTION_UNDEF;
  text[0]        = "Show Chests";
  text[1]        = "Display chest markers on map";
};

instance MenuItem_MM_OPT_ShowChests_Choice(C_MENU_ITEM_DEF)
{
  backpic  = "MENU_CHOICE_BACK.TGA";
  type     = MENU_ITEM_CHOICEBOX;
  fontname = "font_old_10_white.tga";
  posx     = Choice_PX;
  posy     = Start_PY + Menu_DY + Choice_DY;
  dimx     = Choice_SX;
  dimy     = Choice_SY;
  flags    = flags & ~IT_SELECTABLE;
  flags    = flags | IT_TXT_CENTER;

  onchgsetoption        = "ShowChests";
  onchgsetoptionsection = "QUESTHELPER_V2";
  text[0]               = "Off|On";
};

// --- Hide Locked Chests ---
instance MenuItem_MM_OPT_HideLocked(C_MENU_ITEM_DEF)
{
  backpic        = "";
  posx           = Text_PX;
  posy           = Start_PY + (Menu_DY * 2);
  dimx           = Text_SX;
  dimy           = Text_SY;
  flags          = flags | IT_EFFECTS_NEXT;
  onselaction[0] = SEL_ACTION_UNDEF;
  text[0]        = "Hide Key-locked";
  text[1]        = "Hide chests that require a key";
};

instance MenuItem_MM_OPT_HideLocked_Choice(C_MENU_ITEM_DEF)
{
  backpic  = "MENU_CHOICE_BACK.TGA";
  type     = MENU_ITEM_CHOICEBOX;
  fontname = "font_old_10_white.tga";
  posx     = Choice_PX;
  posy     = Start_PY + (Menu_DY * 2) + Choice_DY;
  dimx     = Choice_SX;
  dimy     = Choice_SY;
  flags    = flags & ~IT_SELECTABLE;
  flags    = flags | IT_TXT_CENTER;

  onchgsetoption        = "HideLockedChests";
  onchgsetoptionsection = "QUESTHELPER_V2";
  text[0]               = "Off|On";
};

// --- All NPC White ---
instance MenuItem_MM_OPT_AllWhite(C_MENU_ITEM_DEF)
{
  backpic        = "";
  posx           = Text_PX;
  posy           = Start_PY + (Menu_DY * 3);
  dimx           = Text_SX;
  dimy           = Text_SY;
  flags          = flags | IT_EFFECTS_NEXT;
  onselaction[0] = SEL_ACTION_UNDEF;
  text[0]        = "All NPCs White";
  text[1]        = "Disable NPC colors (enemies stay red)";
};

instance MenuItem_MM_OPT_AllWhite_Choice(C_MENU_ITEM_DEF)
{
  backpic  = "MENU_CHOICE_BACK.TGA";
  type     = MENU_ITEM_CHOICEBOX;
  fontname = "font_old_10_white.tga";
  posx     = Choice_PX;
  posy     = Start_PY + (Menu_DY * 3) + Choice_DY;
  dimx     = Choice_SX;
  dimy     = Choice_SY;
  flags    = flags & ~IT_SELECTABLE;
  flags    = flags | IT_TXT_CENTER;

  onchgsetoption        = "AllNpcWhite";
  onchgsetoptionsection = "QUESTHELPER_V2";
  text[0]               = "Off|On";
};

// --- Show Items ---
instance MenuItem_MM_OPT_ShowItems(C_MENU_ITEM_DEF)
{
  backpic        = "";
  posx           = Text_PX;
  posy           = Start_PY + (Menu_DY * 4);
  dimx           = Text_SX;
  dimy           = Text_SY;
  flags          = flags | IT_EFFECTS_NEXT;
  onselaction[0] = SEL_ACTION_UNDEF;
  text[0]        = "Show Items";
  text[1]        = "Display items on minimap (blue)";
};

instance MenuItem_MM_OPT_ShowItems_Choice(C_MENU_ITEM_DEF)
{
  backpic  = "MENU_CHOICE_BACK.TGA";
  type     = MENU_ITEM_CHOICEBOX;
  fontname = "font_old_10_white.tga";
  posx     = Choice_PX;
  posy     = Start_PY + (Menu_DY * 4) + Choice_DY;
  dimx     = Choice_SX;
  dimy     = Choice_SY;
  flags    = flags & ~IT_SELECTABLE;
  flags    = flags | IT_TXT_CENTER;

  onchgsetoption        = "ShowItems";
  onchgsetoptionsection = "QUESTHELPER_V2";
  text[0]               = "Off|On";
};

// --- Show Plants ---
instance MenuItem_MM_OPT_ShowPlants(C_MENU_ITEM_DEF)
{
  backpic        = "";
  posx           = Text_PX;
  posy           = Start_PY + (Menu_DY * 5);
  dimx           = Text_SX;
  dimy           = Text_SY;
  flags          = flags | IT_EFFECTS_NEXT;
  onselaction[0] = SEL_ACTION_UNDEF;
  text[0]        = "Show Plants";
  text[1]        = "Display plants and herbs on minimap (green)";
};

instance MenuItem_MM_OPT_ShowPlants_Choice(C_MENU_ITEM_DEF)
{
  backpic  = "MENU_CHOICE_BACK.TGA";
  type     = MENU_ITEM_CHOICEBOX;
  fontname = "font_old_10_white.tga";
  posx     = Choice_PX;
  posy     = Start_PY + (Menu_DY * 5) + Choice_DY;
  dimx     = Choice_SX;
  dimy     = Choice_SY;
  flags    = flags & ~IT_SELECTABLE;
  flags    = flags | IT_TXT_CENTER;

  onchgsetoption        = "ShowPlants";
  onchgsetoptionsection = "QUESTHELPER_V2";
  text[0]               = "Off|On";
};

// --- Show Enemies ---
instance MenuItem_MM_OPT_ShowEnemies(C_MENU_ITEM_DEF)
{
  backpic        = "";
  posx           = Text_PX;
  posy           = Start_PY + (Menu_DY * 6);
  dimx           = Text_SX;
  dimy           = Text_SY;
  flags          = flags | IT_EFFECTS_NEXT;
  onselaction[0] = SEL_ACTION_UNDEF;
  text[0]        = "Show Enemies";
  text[1]        = "Display enemies on minimap (red)";
};

instance MenuItem_MM_OPT_ShowEnemies_Choice(C_MENU_ITEM_DEF)
{
  backpic  = "MENU_CHOICE_BACK.TGA";
  type     = MENU_ITEM_CHOICEBOX;
  fontname = "font_old_10_white.tga";
  posx     = Choice_PX;
  posy     = Start_PY + (Menu_DY * 6) + Choice_DY;
  dimx     = Choice_SX;
  dimy     = Choice_SY;
  flags    = flags & ~IT_SELECTABLE;
  flags    = flags | IT_TXT_CENTER;

  onchgsetoption        = "ShowEnemies";
  onchgsetoptionsection = "QUESTHELPER_V2";
  text[0]               = "Off|On";
};

// --- Show NPC Names ---
instance MenuItem_MM_OPT_ShowNpcNames(C_MENU_ITEM_DEF)
{
  backpic        = "";
  posx           = Text_PX;
  posy           = Start_PY + (Menu_DY * 7);
  dimx           = Text_SX;
  dimy           = Text_SY;
  flags          = flags | IT_EFFECTS_NEXT;
  onselaction[0] = SEL_ACTION_UNDEF;
  text[0]        = "Show NPC Names";
  text[1]        = "Display NPC names above markers";
};

instance MenuItem_MM_OPT_ShowNpcNames_Choice(C_MENU_ITEM_DEF)
{
  backpic  = "MENU_CHOICE_BACK.TGA";
  type     = MENU_ITEM_CHOICEBOX;
  fontname = "font_old_10_white.tga";
  posx     = Choice_PX;
  posy     = Start_PY + (Menu_DY * 7) + Choice_DY;
  dimx     = Choice_SX;
  dimy     = Choice_SY;
  flags    = flags & ~IT_SELECTABLE;
  flags    = flags | IT_TXT_CENTER;

  onchgsetoption        = "ShowNpcNames";
  onchgsetoptionsection = "QUESTHELPER_V2";
  text[0]               = "Off|On";
};

// --- Show Item Names ---
instance MenuItem_MM_OPT_ShowItemNames(C_MENU_ITEM_DEF)
{
  backpic        = "";
  posx           = Text_PX;
  posy           = Start_PY + (Menu_DY * 8);
  dimx           = Text_SX;
  dimy           = Text_SY;
  flags          = flags | IT_EFFECTS_NEXT;
  onselaction[0] = SEL_ACTION_UNDEF;
  text[0]        = "Show Item Names";
  text[1]        = "Display item names (not plants)";
};

instance MenuItem_MM_OPT_ShowItemNames_Choice(C_MENU_ITEM_DEF)
{
  backpic  = "MENU_CHOICE_BACK.TGA";
  type     = MENU_ITEM_CHOICEBOX;
  fontname = "font_old_10_white.tga";
  posx     = Choice_PX;
  posy     = Start_PY + (Menu_DY * 8) + Choice_DY;
  dimx     = Choice_SX;
  dimy     = Choice_SY;
  flags    = flags & ~IT_SELECTABLE;
  flags    = flags | IT_TXT_CENTER;

  onchgsetoption        = "ShowItemNames";
  onchgsetoptionsection = "QUESTHELPER_V2";
  text[0]               = "Off|On";
};

// --- Show Plant Names ---
instance MenuItem_MM_OPT_ShowPlantNames(C_MENU_ITEM_DEF)
{
  backpic        = "";
  posx           = Text_PX;
  posy           = Start_PY + (Menu_DY * 9);
  dimx           = Text_SX;
  dimy           = Text_SY;
  flags          = flags | IT_EFFECTS_NEXT;
  onselaction[0] = SEL_ACTION_UNDEF;
  text[0]        = "Show Plant Names";
  text[1]        = "Display plant and herb names";
};

instance MenuItem_MM_OPT_ShowPlantNames_Choice(C_MENU_ITEM_DEF)
{
  backpic  = "MENU_CHOICE_BACK.TGA";
  type     = MENU_ITEM_CHOICEBOX;
  fontname = "font_old_10_white.tga";
  posx     = Choice_PX;
  posy     = Start_PY + (Menu_DY * 9) + Choice_DY;
  dimx     = Choice_SX;
  dimy     = Choice_SY;
  flags    = flags & ~IT_SELECTABLE;
  flags    = flags | IT_TXT_CENTER;

  onchgsetoption        = "ShowPlantNames";
  onchgsetoptionsection = "QUESTHELPER_V2";
  text[0]               = "Off|On";
};

// --- Rotate Map ---
instance MenuItem_MM_OPT_RotateMap(C_MENU_ITEM_DEF)
{
  backpic        = "";
  posx           = Text_PX;
  posy           = Start_PY + (Menu_DY * 10);
  dimx           = Text_SX;
  dimy           = Text_SY;
  flags          = flags | IT_EFFECTS_NEXT;
  onselaction[0] = SEL_ACTION_UNDEF;
  text[0]        = "Rotate Map";
  text[1]        = "Map rotates with player direction";
};

instance MenuItem_MM_OPT_RotateMap_Choice(C_MENU_ITEM_DEF)
{
  backpic  = "MENU_CHOICE_BACK.TGA";
  type     = MENU_ITEM_CHOICEBOX;
  fontname = "font_old_10_white.tga";
  posx     = Choice_PX;
  posy     = Start_PY + (Menu_DY * 10) + Choice_DY;
  dimx     = Choice_SX;
  dimy     = Choice_SY;
  flags    = flags & ~IT_SELECTABLE;
  flags    = flags | IT_TXT_CENTER;

  onchgsetoption        = "RotateMap";
  onchgsetoptionsection = "QUESTHELPER_V2";
  text[0]               = "Off|On";
};

// --- NPC Name Range ---
instance MenuItem_MM_OPT_NpcNameRange(C_MENU_ITEM_DEF)
{
  backpic        = "";
  posx           = Text_PX;
  posy           = Start_PY + (Menu_DY * 11);
  dimx           = Text_SX;
  dimy           = Text_SY;
  flags          = flags | IT_EFFECTS_NEXT;
  onselaction[0] = SEL_ACTION_UNDEF;
  text[0]        = "NPC Name Range";
  text[1]        = "Maximum distance to display NPC names";
};

instance MenuItem_MM_OPT_NpcNameRange_Choice(C_MENU_ITEM_DEF)
{
  backpic  = "MENU_CHOICE_BACK.TGA";
  type     = MENU_ITEM_CHOICEBOX;
  fontname = "font_old_10_white.tga";
  posx     = Choice_PX;
  posy     = Start_PY + (Menu_DY * 11) + Choice_DY;
  dimx     = Choice_SX;
  dimy     = Choice_SY;
  flags    = flags & ~IT_SELECTABLE;
  flags    = flags | IT_TXT_CENTER;

  onchgsetoption        = "NpcNameRange";
  onchgsetoptionsection = "QUESTHELPER_V2";
  text[0]               = "1000|1500|2000|2500|3500";
};

// --- Map Zoom ---
instance MenuItem_MM_OPT_MapZoom(C_MENU_ITEM_DEF)
{
  backpic        = "";
  posx           = Text_PX;
  posy           = Start_PY + (Menu_DY * 12);
  dimx           = Text_SX;
  dimy           = Text_SY;
  flags          = flags | IT_EFFECTS_NEXT;
  onselaction[0] = SEL_ACTION_UNDEF;
  text[0]        = "Map Zoom";
  text[1]        = "Map zoom level";
};

instance MenuItem_MM_OPT_MapZoom_Choice(C_MENU_ITEM_DEF)
{
  backpic  = "MENU_CHOICE_BACK.TGA";
  type     = MENU_ITEM_CHOICEBOX;
  fontname = "font_old_10_white.tga";
  posx     = Choice_PX;
  posy     = Start_PY + (Menu_DY * 12) + Choice_DY;
  dimx     = Choice_SX;
  dimy     = Choice_SY;
  flags    = flags & ~IT_SELECTABLE;
  flags    = flags | IT_TXT_CENTER;

  onchgsetoption        = "MapZoom";
  onchgsetoptionsection = "QUESTHELPER_V2";
  text[0]               = "1500|2000|2500|3000|3500|4000";
};

// ================================================================================================
// KEY BINDINGS - "Press any key to assign"
// ================================================================================================

// --- Key: Zoom In ---
instance MenuItem_MM_OPT_KeyZoomIn(C_MENU_ITEM_DEF)
{
  backpic        = "";
  posx           = Text_PX;
  posy           = Start_PY + (Menu_DY * 13);
  dimx           = Text_SX;
  dimy           = Text_SY;
  flags          = flags | IT_EFFECTS_NEXT;
  onselaction[0] = SEL_ACTION_UNDEF;
  text[0]        = "Key: Zoom In";
  text[1]        = "Select Assign and press any key";
};

instance MenuItem_MM_OPT_KeyZoomIn_Choice(C_MENU_ITEM_DEF)
{
  backpic  = "MENU_CHOICE_BACK.TGA";
  type     = MENU_ITEM_CHOICEBOX;
  fontname = "font_old_10_white.tga";
  posx     = Choice_PX;
  posy     = Start_PY + (Menu_DY * 13) + Choice_DY;
  dimx     = Choice_SX;
  dimy     = Choice_SY;
  flags    = flags & ~IT_SELECTABLE;
  flags    = flags | IT_TXT_CENTER;

  onchgsetoption        = "KeyBindZoomIn";
  onchgsetoptionsection = "QUESTHELPER_V2";
  text[0]               = "-|Assign";
};

// --- Key: Zoom Out ---
instance MenuItem_MM_OPT_KeyZoomOut(C_MENU_ITEM_DEF)
{
  backpic        = "";
  posx           = Text_PX;
  posy           = Start_PY + (Menu_DY * 14);
  dimx           = Text_SX;
  dimy           = Text_SY;
  flags          = flags | IT_EFFECTS_NEXT;
  onselaction[0] = SEL_ACTION_UNDEF;
  text[0]        = "Key: Zoom Out";
  text[1]        = "Select Assign and press any key";
};

instance MenuItem_MM_OPT_KeyZoomOut_Choice(C_MENU_ITEM_DEF)
{
  backpic  = "MENU_CHOICE_BACK.TGA";
  type     = MENU_ITEM_CHOICEBOX;
  fontname = "font_old_10_white.tga";
  posx     = Choice_PX;
  posy     = Start_PY + (Menu_DY * 14) + Choice_DY;
  dimx     = Choice_SX;
  dimy     = Choice_SY;
  flags    = flags & ~IT_SELECTABLE;
  flags    = flags | IT_TXT_CENTER;

  onchgsetoption        = "KeyBindZoomOut";
  onchgsetoptionsection = "QUESTHELPER_V2";
  text[0]               = "-|Assign";
};

// --- Key: Legend ---
instance MenuItem_MM_OPT_KeyLegend(C_MENU_ITEM_DEF)
{
  backpic        = "";
  posx           = Text_PX;
  posy           = Start_PY + (Menu_DY * 15);
  dimx           = Text_SX;
  dimy           = Text_SY;
  flags          = flags | IT_EFFECTS_NEXT;
  onselaction[0] = SEL_ACTION_UNDEF;
  text[0]        = "Key: Legend";
  text[1]        = "Select Assign and press any key";
};

instance MenuItem_MM_OPT_KeyLegend_Choice(C_MENU_ITEM_DEF)
{
  backpic  = "MENU_CHOICE_BACK.TGA";
  type     = MENU_ITEM_CHOICEBOX;
  fontname = "font_old_10_white.tga";
  posx     = Choice_PX;
  posy     = Start_PY + (Menu_DY * 15) + Choice_DY;
  dimx     = Choice_SX;
  dimy     = Choice_SY;
  flags    = flags & ~IT_SELECTABLE;
  flags    = flags | IT_TXT_CENTER;

  onchgsetoption        = "KeyBindLegend";
  onchgsetoptionsection = "QUESTHELPER_V2";
  text[0]               = "-|Assign";
};

// --- Key: NPC Search ---
instance MenuItem_MM_OPT_KeyNpcSearch(C_MENU_ITEM_DEF)
{
  backpic        = "";
  posx           = Text_PX;
  posy           = Start_PY + (Menu_DY * 16);
  dimx           = Text_SX;
  dimy           = Text_SY;
  flags          = flags | IT_EFFECTS_NEXT;
  onselaction[0] = SEL_ACTION_UNDEF;
  text[0]        = "Key: NPC Search";
  text[1]        = "Select Assign and press any key";
};

instance MenuItem_MM_OPT_KeyNpcSearch_Choice(C_MENU_ITEM_DEF)
{
  backpic  = "MENU_CHOICE_BACK.TGA";
  type     = MENU_ITEM_CHOICEBOX;
  fontname = "font_old_10_white.tga";
  posx     = Choice_PX;
  posy     = Start_PY + (Menu_DY * 16) + Choice_DY;
  dimx     = Choice_SX;
  dimy     = Choice_SY;
  flags    = flags & ~IT_SELECTABLE;
  flags    = flags | IT_TXT_CENTER;

  onchgsetoption        = "KeyBindNpcSearch";
  onchgsetoptionsection = "QUESTHELPER_V2";
  text[0]               = "-|Assign";
};
