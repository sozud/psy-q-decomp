#include <libsnd.h>

void do_libsnd()
{
    SndVolume m_vol;
    short voll, volr;
    SsMarkCallbackProc proc;
    VabHdr* vabhdr; 
    ProgAtr* progatr;
    VagAtr* vagatr;

    SsVabOpen (0, vabhdr);
    SsVabClose (0);          
    SsVabTransCompleted (0);
    SsVabOpenHead (0, 0);
    SsVabTransBody (0, 0);
    SsVabTransBodyPartly (0, 0, 0);
    SsVabOpenHeadSticky (0, 0, 0);
    SsVabTransfer (0, 0, 0, 0);

    SsInit ();
    SsInitHot ();
    SsQuit ();
    SsSetTableSize (0, 0, 0);
    SsSetTickMode (0);
    SsSetTickCallback ( proc);
    SsStart ();
    SsStart2 ();
    SsEnd ();

    SsSeqCalledTbyT ();           
    SsSeqOpen (0, 0); 
    SsSeqPlay (0, 0, 0);    
    SsSeqPause (0);                    
    SsSeqReplay (0);                  
    SsSeqStop (0);                   
    SsSeqSetVol (0, 0, 0);  
    SsSeqSetNext (0, 0);             
    SsSeqSetCrescendo (0, 0, 0); 
    SsSeqSetDecrescendo (0, 0, 0); 
    SsSeqSetRitardando (0, 0, 0);   
    SsSeqSetAccelerando (0, 0, 0);  
    SsSeqClose (0);                       

    SsSepOpen (0, 0, 0);
    SsSepPlay (0, 0, 0, 0);
    SsSepPause (0, 0);
    SsSepReplay (0, 0);
    SsSepStop (0, 0);
    SsSepSetVol (0, 0, 0, 0);
    SsSepSetCrescendo (0, 0, 0, 0);
    SsSepSetDecrescendo (0, 0, 0, 0);
    SsSepSetRitardando (0, 0, 0, 0);   
    SsSepSetAccelerando (0, 0, 0, 0); 
    SsSepClose (0);

    SsVoKeyOn (0, 0, 0, 0);
    SsVoKeyOff (0, 0);                
    SsSetMVol (0, 0);              
    SsGetMVol (&m_vol);             
    SsSetRVol (0, 0);             
    SsGetRVol (&m_vol);              
    SsSetMute (0);                        
    SsSetSerialAttr (0, 0, 0);     
    SsGetSerialAttr (0, 0);           
    SsSetSerialVol (0, 0, 0);    
    SsGetSerialVol (0, &m_vol); 
    SsSetNck (0);                         
    SsGetNck ();                          
    SsSetNoiseOn (0, 0);    
    SsSetNoiseOff ();                     
    SsSetMono ();                         
    SsSetStereo ();                       

    SsSetTempo (0, 0, 0);
    SsSetLoop (0, 0, 0);
    SsIsEos (0, 0);
    SsPlayBack (0, 0, 0);
    SsSetMarkCallback (0, 0, proc);
    SsSetReservedVoice (0);

    SsUtKeyOn (0, 0, 0, 0, 0, 0, 0);
    SsUtKeyOff (0, 0, 0, 0, 0);

    SsUtKeyOnV (0 , 0 , 0 , 0 , 0 , 0 , 0 , 0);
    SsUtKeyOffV (0);

    SsUtPitchBend (0, 0, 0, 0, 0);
    SsUtChangePitch (0, 0, 0, 0, 0,
                                0, 0);
    SsUtChangeADSR (0, 0, 0, 0, 
                                0, 0);
    SsUtSetVabHdr (0, vabhdr);
    SsUtGetVabHdr (0, vabhdr);
    SsUtSetProgAtr (0, 0, 0);
    SsUtGetProgAtr (0, 0, 0);
    SsUtSetVagAtr (0, 0, 0, vagatr);
    SsUtGetVagAtr (0, 0, 0, vagatr);
    SsUtSetDetVVol (0, 0, 0);
    SsUtGetDetVVol (0, 0, 0);
    SsUtSetVVol (0, 0, 0);
    SsUtGetVVol (0, 0, 0);
    SsUtAutoVol (0, 0, 0, 0);
    SsUtAutoPan (0, 0, 0, 0);
    SsUtReverbOn ();
    SsUtReverbOff ();
    SsUtSetReverbType (0);
    SsUtGetReverbType ();
    SsUtSetReverbDepth (0, 0);
    SsUtSetReverbFeedback (0);
    SsUtSetReverbDelay (0);
    SsUtAllKeyOff (0);

    SsSetAutoKeyOffMode( 0 );
    SsUtFlush(  );
    SsUtVibrateOn( 0 , 0 , 0  );
    SsUtVibrateOff( 0  );

    SsVabFakeHead (0, 0, 0);
    SsVabFakeBody (0);
    SsUtGetVBaddrInSB (0);
    SsUtGetVagAddr( 0, 0 );
    SsUtGetVagAddrFromTone(0, 0, 0);

    SsSetNext (0, 0, 0, 0);             
    SsSeqGetVol (0, 0, 0, 0);
}

int main()
{
    do_libsnd();
}
