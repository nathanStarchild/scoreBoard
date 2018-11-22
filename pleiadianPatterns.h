struct pattern {
  bool enabled;
  int8_t pspeed;
  uint8_t plength;
  int16_t decay; 
};

typedef struct patternState {
  uint32_t lastUpdate;
  uint32_t nextUpdate;
  bool stale;
  uint8_t hue;
  uint16_t patternStep;
  pattern wave;
  pattern tail;
  pattern breathe;
  pattern glitter;
  pattern crazytown;
  pattern enlightenment;
  pattern ripple;
  pattern blendwave;
  pattern rain;
  pattern holdingPattern;
  pattern mapPattern;
  pattern paletteDisplay;
  pattern sweep;
  pattern dimmer;
  pattern skaters;
  pattern poleChaser;
  pattern powerSaver;
  pattern ants;
  pattern launch;
  pattern houseLights;
};

// wave length 1 to 10
// crazytown length is density
// speed -4 to 5
patternState mainState = {
  0, 0, true, 0, 0, 
  {false, 1, 20, 0}, //wave
  {false, 5, 30, 0}, //tail
  {false, 15, 200, 0}, //breathe
  {false, 0, 50, 0}, //glitter
  {false, 0, 15, 0}, //crazytown
  {false, 0, 0, 0}, //enlightenment
  {true, 0, 0, 0}, //ripple
  {false, 0, 0, 0}, //blendwave
  {true, 50, 10, 150}, //rain
  {false, 0, 0, 0}, //holdingPattern
  {false, 0, 0, 0}, //mapPattern
  {false, 0, 0, 0}, //paletteDisplay
  {false, 0, 0, 0}, //sweep
  {true, 0, 0, 0}, //dimmer
  {false, 1, 1, 0}, //skaters
  {false, 1, 1, 1}, //poleChaser
  {false, 0, 0, 0}, //powerSaver
  {false, 3, 10, 200}, //ants
  {false, 0, 0, 0}, //launch
  {false, 0, 0, 0}, //houseLights
  };
