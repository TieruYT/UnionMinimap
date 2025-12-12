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
// POZIOM 1: GŁÓWNY WPIS "TIERU PLUGINS" W MENU UNION
// ================================================================================================

instance MenuItem_Union_Auto_Tieru(C_MENU_ITEM_UNION_DEF)
{
  text[0]          = "Tieru Plugins";
  text[1]          = "Zestaw modow Tieru";
  onselaction[0]   = SEL_ACTION_STARTMENU;
  onselaction_s[0] = "QuestHelper:Menu_Tieru_Main";
};

// ================================================================================================
// POZIOM 2: LISTA MODÓW (np. MINIMAP)
// ================================================================================================

instance Menu_Tieru_Main(C_MENU_DEF)
{
  backpic    = "UnionMenu_BackPic.tga";
  items[0]   = "";
  items[100] = "Union_menuitem_back";
  flags      = flags | MENU_SHOW_INFO;
  Menu_SearchItems("QuestHelper:MENUITEM_TIERU_LIST_*");
};

// Nagłówek
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

// ================================================================================================
// POZIOM 3: KONFIGURACJA MINIMAPY
// ================================================================================================

instance Menu_Minimap_Settings(C_MENU_DEF)
{
  backpic    = "UnionMenu_BackPic.tga";
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
  text[0]        = "Pokaz Minimape";
  text[1]        = "Wlacz lub wylacz minimape";
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
  text[0]               = "Wyl|Wl";
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
  text[0]        = "Pokaz Skrzynie";
  text[1]        = "Pokazuj kropki skrzyn na mapie";
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
  text[0]               = "Wyl|Wl";
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
  text[0]        = "Ukryj zamkniete na klucz";
  text[1]        = "Ukrywa skrzynie wymagajace klucza";
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
  text[0]               = "Wyl|Wl";
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
  text[0]        = "Wszyscy NPC na bialo";
  text[1]        = "Wylacz kolory wrogow i questow";
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
  text[0]               = "Wyl|Wl";
};
