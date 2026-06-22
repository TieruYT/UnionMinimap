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
// POZIOM 1: G£ÓWNY WPIS "TIERU PLUGINS" W MENU UNION
// ================================================================================================

instance MenuItem_Union_Auto_Tieru(C_MENU_ITEM_UNION_DEF)
{
  text[0]          = "Tieru Plugins";
  text[1]          = "Zestaw modów Tieru";
  onselaction[0]   = SEL_ACTION_STARTMENU;
  onselaction_s[0] = "QuestHelper:Menu_Tieru_Main";
};

// ================================================================================================
// POZIOM 2: LISTA MODÓW (np. MINIMAP)
// ================================================================================================

instance Menu_Tieru_Main(C_MENU_DEF)
{
  backpic    = "";
  items[0]   = "";
  items[100] = "Union_menuitem_back";
  flags      = flags | MENU_SHOW_INFO;
  Menu_SearchItems("QuestHelper:MENUITEM_TIERU_LIST_*");
};

// Nag³ówek
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

// Wpis "Minimap"
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
  text[1]        = "Konfiguracja minimapy";
};

// Wpis "Skróty klawiszowe"
instance MenuItem_Tieru_List_Keybinds(C_MENU_ITEM_DEF)
{
  backpic        = "";
  posx           = Text_PX;
  posy           = Start_PY + Menu_DY;
  dimx           = Text_SX;
  dimy           = Text_SY;
  flags          = flags | IT_EFFECTS_NEXT;
  onselaction[0] = SEL_ACTION_STARTMENU;
  onselaction_s[0] = "QuestHelper:Menu_Keybinds_Settings";
  text[0]        = "Skróty klawiszowe";
  text[1]        = "Konfiguracja skrótow klawiszowych";
};

// ================================================================================================
// POZIOM 3: KONFIGURACJA MINIMAPY
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
  text[0] = "USTAWIENIA MINIMAPY";
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
  text[0]        = "Poka¿ Minimape";
  text[1]        = "W³¹cz lub wy³¹cz minimapê";
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
  text[0]               = "Wy³|W³";
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
  text[0]        = "Poka¿ Skrzynie";
  text[1]        = "Pokazuj kropki skrzyñ na mapie";
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
  text[0]               = "Wy³|W³";
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
  text[0]        = "Ukryj zamkniête na klucz";
  text[1]        = "Ukrywa skrzynie wymagaj¹ce klucza";
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
  text[0]               = "Wy³|W³";
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
  text[0]        = "Wszyscy NPC na bia³o";
  text[1]        = "Wy³¹cz kolory wrogów i questów";
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
  text[0]               = "Wy³|W³";
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
  text[0]        = "Poka¿ Przedmioty";
  text[1]        = "Wyœwietla przedmioty na minimapie (niebieski)";
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
  text[0]               = "Wy³|W³";
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
  text[0]        = "Poka¿ Roœliny";
  text[1]        = "Wyœwietla roœliny i zio³a na minimapie (zielony)";
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
  text[0]               = "Wy³|W³";
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
  text[0]        = "Poka¿ Wrogów";
  text[1]        = "Wyœwietla wrogow na minimapie (czerwony)";
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
  text[0]               = "Wy³|W³";
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
  text[0]        = "Poka¿ Nazwy NPC";
  text[1]        = "Wyœwietla nazwy NPC nad kropkami";
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
  text[0]               = "Wy³|W³";
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
  text[0]        = "Poka¿ Nazwy Przedmiotów";
  text[1]        = "Wyœwietla nazwy przedmiotów (bez roœlin)";
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
  text[0]               = "Wy³|W³";
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
  text[0]        = "Poka¿ Nazwy Roœlin";
  text[1]        = "Wyœwietla nazwy roœlin i zió³";
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
  text[0]               = "Wy³|W³";
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
  text[0]        = "Obracanie Mapy";
  text[1]        = "Mapa obraca siê zgodnie z kierunkiem gracza";
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
  text[0]               = "Wy³|W³";
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
  text[0]        = "Zasiêg Nazw NPC";
  text[1]        = "Maksymalna odlegloœæ wyœwietlania nazw NPC";
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
  text[0]        = "Zoom Mapy";
  text[1]        = "Przybli¿enie mapy (klawisze ; i ')";
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

// --- Show Compass ---
instance MenuItem_MM_OPT_ShowCompass(C_MENU_ITEM_DEF)
{
  backpic        = "";
  posx           = Text_PX;
  posy           = Start_PY + (Menu_DY * 13);
  dimx           = Text_SX;
  dimy           = Text_SY;
  flags          = flags | IT_EFFECTS_NEXT;
  onselaction[0] = SEL_ACTION_UNDEF;
  text[0]        = "Poka¿ Kompas";
  text[1]        = "Wyœwietla kierunki œwiata (N/E/S/W) na ramce";
};

instance MenuItem_MM_OPT_ShowCompass_Choice(C_MENU_ITEM_DEF)
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

  onchgsetoption        = "ShowCompass";
  onchgsetoptionsection = "QUESTHELPER_V2";
  text[0]               = "Wy³|W³";
};

// --- Use World Map ---
instance MenuItem_MM_OPT_UseWorldMap(C_MENU_ITEM_DEF)
{
  backpic        = "";
  posx           = Text_PX;
  posy           = Start_PY + (Menu_DY * 14);
  dimx           = Text_SX;
  dimy           = Text_SY;
  flags          = flags | IT_EFFECTS_NEXT;
  onselaction[0] = SEL_ACTION_UNDEF;
  text[0]        = "Tekstura Mapy Œwiata";
  text[1]        = "Wyœwietla teksturê mapy zamiast kamiennego t³a";
};

instance MenuItem_MM_OPT_UseWorldMap_Choice(C_MENU_ITEM_DEF)
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

  onchgsetoption        = "UseWorldMap";
  onchgsetoptionsection = "QUESTHELPER_V2";
  text[0]               = "Wy³|W³";
};

// ================================================================================================
// POZIOM 3: SKRÓTY KLAWISZOWE
// ================================================================================================

instance Menu_Keybinds_Settings(C_MENU_DEF)
{
  backpic    = "";
  items[0]   = "";
  items[100] = "Union_menuitem_back";
  flags      = flags | MENU_SHOW_INFO;
  Menu_SearchItems("QuestHelper:MENUITEM_KB_OPT_*");
};

// --- Headline ---
instance MenuItem_KB_OPT_Headline(C_MENU_ITEM_DEF)
{
  type    = MENU_ITEM_TEXT;
  posx    = 0;
  posy    = Title_PY;
  dimx    = 8100;
  flags   = flags & ~IT_SELECTABLE;
  flags   = flags | IT_TXT_CENTER;
  text[0] = "SKRÓTY KLAWISZOWE";
};

// --- Key Zoom In ---
instance MenuItem_KB_OPT_ZoomIn(C_MENU_ITEM_DEF)
{
  backpic        = "";
  posx           = Text_PX;
  posy           = Start_PY;
  dimx           = Text_SX;
  dimy           = Text_SY;
  flags          = flags | IT_EFFECTS_NEXT;
  onselaction[0] = SEL_ACTION_UNDEF;
  text[0]        = "Przybli¿ mapê";
  text[1]        = "Klawisz do przybli¿ania minimapy";
};

instance MenuItem_KB_OPT_ZoomIn_Choice(C_MENU_ITEM_DEF)
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

  onchgsetoption        = "KeyZoomIn";
  onchgsetoptionsection = "QUESTHELPER_V2";
  text[0]               = ".|,|;|'|[|]|-|=|/|F1|F2|F3|F4|F5|F6|NUM+|NUM-|NUM*|PGUP|PGDN";
};

// --- Key Zoom Out ---
instance MenuItem_KB_OPT_ZoomOut(C_MENU_ITEM_DEF)
{
  backpic        = "";
  posx           = Text_PX;
  posy           = Start_PY + Menu_DY;
  dimx           = Text_SX;
  dimy           = Text_SY;
  flags          = flags | IT_EFFECTS_NEXT;
  onselaction[0] = SEL_ACTION_UNDEF;
  text[0]        = "Oddal mapê";
  text[1]        = "Klawisz do oddalania minimapy";
};

instance MenuItem_KB_OPT_ZoomOut_Choice(C_MENU_ITEM_DEF)
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

  onchgsetoption        = "KeyZoomOut";
  onchgsetoptionsection = "QUESTHELPER_V2";
  text[0]               = ",|.|;|'|[|]|-|=|/|F1|F2|F3|F4|F5|F6|NUM+|NUM-|NUM*|PGUP|PGDN";
};

// --- Key Legend ---
instance MenuItem_KB_OPT_Legend(C_MENU_ITEM_DEF)
{
  backpic        = "";
  posx           = Text_PX;
  posy           = Start_PY + (Menu_DY * 2);
  dimx           = Text_SX;
  dimy           = Text_SY;
  flags          = flags | IT_EFFECTS_NEXT;
  onselaction[0] = SEL_ACTION_UNDEF;
  text[0]        = "Legenda";
  text[1]        = "Klawisz do W³¹czania legendy";
};

instance MenuItem_KB_OPT_Legend_Choice(C_MENU_ITEM_DEF)
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

  onchgsetoption        = "KeyLegend";
  onchgsetoptionsection = "QUESTHELPER_V2";
  text[0]               = "/|.|,|;|'|[|]|-|=|F1|F2|F3|F4|F5|F6|NUM+|NUM-|NUM*|PGUP|PGDN";
};

// --- Key NPC Search ---
instance MenuItem_KB_OPT_NpcSearch(C_MENU_ITEM_DEF)
{
  backpic        = "";
  posx           = Text_PX;
  posy           = Start_PY + (Menu_DY * 3);
  dimx           = Text_SX;
  dimy           = Text_SY;
  flags          = flags | IT_EFFECTS_NEXT;
  onselaction[0] = SEL_ACTION_UNDEF;
  text[0]        = "Szukaj NPC";
  text[1]        = "Klawisz do otwierania wyszukiwarki NPC";
};

instance MenuItem_KB_OPT_NpcSearch_Choice(C_MENU_ITEM_DEF)
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

  onchgsetoption        = "KeyNpcSearch";
  onchgsetoptionsection = "QUESTHELPER_V2";
  text[0]               = "\\|/|.|,|;|'|[|]|-|=|F1|F2|F3|F4|F5|F6|NUM+|NUM-|NUM*|PGUP|PGDN";
};
