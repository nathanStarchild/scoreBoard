class MilliTimer {
  public:
    MilliTimer(uint32_t);
    bool isItTime();
    void startTimer();
    void stopTimer();
    void resetTimer();
    void setInterval(uint32_t);
    void updateTimer();
    bool isRunning();
    uint32_t elapsed();
  private:
    uint32_t started;
    uint32_t interval;
    uint32_t next;
    bool _running;
  
};

MilliTimer::MilliTimer(uint32_t intv) {
  interval = intv;
  _running = true;
  startTimer();
}

bool MilliTimer::isItTime() {
  uint32_t now = millis();
  return( (((int) now - (int) next) >= 0) && _running );
}

void MilliTimer::setInterval(uint32_t intv) {
  interval = intv;
  resetTimer();
}

void MilliTimer::startTimer() {
  _running = true;
  resetTimer();
}

void MilliTimer::stopTimer() {
  _running = false;
}

void MilliTimer::resetTimer() {
   started = millis();
   next = started + interval;
}

bool MilliTimer::isRunning() {
  return(_running);
}

uint32_t MilliTimer::elapsed() {
  if (_running) {
    uint32_t now = millis();
    return ( now - started);
  }
  return 0;
}

void MilliTimer::updateTimer() {
  started = next;
  next = started + interval;
};


