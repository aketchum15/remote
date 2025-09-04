#include <array>
#include <iostream>
#include <opus/opus_defines.h>

#include "recorder.hxx"

Recorder::Recorder(std::string dev, ThreadSafeQueue &q): snd_device{dev}, q{q} { }

Recorder::~Recorder() {}

void Recorder::closeSoundCard() {
    if (snd_pcm_close(capture_handle) != 0) {
        //todo error
    }
};

void Recorder::alsa_init() {

    int err = 0;

    uint32_t local_srate = srate;

    if ((err = snd_pcm_open(&capture_handle, snd_device.c_str(), SND_PCM_STREAM_CAPTURE, 0)) < 0) {
        std::cerr << "cannot open audio device " << snd_device << "(" << snd_strerror(err) << ", " << err << ")" << "\n";
        //TODO: throw
    }
    if ((err = snd_pcm_hw_params_malloc(&hw_params)) < 0) {
        std::cerr << "cannot allocate hardware parameter structure " << "(" << snd_strerror(err) << ", " << err << ")" << "\n";
        //TODO: throw
    }
    if ((err = snd_pcm_hw_params_any(capture_handle, hw_params)) < 0) {
        std::cerr << "cannot initialize hardware parameter structure " << "(" << snd_strerror(err) << ", " << err << ")" << "\n";
        //TODO: throw
    }
    if ((err = snd_pcm_hw_params_set_access(capture_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0) {
        std::cerr << "cannot set access type " << "(" << snd_strerror(err) << ", " << err << ")" << "\n";
        //TODO: throw
    }
    if ((err = snd_pcm_hw_params_set_format(capture_handle, hw_params, SND_PCM_FORMAT_S16_LE)) < 0) {
        std::cerr << "cannot set sample format " << "(" << snd_strerror(err) << ", " << err << ")" << "\n";
        //TODO: throw
    }
    if ((err = snd_pcm_hw_params_set_rate_near(capture_handle, hw_params, &local_srate, 0)) < 0) {
        std::cerr << "cannot set sample rate " << "(" << snd_strerror(err) << ", " << err << ")" << "\n";
        //TODO: throw
    }
    if ((err = snd_pcm_hw_params_set_channels(capture_handle, hw_params, nchan))< 0) {
        std::cerr << "cannot set channel count " << "(" << snd_strerror(err) << ", " << err << ")" << "\n";
        //TODO: throw
    }
    if ((err = snd_pcm_hw_params(capture_handle, hw_params)) < 0) {
        std::cerr << "cannot set parameters " << "(" << snd_strerror(err) << ", " << err << ")" << "\n";
        //TODO: throw
    }
    if ((err = snd_pcm_prepare(capture_handle)) < 0) {
        std::cerr << "cannot prepare audio interface for use " << "(" << snd_strerror(err) << ", " << err << ")" << "\n";
        //TODO: throw
    }
    if ((err = snd_pcm_start(capture_handle)) < 0) {
        std::cerr << "cannot start soundcard " << "(" << snd_strerror(err) << ", " << err << ")" << "\n";
        //TODO: throw
    }
};
void Recorder::opus_init() {

    int err = 0;
    enc = opus_encoder_create(srate, 1, OPUS_APPLICATION_VOIP, &err);
    if (err != 0) {
        std::cerr << "opus init error";
    };
};


void Recorder::init(){
     
    alsa_init();
    opus_init();
    //TODO: errors
}

int32_t Recorder::record() {

    int ret = 0;
    std::array<int16_t, fsize> raw_buf;
    std::array<uint8_t, fsize> encoded_buf;

    if ( (ret = snd_pcm_readi(capture_handle, raw_buf.data(), fsize)) != static_cast<int>(fsize)) {
        
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
    ret = opus_encode(enc, raw_buf.data(), fsize, encoded_buf.data(), fsize);

    if (ret < 0) {
        //TODO: throw 
        std::cout << "encoding error\n";
    };

    // push encoded data onto queue
    q.push_range(encoded_buf);
    encoded_buf.fill(0);

    return ret;
}

void Recorder::close() {
    q.close();
}
