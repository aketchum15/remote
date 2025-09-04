#pragma once

#include <alsa/asoundlib.h>
#include <opus/opus.h>
#include <string>

#include "tsq.hxx"

enum RecorderError {
    None,
    Error
};

class Recorder {

    public:
        Recorder(std::string, ThreadSafeQueue &);

        ~Recorder();

        void init(void);
        int32_t record();

        void closeSoundCard();

        void close();


    private:
        snd_pcm_t* capture_handle;
        snd_pcm_hw_params_t* hw_params;
        snd_pcm_info_t* s_info;

        std::string snd_device;

        static constexpr uint32_t srate = 48000;
        static constexpr uint32_t nchan = 1;

        // frame size for 20 ms of audio, srate is in MHz 
        static constexpr uint32_t fsize = srate / 1000 * 20;

        OpusEncoder *enc;

        ThreadSafeQueue &q;

        void alsa_init(void);
        void opus_init(void);
};
