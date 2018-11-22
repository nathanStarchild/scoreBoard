class Spiraliser {
    public:
        Spiraliser();
        void animate();
        bool enabled;
        void enable();
        void disable();
    private:
        uint8_t rep;
        uint8_t segments;
        uint8_t fibPow;
        uint8_t revs;
        uint8_t rfacInit;
        long fTime;
        float symmetry;
        float tf;
        float rfacDt;
        float rainbowRate;
        float rotRate;
        float repShift;
        boolean rotateOn;
        boolean cloackwiseOn;
        boolean counterClockwiseOn;
        uint8_t hue;
        float rotate;
        uint8_t cShift;
};

Spiraliser::Spiraliser() {
    enabled = false;
    revs = 2;
    symmetry = 4;//0.74;
    segments = 10;
    fibPow = 6;
    rep = 3;
    rainbowRate = 1;//0.0024?
    tf = 7;
    repShift = 0.000001;
    rotateOn = true;
    rfacDt = 1/pow(PHI,5);
    rfacInit = 10;
    rotRate = 1;
    clockwiseOn = true;
    counterClockwiseOn = true;
    fTime = 0;
    rotate = 0;
    cShift = 0;
    void draw();
}

void Spiraliser::animate() {  
  rfac = rfacInit + pow(fTime*rfacDt, 2);
  if (rotateOn) {
    rotate = fTime * rotRate * PI / 360;
  } else {
    rotate = 0;
  }
  for (int n=0; n<rep; n++) {
    rotate += 2 * PI / rep;
    hue = int((n*repShift+(fTime / rainbowRate)) % 256);
    for (int t=0; t<segments*revs; t++) {
      uint8_t c1 = (hue+(t*tf))%256
      uint8_t c2 = (hue+(t*tf)+cShift)%256;
      float theta = t * 2 * PI / (segments);
      float theta2 = 2 * PI - theta;
      float r =  rfac * pow(fib, (-1 * symmetry * t/float(segments)));
      float s = r/pow(fib, fibPow);
      if (clockwiseOn) {
        draw(r*sin(theta+rotate), r*cos(theta+rotate), s);
      }
      if (counterClockwiseOn) {
        draw(r*sin(theta2+rotate), r*cos(theta2+rotate), s);
      }
    }
  }
  
  if (frameCount % stepRate == 0) {
    fTime ++;// Next step.
  }
}

void Spiraliser::restart() { 
  fTime = 0;
}

void Spiraliser::draw(float xIn, float yIn, float s) { 
  uint8_t spacing = 4;
  int minX = xIn + (nStrips * spacing / 2) - s;
  int maxX = xIn + (nStrips * spacing / 2) + s;
  int minY = yIn + (ledsPerStrip / 2) - s;
  int maxY = yIn + (ledsPerStrip / 2) + s;
}

void Spiraliser::enable() {
  enabled = true;
}

void Spiraliser::disable() {
  enabled = false;
}

