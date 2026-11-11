s16 SsUtAutoVol(s16 voice, s16 start_vol, s16 end_vol, s16 delta_time) {
    if (voice >= 0 && voice < NUM_SPU_CHANNELS) {
        SeAutoVol(voice, start_vol, end_vol, delta_time);
        return 0;
    }
    return -1;
}

s16 SsUtAutoPan(s16 voice, s16 start_pan, s16 end_pan, s16 delta_time) {
    if (voice >= 0 && voice < NUM_SPU_CHANNELS) {
        SeAutoPan(voice, start_pan, end_pan, delta_time);
        return 0;
    }
    return -1;
}
