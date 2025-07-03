#include "inc/recorder.hxx"
#include <alsa/asoundlib.h>
#include <iostream>

void Recorder::setSoundDevice(char *str) {
    this->snd_device = str;
}

RecorderError Recorder::closeSoundCard() {
    if (snd_pcm_close(capture_handle) != 0) 
        return Error;
    return None;
};


RecorderError Recorder::init(){
     
    int err = 0;

    if ((err = snd_pcm_open(&capture_handle, snd_device, SND_PCM_STREAM_CAPTURE, 0)) < 0)

    {
        std::cerr << "cannot open audio device " << snd_device << "(" << snd_strerror(err) << ", " << err << ")" << "\n";
        return RecorderError::Error;
    }

    if ((err = snd_pcm_hw_params_malloc(&hw_params)) < 0)
    {
        std::cerr << "cannot allocate hardware parameter structure " << "(" << snd_strerror(err) << ", " << err << ")" << "\n";
        return RecorderError::Error;
    }

    if ((err = snd_pcm_hw_params_any(capture_handle, hw_params)) < 0)
    {
        std::cerr << "cannot initialize hardware parameter structure " << "(" << snd_strerror(err) << ", " << err << ")" << "\n";
        return RecorderError::Error;
    }

    if ((err = snd_pcm_hw_params_set_access(capture_handle, hw_params,
                SND_PCM_ACCESS_RW_INTERLEAVED)) < 0)
    {
        std::cerr << "cannot set access type " << "(" << snd_strerror(err) << ", " << err << ")" << "\n";
        return RecorderError::Error;
    }

    if ((err = snd_pcm_hw_params_set_format(capture_handle, hw_params, SND_PCM_FORMAT_S16_LE)) < 0)
    {
        std::cerr << "cannot set sample format " << "(" << snd_strerror(err) << ", " << err << ")" << "\n";
        return RecorderError::Error;
    }

    if ((err = snd_pcm_hw_params_set_rate_near(capture_handle, hw_params,
                &srate, 0)) < 0)
    {
        std::cerr << "cannot set sample rate " << "(" << snd_strerror(err) << ", " << err << ")" << "\n";
        return RecorderError::Error;
    }

    if ((err = snd_pcm_hw_params_set_channels(capture_handle, hw_params, nchan))< 0)
    {
        std::cerr << "cannot set channel count " << "(" << snd_strerror(err) << ", " << err << ")" << "\n";
        return RecorderError::Error;
    }

    if ((err = snd_pcm_hw_params(capture_handle, hw_params)) < 0)
    {
        std::cerr << "cannot set parameters " << "(" << snd_strerror(err) << ", " << err << ")" << "\n";
        return RecorderError::Error;
    }

    if ((err = snd_pcm_prepare(capture_handle)) < 0)
    {
        std::cerr << "cannot prepare audio interface for use " << "(" << snd_strerror(err) << ", " << err << ")" << "\n";
        return RecorderError::Error;
    }

    if ((err = snd_pcm_start(capture_handle)) < 0)
    {
        std::cerr << "cannot start soundcard " << "(" << snd_strerror(err) << ", " << err << ")" << "\n";
        return RecorderError::Error;
    }

    return RecorderError::None;
}


RecorderError Recorder::record() {

    int ret = 0;
    char buf[1024 * 4];
    do {

        if ( (ret = snd_pcm_readi(capture_handle, buf, 1024)) != 1024) {
            
            std::cerr << "read from audio interface failed " << snd_strerror(ret) << "\n";

            if (ret == -32) {
                if ( (ret = snd_pcm_prepare(capture_handle)) < 0) {
                    std::cerr << "cannot prepare interface " << snd_strerror(ret) << "\n";
                    return Error;
                }
            } else {
                return Error;
            }
        }

        for (size_t i = 0; i < 1024; i++) {
            std::cout << buf[i];
        }
        std::cout << std::endl;

    } while(true);

}
