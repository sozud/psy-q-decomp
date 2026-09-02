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

ninja.rule(
        'check',
        command='./tools/psy-q-splitter/splitter/target/release/splitter diff_obj_with_lib $LIBRARY $in',
        description='Checking that $in matches')



def add_lib_wibo(srcs, output_dir, lib_name, flags, folder):
    for src in srcs:
        filename_without_extension = os.path.splitext(os.path.basename(src))[0]
        obj_name = f"{output_dir}/{filename_without_extension}.obj"
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
           command='WIBO_DEBUG=0 PSYQ_PATH=build/4.0 COMPILER_PATH=build/4.0 C_INCLUDE_PATH=include build/4.0/wibo build/4.0/CCPSX.EXE $in $FLAGS -o$out',
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
    snd_srcs = [
        'src/snd/adsr.c',
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
        'src/snd/vm_doff.c',
        'src/snd/vm_don.c',
        'src/snd/vm_g.c',
        'src/snd/vm_vsu.c',
        'src/snd/vs_mono.c',
        'src/snd/vs_srv.c',
        'src/snd/vs_vab.c',
        'src/snd/vs_vfb.c',
        'src/snd/vs_vtb.c',
        'src/snd/vs_vtbp.c',
        'src/snd/vs_vtc.c',
    ]

    add_lib_263(snd_srcs, "build/3.5/snd", "./psy-q/3.5/PSX/LIB/LIBSND.LIB", "-DVERSION=35", "3.5")

    spu_srcs = [
        'src/spu/s_cb.c',
        'src/spu/s_crwa.c',
        'src/spu/s_dcb.c',
        'src/spu/s_i.c',
        'src/spu/s_ini.c',
        'src/spu/s_it.c',
        'src/spu/s_m_f.c',
        'src/spu/s_m_init.c',
        'src/spu/s_m_util.c',
        'src/spu/s_r.c',
        'src/spu/s_sav.c',
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
    ]

    add_lib_263(c_srcs, "build/3.5/c", "./psy-q/3.5/PSX/LIB/LIBC.LIB", "-DVERSION=35", "3.5")

    c_srcs = [
        'src/cd/c_002.c',
    ]

    add_lib_263(c_srcs, "build/3.5/cd", "./psy-q/3.5/PSX/LIB/LIBCD.LIB", "-DVERSION=35", "3.5")

    c_srcs = [
        'src/press/libpress.c',
    ]

    add_lib_263(c_srcs, "build/3.5/press", "./psy-q/3.5/PSX/LIB/LIBPRESS.LIB", "-DVERSION=35", "3.5")

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
        # 'src/spu/s_sk.c',
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
        # 'src/spu/s_wp.c',
        # 'src/spu/sr_gaks.c',
    ]

    add_lib_wibo(spu_srcs, "build/4.0/spu", "./psy-q/4.0/PSX/LIB/LIBSPU.LIB", "-O2 -g0 -G0 -funsigned-char -c -I./src/snd -I./include -DVERSION=40", "4.0")

build_33()
build_35()
build_36()
build_40()

ninja.close()
