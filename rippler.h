class Rippler {
    public:
        Rippler();
        void animate();
        bool enabled;
        void enable();
        void disable();
    private:
        int rippleLoc;
        uint8_t colour;                                               // Ripple colour is randomized.
        int center;                                               // Center of the current ripple.
        int rippleStep;                                          // -1 is the initializing step.
        uint8_t myfade;    
        uint8_t nSteps;
        void restart();
};

Rippler::Rippler() {
    center = 0;
    rippleStep = -1;
    myfade = 255; 
    nSteps = random8(6, 60);
    enabled = false;
    restart();
}

void Rippler::animate() {                                                       // Middle of the ripples.
    rippleLoc = ease8InOutApprox(rippleStep);
    leds[(center + rippleLoc + num_leds) % num_leds] += ColorFromPalette(currentPalette, colour, myfade / (rippleLoc + 1) * 2, currentBlending); // Simple wrap from Marc Miller
    leds[(center - rippleLoc + num_leds) % num_leds] += ColorFromPalette(currentPalette, colour, myfade / (rippleLoc + 1) * 2, currentBlending);
    
    if (frameCount % stepRate == 0) {
      rippleStep ++;// Next step.
    }
    if (rippleStep == nSteps) {
        restart();
    }
}

void Rippler::restart() {                                                // Initialize ripple variables.
    center = random(num_leds);
    colour = random8();
    leds[center] = ColorFromPalette(currentPalette, colour, myfade, currentBlending);
    rippleStep = 1;
}

void Rippler::enable() {
  enabled = true;
}

void Rippler::disable() {
  enabled = false;
}

