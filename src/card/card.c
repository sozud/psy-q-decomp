void _new_card(void);
long _card_write(long, long, long);

long _card_clear(long chan) {
    _new_card();
    return _card_write(chan, 0x3F, 0);
}
