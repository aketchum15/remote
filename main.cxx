#include <alsa/asoundlib.h>
#include "inc/recorder.hxx"
#include <csignal>

Recorder r;


void intHandler(int dummy) {
   r.closeSoundCard();
}


int main() {

    signal(SIGINT, intHandler);


    Recorder r; 
    r.setSoundDevice("hw:Device,0");

    r.init();
    r.record();
    return 0;
}
