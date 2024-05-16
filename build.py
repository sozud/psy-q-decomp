import os
import subprocess
import ninja_syntax

def add_lib(srcs, output_dir, lib_name, flags, folder):
    for src in srcs:
        filename_without_extension = os.path.splitext(os.path.basename(src))[0]
        obj_name = f"{output_dir}/{filename_without_extension}.obj"
        ninja.build(
            obj_name, 
            'compile', 
            inputs=[src],
            variables={'FLAGS': flags, 'FOLDER': folder})

        # this doesn't generate a file output but ninja apparently needs an output name
        ninja.build(
            f"{obj_name}.check",
            'check',
            inputs=[obj_name],
            variables={'LIBRARY': lib_name}
        )

# original compiler missing so cobbling it together
def add_lib_263(srcs, output_dir, lib_name, flags, folder):
    for src in srcs:
        filename_without_extension = os.path.splitext(os.path.basename(src))[0]
        obj_name = f"{output_dir}/{filename_without_extension}.obj"

        cpp_flags = f"-undef -D__GNUC__=2 {flags} -v -D__OPTIMIZE__ -I./src/snd -I./include -lang-c -Dmips -D__mips__ -D__mips -Dpsx -D__psx__ -D__psx -D__EXTENSIONS__ -D_MIPSEL -D__CHAR_UNSIGNED__ -D_LANGUAGE_C -DLANGUAGE_C"

        # run c preprocessor
        ninja.build(
            f"{output_dir}/{filename_without_extension}.cpp",
            'cpp_263',
            inputs=[src],
            variables={'FLAGS': flags, 'FOLDER': folder, 'CPP_FLAGS': cpp_flags})
        
        # run cc1
        ninja.build(
            f"{output_dir}/{filename_without_extension}.s",
            'cc1_263',
            inputs=[f"{output_dir}/{filename_without_extension}.cpp"],
            variables={'FLAGS': flags, 'FOLDER': folder})
        
        # convert to dos
        ninja.build(
            f"{output_dir}/{filename_without_extension}.d",
            'unix2dos_263',
            inputs=[f"{output_dir}/{filename_without_extension}.s"],
            variables={'FLAGS': flags, 'FOLDER': folder})
        
        # run aspsx
        ninja.build(
            f"{output_dir}/{filename_without_extension}.obj",
            'aspsx_263',
            inputs=[f"{output_dir}/{filename_without_extension}.d"],
            variables={'FLAGS': flags, 'FOLDER': folder})
        
        # # check it
        # # this doesn't generate a file output but ninja apparently needs an output name
        ninja.build(
            f"{obj_name}.check",
            'check',
            inputs=[obj_name],
            variables={'LIBRARY': lib_name})

def add_lib_asmpsx(srcs, output_dir, lib_name, flags, folder):
    for src in srcs:
        filename_without_extension = os.path.splitext(os.path.basename(src))[0]
        obj_name = f"{output_dir}/{filename_without_extension}.obj"

        ninja.build(
            f"{output_dir}/{filename_without_extension}.d",
            'unix2dos_263',
            inputs=[src],
            variables={'FLAGS': flags, 'FOLDER': folder})
        
        # run aspsx
        ninja.build(
            f"{output_dir}/{filename_without_extension}.obj",
            'aspsx_263',
            inputs=[f"{output_dir}/{filename_without_extension}.d"],
            variables={'FLAGS': flags, 'FOLDER': folder})

        ninja.build(
            f"{obj_name}.check",
            'check',
            inputs=[obj_name],
            variables={'LIBRARY': lib_name})

ninja = ninja_syntax.Writer(open("build.ninja", "w"))

ninja.rule('compile',
           command='sh dosemu_wrapper.sh $in $out $FLAGS $FOLDER',
           description='Building $out from $in')

ninja.rule('cpp_263',
           command=f'cpp $CPP_FLAGS $in $out',
           description='Running preprocessor on $out from $in')

ninja.rule('cc1_263',
           command='./build/3.5/cc1-psx-26 -O2 -g0 -G0 -funsigned-char $in -o $out',
           description='Running cc1 on $out from $in')

ninja.rule('aspsx_263',
           command='sh aspsx_wrapper.sh $in $out dummy 3.5',
           description='Running aspsx on $out from $in')

ninja.rule('unix2dos_263',
           command='unix2dos -n $in $out',
           description='Running unix2dos on $out from $in')

ninja.rule('asmpsx',
           command='sh asmpsx_wrapper.sh -n $in $out',
           description='Running asmpsx on $out from $in')

ninja.rule(
        'check',
        command='./tools/psy-q-splitter/splitter/target/release/splitter diff_obj_with_lib $LIBRARY $in',
        description='Checking that $in matches')

def build_33():
    snd_srcs = [
        'src/snd/next.c',
        'src/snd/pause.c',
        'src/snd/play.c',
        'src/snd/replay.c',
        'src/snd/scsmvol.c',
        'src/snd/scssattr.c',
        'src/snd/scssvol.c',
        'src/snd/seskoff.c',
        'src/snd/seskon.c',
        'src/snd/sscall.c',
        'src/snd/ssclose.c',
        # 'src/snd/ssend.c', # doesn't exist
        # 'src/snd/ssinit_h.c', # doesn't exist
        'src/snd/ssplay.c',
        # 'src/snd/ssquit.c',
        # 'src/snd/sstable.c',
        # 'src/snd/sstick.c',
        # 'src/snd/ssvol.c',
        'src/snd/stop.c',
        # 'src/snd/tempo.c',
        # 'src/snd/ut_gpa.c',

        # 'src/snd/ut_gva.c',
        # 'src/snd/ut_rdel.c',
        # 'src/snd/ut_rdep.c',
        # 'src/snd/ut_rev.c',
        # 'src/snd/ut_rfb.c',
        # 'src/snd/ut_roff.c',
        # 'src/snd/ut_ron.c',
        # 'src/snd/ut_sva.c',
        'src/snd/vm_doff.c',
        'src/snd/vm_don.c',
        'src/snd/vm_g.c',
        # 'src/snd/vm_vsu.c',
        'src/snd/vs_mono.c',
        'src/snd/vs_srv.c',
        # 'src/snd/vs_vab.c',
        # 'src/snd/vs_vfb.c',
        # 'src/snd/vs_vtb.c',
        # 'src/snd/vs_vtbp.c',
        'src/snd/vs_vtc.c',
    ]

    add_lib_263(snd_srcs, "build/3.3/snd", "./psy-q/3.3/PSX/LIB/LIBSND.LIB", "-DVERSION=33", "3.3")

    spu_srcs = [
        'src/spu/s_cb.c',
        # 'src/spu/s_crwa.c',
        'src/spu/s_dcb.c',
        'src/spu/s_i.c',
        # 'src/spu/s_ini.c',
        'src/spu/s_it.c',
        'src/spu/s_m_f.c',
        'src/spu/s_m_init.c',
        'src/spu/s_m_util.c',
        'src/spu/s_r.c',
        'src/spu/s_sav.c',
        'src/spu/s_sic.c',
        # 'src/spu/s_sk.c',
        'src/spu/s_snv.c',
        'src/spu/s_sr.c',
        'src/spu/s_stm.c',
        'src/spu/s_stsa.c',
        'src/spu/s_w.c',
        'src/spu/s_wp.c',
        'src/spu/sr_gaks.c',
    ]

    add_lib_263(spu_srcs, "build/3.3/spu", "./psy-q/3.3/PSX/LIB/LIBSPU.LIB", "-DVERSION=33", "3.3")

def build_35():
    # snd_srcs = [
    #     'src/snd/adsr.c',
    #     'src/snd/next.c',
    #     'src/snd/pause.c',
    #     'src/snd/play.c',
    #     'src/snd/replay.c',
    #     'src/snd/scsmvol.c',
    #     'src/snd/scssattr.c',
    #     'src/snd/scssvol.c',
    #     'src/snd/seskoff.c',
    #     'src/snd/seskon.c',
    #     'src/snd/sscall.c',
    #     'src/snd/ssclose.c',
    #     'src/snd/ssend.c',
    #     'src/snd/ssinit_h.c',
    #     'src/snd/ssinit.c',
    #     'src/snd/ssplay.c',
    #     'src/snd/ssquit.c',
    #     'src/snd/ssstart.c',
    #     'src/snd/sstable.c',
    #     'src/snd/sstick.c',
    #     'src/snd/ssvol.c',
    #     'src/snd/stop.c',
    #     'src/snd/tempo.c',
    #     'src/snd/ut_gpa.c',
    #     'src/snd/ut_gva.c',
    #     'src/snd/ut_rdel.c',
    #     'src/snd/ut_rdep.c',
    #     'src/snd/ut_rev.c',
    #     'src/snd/ut_rfb.c',
    #     'src/snd/ut_roff.c',
    #     'src/snd/ut_ron.c',
    #     'src/snd/ut_sva.c',
    #     'src/snd/vm_doff.c',
    #     'src/snd/vm_don.c',
    #     'src/snd/vm_g.c',
    #     'src/snd/vm_vsu.c',
    #     'src/snd/vs_mono.c',
    #     'src/snd/vs_srv.c',
    #     'src/snd/vs_vab.c',
    #     'src/snd/vs_vfb.c',
    #     'src/snd/vs_vtb.c',
    #     'src/snd/vs_vtbp.c',
    #     'src/snd/vs_vtc.c',
    # ]

    # add_lib_263(snd_srcs, "build/3.5/snd", "./psy-q/3.5/PSX/LIB/LIBSND.LIB", "-DVERSION=35", "3.5")

    # spu_srcs = [
    #     'src/spu/s_cb.c',
    #     'src/spu/s_crwa.c',
    #     'src/spu/s_dcb.c',
    #     'src/spu/s_i.c',
    #     'src/spu/s_ini.c',
    #     'src/spu/s_it.c',
    #     'src/spu/s_m_f.c',
    #     'src/spu/s_m_init.c',
    #     'src/spu/s_m_util.c',
    #     'src/spu/s_r.c',
    #     'src/spu/s_sav.c',
    #     'src/spu/s_sic.c',
    #     'src/spu/s_sk.c',
    #     'src/spu/s_snv.c',
    #     'src/spu/s_sr.c',
    #     'src/spu/s_stm.c',
    #     'src/spu/s_stsa.c',
    #     'src/spu/s_w.c',
    #     'src/spu/s_wp.c',
    #     'src/spu/sr_gaks.c',
    # ]

    # add_lib_263(spu_srcs, "build/3.5/spu", "./psy-q/3.5/PSX/LIB/LIBSPU.LIB", "-DVERSION=35", "3.5")

    # etc_srcs = [
    #     'src/etc/intr_dma.c',
    #     'src/etc/intr_vb.c',
    #     'src/etc/pad.c',
    #     'src/etc/vmode.c',
    # ]

    # add_lib_263(etc_srcs, "build/3.5/etc", "./psy-q/3.5/PSX/LIB/LIBETC.LIB", "-DVERSION=35", "3.5")

    # gs_srcs = [
    #     'src/gs/gs_007.c'
    # ]

    # add_lib_263(gs_srcs, "build/3.5/gs", "./psy-q/3.5/PSX/LIB/LIBGS.LIB", "-DVERSION=35", "3.5")



    # add_lib_263(api_srcs, "build/3.5/gs", "./psy-q/3.5/PSX/LIB/LIBAPI.LIB", "-DVERSION=35", "3.5")

    api_srcs = [
        'src/api/a07.s'
    ] 

    add_lib_asmpsx(api_srcs, "build/3.5/api", "./psy-q/3.5/PSX/LIB/LIBAPI.LIB", "-DVERSION=35", "3.5")



def build_36():
    snd_srcs = [
        # 'src/snd/next.c',
        # 'src/snd/pause.c',
        'src/snd/play.c',
        # 'src/snd/replay.c',
        # 'src/snd/scsmvol.c',
        # 'src/snd/scssattr.c',
        # 'src/snd/scssvol.c',
        # 'src/snd/seskoff.c',
        # 'src/snd/seskon.c',
        # 'src/snd/sscall.c',
        # 'src/snd/ssclose.c',
        'src/snd/ssend.c',
        'src/snd/ssinit_h.c',
        # 'src/snd/ssplay.c',
        'src/snd/ssquit.c',
        # 'src/snd/sstable.c',
        'src/snd/sstick.c',
        'src/snd/ssvol.c',
        # 'src/snd/tempo.c',
        # 'src/snd/ut_gpa.c',
        # 'src/snd/ut_gva.c',
        'src/snd/ut_rdel.c',
        # 'src/snd/ut_rdep.c',
        'src/snd/ut_rev.c',
        'src/snd/ut_rfb.c',
        'src/snd/ut_roff.c',
        'src/snd/ut_ron.c',
        # 'src/snd/ut_sva.c',
        'src/snd/vm_doff.c',
        'src/snd/vm_don.c',
        # 'src/snd/vm_g.c',
        # 'src/snd/vm_vsu.c',
        'src/snd/vs_mono.c',
        # 'src/snd/vs_srv.c',
        # 'src/snd/vs_vab.c',
        'src/snd/vs_vfb.c',
        'src/snd/vs_vtb.c',
        # 'src/snd/vs_vtbp.c',
        # 'src/snd/vs_vtc.c',

        # 'src/3.6/snd/ssvkoff.c',
        # 'src/3.6/snd/ssvkon.c',
    ]

    add_lib(snd_srcs, "build/3.6/snd", "./psy-q/3.6/PSX/LIB/LIBSND.LIB", "-DVERSION=36", "3.6")

    spu_srcs = [
        'src/spu/s_cb.c',
        'src/spu/s_crwa.c',
        'src/spu/s_dcb.c',
        'src/spu/s_i.c',
        # 'src/spu/s_ini.c',
        # 'src/spu/s_it.c',
        # 'src/spu/s_m_f.c',
        'src/spu/s_m_init.c',
        # 'src/spu/s_m_util.c',
        # 'src/spu/s_r.c',
        # 'src/spu/s_sav.c',
        'src/spu/s_sic.c',
        # 'src/spu/s_sk.c',
        'src/spu/s_snv.c',
        'src/spu/s_sr.c',
        'src/spu/s_stm.c',
        # 'src/spu/s_stsa.c',
        # 'src/spu/s_w.c',
        # 'src/spu/s_wp.c',
        # 'src/spu/sr_gaks.c',
    ]

    add_lib(spu_srcs, "build/3.6/spu", "./psy-q/3.6/PSX/LIB/LIBSPU.LIB", "-DVERSION=36", "3.6")

# build_33()
build_35()
# build_36()


ninja.close()
