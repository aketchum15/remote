#include <alsa/asoundlib.h>
#include <opus/opus.h>
#include <cstddef>
#include <vector>

enum RecorderError {
    None,
    Error
};

class Recorder {

    public:
        Recorder();

        ~Recorder();

        RecorderError init(void);
        void record(std::vector<uint8_t> &);

        void setSoundDevice(char *);

        RecorderError closeSoundCard();

        static const size_t BUF_SIZE = 1024;


    private:
        snd_pcm_t* capture_handle;
        snd_pcm_hw_params_t* hw_params;
        snd_pcm_info_t* s_info;

        const char * snd_device;

        unsigned int srate = 48000;
        unsigned const int nchan = 1;

        unsigned char buffer[BUF_SIZE];


        OpusEncoder *enc;

        void alsa_init(void);
        void opus_init(void);
};
