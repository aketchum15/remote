#include "inc/recorder.hxx"
#include <iostream>
#include <opus/opus_defines.h>
#include <vector>

void Recorder::setSoundDevice(char *str) {
    this->snd_device = str;
}

Recorder::Recorder() {}

Recorder::~Recorder() {}

RecorderError Recorder::closeSoundCard() {
    if (snd_pcm_close(capture_handle) != 0) 
        return Error;
    return None;
};

void Recorder::alsa_init() {

    int err = 0;

    if ((err = snd_pcm_open(&capture_handle, snd_device, SND_PCM_STREAM_CAPTURE, 0)) < 0)
    {
        std::cerr << "cannot open audio device " << snd_device << "(" << snd_strerror(err) << ", " << err << ")" << "\n";
        //TODO: throw
    }

    if ((err = snd_pcm_hw_params_malloc(&hw_params)) < 0)
    {
        std::cerr << "cannot allocate hardware parameter structure " << "(" << snd_strerror(err) << ", " << err << ")" << "\n";
        //TODO: throw
    }

    if ((err = snd_pcm_hw_params_any(capture_handle, hw_params)) < 0)
    {
        std::cerr << "cannot initialize hardware parameter structure " << "(" << snd_strerror(err) << ", " << err << ")" << "\n";
        //TODO: throw
    }

    if ((err = snd_pcm_hw_params_set_access(capture_handle, hw_params,
                SND_PCM_ACCESS_RW_INTERLEAVED)) < 0)
    {
        std::cerr << "cannot set access type " << "(" << snd_strerror(err) << ", " << err << ")" << "\n";
        //TODO: throw
    }

    if ((err = snd_pcm_hw_params_set_format(capture_handle, hw_params, SND_PCM_FORMAT_S16_LE)) < 0)
    {
        std::cerr << "cannot set sample format " << "(" << snd_strerror(err) << ", " << err << ")" << "\n";
        //TODO: throw
    }

    if ((err = snd_pcm_hw_params_set_rate_near(capture_handle, hw_params,
                &srate, 0)) < 0)
    {
        std::cerr << "cannot set sample rate " << "(" << snd_strerror(err) << ", " << err << ")" << "\n";
        //TODO: throw
    }

    if ((err = snd_pcm_hw_params_set_channels(capture_handle, hw_params, nchan))< 0)
    {
        std::cerr << "cannot set channel count " << "(" << snd_strerror(err) << ", " << err << ")" << "\n";
        //TODO: throw
    }

    if ((err = snd_pcm_hw_params(capture_handle, hw_params)) < 0)
    {
        std::cerr << "cannot set parameters " << "(" << snd_strerror(err) << ", " << err << ")" << "\n";
        //TODO: throw
    }

    if ((err = snd_pcm_prepare(capture_handle)) < 0)
    {
        std::cerr << "cannot prepare audio interface for use " << "(" << snd_strerror(err) << ", " << err << ")" << "\n";
        //TODO: throw
    }

    if ((err = snd_pcm_start(capture_handle)) < 0)
    {
        std::cerr << "cannot start soundcard " << "(" << snd_strerror(err) << ", " << err << ")" << "\n";
        //TODO: throw
    }
};
void Recorder::opus_init() {

    int err = 0;
    enc = opus_encoder_create(srate, 1, OPUS_APPLICATION_VOIP, &err);
    if (err != 0) {
        //TODO:throw
    };
};


RecorderError Recorder::init(){
     
    alsa_init();
    opus_init();

    return RecorderError::None;
}

void Recorder::record(std::vector<uint8_t> &buf) {

    int ret = 0;
    uint32_t rawbuf_size = srate/5;
    int16_t raw_buf[rawbuf_size];

    //TODO: magi number, want 20ms of audio
    if ( (ret = snd_pcm_readi(capture_handle, raw_buf, rawbuf_size)) != static_cast<int>(rawbuf_size)) {
        
        std::cerr << "read from audio interface failed " << snd_strerror(ret) << "\n";

        if (ret == -32) {
            if ( (ret = snd_pcm_prepare(capture_handle)) < 0) {
                std::cerr << "cannot prepare interface " << snd_strerror(ret) << "\n";
                //TODO: err
            }
        } else {
            std::cerr << "did not fill buffer\n";
            //TODO: err
        }
    }

    //encode that bitch
    ret = opus_encode(enc, raw_buf, 960, buf.data(), (sizeof(int16_t) * rawbuf_size) );
    if (ret < 0) {
        //TODO: throw 
    };
    


}
