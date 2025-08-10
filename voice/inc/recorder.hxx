#pragma once

#include <alsa/asoundlib.h>
#include <array>
#include <opus/opus.h>
#include <string>

#include "common.hxx"
#include "threadSafeQueue.hxx"
#include "packet.hxx"

enum RecorderError {
    None,
    Error
};

class Recorder {

    public:
        Recorder(std::string);

        ~Recorder();

        void init(void);
        int32_t record();

        RecorderError closeSoundCard();


    private:
        snd_pcm_t* capture_handle;
        snd_pcm_hw_params_t* hw_params;
        snd_pcm_info_t* s_info;

        std::string snd_device;

        uint32_t srate = 48000;
        const uint32_t nchan = 1;

        // frame size for 20 ms of audio, srate is in MHz 
        uint32_t fsize  = srate / 1000 * 20;

        OpusEncoder *enc;

        ThreadSafeQueue<Packet<uint8_t>> q;

        void alsa_init(void);
        void opus_init(void);
};
