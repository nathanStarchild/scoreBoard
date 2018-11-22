//************************************************************
//palettes, shared

CRGBPalette16 currentPalette;
CRGBPalette16 targetPalette;
TBlendType    currentBlending;                                // NOBLEND or LINEARBLEND
CRGB designColour = CRGB::Black;
CRGBPalette16 currentDesignPalette;
//CRGBPalette16 targetDesignPalette;
uint8_t paletteCycleIndex = 0;
const uint8_t nPalettes = 23;


#ifndef NO_PALETTES
DEFINE_GRADIENT_PALETTE( departure_gp ) {
    0,   8,  3,  0,
   42,  23,  7,  0,
   63,  75, 38,  6,
   84, 169, 99, 38,
  106, 213,169,119,
  116, 255,255,255,
  138, 135,255,138,
  148,  22,255, 24,
  170,   0,255,  0,
  191,   0,136,  0,
  212,   0, 55,  0,
  255, 0, 55, 0};

DEFINE_GRADIENT_PALETTE( rainbowsherbet_gp ) {
    0, 255, 33,  4,
   43, 255, 68, 25,
   86, 255,  7, 25,
  127, 255, 82,103,
  170, 255,255,242,
  209,  42,255, 22,
  255, 87,255, 65};

DEFINE_GRADIENT_PALETTE( GMT_drywet_gp ) {
    0,  47, 30,  2,
   42, 213,147, 24,
   84, 103,219, 52,
  127,   3,219,207,
  170,   1, 48,214,
  212,   1,  1,111,
  255, 1, 7, 33};

DEFINE_GRADIENT_PALETTE( Colorfull_gp ) {
    0,  10, 85,  5,
   25,  29,109, 18,
   60,  59,138, 42,
   93,  83, 99, 52,
  106, 110, 66, 64,
  109, 123, 49, 65,
  113, 139, 35, 66,
  116, 192,117, 98,
  124, 255,255,137,
  168, 100,180,155,
  255, 22,121,174};

DEFINE_GRADIENT_PALETTE( Sunset_Real_gp ) {
    0, 120,  0,  0,
   22, 179, 22,  0,
   51, 255,104,  0,
   85, 167, 22, 18,
  135, 100,  0,103,
  198,  16,  0,130,
  255, 0, 0,160};

DEFINE_GRADIENT_PALETTE( Blue_Cyan_Yellow_gp ) {
    0,   0,  0,255,
   63,   0, 55,255,
  127,   0,255,255,
  191,  42,255, 45,
  255, 255,255,  0};

DEFINE_GRADIENT_PALETTE( byr_gp ) {
  // Gradient palette "byr_gp", originally from
  // http://soliton.vm.bytemark.co.uk/pub/cpt-city/grass/tn/byr.png.index.html
  // converted for FastLED with gammas (2.6, 2.2, 2.5)
  // Size: 12 bytes of program space.
    0,   0,  0,255,
  127, 255,255,  0,
  255, 255,  0,  0};

DEFINE_GRADIENT_PALETTE( Adrift_in_Dreams_gp ) {//woah mamma its delicious
  // Gradient palette "Adrift_in_Dreams_gp", originally from
  // http://soliton.vm.bytemark.co.uk/pub/cpt-city/colo/Skyblue2u/tn/Adrift_in_Dreams.png.index.html
  // converted for FastLED with gammas (2.6, 2.2, 2.5)
  // Size: 40 bytes of program space.
    0, 148,223, 77,
   51, 148,223, 77,
   51,  86,182, 89,
  102,  86,182, 89,
  102,  36,131, 72,
  153,  36,131, 72,
  153,   5, 61, 51,
  204,   5, 61, 51,
  204,   1, 15, 29,
  255,   1, 15, 29};

DEFINE_GRADIENT_PALETTE( Crystal_Ice_Palace_gp ) {
  // Gradient palette "Crystal_Ice_Palace_gp", originally from
  // http://soliton.vm.bytemark.co.uk/pub/cpt-city/colo/Skyblue2u/tn/Crystal_Ice_Palace.png.index.html
  // converted for FastLED with gammas (2.6, 2.2, 2.5)
  // Size: 40 bytes of program space.
    0, 242,223,252,
   51, 242,223,252,
   51, 227,237,235,
  102, 227,237,235,
  102, 188,235,226,
  153, 188,235,226,
  153, 128,217,192,
  204, 128,217,192,
  204,  92,176,137,
  255,  92,176,137};

DEFINE_GRADIENT_PALETTE( Dusk_Finds_Us_gp ) {
  // Gradient palette "Dusk_Finds_Us_gp", originally from
  // http://soliton.vm.bytemark.co.uk/pub/cpt-city/colo/Skyblue2u/tn/Dusk_Finds_Us.png.index.html
  // converted for FastLED with gammas (2.6, 2.2, 2.5)
  // Size: 40 bytes of program space.
    0,  64, 66, 82,
  132,  64, 66, 82,
  132,  78, 47, 65,
  142,  78, 47, 65,
  142,  92, 32, 51,
  153,  92, 32, 51,
  153, 110, 19, 39,
  170, 110, 19, 39,
  170, 148,  4, 20,
  255, 148,  4, 20};

DEFINE_GRADIENT_PALETTE( Lucy_in_the_Sky_gp ) {//pink to orange
   // Gradient palette "Lucy_in_the_Sky_gp", originally from
  // http://soliton.vm.bytemark.co.uk/pub/cpt-city/colo/vredeling/tn/Lucy_in_the_Sky.png.index.html
  // converted for FastLED with gammas (2.6, 2.2, 2.5)
  // Size: 40 bytes of program space.
    0, 175,  1, 44,
   51, 175,  1, 44,
   51, 194, 14, 35,
  102, 194, 14, 35,
  102, 213, 40, 27,
  153, 213, 40, 27,
  153, 234, 80, 21,
  204, 234, 80, 21,
  204, 255,136, 15,
  255, 255,136, 15};

DEFINE_GRADIENT_PALETTE( Alive_And_Kicking_gp ) {// Bright pastels
  // Gradient palette "Alive_And_Kicking_gp", originally from
  // http://soliton.vm.bytemark.co.uk/pub/cpt-city/colo/rphnick/tn/Alive_And_Kicking.png.index.html
  // converted for FastLED with gammas (2.6, 2.2, 2.5)
  // Size: 40 bytes of program space.
    0, 192, 50,207,
   51, 192, 50,207,
   51,  87, 50,207,
  102,  87, 50,207,
  102,  37, 73,207,
  153,  37, 73,207,
  153,  37,127,207,
  204,  37,127,207,
  204,  37,213,140,
  255,  37,213,140};

DEFINE_GRADIENT_PALETTE( Warm_summer_day_gp ) {//as advertised
  // Gradient palette "Warm_summer_day_gp", originally from
  // http://soliton.vm.bytemark.co.uk/pub/cpt-city/colo/smorin2002/tn/Warm_summer_day.png.index.html
  // converted for FastLED with gammas (2.6, 2.2, 2.5)
  // Size: 40 bytes of program space.
    0, 255,225, 39,
   51, 255,225, 39,
   51, 206,255, 38,
  102, 206,255, 38,
  102, 137,255, 38,
  153, 137,255, 38,
  153,  84,255, 38,
  204,  84,255, 38,
  204,  46,255, 38,
  255,  46,255, 38};

DEFINE_GRADIENT_PALETTE( Id_Like_To_Dive_gp ) {//aqua and purple
  // Gradient palette "Id_Like_To_Dive_gp", originally from
  // http://soliton.vm.bytemark.co.uk/pub/cpt-city/colo/sugar/tn/Id_Like_To_Dive.png.index.html
  // converted for FastLED with gammas (2.6, 2.2, 2.5)
  // Size: 40 bytes of program space.
    0,  59,125, 77,
   40,  59,125, 77,
   40,  11, 85, 74,
  109,  11, 85, 74,
  109,  17, 33, 84,
  124,  17, 33, 84,
  124,  18,  6, 63,
  158,  18,  6, 63,
  158,   7,  1, 22,
  255,   7,  1, 22};

DEFINE_GRADIENT_PALETTE( Palomino_gp ) {//orangish pastels
  // Gradient palette "Palomino_gp", originally from
  // http://soliton.vm.bytemark.co.uk/pub/cpt-city/colo/smorin2002/tn/Palomino.png.index.html
  // converted for FastLED with gammas (2.6, 2.2, 2.5)
  // Size: 40 bytes of program space.
    0, 242,237,114,
   51, 242,237,114,
   51, 222,149, 11,
  102, 222,149, 11,
  102, 210,100,  5,
  153, 210,100,  5,
  153, 177, 80,  4,
  204, 177, 80,  4,
  204,  19, 11,  1,
  255,  19, 11,  1};

DEFINE_GRADIENT_PALETTE( Sleep_Deprevation_gp ) {//!!!!!!11purple to blue grey
  // Gradient palette "Sleep_Deprevation_gp", originally from
  // http://soliton.vm.bytemark.co.uk/pub/cpt-city/colo/vredeling/tn/Sleep_Deprevation.png.index.html
  // converted for FastLED with gammas (2.6, 2.2, 2.5)
  // Size: 40 bytes of program space.
    0,  21,  3, 56,
   51,  21,  3, 56,
   51,  39, 21,145,
  102,  39, 21,145,
  102,  43, 47,170,
  153,  43, 47,170,
  153,  77,105,197,
  204,  77,105,197,
  204, 146,184,223,
  255, 146,184,223};
  
DEFINE_GRADIENT_PALETTE( srtYlGnBu05_gp ) {
// Gradient palette "srtYlGnBu05_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/jjg/serrate/seq/tn/srtYlGnBu05.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 40 bytes of program space.
    0, 133,209,106,
   51, 255,255,145,
   51,  41,157,117,
  102, 133,209,106,
  102,   7,121,132,
  153,  41,157,117,
  153,   2, 55,112,
  204,   7,121,132,
  204,   1,  7, 65,
  255,   2, 55,112};

DEFINE_GRADIENT_PALETTE( Emo_Barbie_Playmate_gp ) {//!!!!!!!!!red
  // Gradient palette "Emo_Barbie_Playmate_gp", originally from
  // http://soliton.vm.bytemark.co.uk/pub/cpt-city/colo/rphnick/tn/Emo_Barbie_Playmate.png.index.html
  // converted for FastLED with gammas (2.6, 2.2, 2.5)
  // Size: 40 bytes of program space.
    0,  64,  1,  1,
   51,  64,  1,  1,
   51, 152,  6,  4,
  102, 152,  6,  4,
  102, 206, 36, 29,
  153, 206, 36, 29,
  153, 247,100, 77,
  204, 247,100, 77,
  204, 234,176,123,
  255, 234,176,123};

DEFINE_GRADIENT_PALETTE( autumnrose_gp ) {//!!!!Orange
// Gradient palette "autumnrose_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/rc/tn/autumnrose.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 32 bytes of program space.
    0,  71,  3,  1,
   45, 128,  5,  2,
   84, 186, 11,  3,
  127, 215, 27,  8,
  153, 224, 69, 13,
  188, 229, 84,  6,
  226, 242,135, 17,
  255, 247,161, 79};

DEFINE_GRADIENT_PALETTE( ib53_gp ) {//!!!yellow/orange
  // Gradient palette "ib53_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/ing/general2/tn/ib53.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 36 bytes of program space.
    0, 247,105,  0,
   16, 252,141,  1,
   22, 255,180,  1,
   47, 234,125,  1,
   51, 215, 81,  0,
   68, 232,141,  0,
  170, 249,219,  0,
  217, 252,164,  0,
  255, 255,118,  0};

DEFINE_GRADIENT_PALETTE( srtYlGn04_gp ) {//!!green(yellow green)
// Gradient palette "srtYlGn04_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/jjg/serrate/seq/tn/srtYlGn04.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 32 bytes of program space.
    0, 125,203, 71,
   63, 255,255,145,
   63,  35,146, 39,
  127, 125,203, 71,
  127,   3, 95, 15,
  191,  35,146, 39,
  191,   0, 35,  5,
  255,   3, 95, 15};

DEFINE_GRADIENT_PALETTE( velvet_ocean_gp ) { //!!!!!!!aqua
  // Gradient palette "velvet_ocean_gp", originally from
  // http://soliton.vm.bytemark.co.uk/pub/cpt-city/colo/Skyblue2u/tn/velvet_ocean.png.index.html
  // converted for FastLED with gammas (2.6, 2.2, 2.5)
  // Size: 40 bytes of program space.
    0,   1, 79, 80,
   51,   1, 79, 80,
   51,   1, 62, 62,
  102,   1, 62, 62,
  102,   1, 43, 40,
  153,   1, 43, 40,
  153,   1, 27, 23,
  204,   1, 27, 23,
  204,   1, 13, 10,
  255,   1, 13, 10};

DEFINE_GRADIENT_PALETTE( Deep_Skyblues_gp ) { //!!!!blue
  // Gradient palette "Deep_Skyblues_gp", originally from
  // http://soliton.vm.bytemark.co.uk/pub/cpt-city/colo/Skyblue2u/tn/Deep_Skyblues.png.index.html
  // converted for FastLED with gammas (2.6, 2.2, 2.5)
  // Size: 40 bytes of program space.
    0,   1, 55,140,
   51,   1, 55,140,
   51,   1, 18, 96,
  102,   1, 18, 96,
  102,   1,  1, 56,
  153,   1,  1, 56,
  153,   1,  1, 25,
  204,   1,  1, 25,
  204,   1,  1,  7,
  255,   1,  1,  7};

DEFINE_GRADIENT_PALETTE( srtPurples04_gp ) {//!!!!!Purple
// Gradient palette "srtPurples04_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/jjg/serrate/seq/tn/srtPurples04.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 32 bytes of program space.
    0, 140,151,188,
   63, 222,223,235,
   63,  73, 84,138,
  127, 140,151,188,
  127,  33, 37,102,
  191,  73, 84,138,
  191,  14,  4, 60,
  255,  33, 37,102};

CRGBPalette16 mapPalettes[7] = {
  Emo_Barbie_Playmate_gp, 
  Lucy_in_the_Sky_gp,//pink and yellow - maybe shorten the pink
  srtYlGn04_gp, 
  Adrift_in_Dreams_gp,//blue to whitish blue - 1st blue
  Id_Like_To_Dive_gp,//light blue to purple
  Sleep_Deprevation_gp,//blue and purple - maybe the purple
  RainbowColors_p,
  };

CRGBPalette16 cyclePalettes[nPalettes] = {
  OceanColors_p,//0
  Id_Like_To_Dive_gp,//light blue to purple
  departure_gp,//green brown white
  Sleep_Deprevation_gp,//1 blue and purple - maybe the purple
  rainbowsherbet_gp,//pink green whiteish purple
  GMT_drywet_gp,//blue and brown
  CloudColors_p, //2
  Colorfull_gp,//white pastels
  Sunset_Real_gp,//bold blue purple yellow red NOPE
  Blue_Cyan_Yellow_gp,//3
  RainbowColors_p,
  byr_gp,
  Adrift_in_Dreams_gp,//4 blue to whitish blue - 1st blue
  Crystal_Ice_Palace_gp,// washed out blue and purple NOPE
  Dusk_Finds_Us_gp,//pale blue and pink
  Alive_And_Kicking_gp,//blue and purple
  srtYlGnBu05_gp,  
  Adrift_in_Dreams_gp,//5 blue to whitish blue - 1st blue
  ib53_gp, 
  velvet_ocean_gp, //6
  autumnrose_gp,
  Deep_Skyblues_gp, //7
  srtPurples04_gp,   
};

#else

CRGBPalette16 mapPalettes[7] = {
  RainbowColors_p,
  RainbowColors_p,
  RainbowColors_p,
  RainbowColors_p,
  RainbowColors_p,
  RainbowColors_p,
  RainbowColors_p,
};

CRGBPalette16 cyclePalettes[nPalettes] = {
  OceanColors_p,//0
  OceanColors_p,//0
  OceanColors_p,//0
  OceanColors_p,//0
  OceanColors_p,//0
  OceanColors_p,//0
  OceanColors_p,//0
  OceanColors_p,//0
  OceanColors_p,//0
  OceanColors_p,//0
  OceanColors_p,//0
  OceanColors_p,//0
  OceanColors_p,//0
  OceanColors_p,//0
  OceanColors_p,//0
  OceanColors_p,//0
  OceanColors_p,//0
  OceanColors_p,//0
  OceanColors_p,//0
  OceanColors_p,//0
  OceanColors_p,//0
  OceanColors_p,//0
  OceanColors_p,//0
};
#endif


