#include <iostream>
#include <alsa/asoundlib.h>
#include "inc/recorder.hxx"
#include <csignal>

Recorder r;

void list_devices(snd_pcm_stream_t stream_type) {
    int card = -1;
    char *name = nullptr;

    // Iterate over sound cards
    if (snd_card_next(&card) < 0 || card < 0) {
        std::cerr << "No sound cards found." << std::endl;
        return;
    }

    std::cout << (stream_type == SND_PCM_STREAM_PLAYBACK ? "Playback" : "Capture") << " Devices:" << std::endl;

    while (card >= 0) {
        snd_ctl_t *ctl;
        snd_ctl_card_info_t *info;
        snd_pcm_info_t *pcminfo;
        char card_id[32];

        snprintf(card_id, sizeof(card_id), "hw:%d", card);

        if (snd_ctl_open(&ctl, card_id, 0) < 0) {
            snd_card_next(&card);
            continue;
        }

        snd_ctl_card_info_alloca(&info);
        snd_ctl_card_info(ctl, info);

        int dev = -1;
        while (snd_ctl_pcm_next_device(ctl, &dev) >= 0 && dev >= 0) {
            snd_pcm_info_alloca(&pcminfo);
            snd_pcm_info_set_device(pcminfo, dev);
            snd_pcm_info_set_subdevice(pcminfo, 0);
            snd_pcm_info_set_stream(pcminfo, stream_type);

            if (snd_ctl_pcm_info(ctl, pcminfo) >= 0) {
                std::cout << "  Card " << card << ", Device " << dev
                          << ": " << snd_pcm_info_get_name(pcminfo)
                          << " [" << snd_pcm_info_get_id(pcminfo) << "]"
                          << std::endl;
            }
        }

        snd_ctl_close(ctl);
        snd_card_next(&card);
    }
}

void intHandler(int dummy) {
   r.closeSoundCard();
}


int main() {

    signal(SIGINT, intHandler);


    list_devices(SND_PCM_STREAM_PLAYBACK);
    std::cout << std::endl;
    list_devices(SND_PCM_STREAM_CAPTURE);

    Recorder r; 
    r.setSoundDevice("hw:Device,0");

    r.init();
    r.record();
    return 0;
}
