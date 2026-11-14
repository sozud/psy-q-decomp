import os
import subprocess
import glob
import ninja_syntax

progress_objects = {"3.3": [], "3.5": [], "3.6": [], "3.7": [], "4.0": [], "4.3": []}

def track_progress_object(obj_name):
    parts = obj_name.split('/')
    if len(parts) > 2 and parts[0] == 'build' and parts[1] in progress_objects:
        progress_objects[parts[1]].append(obj_name)

def add_lib(srcs, output_dir, lib_name, flags, folder):
    for src in srcs:
        filename_without_extension = os.path.splitext(os.path.basename(src))[0]
        obj_name = f"{output_dir}/{filename_without_extension}.obj"
        track_progress_object(obj_name)
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
def add_lib_263(srcs, output_dir, lib_name, flags, folder, aspsx="3.5"):
    for src in srcs:
        filename_without_extension = os.path.splitext(os.path.basename(src))[0]
        obj_name = f"{output_dir}/{filename_without_extension}.obj"
        track_progress_object(obj_name)

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
            variables={'FLAGS': flags, 'FOLDER': folder, 'ASPSX': aspsx})

        # # check it
        # # this doesn't generate a file output but ninja apparently needs an output name
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
           command='bash daze_wrapper.sh aspsx $in $out -G0 $ASPSX',
           description='Running aspsx on $out from $in')

ninja.rule('unix2dos_263',
           command='unix2dos -n $in $out',
           description='Running unix2dos on $out from $in')

ninja.rule(
        'check',
        command='./tools/psy-q-splitter/splitter/target/release/splitter diff_obj_with_lib $LIBRARY $in',
        description='Checking that $in matches')



def add_lib_wibo(srcs, output_dir, lib_name, flags, folder):
    for src in srcs:
        filename_without_extension = os.path.splitext(os.path.basename(src))[0]
        obj_name = f"{output_dir}/{filename_without_extension}.obj"
        track_progress_object(obj_name)
        ninja.build(
            obj_name,
            'compile_wibo',
            inputs=[src],
            variables={'FLAGS': flags, 'FOLDER': folder})

        # this doesn't generate a file output but ninja apparently needs an output name
        ninja.build(
            f"{obj_name}.check",
            'check',
            inputs=[obj_name],
            variables={'LIBRARY': lib_name}
        )

ninja.rule('compile_wibo',
           command='bash wibo_ccpsx_wrapper.sh $in $out $FLAGS',
           description='Building $out from $in')

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
        'src/snd/ssvol.c',
        'src/snd/stop.c',
        # 'src/snd/tempo.c',
        # 'src/snd/ut_gpa.c',

        # 'src/snd/ut_gva.c',
        # 'src/snd/ut_rdel.c',
        # 'src/snd/ut_rdep.c',
        'src/snd/ut_rev.c',
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
        'src/spu/s_itc.c',
        'src/spu/s_m_f.c',
        'src/spu/s_m_init.c',
        'src/spu/s_m_int.c',
        'src/spu/s_m_m.c',
        'src/spu/s_m_util.c',
        'src/spu/s_r.c',
        'src/spu/s_sav.c',
        'src/spu/s_si.c',
        'src/spu/s_sic.c',
        'src/spu/s_sk.c',
        'src/spu/s_snv.c',
        'src/spu/s_sr.c',
        'src/spu/s_stm.c',
        'src/spu/s_stsa.c',
        'src/spu/s_w.c',
        'src/spu/s_wp.c',
        'src/spu/sr_gaks.c',
    ]

    add_lib_263(spu_srcs, "build/3.3/spu", "./psy-q/3.3/PSX/LIB/LIBSPU.LIB", "-DVERSION=33", "3.3")

    add_lib_263(['src/press/libpress.c'], "build/3.3/press", "./psy-q/3.3/PSX/LIB/LIBPRESS.LIB", "-DVERSION=33", "3.3", aspsx="aspsx221")

def build_35():
    snd_srcs = [
        'src/snd/adsr.c',
        'src/snd/cres.c',
        'src/snd/decre.c',
        'src/snd/next.c',
        'src/snd/pause.c',
        'src/snd/play.c',
        'src/snd/replay.c',
        'src/snd/scsmvol.c',
        'src/snd/scssattr.c',
        'src/snd/scssvol.c',
        'src/snd/seqinit.c',
        'src/snd/seqread.c',
        'src/snd/seskoff.c',
        'src/snd/seskon.c',
        'src/snd/sscall.c',
        'src/snd/ssclose.c',
        'src/snd/ssend.c',
        'src/snd/ssinit_h.c',
        'src/snd/ssinit.c',
        'src/snd/ssplay.c',
        'src/snd/ssquit.c',
        'src/snd/ssstart.c',
        'src/snd/sstable.c',
        'src/snd/sstick.c',
        'src/snd/ssvol.c',
        'src/snd/stop.c',
        'src/snd/tempo.c',
        'src/snd/ut_gpa.c',
        'src/snd/ut_gva.c',
        'src/snd/ut_rdel.c',
        'src/snd/ut_rdep.c',
        'src/snd/ut_rev.c',
        'src/snd/ut_rfb.c',
        'src/snd/ut_roff.c',
        'src/snd/ut_ron.c',
        'src/snd/ut_sva.c',
        'src/snd/vmanager.c',
        'src/snd/vm_doff.c',
        'src/snd/vm_don.c',
        'src/snd/vm_g.c',
        'src/snd/vm_vsu.c',
        'src/snd/vs_mono.c',
        'src/snd/vs_srv.c',
        'src/snd/vs_vab.c',
        'src/snd/vs_vfb.c',
        'src/snd/vs_vh.c',
        'src/snd/vs_vtb.c',
        'src/snd/vs_vtbp.c',
        'src/snd/vs_vtc.c',
    ]

    add_lib_263(snd_srcs, "build/3.5/snd", "./psy-q/3.5/PSX/LIB/LIBSND.LIB", "-DVERSION=35", "3.5")

    spu_srcs = [
        'src/spu/s_cb.c',
        'src/spu/s_crwa.c',
        'src/spu/s_dcb.c',
        'src/spu/s_gva.c',
        'src/spu/s_i.c',
        'src/spu/s_ini.c',
        'src/spu/s_it.c',
        'src/spu/s_itc.c',
        'src/spu/s_m_f.c',
        'src/spu/s_m_init.c',
        'src/spu/s_m_int.c',
        'src/spu/s_m_m.c',
        'src/spu/s_m_util.c',
        'src/spu/s_n2p.c',
        'src/spu/s_r.c',
        'src/spu/s_sav.c',
        'src/spu/s_sca.c',
        'src/spu/s_si.c',
        'src/spu/s_sic.c',
        'src/spu/s_sk.c',
        'src/spu/s_snv.c',
        'src/spu/s_sr.c',
        'src/spu/s_srmp.c',
        'src/spu/s_stm.c',
        'src/spu/s_stsa.c',
        'src/spu/s_sva.c',
        'src/spu/s_w.c',
        'src/spu/s_wp.c',
        'src/spu/spu.c',
        'src/spu/sr_gaks.c',
    ]

    add_lib_263(spu_srcs, "build/3.5/spu", "./psy-q/3.5/PSX/LIB/LIBSPU.LIB", "-DVERSION=35", "3.5")

    etc_srcs = [
        'src/etc/intr_dma.c',
        'src/etc/intr_vb.c',
        'src/etc/pad.c',
        'src/etc/vmode.c',
        'src/etc/intr.c',
        'src/etc/vsync.c'
    ]

    add_lib_263(etc_srcs, "build/3.5/etc", "./psy-q/3.5/PSX/LIB/LIBETC.LIB", "-DVERSION=35", "3.5")

    gs_srcs = [
        'src/gs/gs_007.c',
        'src/gs/gs_008.c',
        'src/gs/gs_009.c'
    ]

    add_lib_263(gs_srcs, "build/3.5/gs", "./psy-q/3.5/PSX/LIB/LIBGS.LIB", "-DVERSION=35", "3.5")

    api_srcs = [
        'src/api/counter.c'
    ]

    add_lib_263(api_srcs, "build/3.5/api", "./psy-q/3.5/PSX/LIB/LIBAPI.LIB", "-DVERSION=35", "3.5")

    c_srcs = [
        'src/c/memmove.c',
        'src/c/sprintf.c',
    ]

    add_lib_263(c_srcs, "build/3.5/c", "./psy-q/3.5/PSX/LIB/LIBC.LIB", "-DVERSION=35", "3.5")

    c_srcs = [
        'src/cd/bios.c',
        'src/cd/c_002.c',
        'src/cd/c_003.c',
        'src/cd/c_004.c',
        'src/cd/c_005.c',
        'src/cd/c_007.c',
        'src/cd/c_008.c',
        'src/cd/c_009.c',
        'src/cd/c_010.c',
        'src/cd/c_011.c',
        'src/cd/cdread.c',
        'src/cd/cdread2.c',
        'src/cd/cdrom.c',
        'src/cd/event.c',
        'src/cd/iso9660.c',
        'src/cd/sys.c',
    ]

    add_lib_263(c_srcs, "build/3.5/cd", "./psy-q/3.5/PSX/LIB/LIBCD.LIB", "-DVERSION=35", "3.5")

    c_srcs = [
        'src/press/libpress.c',
    ]

    add_lib_263(c_srcs, "build/3.5/press", "./psy-q/3.5/PSX/LIB/LIBPRESS.LIB", "-DVERSION=35", "3.5")

    c2_srcs = [
        'src/c/sprintf.c',
    ]

    add_lib_263(c2_srcs, "build/3.5/c2", "./psy-q/3.5/PSX/LIB/LIBC2.LIB", "-DVERSION=35", "3.5")

    card_srcs = [
        'src/card/card.c',
    ]

    add_lib_263(card_srcs, "build/3.5/card", "./psy-q/3.5/PSX/LIB/LIBCARD.LIB", "-DVERSION=35", "3.5")

    gpu_srcs = [
        'src/gpu/ext.c',
        'src/gpu/font.c',
        'src/gpu/prim.c',
        'src/gpu/sys.c',
    ]

    add_lib_263(gpu_srcs, "build/3.5/gpu", "./psy-q/3.5/PSX/LIB/LIBGPU.LIB", "-DVERSION=35", "3.5")

    gte_srcs = [
        'src/gte/fog_01.c',
        'src/gte/geo_00.c',
        'src/gte/geo_01.c',
        'src/gte/ratan.c',
    ]

    add_lib_263(gte_srcs, "build/3.5/gte", "./psy-q/3.5/PSX/LIB/LIBGTE.LIB", "-DVERSION=35", "3.5")

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
        'src/spu/s_itc.c',
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

    snd_srcs = [
        'src/snd/cres.c',
        'src/snd/next.c',
        'src/snd/pause.c',
        'src/snd/replay.c',
        'src/snd/seqinit.c',
        'src/snd/sscall.c',
        'src/snd/ssclose.c',
        'src/snd/ssinit.c',
        'src/snd/ssplay.c',
        'src/snd/ssstart.c',
        'src/snd/sstable.c',
        'src/snd/tempo.c',
        'src/snd/ut_gpa.c',
        'src/snd/ut_gva.c',
        'src/snd/ut_sva.c',
        'src/snd/vm_vsu.c',
        'src/snd/vs_srv.c',
        'src/snd/vs_vab.c',
        'src/snd/vs_vtbp.c',
    ]

    add_lib_263(snd_srcs, "build/3.6/snd", "./psy-q/3.6/PSX/LIB/LIBSND.LIB", "-DVERSION=36", "3.6")

    snd_srcs = [
        'src/snd/vs_vh.c',
    ]

    add_lib_263(snd_srcs, "build/3.6/snd", "./psy-q/3.6/PSX/LIB/LIBSND.LIB", "-DVERSION=35", "3.6")

    spu_srcs = [
        'src/spu/s_gva.c',
        'src/spu/s_it.c',
        'src/spu/s_m_int.c',
        'src/spu/s_m_m.c',
        'src/spu/s_n2p.c',
        'src/spu/s_r.c',
        'src/spu/s_srmp.c',
        'src/spu/s_w.c',
        'src/spu/s_wp.c',
        'src/spu/sr_gaks.c',
    ]

    add_lib_263(spu_srcs, "build/3.6/spu", "./psy-q/3.6/PSX/LIB/LIBSPU.LIB", "-DVERSION=36", "3.6")

    api_srcs = [
        'src/api/counter.c',
    ]

    add_lib_263(api_srcs, "build/3.6/api", "./psy-q/3.6/PSX/LIB/LIBAPI.LIB", "-DVERSION=36", "3.6")

    c_srcs = [
        'src/c/sprintf.c',
    ]

    add_lib_263(c_srcs, "build/3.6/c", "./psy-q/3.6/PSX/LIB/LIBC.LIB", "-DVERSION=36", "3.6")

    c2_srcs = [
        'src/c/sprintf.c',
    ]

    add_lib_263(c2_srcs, "build/3.6/c2", "./psy-q/3.6/PSX/LIB/LIBC2.LIB", "-DVERSION=36", "3.6")

    card_srcs = [
        'src/card/card.c',
    ]

    add_lib_263(card_srcs, "build/3.6/card", "./psy-q/3.6/PSX/LIB/LIBCARD.LIB", "-DVERSION=36", "3.6")

    cd_srcs = [
        'src/cd/bios.c',
        'src/cd/c_002.c',
        'src/cd/c_003.c',
        'src/cd/c_004.c',
        'src/cd/c_005.c',
        'src/cd/c_007.c',
        'src/cd/c_008.c',
        'src/cd/c_009.c',
        'src/cd/c_010.c',
        'src/cd/c_011.c',
        'src/cd/cdread.c',
        'src/cd/cdread2.c',
        'src/cd/cdrom.c',
        'src/cd/event.c',
        'src/cd/iso9660.c',
        'src/cd/sys.c',
    ]

    add_lib_263(cd_srcs, "build/3.6/cd", "./psy-q/3.6/PSX/LIB/LIBCD.LIB", "-DVERSION=36", "3.6")

    etc_srcs = [
        'src/etc/intr_dma.c',
        'src/etc/intr_vb.c',
        'src/etc/pad.c',
        'src/etc/vmode.c',
        'src/etc/intr.c',
        'src/etc/vsync.c',
    ]

    add_lib_263(etc_srcs, "build/3.6/etc", "./psy-q/3.6/PSX/LIB/LIBETC.LIB", "-DVERSION=36", "3.6")

    gpu_srcs = [
        'src/gpu/ext.c',
        'src/gpu/font.c',
        'src/gpu/prim.c',
        'src/gpu/sys.c',
    ]

    add_lib_263(gpu_srcs, "build/3.6/gpu", "./psy-q/3.6/PSX/LIB/LIBGPU.LIB", "-DVERSION=36", "3.6")

    gs_srcs = [
        'src/gs/gs_007.c',
        'src/gs/gs_008.c',
        'src/gs/gs_009.c',
    ]

    add_lib_263(gs_srcs, "build/3.6/gs", "./psy-q/3.6/PSX/LIB/LIBGS.LIB", "-DVERSION=36", "3.6")

    gte_srcs = [
        'src/gte/fog_01.c',
        'src/gte/geo_00.c',
        'src/gte/geo_01.c',
        'src/gte/ratan.c',
    ]

    add_lib_263(gte_srcs, "build/3.6/gte", "./psy-q/3.6/PSX/LIB/LIBGTE.LIB", "-DVERSION=36", "3.6")

def build_40():
    spu_srcs = [
        'src/spu/s_cb.c',
        'src/spu/s_crwa.c',
        'src/spu/s_dcb.c',
        'src/spu/s_f.c',
        'src/spu/s_gav.c',
        'src/spu/s_gca.c',
        'src/spu/s_gcmv.c',
        'src/spu/s_gcmva.c',
        'src/spu/s_gcmvx.c',
        'src/spu/s_gi.c',
        'src/spu/s_gia.c',
        'src/spu/s_gks.c',
        'src/spu/s_gm.c',
        'src/spu/s_gnc.c',
        'src/spu/s_gnv.c',
        'src/spu/s_gplv.c',
        'src/spu/s_gr.c',
        'src/spu/s_grmd.c',
        'src/spu/s_grmdt.c',
        'src/spu/s_grmfb.c',
        'src/spu/s_grmp.c',
        'src/spu/s_grmt.c',
        'src/spu/s_grv.c',
        'src/spu/s_gtm.c',
        'src/spu/s_gtsa.c',
        'src/spu/s_gva.c',
        'src/spu/s_gvad.c',
        'src/spu/s_gvada.c',
        'src/spu/s_gvar.c',
        'src/spu/s_gvara.c',
        'src/spu/s_gvdr.c',
        'src/spu/s_gvea.c',
        'src/spu/s_gvex.c',
        'src/spu/s_gvlsa.c',
        'src/spu/s_gvn.c',
        'src/spu/s_gvp.c',
        'src/spu/s_gvrr.c',
        'src/spu/s_gvrra.c',
        'src/spu/s_gvsa.c',
        'src/spu/s_gvsl.c',
        'src/spu/s_gvsn.c',
        'src/spu/s_gvsr.c',
        'src/spu/s_gvsra.c',
        'src/spu/s_gvv.c',
        'src/spu/s_gvva.c',
        'src/spu/s_gvvx.c',
        'src/spu/s_i.c',
        'src/spu/s_ih.c',
        'src/spu/s_ini.c',
        'src/spu/s_irwar.c',
        'src/spu/s_it.c',
        'src/spu/s_itc.c',
        'src/spu/s_m.c',
        'src/spu/s_m_f.c',
        'src/spu/s_m_init.c',
        'src/spu/s_m_int.c',
        'src/spu/s_m_m.c',
        'src/spu/s_m_util.c',
        'src/spu/s_m_wsa.c',
        'src/spu/s_m_x.c',
        'src/spu/s_n2p.c',
        'src/spu/s_q.c',
        'src/spu/s_r.c',
        'src/spu/s_rdd.c',
        'src/spu/s_rmp.c',
        'src/spu/s_rrwa.c',
        'src/spu/s_sav.c',
        'src/spu/s_sca.c',
        'src/spu/s_sccv.c',
        'src/spu/s_scmva.c',
        'src/spu/s_si.c',
        'src/spu/s_sia.c',
        'src/spu/s_sic.c',
        'src/spu/s_sk.c',
        'src/spu/s_skowa.c',
        'src/spu/s_sm.c',
        'src/spu/s_snc.c',
        'src/spu/s_snv.c',
        'src/spu/s_splv.c',
        'src/spu/s_sr.c',
        'src/spu/s_sra.c',
        'src/spu/s_srd.c',
        'src/spu/s_srmd.c',
        'src/spu/s_srmdt.c',
        'src/spu/s_srmfb.c',
        'src/spu/s_srmp.c',
        'src/spu/s_srmt.c',
        'src/spu/s_srv.c',
        'src/spu/s_stc.c',
        'src/spu/s_stm.c',
        'src/spu/s_stsa.c',
        'src/spu/s_sva.c',
        'src/spu/s_svad.c',
        'src/spu/s_svar.c',
        'src/spu/s_svdr.c',
        'src/spu/s_svlsa.c',
        'src/spu/s_svn.c',
        'src/spu/s_svp.c',
        'src/spu/s_svrr.c',
        'src/spu/s_svsa.c',
        'src/spu/s_svsl.c',
        'src/spu/s_svsn.c',
        'src/spu/s_svsr.c',
        'src/spu/s_svv.c',
        'src/spu/s_w.c',
        'src/spu/s_w0.c',
        'src/spu/s_wp.c',
        'src/spu/spu.c',
        'src/spu/sr_gaks.c',
    ]

    add_lib_wibo(spu_srcs, "build/4.0/spu", "./psy-q/4.0/PSX/LIB/LIBSPU.LIB", "-O3 -g0 -G0 -funsigned-char -c -I./src/snd -I./include -DVERSION=40", "4.0")

    snd_srcs = [
        'src/snd/cres.c',
        'src/snd/next.c',
        'src/snd/pause.c',
        'src/snd/play.c',
        'src/snd/replay.c',
        'src/snd/seqinit.c',
        'src/snd/sscall.c',
        'src/snd/ssclose.c',
        'src/snd/ssend.c',
        'src/snd/ssinit_h.c',
        'src/snd/ssinit.c',
        'src/snd/ssplay.c',
        'src/snd/ssstart.c',
        'src/snd/sstable.c',
        'src/snd/sstick.c',
        'src/snd/ssvol.c',
        'src/snd/tempo.c',
        'src/snd/ut_gpa.c',
        'src/snd/ut_gva.c',
        'src/snd/ut_rev.c',
        'src/snd/ut_sva.c',
        'src/snd/vm_doff.c',
        'src/snd/vm_don.c',
        'src/snd/vm_vsu.c',
        'src/snd/vs_srv.c',
        'src/snd/vs_vab.c',
        'src/snd/vs_vfb.c',
        'src/snd/vs_vh.c',
        'src/snd/vs_vtb.c',
        'src/snd/vs_vtbp.c',
    ]

    add_lib_wibo(snd_srcs, "build/4.0/snd", "./psy-q/4.0/PSX/LIB/LIBSND.LIB", "-O3 -g0 -G0 -funsigned-char -c -I./src/snd -I./include -DVERSION=40", "4.0")

    api_srcs = [
        'src/api/counter.c',
    ]

    add_lib_wibo(api_srcs, "build/4.0/api", "./psy-q/4.0/PSX/LIB/LIBAPI.LIB", "-O3 -g0 -G0 -funsigned-char -c -I./src/snd -I./include -DVERSION=40", "4.0")

    c_srcs = [
        'src/c/sprintf.c',
    ]

    add_lib_wibo(c_srcs, "build/4.0/c", "./psy-q/4.0/PSX/LIB/LIBC.LIB", "-O3 -g0 -G0 -funsigned-char -c -I./src/snd -I./include -DVERSION=40", "4.0")

    c2_srcs = [
        'src/c/sprintf.c',
    ]

    add_lib_wibo(c2_srcs, "build/4.0/c2", "./psy-q/4.0/PSX/LIB/LIBC2.LIB", "-O3 -g0 -G0 -funsigned-char -c -I./src/snd -I./include -DVERSION=40", "4.0")

    card_srcs = [
        'src/card/card.c',
    ]

    add_lib_wibo(card_srcs, "build/4.0/card", "./psy-q/4.0/PSX/LIB/LIBCARD.LIB", "-O3 -g0 -G0 -funsigned-char -c -I./src/snd -I./include -DVERSION=40", "4.0")

    cd_srcs = [
        'src/cd/bios.c',
        'src/cd/c_002.c',
        'src/cd/c_003.c',
        'src/cd/c_004.c',
        'src/cd/c_005.c',
        'src/cd/c_007.c',
        'src/cd/c_008.c',
        'src/cd/c_009.c',
        'src/cd/c_010.c',
        'src/cd/c_011.c',
        'src/cd/cdread.c',
        'src/cd/cdread2.c',
        'src/cd/cdrom.c',
        'src/cd/event.c',
        'src/cd/iso9660.c',
        'src/cd/sys.c',
    ]

    add_lib_wibo(cd_srcs, "build/4.0/cd", "./psy-q/4.0/PSX/LIB/LIBCD.LIB", "-O2 -g0 -G0 -funsigned-char -c -I./src/snd -I./include -DVERSION=40", "4.0")

    etc_srcs = [
        'src/etc/intr_dma.c',
        'src/etc/intr_vb.c',
        'src/etc/pad.c',
        'src/etc/vmode.c',
        'src/etc/intr.c',
        'src/etc/vsync.c',
    ]

    add_lib_wibo(etc_srcs, "build/4.0/etc", "./psy-q/4.0/PSX/LIB/LIBETC.LIB", "-O2 -g0 -G0 -funsigned-char -c -I./src/snd -I./include -DVERSION=40", "4.0")

    gpu_srcs = [
        'src/gpu/ext.c',
        'src/gpu/font.c',
        'src/gpu/prim.c',
        'src/gpu/sys.c',
    ]

    add_lib_wibo(gpu_srcs, "build/4.0/gpu", "./psy-q/4.0/PSX/LIB/LIBGPU.LIB", "-O3 -g0 -G0 -funsigned-char -c -I./src/snd -I./include -DVERSION=40", "4.0")

    gs_srcs = [
        'src/gs/gs_007.c',
        'src/gs/gs_008.c',
        'src/gs/gs_009.c',
    ]

    add_lib_wibo(gs_srcs, "build/4.0/gs", "./psy-q/4.0/PSX/LIB/LIBGS.LIB", "-O3 -g0 -G0 -funsigned-char -c -I./src/snd -I./include -DVERSION=40", "4.0")

    gte_srcs = [
        'src/gte/fog_01.c',
        'src/gte/geo_00.c',
        'src/gte/geo_01.c',
        'src/gte/ratan.c',
    ]

    add_lib_wibo(gte_srcs, "build/4.0/gte", "./psy-q/4.0/PSX/LIB/LIBGTE.LIB", "-O3 -g0 -G0 -funsigned-char -c -I./src/snd -I./include -DVERSION=40", "4.0")

def build_37():
    api37_srcs = [
        'src/api/pad.c',
    ]
    add_lib_wibo(api37_srcs, "build/3.7/api", "./psy-q/3.7/PSX/LIB/LIBAPI.LIB", "-O3 -g0 -G0 -funsigned-char -c -I./src/snd -I./include -DVERSION=37", "3.7")

    api40_srcs = [
        'src/api/counter.c',
    ]
    add_lib_wibo(api40_srcs, "build/3.7/api", "./psy-q/3.7/PSX/LIB/LIBAPI.LIB", "-O3 -g0 -G0 -funsigned-char -c -I./src/snd -I./include -DVERSION=40", "3.7")

    card37_srcs = [
        'src/card/init.c',
    ]
    add_lib_wibo(card37_srcs, "build/3.7/card", "./psy-q/3.7/PSX/LIB/LIBCARD.LIB", "-O3 -g0 -G0 -funsigned-char -c -I./src/snd -I./include -DVERSION=37", "3.7")

    card40_srcs = [
        'src/card/card.c',
    ]
    add_lib_wibo(card40_srcs, "build/3.7/card", "./psy-q/3.7/PSX/LIB/LIBCARD.LIB", "-O3 -g0 -G0 -funsigned-char -c -I./src/snd -I./include -DVERSION=40", "3.7")

    cd37_srcs = [
        'src/cd/bios.c',
        'src/cd/c_002.c',
        'src/cd/c_003.c',
        'src/cd/c_004.c',
        'src/cd/c_005.c',
        'src/cd/c_007.c',
        'src/cd/c_008.c',
        'src/cd/c_009.c',
        'src/cd/c_010.c',
        'src/cd/c_011.c',
        'src/cd/cdread.c',
        'src/cd/cdread2.c',
        'src/cd/cdrom.c',
        'src/cd/event.c',
        'src/cd/sys.c',
    ]
    add_lib_wibo(cd37_srcs, "build/3.7/cd", "./psy-q/3.7/PSX/LIB/LIBCD.LIB", "-O2 -g0 -G0 -funsigned-char -c -I./src/snd -I./include -DVERSION=37", "3.7")

    etc37_srcs = [
        'src/etc/intr.c',
        'src/etc/vsync.c',
    ]
    add_lib_wibo(etc37_srcs, "build/3.7/etc", "./psy-q/3.7/PSX/LIB/LIBETC.LIB", "-O2 -g0 -G0 -funsigned-char -c -I./src/snd -I./include -DVERSION=37", "3.7")

    etc40_srcs = [
        'src/etc/intr_dma.c',
        'src/etc/intr_vb.c',
        'src/etc/vmode.c',
    ]
    add_lib_wibo(etc40_srcs, "build/3.7/etc", "./psy-q/3.7/PSX/LIB/LIBETC.LIB", "-O2 -g0 -G0 -funsigned-char -c -I./src/snd -I./include -DVERSION=40", "3.7")

    gpu37_srcs = [
        'src/gpu/ext.c',
        'src/gpu/prim.c',
        'src/gpu/sys.c',
    ]
    add_lib_wibo(gpu37_srcs, "build/3.7/gpu", "./psy-q/3.7/PSX/LIB/LIBGPU.LIB", "-O3 -g0 -G0 -funsigned-char -c -I./src/snd -I./include -DVERSION=37", "3.7")

    press37_srcs = [
        'src/press/libpress.c',
    ]
    add_lib_wibo(press37_srcs, "build/3.7/press", "./psy-q/3.7/PSX/LIB/LIBPRESS.LIB", "-O3 -g0 -G0 -funsigned-char -c -I./src/snd -I./include -DVERSION=37", "3.7")

    press40_srcs = [
        'src/press/table.c',
    ]
    add_lib_wibo(press40_srcs, "build/3.7/press", "./psy-q/3.7/PSX/LIB/LIBPRESS.LIB", "-O3 -g0 -G0 -funsigned-char -c -I./src/snd -I./include -DVERSION=40", "3.7")

    snd37_srcs = [
        'src/snd/cc_100.c',
        'src/snd/cc_7.c',
        'src/snd/cres.c',
        'src/snd/decres.c',
        'src/snd/dmyprint.c',
        'src/snd/midicc.c',
        'src/snd/midinote.c',
        'src/snd/midiprog.c',
        'src/snd/midiread.c',
        'src/snd/miditime.c',
        'src/snd/next.c',
        'src/snd/pause.c',
        'src/snd/playmode.c',
        'src/snd/replay.c',
        'src/snd/sepinit.c',
        'src/snd/sscall.c',
        'src/snd/ssclose.c',
        'src/snd/ssinit.c',
        'src/snd/ssloop.c',
        'src/snd/ssstop.c',
        'src/snd/sstable.c',
        'src/snd/ssvol.c',
        'src/snd/tempo.c',
        'src/snd/ut_vvol.c',
        'src/snd/vm_aloc1.c',
        'src/snd/vm_key.c',
        'src/snd/vm_no1.c',
        'src/snd/vm_nowon.c',
        'src/snd/vm_seq.c',
        'src/snd/vm_vol.c',
    ]
    add_lib_wibo(snd37_srcs, "build/3.7/snd", "./psy-q/3.7/PSX/LIB/LIBSND.LIB", "-O3 -g0 -G0 -funsigned-char -c -I./src/snd -I./include -DVERSION=37", "3.7")

    snd40_srcs = [
        'src/snd/play.c',
        'src/snd/ssend.c',
        'src/snd/ssinit_c.c',
        'src/snd/ssopenpj.c',
        'src/snd/ssplay.c',
        'src/snd/ssquit.c',
        'src/snd/sssattr.c',
        'src/snd/sssmv.c',
        'src/snd/ssstart.c',
        'src/snd/sssv.c',
        'src/snd/sstick.c',
        'src/snd/ut_gva.c',
        'src/snd/ut_keyv.c',
        'src/snd/ut_rdep.c',
        'src/snd/ut_rev.c',
        'src/snd/ut_roff.c',
        'src/snd/ut_ron.c',
        'src/snd/vm_aloc2.c',
        'src/snd/vm_f.c',
        'src/snd/vm_init.c',
        'src/snd/vm_n2p.c',
        'src/snd/vm_noff.c',
        'src/snd/vm_nowof.c',
        'src/snd/vm_stav.c',
        'src/snd/vm_vsu.c',
        'src/snd/vs_mono.c',
        'src/snd/vs_vab.c',
        'src/snd/vs_vh.c',
        'src/snd/vs_vtbp.c',
        'src/snd/vs_vtc.c',
    ]
    add_lib_wibo(snd40_srcs, "build/3.7/snd", "./psy-q/3.7/PSX/LIB/LIBSND.LIB", "-O3 -g0 -G0 -funsigned-char -c -I./src/snd -I./include -DVERSION=40", "3.7")

    spu37_srcs = [
        'src/spu/s_ini.c',
        'src/spu/s_wp.c',
        'src/spu/spu.c',
    ]
    add_lib_wibo(spu37_srcs, "build/3.7/spu", "./psy-q/3.7/PSX/LIB/LIBSPU.LIB", "-O3 -g0 -G0 -funsigned-char -c -I./src/snd -I./include -DVERSION=37", "3.7")

    spu40_srcs = [
        'src/spu/s_crwa.c',
        'src/spu/s_dcb.c',
        'src/spu/s_gks.c',
        'src/spu/s_i.c',
        'src/spu/s_it.c',
        'src/spu/s_itc.c',
        'src/spu/s_m.c',
        'src/spu/s_m_f.c',
        'src/spu/s_m_init.c',
        'src/spu/s_m_int.c',
        'src/spu/s_m_m.c',
        'src/spu/s_m_util.c',
        'src/spu/s_q.c',
        'src/spu/s_rmp.c',
        'src/spu/s_sav.c',
        'src/spu/s_sca.c',
        'src/spu/s_snv.c',
        'src/spu/s_sr.c',
        'src/spu/s_sra.c',
        'src/spu/s_srmp.c',
        'src/spu/s_stm.c',
        'src/spu/s_stsa.c',
    ]
    add_lib_wibo(spu40_srcs, "build/3.7/spu", "./psy-q/3.7/PSX/LIB/LIBSPU.LIB", "-O3 -g0 -G0 -funsigned-char -c -I./src/snd -I./include -DVERSION=40", "3.7")


GNU_CONFIG = 'config/gnu.txt'
GNU_DEFAULT_FLAGS = '-O3 -G0'
GNU_TOOLS = ('cygnus-2.7.2-970404', '2.8-970404')

ninja.rule('gnu_ecoff',
           command="FUNCS='$CFUNCS' bash tools/gnu_ecoff.sh $in $out '$CPPFLAGS' '$CC1FLAGS' $GCC $BINUTILS"
                   " && (cpp -MM -MT $out -MF $out.d -I./include -I$$(dirname $in) $CPPFLAGS $in 2>/dev/null || true)",
           depfile='$out.d', deps='gcc',
           description='GNU gcc $GCC + as $BINUTILS $in -> $out')
ninja.rule('check_member',
           command='python3 tools/check_member.py $LIBRARY $in',
           description='Checking that $in matches')


def build_gnu():
    import re
    if not os.path.exists(GNU_CONFIG):
        return
    for line in open(GNU_CONFIG):
        line = line.split('#')[0].split()
        if not line:
            continue
        v, lib, src, flags = line[0], line[1], line[2], ' '.join(line[3:]) or GNU_DEFAULT_FLAGS
        if flags == '-':
            flags = ''
        if not os.path.isdir(f'psy-q/{v}/PSX/LIB'):
            continue
        lib = lib if lib.upper().endswith('.LIB') else lib + '.LIB'
        cpp = ' '.join(f for f in flags.split() if f.startswith(('-I', '-D')))
        cc1 = ' '.join(f for f in flags.split() if not f.startswith(('-I', '-D')))
        gcc, binutils = GNU_TOOLS
        objname = func = None
        for f in cc1.split():
            if f.startswith('gcc='):
                gcc = f[4:]
            elif f.startswith('as='):
                binutils = f[3:]
            elif f.startswith('obj='):
                objname = f[4:].lower()
            elif f.startswith('func='):
                func = f[5:]
        cc1 = ' '.join(f for f in cc1.split() if not f.startswith(('gcc=', 'as=', 'obj=', 'func=')))
        name = os.path.splitext(os.path.basename(src))[0]
        if '.v' in name:
            name = name.split('.v')[0]
        m = re.match(r'/\* LIB\w+\.LIB (\w+):', open(src).readline())
        if m:
            name = m.group(1).lower()
        if objname:
            name = objname
        obj = f"build/{v}/{lib.upper().replace('.LIB', '')[3:].lower()}/{name}.obj"
        deps = ['tools/gnu_ecoff.sh', 'tools/ecoff2psyq.py',
                f'build/gnu/gcc-{gcc}/cc1', f'build/gnu/binutils-{binutils}/as']
        if func:
            assert src.endswith('.c'), line
            deps.append('tools/cfunc.py')
        track_progress_object(obj)
        ninja.build(obj, 'gnu_ecoff', inputs=[src], implicit=deps,
                    variables={'CPPFLAGS': f"-I./src/snd -I./include {cpp} -DVERSION={v.replace('.', '')}",
                               'CC1FLAGS': cc1, 'GCC': gcc, 'BINUTILS': binutils, 'CFUNCS': func or ''})
        ninja.build(f"{obj}.check", 'check_member', inputs=[obj], implicit=['tools/check_member.py'],
                    variables={'LIBRARY': f'./psy-q/{v}/PSX/LIB/{lib}'})


build_33()
build_35()
build_36()
build_37()
build_40()
build_gnu()

ninja.rule(
    'progress_report',
    command='./tools/psy-q-splitter/splitter/target/release/splitter report --manifest config/progress.json --version $VERSION --output $out',
    description='Generating progress report for PSY-Q $VERSION')

reports = []
for version, objects in progress_objects.items():
    report = f"build/{version}/report.json"
    ninja.build(
        report,
        'progress_report',
        inputs=['config/progress.json'],
        implicit=sorted(objects + glob.glob(f'psy-q/{version}/PSX/LIB/*.LIB')),
        variables={'VERSION': version})
    ninja.build(f"progress-{version}", 'phony', implicit=[report])
    reports.append(report)
ninja.build('progress', 'phony', implicit=reports)

ninja.close()
