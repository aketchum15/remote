#include <alsa/asoundlib.h>

enum RecorderError {
    None,
    Error
};

class Recorder {

    public:
        Recorder();

        ~Recorder();

        RecorderError init(void);
        RecorderError record(void);

        void setSoundDevice(char *);

        RecorderError closeSoundCard();


    private:
        snd_pcm_t* capture_handle;
        snd_pcm_hw_params_t* hw_params;
        snd_pcm_info_t* s_info;

        const char * snd_device;

        unsigned int srate = 48000;
        unsigned const int nchan = 1;


};
