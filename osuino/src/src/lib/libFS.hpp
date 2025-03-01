#ifndef _IS_ARDUINO_
#include <fstream>
#include <iostream>
#endif

class ConfigData
{
public:
    IStringType data;
    bool hasData = false;
    ConfigData() {}
    ConfigData(IStringType data) : data(data), hasData(true) {}
    ConfigData(bool hasData) : hasData(hasData) {}
};

#ifdef _IS_ARDUINO_
class FS
{
public:
    IVector<IStringType> list()
    {
        IVector<IStringType> result;
        result.push_back("map-2842217.meta");
        result.push_back("chunk-2842217-0-0.chunk");
        result.push_back("chunk-2842217-0-1.chunk");
        result.push_back("chunk-2842217-0-2.chunk");
        result.push_back("chunk-2842217-0-3.chunk");
        result.push_back("chunk-2842217-0-4.chunk");
        result.push_back("chunk-2842217-0-5.chunk");
        result.push_back("chunk-2842217-1-0.chunk");
        result.push_back("chunk-2842217-1-1.chunk");
        result.push_back("chunk-2842217-1-2.chunk");
        result.push_back("chunk-2842217-1-3.chunk");
        result.push_back("chunk-2842217-1-4.chunk");
        result.push_back("chunk-2842217-1-5.chunk");
        result.push_back("chunk-2842217-1-6.chunk");
        result.push_back("chunk-2842217-2-0.chunk");
        result.push_back("chunk-2842217-2-1.chunk");
        result.push_back("chunk-2842217-2-2.chunk");
        result.push_back("chunk-2842217-2-3.chunk");
        result.push_back("chunk-2842217-2-4.chunk");
        result.push_back("chunk-2842217-2-5.chunk");
        result.push_back("chunk-2842217-3-0.chunk");
        result.push_back("chunk-2842217-3-1.chunk");
        result.push_back("chunk-2842217-3-2.chunk");
        result.push_back("chunk-2842217-3-3.chunk");
        result.push_back("chunk-2842217-3-4.chunk");
        return result;
    }

    IVector<IStringType> listMetas() {
        IVector<IStringType> result;
        result.push_back("map-2842217.meta");
        return result;
    }

    IStringType get(IStringType filename)
    {
        if(filename == "map-2842217.meta") return "__TrPF__p51K_OYoru ni Kakeru_HYOASOBI_HYOASOBI_G_H_G_F_oI_nJ_FJ_gH";
        if(filename == "chunk-2842217-0-0.chunk") return "__OQ@__pe__2l__Et@__f7@_6JB@_VYB@_wmB@_L1B@_Z8B@_mDC@_BSC_PZC_cgC@_qnC@_3uC@_F2C@_S9C@_gED@_uLD@_JaD@_YzD_L1D_/2D@_M+D@_aFE@_nME@_1TE@_CbE@_QiE@_epE@_rwE@_F2E@_G/E@_UGF@_IRF@_8bF@_KjF@_XqF@_lxF@_AAG@_nDG@_bOG@_pVG@_2cG@_dgG@_epG@_fyG@_s5G@_hEH@_HIH@_VPH@_iWH@_wdH@_+kH@_LsH@_ZzH_/2H_m6H@_0BI@_BJI@_PQI@_2TI@_QZI@_qeI@_3lI@_FtI@_fyI@_G2I@_g7I@_uCJ@_7JJ@_JRJ@_WYJ@_kfJ@_xmJ@_YqJ@_M1J@_z4J@_N+J@_nDK_OHK_cOK_CSK_pVK@_DbK@_egK@_rnK@_5uK@_G2K@_g7K@_6AL@_hEL@_vLL_WPL_8SL@_KaL@_XhL@_loL_MsL_yvL@_A3L@_0BM_bFM_pMM_PQM_2TM@_dXM_EbM_RiM_4lM_fpM_GtM";
        if(filename == "chunk-2842217-0-1.chunk") return "_swM@_63M@_H/M_uCN_VGN_8JN_JRN@_wUN@_+bN@_LjN_ymN_ZqN@_/tN@_N1N_04N_a8N@_BAO@_PHO@_cOO@_qVO@_EbO@_FkO@_snO@_SrO@_gyO@_h7O@_I/O@_iEP@_JIP@_vLP@_WPP@_9SP@_kWP@_+bP@_LjP@_MsP@_zvP@_B3P@_CAQ@_CJQ@_pMQ@_QQQ@_3TQ@_eXQ@_EbQ@_SiQ@_5lQ@_GtQ@_U0Q@_h7Q_I/Q_vCR@_JIR@_jNR@_KRR@_xUR@_XYR@_lfR@_ymR@_zvR@_O1R@_04R@_O+R@_1BS@_ESS@_RZS@_fgS@_snS@_6uS@_H2S@_u5S@_8AT@_iET@_wLT@_+ST@_LaT@_ZhT@_moT@_0vT@_B3T@_P+T@_DJU@_qMU@_3TU@_FbU@_5lU@_uwU@_73U@_J/U@_WGV@_kNV@_xUV@_/bV@_MjV@_aqV@_nxV@_14V@_CAW@_QHW@_3KW@_eOW@_ESW@_rVW@_5cW@_GkW@";
        if(filename == "chunk-2842217-0-2.chunk") return "_UrW@_hyW@_I2W@_v5W@_8AX@_KIX@_XPX@_lWX@_ydX@_AlX@_OsX@_bzX@_p6X@_P+X@_dFY@_EJY@_RQY@_fXY@_seY@_6lY@_hpY@_HtY@_uwY@_i7Y@_wCZ@_KIZ@_LRZ@_ZYZ@_mfZ@_0mZ@_BuZ@_P1Z@_c8Z@_qDa@_3Ka@_FSa@_SZa@_gga@_una@_7ua@_J2a@_j7a@_KIb@_xLb@_LRb@_Mab@_nob@_Cub@_C3b@_Q+b@_eFc@_rMc@_5Tc@_Gbc@_Uic@_hpc@_vwc@_83c@_K/c@_XGd@_yUd@_Acd@_Ojd@_bqd@_pxd@_24d@_EAe@_FJe@_fOe@_sVe@_6ce@_Uie@_Vre@_iye@_w5e@_+Af@_LIf@_MRf@_mWf@_0df@_Blf@_Psf@_czf@_q6f@_3Bg@_SHg@_SQg@_5Tg@_gXg@_ueg@_7lg@_Jtg@_W0g@_k7g@_K/g@_xCh@_/Jh@_MRh@_aYh@_nfh@";
        if(filename == "chunk-2842217-0-3.chunk") return "_ooh@_Cuh@_Q1h@_34h@_e8h@_rDi@_5Ki@_TQi@_UZi@_6ci@_Vii@_Wri@_wwi@_K2i@_X9i@_+Aj@_yLj@_ATj@_Oaj@_ofj@_Ojj@_dzj@_E3j@_rDk@_SHk@_gOk@_6Tk@_7ck@_9uk@_+3k@_+Al@_mNl@_2ml@_2vl@_E3l@_r6l@_fFm@_Vim@_vnm@_Ktm@_/An@_MIn@_mNn@_0Un@_1dn@_Cln@_Qsn@_3vn@_ezn@_r6n@_5Bo@_GJo@_UQo@_hXo@_veo@_8lo@_Kto@_X0o@_l7o@_yCp@_AKp@_ORp@_bYp@_pfp@_2mp@_Eup@_R1p@_44p@_sDq@_6Kq@_HSq@_iXq@_Ibq@_igq@_wnq@_+uq@_+3q@_Z9q@_zCr@_NIr@_nNr@_1Ur@_Par@_chr@_qor@_3vr@_F3r@_S+r@_gFs@_HJs@_uMs@_7Ts@_Jbs@_Wis@_kps@_xws@_/3s@_M/s@_aGt@_BKt@";
        if(filename == "chunk-2842217-0-4.chunk") return "_nNt@_1Ut@_Cct@_Qjt@_eqt@_rxt@_54t@_f8t_tDu_UHu@_Xru@_y5u@_OIv@_pWv@_2dv@_Elv@_fzv@_6Bw@_VQw@_wew@_+lw@_Ltw@_lyw@_m7w@_0Cx@_oNx@_cYx@_Dcx@_3mx@_Ssx@_S1x@_HAy_7Ky_JSy@_vVy@_WZy@_9cy@_kgy@_xny@_/uy@_M2y@_a9y@_BBz@_nEz@_cPz@_CTz@_Qaz@_ehz@_roz@_5vz@_G3z@_U+z@_hF0@_7K0@_WQ0@_8T0@_Kb0@_Xi0@_lp0@_yw0@_A40@_O/0@_bG1@_pN1@_2U1@_Ec1@_Rj1@_fq1@_sx1@_641@_HA2@_uD2@_VH2@_iO2@_JS2@_wV2@_+c2@_Lk2@_Zr2@_my2@_052@_BB3@_PI3@_cP3@_qW3@_3d3@_Fl3@_Ss3@_gz3@_u63@_7B4@_JJ4@_9T4@_Kb4@_xe4@_/l4@_Mt4@_a04@_n74@_1C5@_CK5@_QR5@";
        if(filename == "chunk-2842217-0-5.chunk") return "_3U5@_Ec5@_Fl5@_so5@_tx5@_U15_h85_vD6@_JJ6@_KS6@_XZ6@_lg6@_Mk6@_yn6@_Nt6@_zw6@_O26@_b96@_1C7@_PI7@_ET7@_Ra7@_fh7@_so7@_6v7@_V+7@_iF8@_wM8@_Lb8@_Zi8@_mp8@_0w8@_B48@_P/8@_cG9@_qN9@_3U9@_Fc9@_sf9@_Sj9@_gq9@_749@_JA+@_WH+@_kO+@_xV+@_/c+@_mg+@_Mk+@_zn+@_ar+@_0w+@_b0+@";
        if(filename == "chunk-2842217-1-0.chunk") return "__OQ@__43@_sCB@_uUB@_8bB@_kxB@_y4B@_0KC@_BSC@_PZC_cgC_RrC@_eyC_F2C_tCD@_uLD@_7SD@_vdD_jfD_WhD_KjD_wmD@_koD_XqD_+tD_xvD_YzD@_/2D@_M+D@_nME@_OQE@_oVE@_CbE@_QiE@_epE@_rwE@_53E@_tCF@_6JF@_7SF@_WYF@_jfF@_KjF@_xmF@_XqF@_yvF@_M1F@_Z8F@_AAG@_OHG@_oMG@_CSG@_PZG@_qeG@_QiG@_3lG@_RrG@_fyG@_s5G@_T9G@_UGH@_7JH@_VPH@_vUH@_JaH@_XhH_+kH_YqH_+tH_ZzH@_m6H@_0BI@_OHI@_oMI@_PQI@_pVI@_qeI@_3lI@_FtI@_S0I@_53I_g7I_H/I@_uCJ@_IIJ@_iNJ@_JRJ@_WYJ@_wdJ@_+kJ@_LsJ@_mxJ_M1J_a8J@_0BK@_OHK@_1KK@_CSK@_QZK@_egK@_EkK@_SrK_5uK_fyK@_hEL@_7JL@_8SL_jWL_KaL@_xdL_+kL_loL@_MsL@";
        if(filename == "chunk-2842217-1-1.chunk") return "_ZzL@_n6L_O+L_bFM@_pMM@_PQM@_EbM@_RiM@_4lM@_GtM@_63M@_H/M@_VGN@_vLN_WPN_wUN@_+bN_kfN_LjN@_loN@_zvN@_04N_a8N_PHO@_DSO_qVO_RZO@_3cO@_egO@_4lO@_fpO@_U9O@_uCP@_VGP@_vLP@_KRP@_XYP@_xdP@_YhP@_/kP@_mxP@_n6P@_oDQ@_2KQ@_dOQ@_qVQ@_EbQ@_reQ@_5lQ@_fpQ@_GtQ@_U0Q_63Q_h7Q@_I/Q_vCR_WGR@_8JR@_KRR@_kWR@_+bR@_MjR@_ZqR@_AuR@_nxR@_o6R@_CAS@_pDS@_ESS@_eXS@_4cS@_GkS@_TrS@_hyS@_H2S@_V9S@_iET@_JIT@_XPT@_+ST@_kWT@_ydT@_/kT@_NsT@_azT@_14T@_P+T@_2BU@_2KU@_RQU@_eXU@_FbU@_seU@_gpU@_HtU@_uwU@_i7U@_vCV@_WGV@_9JV@_KRV@_YYV@_mfV@_zmV@_BuV@_C3V@_c8V@";
        if(filename == "chunk-2842217-1-2.chunk") return "_CAW@_pDW@_QHW@_qMW@_rVW@_SZW@_seW@_TiW@_tnW@_6uW@_73W@_W9W@_8AX@_jEX@_xLX@_+SX@_YYX@_/bX@_ZhX@_AlX@_noX@_OsX@_bzX@_C3X@_P+X@_2BY@_eOY@_4TY@_GbY@_TiY@_hpY@_HtY@_83Y@_J/Y@_jEZ@_+JZ@_LRZ@_ZYZ@_/bZ@_ahZ@_BuZ@_P1Z@_c8Z@_2Ba@_RHa@_eOa@_sVa@_5ca@_una@_uwa@_83a@_W9a@_kEb@_xLb@_YPb@_/Sb@_ZYb@_zdb@_Blb@_bqb@_1vb@_C3b@_3Bc@_EJc@_FSc@_sVc@_Gbc@_ggc@_6lc@_vwc@_83c@_K/c@_lNd@_ZYd@_Acd@_Ojd@_pxd@_24d@_RHe@_4Ke@_GSe@_gXe@_Gbe@_hge@_Hke@_ipe@_8ue@_93e@_j7e@_K/e@_kEf@_+Jf@_ZPf@_/Sf@_aYf@_Acf@_nff@_Blf@_oof@_Psf@";
        if(filename == "chunk-2842217-1-3.chunk") return "_pxf@_D3f@_d8f@_3Bg@_eFg@_FJg@_fOg@_GSg@_tVg@_6cg@_Uig@_Blg@_ipg@_8ug@_K2g@_k7g@_xCh@_/Jh@_ZPh@_ATh@_mWh@_Bch@_Ojh@_1mh@_cqh@_pxh@_Q1h@_q6h@_R+h@_rDi@_SHi@_GSi@_tVi@_Hbi@_hgi@_Iki@_vni@_8ui@_W0i@_93i@_k7i@_yCj@_MIj@_yLj@_ATj@_nWj@_0dj@_Clj@_cqj@_2vj@_Q1j@_e8j@_4Bk@_GJk@_sMk@_TQk@_tVk@_uek@_Ikk@_Jtk@_jyk@_K2k@_k7k@_yCl@_zLl@_ATl@_Pjl@_Qsl@_R1l@_e8l@_sDm@_SHm@_5Km@_TQm@_Ibm@_igm@_Jkm@_Wrm@_9um@_kym@_K2m@_mEn@_AKn@_aPn@_nWn@_Oan@_chn@_pon@_3vn@_E3n@_S+n@_fFo@_tMo@_6To@_Ibo@_8lo@_jpo@_xwo@_X0o@_l7o@";
        if(filename == "chunk-2842217-1-4.chunk") return "_AKp@_nNp@_ORp_bYp_Pjp@_dqp@_qxp@_f8p@_sDq@_THq@_hOq@_HSq@_uVq@_8cq@_igq@_wnq@_Xrq@_+uq@_L2q@_y5q@_Z9q@_/Ar@_mEr@_AKr@_nNr@_BTr@_oWr@_Par@_2dr@_chr@_qor@_Eur@_ezr@_S+r@_5Bs@_gFs@_6Ks@_UQs@_7Ts@_ves@_9ls@_Kts@_Y0s@_m7s@_0Lt@_ORt@_1Ut@_Cct@_pft@_3mt@_eqt@_rxt@_GAu@_UHu_6Ku_ywu@_y5u@_ABv@_OIv@_bPv@_Pav@_eqv@_fzv@_T+v@_VQw@_iXw@_Xiw@_Ltw@_Z0w@_/3w@_N/w@_0Cx@_BKx@_bPx@_2Ux@_qfx@_Elx@_eqx@_Fux@_8Ty@_jXy@_wey@_Xiy@_lpy@_/uy@_myy@_z5y@_0Cz@_OIz@_1Lz@_PRz@_2Uz@_3dz@_ehz@_Elz@_Ssz@_fzz@_G3z@_t6z@_6B0@_VH0@_vM0@";
        if(filename == "chunk-2842217-1-5.chunk") return "_8T0@_jX0@_xe0@_+l0@_Mt0@_Z00@_A40@_n70@_O/0@_0C1@_CK1@_pN1@_2U1@_dY1@_qf1@_4m1@_Gu1@_sx1@_T11@_h81@_uD2@_8K2@_WQ2@_9T2@_Kb2@_kg2@_yn2@_/u2@_N22@_n72@_BB3@_oE3@_2L3@_2U3@_Ra3@_eh3@_Fl3@_so3@_Ss3@_5v3@_T13@_u63@_U+3@_iF4@_vM4@_WQ4@_kX4@_xe4@_mp4@_zw4@_B44@_O/4@_cG5@_2L5@_3U5@_eY5@_rf5@_Sj5@_5m5@_Ts5@_6v5@_gz5@_645@_h85@_IA6@_iF6@_jO6@_KS6@_xV6@_+c6@_lg6@_/l6@_Zr6@_Av6@_ny6@_056@_b96@_CB7@_3U7@_fh7@_so7@_Ts7@_tx7@_H37@_V+7@_iF8@_JJ8@_jO8@_+T8@_Lb8@_Zi8@_mp8@_Nt8@_a08@_o78@_CB9@_qN9@_3U9@_4d9@_fh9@";
        if(filename == "chunk-2842217-1-6.chunk") return "_to9@_Hu9@_U19@_u69@_JA+@_jF+@_9K+@_KS+@_YZ+@_Zi+@_Am+@_mp+@_ar+@_0w+@_O2+@";
        if(filename == "chunk-2842217-2-0.chunk") return "__Cb@__2l__Et__ey@__f7@_TGB@_UPB@_ifB@_KsB@_Z8B@_mDC@_aOC@_2lC@_F2C_s5C_S9C@_HID@_7SD@_lxD@_/2D@_Z8D@_zBE@_BJE@_bOE@_CSE@_PZE@_3lE@_EtE@_eyE@_53E@_G/E@_UGF@_hNF@_vUF@_8bF@_9kF@_koF@_LsF@_lxF@_y4F@_AAG@_aFG@_BJG@_bOG@_2cG@_EkG@_4uG@_G2G@_HIH@_uLH@_8SH@_iWH@_wdH@_+kH_koH_+tH@_lxH@_z4H@_a8H@_AAI@_aFI@_BJI@_bOI@_cXI@_DbI_qeI_EkI_rnI_SrI@_g7I_H/I_hEJ@_7JJ@_VPJ@_vUJ_WYJ_kfJ@_xmJ_YqJ_/tJ@_M1J@_m6J@_BAK@_nDK@_CJK@_oMK@_CSK@_dXK_DbK_qeK@_rnK@_5uK_fyK_T0K@_IIL@_vLL@_WYL@_loL@_/tL@_N1L@_z4L@_O+L@_CJM@_DSM@_3cM@_RiM@_rnM@_swM@_T0M@_h7M_H/M";
        if(filename == "chunk-2842217-2-1.chunk") return "_iNN@_9SN@_+bN@_YhN@_ZqN@_mxN@_04N@_BAO_oDO_2KO_cOO_QQO@_qVO_RZO_SiO@_4lO@_fpO@_5uO@_T0O@_u5O@_7AP@_JIP@_jNP@_KRP@_KaP@_lfP@_LjP@_MsP@_azP@_n6P_O+P_1BQ@_cFQ@_CJQ@_QQQ@_eXQ@_4cQ@_SiQ@_fpQ@_twQ_U0Q_vCR@_WPR@_xUR@_XYR@_lfR@_ymR@_ZqR@_AuR@_nxR@_O1R@_04R@_O+R@_1BS@_qVS@_4cS@_fgS@_snS@_TrS@_hyS@_H2S@_V9S@_iET@_wLT@_KRT@_kWT@_LaT@_ZhT@_moT@_0vT@_azT@_B3T@_P+T@_cFU@_qMU@_RQU@_3TU@_eXU@_seU@_SiU@_tnU@_HtU@_U0U@_73U@_J/U@_jEV@_9JV@_kNV@_xUV@_/bV@_mfV@_MjV@_aqV@_nxV@_O1V@_14V@_P+V@_pDW@_3KW@_eOW@_5cW@_GkW@_tnW@_UrW@_6uW@";
        if(filename == "chunk-2842217-2-2.chunk") return "_v5W@_W9W@_jEX@_KIX@_XPX@_lWX@_ydX@_ZhX@_noX@_0vX@_C3X@_2BY@_dFY@_qMY@_RQY@_fXY@_seY@_6lY@_UrY@_iyY@_W9Y@_9AZ@_kNZ@_yUZ@_MaZ@_zdZ@_AlZ@_aqZ@_oxZ@_24Z@_qDa@_3Ka@_FSa@_SZa@_tea@_Hka@_Ura@_7ua@_iya@_J2a@_9Ab@_lNb@_yUb@_Mab@_ahb@_nob@_1vb@_P1b@_p6b@_eFc@_SQc@_5Tc@_fXc@_tec@_Uic@_W0c@_W9c@_lNd@_yUd@_nfd@_ood@_Cud@_D3d@_RHe@_rMe@_SQe@_sVe@_tee@_Hke@_une@_Vre@_iye@_J2e@_X9e@_+Af@_LIf@_yLf@_ZPf@_zUf@_Naf@_Ojf@_bqf@_2vf@_czf@_q6f@_R+f@_EAg@_FJg@_sMg@_SQg@_TZg@_hgg@_Ikg@_ung@_Jtg@_vwg@_W0g@_w5g@_+Ah@_lEh@";
        if(filename == "chunk-2842217-2-3.chunk") return "_LIh@_yLh@_MRh@_zUh@_Nah@_nfh@_Clh@_Psh@_2vh@_dzh@_34h@_EAi@_5Ki@_6Ti@_UZi@_uei@_7li@_ipi@_Jti@_jyi@_x5i@_X9i@_lEj@_/Jj@_ZPj@_zUj@_aYj@_Bcj@_1mj@_Psj@_qxj@_34j@_R+j@_5Kk@_GSk@_hXk@_igk@_8lk@_Wrk@_wwk@_Y9k@_lEl@_/Jl@_aPl@_0Ul@_aYl@_1dl@_bhl@_cql@_qxl@_S+l@_GJm@_HSm@_UZm@_uem@_jpm@_wwm@_+3m@_/An@_MIn@_zLn@_BTn@_nWn@_1dn@_Qsn@_qxn@_R1n@_r6n@_S+n@_5Bo@_GJo@_tMo@_UQo@_hXo@_Wio@_jpo@_Kto@_+3o@_yCp_AKp_Ccp@_Pjp@_2mp@_Eup@_qxp@_R1p@_44p@_f8p@_GAq@_THq@_UQq@_7Tq@_VZq@_veq@_Wiq@_9lq@_xwq@_L2q@_l7q@_M/q@_NIr@";
        if(filename == "chunk-2842217-2-4.chunk") return "_0Lr@_aPr@_bYr@_Ccr@_pfr@_qor@_3vr@_F3r@_GAs@_gFs@_HJs@_uMs@_ISs@_iXs@_Wis@_wns@_xws@_/3s@_M/s@_zCt@_nNt@_CTt@_cYt@_dht@_Eut@_54t@_tDu_UHu_Xru@_M2u@_0Lv@_pWv@_dhv@_4vv@_G3v@_6Bw@_uMw@_Jbw@_Kkw@_kpw@_yww@_/3w@_ABx@_aGx@_PRx@_cYx@_2dx@_Rjx@_Fux@_sxx@_7Ky_WZy_9cy@_Kky@_xny@_Yry@_/uy@_M2y@_z5y@_BBz@_nEz@_OIz@_cPz@_CTz@_Qaz@_3dz@_roz@_Fuz@_fzz@_U+z@_hF0@_vM0@_WQ0@_Kb0@_xe0@_Xi0@_lp0@_yw0@_N20@_n70@_oE1@_CK1@_PR1@_dY1@_Ec1@_Rj1@_fq1_sx1_641@_h81@_HA2@_VH2@_8K2@_iO2@_wV2@_XZ2@_kg2@_Lk2@_yn2@_Zr2@_my2@_052@_O/2@";
        if(filename == "chunk-2842217-2-5.chunk") return "_oE3@_PI3@_DT3@_qW3@_Ra3@_eh3@_5v3@_gz3@_643@_U+3@_7B4@_JJ4@_WQ4@_9T4@_kX4@_xe4@_Yi4@_Zr4@_zw4@_a04@_n74@_O/4@_1C5@_CK5@_dP5@_eY5@_Ec5@_eh5@_5m5@_Gu5@_U15@_h85@_7B6@_WH6@_8K6@_jO6@_xV6@_ye6@_Yi6@_mp6@_Av6@_a06@_o76@_1C7@_PI7@_ET7@_eY7@_4d7@_Gl7@_gq7@_Gu7@_U17@_u67@_8B8@_wM8@_KS8@_kX8@_ye8@_/l8@_0w8@_B48@_P/8@_2C9@_cG9@_2L9@_RR9@_eY9@_Fc9@_Sj9@_gq9@_ux9@_749@_V+9@_WH+@_xV+@_/c+@_Zi+@_Am+@_mp+@_0w+@_O2+@";
        if(filename == "chunk-2842217-3-0.chunk") return "__oV@__pe@__Et@_6JB@_VYB@_JjB@_wmB@_L1B@_//B@_AJC@_PZC@_cgC@_3uC@_s5C_JaD_xvD@_L1D@_y4D@_aFE@_nME@_1TE@_peE@_epE@_f7E@_UGF@_hNF@_vUF@_KjF@_XqF@_y4F@_zBG@_OHG@_pVG@_2cG@_EkG@_RrG@_s5G@_6AH@_HIH@_iNH@_IRH@_iWH@_XhH@_+kH@_LsH@_ZzH@_N+H@_0BI_aFI_1KI@_PQI_2TI_cXI@_qeI_RiI_rnI@_FtI_swI_S0I@_g7I@_6AJ_hEJ_JRJ@_WYJ_9bJ_XhJ@_xmJ@_/tJ@_M1J_z4J_a8J@_nDK@_1KK@_CSK_pVK_QZK@_egK@_EkK_SrK_5uK@_G2K@_g7K@_6AL@_vLL@_8SL@_KaL@_XhL@_+kL_loL_yvL_ZzL_A3L@_O+L@_bFM_CJM_pMM@_2TM@_EbM@_qeM_RiM_fpM@_swM@_G2M@_H/M@_iNN@_XYN_+bN_kfN@_ZqN@_mxN@_04N@_bFO_CJO_cOO@_qVO@_3cO@";
        if(filename == "chunk-2842217-3-1.chunk") return "_FkO@_snO@_SrO@_twO@_H2O@_7AP@_jNP@_9SP@_xdP@_/kP@_moP@_azP@_n6P@_1BQ@_PHQ@_QQQ@_3TQ@_eXQ@_reQ@_5lQ@_TrQ_twQ_U0Q@_63Q_h7Q_vCR@_8JR@_KRR@_XYR@_ydR@_MjR@_AuR@_azR@_o6R@_CAS@_pDS@_ESS@_RZS@_GkS@_snS@_6uS@_73S@_i7S@_8AT@_JIT@_+ST@_ydT@_ZhT@_/kT@_AuT@_o6T@_2BU@_cFU@_qMU@_FbU@_SiU@_gpU@_73U@_WGV@_xUV@_MjV@_zmV@_aqV@_BuV@_nxV@_O1V@_14V@_CAW@_eOW@_ESW@_SZW@_5cW@_fgW@_UrW@_hyW@_v5W@_KIX@_xLX@_+SX@_lWX@_MaX@_AlX@_OsX@_bzX@_p6X@_2BY@_EJY@_RQY@_4TY@_seY@_6lY@_HtY@_iyY@_83Y@_i7Y@_wCZ@_+JZ@_ZYZ@_mfZ@_0mZ@_P1Z@_Q+Z@";
        if(filename == "chunk-2842217-3-2.chunk") return "_FSa@_gga@_7ua@_V0a@_W9a@_kEb@_xLb@_/Sb@_ahb@_nob@_Osb@_oxb@_Q+b@_eFc@_rMc@_5Tc@_TZc@_6cc@_Uic@_hpc@_7uc@_xCd@_MRd@_Acd@_bqd@_d8d_RHe_fOe@_5Te@_TZe@_6ce@_7le@_Ite@_vwe@_W0e@_w5e@_xCf@_YGf@_lNf@_mWf@_0df@_ahf@_1mf@_Cuf@_D3f@_d8f@_3Bg@_SHg@_sMg@_SQg@_gXg@_Hbg@_ueg@_7lg@_Vrg@_jyg@_93g@_K/g@_YGh@_mNh@_aYh@_0dh@_bhh@_1mh@_Cuh@_D3h@_e8h@_4Bi@_eFi@_sMi@_GSi@_gXi@_hgi@_vni@_8ui@_K2i@_X9i@_lEj@_MIj@_yLj@_ATj@_Oaj@_Duj@_UZk@_vnk@_ipk@_ZGl@_MIl@_Oal@_ofl@_Cll@_pol@_FAm@_tMm@_6Tm@_hXm@_7cm@_8lm@_X0m@_x5m_/An_MIn@";
        if(filename == "chunk-2842217-3-3.chunk") return "_aPn@_nWn@_Cln@_Qsn@_ezn@_44n@_5Bo@_fFo@_UQo@_6To@_veo@_Kto@_xwo@_X0o@_M/o@_ZGp@_AKp_ORp_0Up@_bYp@_pfp@_2mp@_dqp@_R1p@_GAq@_sDq@_6Kq@_HSq@_VZq@_igq@_Jkq@_Ktq@_kyq@_Z9q@_mEr@_AKr@_aPr@_BTr@_Par@_pfr@_Dlr@_F3r@_f8r@_tDs@_THs@_6Ks@_UQs@_7Ts@_Jbs@_Wis@_kps@_xws@_L2s@_M/s@_aGt@_nNt@_1Ut_Cct_Qjt@_eqt@_rxt_f8t_UHu@_6Ku_hOu_Z9u@_nEv@_ORv@_Elv@_Rsv@_s6v@_UHw@_wew@_+lw@_Z0w@_m7w@_BKx@_PRx@_qfx@_3mx@_sxx@_S1x_HAy_JSy@_jXy@_Kby@_kgy@_+ly@_Lty@_M2y@_a9y@_nEz@_1Lz@_CTz@_pWz@_ehz@_5vz@_t6z@_U+z@_6B0@_IJ0@_wV0@_Kb0@_lp0@_Z00@";
        if(filename == "chunk-2842217-3-4.chunk") return "_A40@_0C1@_bG1@_2U1@_eh1@_4m1@_sx1_641_HA2@_iO2@_JS2@_wV2@_+c2@_Zr2@_/u2@_N22@_052@_a92@_PI3@_cP3@_3d3@_Fl3@_so3@_Gu3@_gz3@_H33@_7B4@_iF4@_jO4@_Kb4@_Yi4_mp4_Mt4@_n74@_PI5@_pN5@_QR5@_eY5@_rf5@_5m5@_fq5@_Gu5@_U15@_h85@_vD6@_8K6@_KS6@_XZ6@_+c6@_lg6@_yn6@_Av6@_O26@_O/6@_1C7@_PI7@_ET7@_Ra7@_so7@_6v7@_H37@_iF8@_+T8@_Zi8@_Av8@_P/8@_pE9@_QI9@_Fc9@_Gl9@_ux9@_749@_i89@_8B+@_kO+@_xV+@_mg+@_Mk+@_zn+@_ar+@_Bv+@_O2+@";
    return "";
    }

    void writeConfig(IStringType key, IStringType value) {
    }

    IStringType readConfig(IStringType key) {
        return "";
    }
};
#else
class FS
{
public:
    FS() {
    }

    IVector<IStringType> listMetas() {
        IVector<IStringType> result;
        FILE *file = fopen("/metalist.bin", "r");
        if (file == NULL) {
#ifdef _PRINTF_FS_INFO_
            printf("Failed to open file\n");
            printf("Creating empty file\n");
#endif
            file = fopen("/metalist.bin", "w");
            fclose(file);
            // init empty list
            initF();

            // reopen
            file = fopen("/metalist.bin", "r");
        }
        // Read the file, line by line
        char *line = NULL;
        size_t len = 0;
        ssize_t read;
        while ((read = getline(&line, &len, file)) != -1) {
#ifdef _PRINTF_FS_INFO_
            printf("Retrieved line of length %zu:\n", read);
            printf("%s", line);
#endif
            
            // remove newline
            line[read - 1] = '\0';
            result.push_back(line);
        }
        fclose(file);
        if (line) {
            free(line);
        }

        return result;
    }

    void writeMetaFile(IStringType filename, IStringType meta) {
#ifdef _PRINTF_FS_INFO_
        printf("[FS] Writing meta file %s\n", filename.c_str());
#endif
        FILE *file = fopen("metalist.bin", "a");
        if (file == NULL) {
#ifdef _PRINTF_FS_INFO_
            printf("Failed to open file\n");
#endif
            return;
        }
        fprintf(file, "%s\n", filename.c_str());
        fclose(file);

        std::ostream *os = new std::ofstream(filename.c_str());
        *os << meta;
        delete os;
    }

    void writeChunkFile(IStringType filename, IStringType chunk) {
#ifdef _PRINTF_FS_INFO_
        printf("[FS] Writing chunk file %s\n", filename.c_str());
#endif
        std::ostream *os = new std::ofstream(filename.c_str());
        *os << chunk;
        delete os;
    }

    IStringType get(IStringType filename) {
        std::ifstream t(filename.c_str());
        std::string str((std::istreambuf_iterator<char>(t)),
                        std::istreambuf_iterator<char>());
        return str;
    }

    void initF()
    {
#ifdef _PRINTF_FS_INFO_
        printf("Writing initial file\n");
#endif
        // Yoru ni Kakeru
        writeMetaFile("map-2842217.meta", "__TrPF__p51K_OYoru ni Kakeru_HYOASOBI_HYOASOBI_G_H_G_F_oI_nJ_FJ_gH");
        writeChunkFile("chunk-2842217-0-0.chunk", "__OQ@__pe__2l__Et@__f7@_6JB@_VYB@_wmB@_L1B@_Z8B@_mDC@_BSC_PZC_cgC@_qnC@_3uC@_F2C@_S9C@_gED@_uLD@_JaD@_YzD_L1D_/2D@_M+D@_aFE@_nME@_1TE@_CbE@_QiE@_epE@_rwE@_F2E@_G/E@_UGF@_IRF@_8bF@_KjF@_XqF@_lxF@_AAG@_nDG@_bOG@_pVG@_2cG@_dgG@_epG@_fyG@_s5G@_hEH@_HIH@_VPH@_iWH@_wdH@_+kH@_LsH@_ZzH_/2H_m6H@_0BI@_BJI@_PQI@_2TI@_QZI@_qeI@_3lI@_FtI@_fyI@_G2I@_g7I@_uCJ@_7JJ@_JRJ@_WYJ@_kfJ@_xmJ@_YqJ@_M1J@_z4J@_N+J@_nDK_OHK_cOK_CSK_pVK@_DbK@_egK@_rnK@_5uK@_G2K@_g7K@_6AL@_hEL@_vLL_WPL_8SL@_KaL@_XhL@_loL_MsL_yvL@_A3L@_0BM_bFM_pMM_PQM_2TM@_dXM_EbM_RiM_4lM_fpM_GtM");
        writeChunkFile("chunk-2842217-0-1.chunk", "_swM@_63M@_H/M_uCN_VGN_8JN_JRN@_wUN@_+bN@_LjN_ymN_ZqN@_/tN@_N1N_04N_a8N@_BAO@_PHO@_cOO@_qVO@_EbO@_FkO@_snO@_SrO@_gyO@_h7O@_I/O@_iEP@_JIP@_vLP@_WPP@_9SP@_kWP@_+bP@_LjP@_MsP@_zvP@_B3P@_CAQ@_CJQ@_pMQ@_QQQ@_3TQ@_eXQ@_EbQ@_SiQ@_5lQ@_GtQ@_U0Q@_h7Q_I/Q_vCR@_JIR@_jNR@_KRR@_xUR@_XYR@_lfR@_ymR@_zvR@_O1R@_04R@_O+R@_1BS@_ESS@_RZS@_fgS@_snS@_6uS@_H2S@_u5S@_8AT@_iET@_wLT@_+ST@_LaT@_ZhT@_moT@_0vT@_B3T@_P+T@_DJU@_qMU@_3TU@_FbU@_5lU@_uwU@_73U@_J/U@_WGV@_kNV@_xUV@_/bV@_MjV@_aqV@_nxV@_14V@_CAW@_QHW@_3KW@_eOW@_ESW@_rVW@_5cW@_GkW@");
        writeChunkFile("chunk-2842217-0-2.chunk", "_UrW@_hyW@_I2W@_v5W@_8AX@_KIX@_XPX@_lWX@_ydX@_AlX@_OsX@_bzX@_p6X@_P+X@_dFY@_EJY@_RQY@_fXY@_seY@_6lY@_hpY@_HtY@_uwY@_i7Y@_wCZ@_KIZ@_LRZ@_ZYZ@_mfZ@_0mZ@_BuZ@_P1Z@_c8Z@_qDa@_3Ka@_FSa@_SZa@_gga@_una@_7ua@_J2a@_j7a@_KIb@_xLb@_LRb@_Mab@_nob@_Cub@_C3b@_Q+b@_eFc@_rMc@_5Tc@_Gbc@_Uic@_hpc@_vwc@_83c@_K/c@_XGd@_yUd@_Acd@_Ojd@_bqd@_pxd@_24d@_EAe@_FJe@_fOe@_sVe@_6ce@_Uie@_Vre@_iye@_w5e@_+Af@_LIf@_MRf@_mWf@_0df@_Blf@_Psf@_czf@_q6f@_3Bg@_SHg@_SQg@_5Tg@_gXg@_ueg@_7lg@_Jtg@_W0g@_k7g@_K/g@_xCh@_/Jh@_MRh@_aYh@_nfh@");
        writeChunkFile("chunk-2842217-0-3.chunk", "_ooh@_Cuh@_Q1h@_34h@_e8h@_rDi@_5Ki@_TQi@_UZi@_6ci@_Vii@_Wri@_wwi@_K2i@_X9i@_+Aj@_yLj@_ATj@_Oaj@_ofj@_Ojj@_dzj@_E3j@_rDk@_SHk@_gOk@_6Tk@_7ck@_9uk@_+3k@_+Al@_mNl@_2ml@_2vl@_E3l@_r6l@_fFm@_Vim@_vnm@_Ktm@_/An@_MIn@_mNn@_0Un@_1dn@_Cln@_Qsn@_3vn@_ezn@_r6n@_5Bo@_GJo@_UQo@_hXo@_veo@_8lo@_Kto@_X0o@_l7o@_yCp@_AKp@_ORp@_bYp@_pfp@_2mp@_Eup@_R1p@_44p@_sDq@_6Kq@_HSq@_iXq@_Ibq@_igq@_wnq@_+uq@_+3q@_Z9q@_zCr@_NIr@_nNr@_1Ur@_Par@_chr@_qor@_3vr@_F3r@_S+r@_gFs@_HJs@_uMs@_7Ts@_Jbs@_Wis@_kps@_xws@_/3s@_M/s@_aGt@_BKt@");
        writeChunkFile("chunk-2842217-0-4.chunk", "_nNt@_1Ut@_Cct@_Qjt@_eqt@_rxt@_54t@_f8t_tDu_UHu@_Xru@_y5u@_OIv@_pWv@_2dv@_Elv@_fzv@_6Bw@_VQw@_wew@_+lw@_Ltw@_lyw@_m7w@_0Cx@_oNx@_cYx@_Dcx@_3mx@_Ssx@_S1x@_HAy_7Ky_JSy@_vVy@_WZy@_9cy@_kgy@_xny@_/uy@_M2y@_a9y@_BBz@_nEz@_cPz@_CTz@_Qaz@_ehz@_roz@_5vz@_G3z@_U+z@_hF0@_7K0@_WQ0@_8T0@_Kb0@_Xi0@_lp0@_yw0@_A40@_O/0@_bG1@_pN1@_2U1@_Ec1@_Rj1@_fq1@_sx1@_641@_HA2@_uD2@_VH2@_iO2@_JS2@_wV2@_+c2@_Lk2@_Zr2@_my2@_052@_BB3@_PI3@_cP3@_qW3@_3d3@_Fl3@_Ss3@_gz3@_u63@_7B4@_JJ4@_9T4@_Kb4@_xe4@_/l4@_Mt4@_a04@_n74@_1C5@_CK5@_QR5@");
        writeChunkFile("chunk-2842217-0-5.chunk", "_3U5@_Ec5@_Fl5@_so5@_tx5@_U15_h85_vD6@_JJ6@_KS6@_XZ6@_lg6@_Mk6@_yn6@_Nt6@_zw6@_O26@_b96@_1C7@_PI7@_ET7@_Ra7@_fh7@_so7@_6v7@_V+7@_iF8@_wM8@_Lb8@_Zi8@_mp8@_0w8@_B48@_P/8@_cG9@_qN9@_3U9@_Fc9@_sf9@_Sj9@_gq9@_749@_JA+@_WH+@_kO+@_xV+@_/c+@_mg+@_Mk+@_zn+@_ar+@_0w+@_b0+@");
        writeChunkFile("chunk-2842217-1-0.chunk", "__OQ@__43@_sCB@_uUB@_8bB@_kxB@_y4B@_0KC@_BSC@_PZC_cgC_RrC@_eyC_F2C_tCD@_uLD@_7SD@_vdD_jfD_WhD_KjD_wmD@_koD_XqD_+tD_xvD_YzD@_/2D@_M+D@_nME@_OQE@_oVE@_CbE@_QiE@_epE@_rwE@_53E@_tCF@_6JF@_7SF@_WYF@_jfF@_KjF@_xmF@_XqF@_yvF@_M1F@_Z8F@_AAG@_OHG@_oMG@_CSG@_PZG@_qeG@_QiG@_3lG@_RrG@_fyG@_s5G@_T9G@_UGH@_7JH@_VPH@_vUH@_JaH@_XhH_+kH_YqH_+tH_ZzH@_m6H@_0BI@_OHI@_oMI@_PQI@_pVI@_qeI@_3lI@_FtI@_S0I@_53I_g7I_H/I@_uCJ@_IIJ@_iNJ@_JRJ@_WYJ@_wdJ@_+kJ@_LsJ@_mxJ_M1J_a8J@_0BK@_OHK@_1KK@_CSK@_QZK@_egK@_EkK@_SrK_5uK_fyK@_hEL@_7JL@_8SL_jWL_KaL@_xdL_+kL_loL@_MsL@");
        writeChunkFile("chunk-2842217-1-1.chunk", "_ZzL@_n6L_O+L_bFM@_pMM@_PQM@_EbM@_RiM@_4lM@_GtM@_63M@_H/M@_VGN@_vLN_WPN_wUN@_+bN_kfN_LjN@_loN@_zvN@_04N_a8N_PHO@_DSO_qVO_RZO@_3cO@_egO@_4lO@_fpO@_U9O@_uCP@_VGP@_vLP@_KRP@_XYP@_xdP@_YhP@_/kP@_mxP@_n6P@_oDQ@_2KQ@_dOQ@_qVQ@_EbQ@_reQ@_5lQ@_fpQ@_GtQ@_U0Q_63Q_h7Q@_I/Q_vCR_WGR@_8JR@_KRR@_kWR@_+bR@_MjR@_ZqR@_AuR@_nxR@_o6R@_CAS@_pDS@_ESS@_eXS@_4cS@_GkS@_TrS@_hyS@_H2S@_V9S@_iET@_JIT@_XPT@_+ST@_kWT@_ydT@_/kT@_NsT@_azT@_14T@_P+T@_2BU@_2KU@_RQU@_eXU@_FbU@_seU@_gpU@_HtU@_uwU@_i7U@_vCV@_WGV@_9JV@_KRV@_YYV@_mfV@_zmV@_BuV@_C3V@_c8V@");
        writeChunkFile("chunk-2842217-1-2.chunk", "_CAW@_pDW@_QHW@_qMW@_rVW@_SZW@_seW@_TiW@_tnW@_6uW@_73W@_W9W@_8AX@_jEX@_xLX@_+SX@_YYX@_/bX@_ZhX@_AlX@_noX@_OsX@_bzX@_C3X@_P+X@_2BY@_eOY@_4TY@_GbY@_TiY@_hpY@_HtY@_83Y@_J/Y@_jEZ@_+JZ@_LRZ@_ZYZ@_/bZ@_ahZ@_BuZ@_P1Z@_c8Z@_2Ba@_RHa@_eOa@_sVa@_5ca@_una@_uwa@_83a@_W9a@_kEb@_xLb@_YPb@_/Sb@_ZYb@_zdb@_Blb@_bqb@_1vb@_C3b@_3Bc@_EJc@_FSc@_sVc@_Gbc@_ggc@_6lc@_vwc@_83c@_K/c@_lNd@_ZYd@_Acd@_Ojd@_pxd@_24d@_RHe@_4Ke@_GSe@_gXe@_Gbe@_hge@_Hke@_ipe@_8ue@_93e@_j7e@_K/e@_kEf@_+Jf@_ZPf@_/Sf@_aYf@_Acf@_nff@_Blf@_oof@_Psf@");
        writeChunkFile("chunk-2842217-1-3.chunk", "_pxf@_D3f@_d8f@_3Bg@_eFg@_FJg@_fOg@_GSg@_tVg@_6cg@_Uig@_Blg@_ipg@_8ug@_K2g@_k7g@_xCh@_/Jh@_ZPh@_ATh@_mWh@_Bch@_Ojh@_1mh@_cqh@_pxh@_Q1h@_q6h@_R+h@_rDi@_SHi@_GSi@_tVi@_Hbi@_hgi@_Iki@_vni@_8ui@_W0i@_93i@_k7i@_yCj@_MIj@_yLj@_ATj@_nWj@_0dj@_Clj@_cqj@_2vj@_Q1j@_e8j@_4Bk@_GJk@_sMk@_TQk@_tVk@_uek@_Ikk@_Jtk@_jyk@_K2k@_k7k@_yCl@_zLl@_ATl@_Pjl@_Qsl@_R1l@_e8l@_sDm@_SHm@_5Km@_TQm@_Ibm@_igm@_Jkm@_Wrm@_9um@_kym@_K2m@_mEn@_AKn@_aPn@_nWn@_Oan@_chn@_pon@_3vn@_E3n@_S+n@_fFo@_tMo@_6To@_Ibo@_8lo@_jpo@_xwo@_X0o@_l7o@");
        writeChunkFile("chunk-2842217-1-4.chunk", "_AKp@_nNp@_ORp_bYp_Pjp@_dqp@_qxp@_f8p@_sDq@_THq@_hOq@_HSq@_uVq@_8cq@_igq@_wnq@_Xrq@_+uq@_L2q@_y5q@_Z9q@_/Ar@_mEr@_AKr@_nNr@_BTr@_oWr@_Par@_2dr@_chr@_qor@_Eur@_ezr@_S+r@_5Bs@_gFs@_6Ks@_UQs@_7Ts@_ves@_9ls@_Kts@_Y0s@_m7s@_0Lt@_ORt@_1Ut@_Cct@_pft@_3mt@_eqt@_rxt@_GAu@_UHu_6Ku_ywu@_y5u@_ABv@_OIv@_bPv@_Pav@_eqv@_fzv@_T+v@_VQw@_iXw@_Xiw@_Ltw@_Z0w@_/3w@_N/w@_0Cx@_BKx@_bPx@_2Ux@_qfx@_Elx@_eqx@_Fux@_8Ty@_jXy@_wey@_Xiy@_lpy@_/uy@_myy@_z5y@_0Cz@_OIz@_1Lz@_PRz@_2Uz@_3dz@_ehz@_Elz@_Ssz@_fzz@_G3z@_t6z@_6B0@_VH0@_vM0@");
        writeChunkFile("chunk-2842217-1-5.chunk", "_8T0@_jX0@_xe0@_+l0@_Mt0@_Z00@_A40@_n70@_O/0@_0C1@_CK1@_pN1@_2U1@_dY1@_qf1@_4m1@_Gu1@_sx1@_T11@_h81@_uD2@_8K2@_WQ2@_9T2@_Kb2@_kg2@_yn2@_/u2@_N22@_n72@_BB3@_oE3@_2L3@_2U3@_Ra3@_eh3@_Fl3@_so3@_Ss3@_5v3@_T13@_u63@_U+3@_iF4@_vM4@_WQ4@_kX4@_xe4@_mp4@_zw4@_B44@_O/4@_cG5@_2L5@_3U5@_eY5@_rf5@_Sj5@_5m5@_Ts5@_6v5@_gz5@_645@_h85@_IA6@_iF6@_jO6@_KS6@_xV6@_+c6@_lg6@_/l6@_Zr6@_Av6@_ny6@_056@_b96@_CB7@_3U7@_fh7@_so7@_Ts7@_tx7@_H37@_V+7@_iF8@_JJ8@_jO8@_+T8@_Lb8@_Zi8@_mp8@_Nt8@_a08@_o78@_CB9@_qN9@_3U9@_4d9@_fh9@");
        writeChunkFile("chunk-2842217-1-6.chunk", "_to9@_Hu9@_U19@_u69@_JA+@_jF+@_9K+@_KS+@_YZ+@_Zi+@_Am+@_mp+@_ar+@_0w+@_O2+@");
        writeChunkFile("chunk-2842217-2-0.chunk", "__Cb@__2l__Et__ey@__f7@_TGB@_UPB@_ifB@_KsB@_Z8B@_mDC@_aOC@_2lC@_F2C_s5C_S9C@_HID@_7SD@_lxD@_/2D@_Z8D@_zBE@_BJE@_bOE@_CSE@_PZE@_3lE@_EtE@_eyE@_53E@_G/E@_UGF@_hNF@_vUF@_8bF@_9kF@_koF@_LsF@_lxF@_y4F@_AAG@_aFG@_BJG@_bOG@_2cG@_EkG@_4uG@_G2G@_HIH@_uLH@_8SH@_iWH@_wdH@_+kH_koH_+tH@_lxH@_z4H@_a8H@_AAI@_aFI@_BJI@_bOI@_cXI@_DbI_qeI_EkI_rnI_SrI@_g7I_H/I_hEJ@_7JJ@_VPJ@_vUJ_WYJ_kfJ@_xmJ_YqJ_/tJ@_M1J@_m6J@_BAK@_nDK@_CJK@_oMK@_CSK@_dXK_DbK_qeK@_rnK@_5uK_fyK_T0K@_IIL@_vLL@_WYL@_loL@_/tL@_N1L@_z4L@_O+L@_CJM@_DSM@_3cM@_RiM@_rnM@_swM@_T0M@_h7M_H/M");
        writeChunkFile("chunk-2842217-2-1.chunk", "_iNN@_9SN@_+bN@_YhN@_ZqN@_mxN@_04N@_BAO_oDO_2KO_cOO_QQO@_qVO_RZO_SiO@_4lO@_fpO@_5uO@_T0O@_u5O@_7AP@_JIP@_jNP@_KRP@_KaP@_lfP@_LjP@_MsP@_azP@_n6P_O+P_1BQ@_cFQ@_CJQ@_QQQ@_eXQ@_4cQ@_SiQ@_fpQ@_twQ_U0Q_vCR@_WPR@_xUR@_XYR@_lfR@_ymR@_ZqR@_AuR@_nxR@_O1R@_04R@_O+R@_1BS@_qVS@_4cS@_fgS@_snS@_TrS@_hyS@_H2S@_V9S@_iET@_wLT@_KRT@_kWT@_LaT@_ZhT@_moT@_0vT@_azT@_B3T@_P+T@_cFU@_qMU@_RQU@_3TU@_eXU@_seU@_SiU@_tnU@_HtU@_U0U@_73U@_J/U@_jEV@_9JV@_kNV@_xUV@_/bV@_mfV@_MjV@_aqV@_nxV@_O1V@_14V@_P+V@_pDW@_3KW@_eOW@_5cW@_GkW@_tnW@_UrW@_6uW@");
        writeChunkFile("chunk-2842217-2-2.chunk", "_v5W@_W9W@_jEX@_KIX@_XPX@_lWX@_ydX@_ZhX@_noX@_0vX@_C3X@_2BY@_dFY@_qMY@_RQY@_fXY@_seY@_6lY@_UrY@_iyY@_W9Y@_9AZ@_kNZ@_yUZ@_MaZ@_zdZ@_AlZ@_aqZ@_oxZ@_24Z@_qDa@_3Ka@_FSa@_SZa@_tea@_Hka@_Ura@_7ua@_iya@_J2a@_9Ab@_lNb@_yUb@_Mab@_ahb@_nob@_1vb@_P1b@_p6b@_eFc@_SQc@_5Tc@_fXc@_tec@_Uic@_W0c@_W9c@_lNd@_yUd@_nfd@_ood@_Cud@_D3d@_RHe@_rMe@_SQe@_sVe@_tee@_Hke@_une@_Vre@_iye@_J2e@_X9e@_+Af@_LIf@_yLf@_ZPf@_zUf@_Naf@_Ojf@_bqf@_2vf@_czf@_q6f@_R+f@_EAg@_FJg@_sMg@_SQg@_TZg@_hgg@_Ikg@_ung@_Jtg@_vwg@_W0g@_w5g@_+Ah@_lEh@");
        writeChunkFile("chunk-2842217-2-3.chunk", "_LIh@_yLh@_MRh@_zUh@_Nah@_nfh@_Clh@_Psh@_2vh@_dzh@_34h@_EAi@_5Ki@_6Ti@_UZi@_uei@_7li@_ipi@_Jti@_jyi@_x5i@_X9i@_lEj@_/Jj@_ZPj@_zUj@_aYj@_Bcj@_1mj@_Psj@_qxj@_34j@_R+j@_5Kk@_GSk@_hXk@_igk@_8lk@_Wrk@_wwk@_Y9k@_lEl@_/Jl@_aPl@_0Ul@_aYl@_1dl@_bhl@_cql@_qxl@_S+l@_GJm@_HSm@_UZm@_uem@_jpm@_wwm@_+3m@_/An@_MIn@_zLn@_BTn@_nWn@_1dn@_Qsn@_qxn@_R1n@_r6n@_S+n@_5Bo@_GJo@_tMo@_UQo@_hXo@_Wio@_jpo@_Kto@_+3o@_yCp_AKp_Ccp@_Pjp@_2mp@_Eup@_qxp@_R1p@_44p@_f8p@_GAq@_THq@_UQq@_7Tq@_VZq@_veq@_Wiq@_9lq@_xwq@_L2q@_l7q@_M/q@_NIr@");
        writeChunkFile("chunk-2842217-2-4.chunk", "_0Lr@_aPr@_bYr@_Ccr@_pfr@_qor@_3vr@_F3r@_GAs@_gFs@_HJs@_uMs@_ISs@_iXs@_Wis@_wns@_xws@_/3s@_M/s@_zCt@_nNt@_CTt@_cYt@_dht@_Eut@_54t@_tDu_UHu_Xru@_M2u@_0Lv@_pWv@_dhv@_4vv@_G3v@_6Bw@_uMw@_Jbw@_Kkw@_kpw@_yww@_/3w@_ABx@_aGx@_PRx@_cYx@_2dx@_Rjx@_Fux@_sxx@_7Ky_WZy_9cy@_Kky@_xny@_Yry@_/uy@_M2y@_z5y@_BBz@_nEz@_OIz@_cPz@_CTz@_Qaz@_3dz@_roz@_Fuz@_fzz@_U+z@_hF0@_vM0@_WQ0@_Kb0@_xe0@_Xi0@_lp0@_yw0@_N20@_n70@_oE1@_CK1@_PR1@_dY1@_Ec1@_Rj1@_fq1_sx1_641@_h81@_HA2@_VH2@_8K2@_iO2@_wV2@_XZ2@_kg2@_Lk2@_yn2@_Zr2@_my2@_052@_O/2@");
        writeChunkFile("chunk-2842217-2-5.chunk", "_oE3@_PI3@_DT3@_qW3@_Ra3@_eh3@_5v3@_gz3@_643@_U+3@_7B4@_JJ4@_WQ4@_9T4@_kX4@_xe4@_Yi4@_Zr4@_zw4@_a04@_n74@_O/4@_1C5@_CK5@_dP5@_eY5@_Ec5@_eh5@_5m5@_Gu5@_U15@_h85@_7B6@_WH6@_8K6@_jO6@_xV6@_ye6@_Yi6@_mp6@_Av6@_a06@_o76@_1C7@_PI7@_ET7@_eY7@_4d7@_Gl7@_gq7@_Gu7@_U17@_u67@_8B8@_wM8@_KS8@_kX8@_ye8@_/l8@_0w8@_B48@_P/8@_2C9@_cG9@_2L9@_RR9@_eY9@_Fc9@_Sj9@_gq9@_ux9@_749@_V+9@_WH+@_xV+@_/c+@_Zi+@_Am+@_mp+@_0w+@_O2+@");
        writeChunkFile("chunk-2842217-3-0.chunk", "__oV@__pe@__Et@_6JB@_VYB@_JjB@_wmB@_L1B@_//B@_AJC@_PZC@_cgC@_3uC@_s5C_JaD_xvD@_L1D@_y4D@_aFE@_nME@_1TE@_peE@_epE@_f7E@_UGF@_hNF@_vUF@_KjF@_XqF@_y4F@_zBG@_OHG@_pVG@_2cG@_EkG@_RrG@_s5G@_6AH@_HIH@_iNH@_IRH@_iWH@_XhH@_+kH@_LsH@_ZzH@_N+H@_0BI_aFI_1KI@_PQI_2TI_cXI@_qeI_RiI_rnI@_FtI_swI_S0I@_g7I@_6AJ_hEJ_JRJ@_WYJ_9bJ_XhJ@_xmJ@_/tJ@_M1J_z4J_a8J@_nDK@_1KK@_CSK_pVK_QZK@_egK@_EkK_SrK_5uK@_G2K@_g7K@_6AL@_vLL@_8SL@_KaL@_XhL@_+kL_loL_yvL_ZzL_A3L@_O+L@_bFM_CJM_pMM@_2TM@_EbM@_qeM_RiM_fpM@_swM@_G2M@_H/M@_iNN@_XYN_+bN_kfN@_ZqN@_mxN@_04N@_bFO_CJO_cOO@_qVO@_3cO@");
        writeChunkFile("chunk-2842217-3-1.chunk", "_FkO@_snO@_SrO@_twO@_H2O@_7AP@_jNP@_9SP@_xdP@_/kP@_moP@_azP@_n6P@_1BQ@_PHQ@_QQQ@_3TQ@_eXQ@_reQ@_5lQ@_TrQ_twQ_U0Q@_63Q_h7Q_vCR@_8JR@_KRR@_XYR@_ydR@_MjR@_AuR@_azR@_o6R@_CAS@_pDS@_ESS@_RZS@_GkS@_snS@_6uS@_73S@_i7S@_8AT@_JIT@_+ST@_ydT@_ZhT@_/kT@_AuT@_o6T@_2BU@_cFU@_qMU@_FbU@_SiU@_gpU@_73U@_WGV@_xUV@_MjV@_zmV@_aqV@_BuV@_nxV@_O1V@_14V@_CAW@_eOW@_ESW@_SZW@_5cW@_fgW@_UrW@_hyW@_v5W@_KIX@_xLX@_+SX@_lWX@_MaX@_AlX@_OsX@_bzX@_p6X@_2BY@_EJY@_RQY@_4TY@_seY@_6lY@_HtY@_iyY@_83Y@_i7Y@_wCZ@_+JZ@_ZYZ@_mfZ@_0mZ@_P1Z@_Q+Z@");
        writeChunkFile("chunk-2842217-3-2.chunk", "_FSa@_gga@_7ua@_V0a@_W9a@_kEb@_xLb@_/Sb@_ahb@_nob@_Osb@_oxb@_Q+b@_eFc@_rMc@_5Tc@_TZc@_6cc@_Uic@_hpc@_7uc@_xCd@_MRd@_Acd@_bqd@_d8d_RHe_fOe@_5Te@_TZe@_6ce@_7le@_Ite@_vwe@_W0e@_w5e@_xCf@_YGf@_lNf@_mWf@_0df@_ahf@_1mf@_Cuf@_D3f@_d8f@_3Bg@_SHg@_sMg@_SQg@_gXg@_Hbg@_ueg@_7lg@_Vrg@_jyg@_93g@_K/g@_YGh@_mNh@_aYh@_0dh@_bhh@_1mh@_Cuh@_D3h@_e8h@_4Bi@_eFi@_sMi@_GSi@_gXi@_hgi@_vni@_8ui@_K2i@_X9i@_lEj@_MIj@_yLj@_ATj@_Oaj@_Duj@_UZk@_vnk@_ipk@_ZGl@_MIl@_Oal@_ofl@_Cll@_pol@_FAm@_tMm@_6Tm@_hXm@_7cm@_8lm@_X0m@_x5m_/An_MIn@");
        writeChunkFile("chunk-2842217-3-3.chunk", "_aPn@_nWn@_Cln@_Qsn@_ezn@_44n@_5Bo@_fFo@_UQo@_6To@_veo@_Kto@_xwo@_X0o@_M/o@_ZGp@_AKp_ORp_0Up@_bYp@_pfp@_2mp@_dqp@_R1p@_GAq@_sDq@_6Kq@_HSq@_VZq@_igq@_Jkq@_Ktq@_kyq@_Z9q@_mEr@_AKr@_aPr@_BTr@_Par@_pfr@_Dlr@_F3r@_f8r@_tDs@_THs@_6Ks@_UQs@_7Ts@_Jbs@_Wis@_kps@_xws@_L2s@_M/s@_aGt@_nNt@_1Ut_Cct_Qjt@_eqt@_rxt_f8t_UHu@_6Ku_hOu_Z9u@_nEv@_ORv@_Elv@_Rsv@_s6v@_UHw@_wew@_+lw@_Z0w@_m7w@_BKx@_PRx@_qfx@_3mx@_sxx@_S1x_HAy_JSy@_jXy@_Kby@_kgy@_+ly@_Lty@_M2y@_a9y@_nEz@_1Lz@_CTz@_pWz@_ehz@_5vz@_t6z@_U+z@_6B0@_IJ0@_wV0@_Kb0@_lp0@_Z00@");
        writeChunkFile("chunk-2842217-3-4.chunk", "_A40@_0C1@_bG1@_2U1@_eh1@_4m1@_sx1_641_HA2@_iO2@_JS2@_wV2@_+c2@_Zr2@_/u2@_N22@_052@_a92@_PI3@_cP3@_3d3@_Fl3@_so3@_Gu3@_gz3@_H33@_7B4@_iF4@_jO4@_Kb4@_Yi4_mp4_Mt4@_n74@_PI5@_pN5@_QR5@_eY5@_rf5@_5m5@_fq5@_Gu5@_U15@_h85@_vD6@_8K6@_KS6@_XZ6@_+c6@_lg6@_yn6@_Av6@_O26@_O/6@_1C7@_PI7@_ET7@_Ra7@_so7@_6v7@_H37@_iF8@_+T8@_Zi8@_Av8@_P/8@_pE9@_QI9@_Fc9@_Gl9@_ux9@_749@_i89@_8B+@_kO+@_xV+@_mg+@_Mk+@_zn+@_ar+@_Bv+@_O2+@");
    
        // Freedom DiVE
        writeMetaFile("map-4229428.meta", "__vhvH__0kIQ_PFreedom Dive EZ_PFreedom Dive EZ_HNo easy_C_D_E_D_SC_9D_EF_GE");
        writeChunkFile("chunk-4229428-0-0.chunk", "_z7B_M+B_mAC_ADC_aFC_0HC_OKC_oMC_BPC_bRC_1TC_PWC_QcC@_QiC@_EnC@_rqC@_rwC@_53C@_G/C@_uID@_7PD_8VD_WeD@_xsD_yyD_Z8D_oJE_DYE@_emE@_vRF@_jWF@_JaF@_CPG@_swG_GzG_g1G@_YkL_ZwL_b8L@_RfM_FkM_U9M@_jQN_kWN_moN_ArN_u2O@_I5O@_xaP@_/hP@_AoP@_ZqP@_0yP@_DMQ@_RZQ@_TlQ@_YVR_+YR_pAS_DDS_eRS@_ScS@_seS@_ToS@_tqS@_HtS@_hvS@_0sX@_iyY_9AZ_GVa@_Uia@_7ra@_83a@_+Jb@_mZb@_Bib@_0mb@_orb@_2yb@_Q1b@_rDc@_EGc@_eIc@_FMc@_Ufc@_7ic@_vtc@_96c@_bnd@_pud@_21d@_fLe@_Ihe@_uke@_Jze@_YAf@_Bif@_2sf@_q3f@_4+f@_rDg@_5Kg@_TTg@_8og@_Jwg@_X3g@_k+g@_yFh@_NUh@_nWh@_r6h_R+h_4Bi_SEi_5Ni@_UWi@_jvi@_6Ko@");
        writeChunkFile("chunk-4229428-0-1.chunk", "_ISo@_VZo@_9oo@_kso@_Lwo@_m+o@_zFp@_bVp@_pcp@_2jp@_Exp@_41p@_s6p@_5Bq@_hRq@_8fq@_+rq@_ZAr@_oNr@_1Ur@_PXr@_2gr@_3mr@_S1r@_TBs@_UNs@_WZs@_Jes@_kms@_yzs_PUt_Qat@_Syt@_57t@_hFu@_IJu@_iRu@_JVu@_jXu@_jdu_A+u_0Cv@_bGv@_1Ov@_PRv@_DWv_HAw_0/w@_BHx_Elx");
        writeChunkFile("chunk-4229428-1-0.chunk", "__Eq@__Rs__yy__G8@__s/_tFB_Z5B_z7B_QcC@_EtC@_uID@_7PD_8VD_WeD@_xsD_yyD_Z2D@_AAE@_OHE@_oPE@_dgE@_RlE@_rnE@_fyE@_s5E@_HCF@_hKF@_JUF@_jcF@_+kF@_koF@_LsF@_yvF@_MyF@_A3F@_N+F@_bFG@_oMG@_CPG@_cXG_DbG_RoG_FtG_swG_GzG_g1G@_YkL_ZwL_n3L_b8L_2KM_cOM_rbM_RfM_FkM_5oM_I8M@_8MN_jQN_kWN_LaN_/kN_moN_ArN_ZtN_n0N_B3N_o6N_B9N_PEO_QKO_EVO_rYO_fjO_GnO_HzO@_h1O@_U6O@_u8O@_wIP@_JLP@_9PP@_+bP@_MjP@_AoP@_ZqP@_0yP@_04P@_pJQ@_DMQ@_RZQ@_TlQ@_ttQ_U3Q_WJR_9MR_xRR_YVR_+YR_lcR_MgR_zjR_nuR_OyR_C9R_pAS_DDS_pGS_RQS@_4TS@_FbS@_5fS@_GnS@_tqS@_HtS@_hvS@_0sX@_TfY_iyY_9AZ_yRZ_GVa@_sYa@_6fa@_hja@_hpa@_Ita@_w2a@_J5a@_K/a@_kBb@_xIb@");
        writeChunkFile("chunk-4229428-1-1.chunk", "_LLb@_mZb@_Acb@_0gb@_0mb@_orb@_pxb@_c2b@_eCc@_4Ec@_eIc@_FMc@_GSc@_5Wc@_Hec@_7ic@_isc@_Jwc@_j4c@_96c@_AZd_ncd_Omd@_ood@_ctd@_2vd@_p0d@_sMe@_hde@_uke@_x8e@_L/e@_zOf@_mTf@_bef@_0gf@_Ojf@_prf@_cwf_2yf_q3f@_4+f@_rDg@_5Kg@_TTg@_vng@_Jqg@_8ug@_Wxg@_K2g@_k4g@_Y9g@_x/g@_lEh@_/Gh@_mKh@_ATh@_nWh@_E3h_r6h_R+h_4Bi_SEi_5Hi_sMi@_HVi@_7Zi@_7fi@_Woi@_wqi@_jvi@_tJo@_HMo@_7Qo@_UTo@_IYo@_Wfo@_Kqo@_xto@_Yxo@_Z9o@_z/o@_mEp@_bPp@_BTp@_oWp@_cbp@_2dp@_pip@_Qmp@_qop@_Erp@_4vp@_r0p@_f5p@_57p@_sAq@_UQq@_7Tq@_weq@_Wiq@_xqq@_Yuq@_l1q@_/3q@");
        writeChunkFile("chunk-4229428-1-2.chunk", "_M/q@_zCr@_bMr@_oTr@_cYr@_pfr@_qlr@_eqr@_rxr@_f2r@_TBs@_UHs@_IMs@_hOs@_JYs@_jas@_8cs@_Wfs@_Xls@_M2s@_m+s@_aDt@_BHt@_BNt@_CTt@_DZt@_Fxt@_fzt@_s6t@_G9t@_UEu@_UKu@_VQu@_wYu@_Xiu@_+lu@_Yuu@_/xu@_Z6u@_nBv@_oHv@_oNv@_cSv@_pZv@_Qdv@_ekv@_Eov@_5yv@_g2v@_6+v@_hCw@_BBx@_0Fx@");
        writeChunkFile("chunk-4229428-2-0.chunk", "__jg__wl_/2B_Z5B_pYC@_2fC@_djC@_epC@_hND@_vUD@_jZD_WeD_9hD@_klD@_LpD@_lxD@_Z2D@_AAE@_nDE@_BME@_cUE@_pbE@_qhE@_EkE@_4oE@_fyE@_s5E@_HCF@_hKF@_JUF@_jcF@_+kF@_koF@_LsF@_yvF@_MyF@_A3F@_N+F@_bFG@_OKG@_CPG@_pSG_cXG_DbG_3fG_ejG_RoG_FtG_swG_g1G@_mrL_MvL_A0L_n3L_b8L@_CGM_2KM_cOM_QTM_EYM_rbM_5oM_gsM_GwM@_tzM@_U3M@_76M@_7AN@_WJN_8MN_LaN_xdN_YhN_/kN_ArN_ZtN_zvN_NyN_O4N@_O+N@_QQO_EVO_EbO@_4fO@_GnO_5rO_6xO@_79O@_VAP@_JFP@_jHP@_WMP@_wOP@_kTP@_YYP@_lfP@_mlP@_MpP@_mrP@_0yP@_O7P@_o9P@_CAQ@_pDQ@_DGQ@_RTQ@_SfQ@_snQ@_TrQ@_ttQ@_hyQ@_U3Q@_u5Q@_V9Q_JCR_wFR_WJR_9MR_xRR_lcR_MgR_zjR_ZnR_ArR_nuR_OyR_01R");
        writeChunkFile("chunk-4229428-2-1.chunk", "_b5R_C9R_pGS_qMS_EPS@_EVS@_4ZS@_GhS@_5lS@_7xS@_U0S@_u2S@_WGT_kNT_zgT_NpT_0sT_bwT_B0T_o3T_14T_P7T_2+T_GkU_0sX@_rPY_TfY_yRZ_1vZ_CxZ_czZ_p0Z_D3Z_Q4Z_p6Z_27Z_Q+Z_d/Z_3Ba_EDa_eFa_rGa_FPa_GVa@_5Za@_tea@_uka@_Uoa@_Vua@_j1a@_W6a@_w8a@_xCb@_kHb@_YMb@_mZb@_Ndb@_nfb@_bkb@_Opb@_Cub@_cwb@_p3b@_d8b@_3+b_eIc_FMc@_TTc@_GYc@_6cc@_Ikc@_Vrc@_Wxc@_W3c@_X9c@_x/c@_LCd@_ZVd_AZd_ncd_Ogd_0jd_Omd_1pd@_Psd@_Dxd@_czd@_d5d@_37d@_R+d@_4Be_FJe_5Ne@_Uce@_jve@_LFf@_MLf@_mNf@_mTf@_nZf@_bkf@_1mf@_cqf@_cwf_2yf_Q1f@_R7f@_RBg@_SHg@_gOg@_HYg@_img@_Wrg@_wtg@_jyg@_90g@_x5g@_L8g@_+Ah@_YDh@_MIh@_zLh@_zLh_ATh");
        writeChunkFile("chunk-4229428-2-2.chunk", "_zRh@_Oah@_0dh_bhh_1jh_cnh_Drh_2vh_dzh_E3h_GJi@_gLi@_gRi@_6Ti@_Ibi@_Ihi@_Jni@_wqi@_jvi@_5Eo@_gIo@_UNo@_uPo@_hUo@_7Wo@_Jeo@_who@_wno@_Xro@_+uo@_kyo@_Y3o@_/6o@_ABp@_ZDp@_NIp@_BNp@_oQp@_1Xp@_Pap@_Dfp@_chp@_Qmp@_qop@_Rsp@_rup@_ezp@_S4p@_G9p@_g/p@_TEq@_6Hq@_uMq@_IPq@_IVq@_jdq@_jjq@_kpq@_kvq@_Y0q@_M5q@_A+q@_AEr@_aGr@_OLr@_bSr@_pZr@_cer@_dkr@_eqr@_Eur@_ewr@_s3r@_g8r@_6Es@_hIs@_7Ks@_uPs@_8Ws@_wbs@_jgs@_Kks@_Lqs@_kss@_Lws@_lys@_Z3s@_Z9s@_nEt@_OIt@_0Lt@_OOt@_CTt@_2Xt@_2dt@_Qgt@_Elt@_ent@_Ert@_ett@_4vt@_s0t@_g5t@_T+t@");
        writeChunkFile("chunk-4229428-2-3.chunk", "_tAu@_HDu@_hLu@_IPu@_8Zu@_Khu@_Lnu@_Ltu@_Mzu@_M5u@_z8u@_aAv@_0Iv@_bMv@_pTv@_cYv@_dev@_Rjv@_Rpv@_sxv@_s3v@_t9v@_OCx@_oEx@");
        writeChunkFile("chunk-4229428-3-0.chunk", "__ey@__Y1__M9_AFB@_6IB@_0MB@_hQB@_IUB@_vXB@_WbB@_8eB@_jiB@_KmB@_xpB@_YtB@_+wB@_l0B_/2B_pYC@_2fC@_djC_epC_EtC_4xC_S0C_G5C_g7C_TAD_6DD_7JD_hND@_vUD@_jZD_WeD_9hD@_klD@_LpD@_lxD@_M7D@_nDE@_BME@_cUE@_pbE@_3iE@_EqE@_fyE_T3E_s5E_g+E_HCF_7GF_hKF_iQF_JUF_8YF_jcF_kiF_MyF_A3F_Z5F_N+F_nAG_bFG_0HG@_CPG@_3fG_ejG_RoG_FtG_swG_GzG_g1G@_MvL_A0L_b8L@_BAM_CGM_QTM_EYM_GwM@_U3M@_h+M@_7AN@_iEN_WJN_xdN_YhN_ArN_ZtN_O4N@_O+N@_1BO@_pGO@_QKO@_pMO_QQO_EbO@_4fO@_5rO_twO_iBP@_8DP@_XSP@_LXP@_YeP@_ZkP@_MpP@_mrP@_AuP@_awP@_0yP@_CAQ@_DGQ@_3QQ@_RTQ@_rVQ@_4cQ@_SfQ@_snQ@_TrQ@_70Q@_u5Q@_JCR_wFR_ZnR_ArR_01R_b5R_3NS@_RWS@_rYS@");
        writeChunkFile("chunk-4229428-3-1.chunk", "_TiS@_skS@_7xS@_U0S@_u2S@_v8S_WGT_kNT_zgT_NpT_0sT_bwT_oxT_B0T_o3T_14T_P7T_2+T_GkU_0sX@_DAY_rPY_fRa@_GVa@_Gba@_gda@_Ina@_vwa@_Jza@_j7a@_+Db@_YGb@_lNb@_/Pb@_ZSb@_zUb@_MXb@_mZb@_aeb@_bkb@_Opb@_Pvb@_24b@_d8b@_3+b_eIc_FMc@_gUc@_TZc@_tbc@_Ulc@_unc@_Iqc@_jyc@_J2c@_k+c@_+Ad@_YDd@_+Gd@_lKd@_MOd@_mQd_ZVd_Ogd_0jd_Crd@_Qyd@_Q4d@_E9d@_4Be_FJe_GPe@_sSe@_TWe@_6Ze@_Wue@_K5e@_YGf@_ZMf@_AWf@_aYf@_1mf@_Ppf@_Q1f@_R7f@_RBg@_SHg@_gOg@_HYg@_ubg@_Ufg@_7ig@_Vlg@_jsg@_wzg@_+6g@_LCh@_ZJh@_ANh@_mQh@_Oah@_bhh_1jh_cnh_Drh_2vh_dzh_TKi@_tSi@_Uci@_Vii@_8li@_wqi@_5Eo@_hOo@");
        writeChunkFile("chunk-4229428-3-2.chunk", "_uVo@_8co@_8io@_jmo@_kso@_Lwo@_xzo@_Y3o@_y5o@_MCp@_NIp_bPp_CZp@_Qgp@_etp@_Ryp@_F3p@_T+p@_TEq@_6Hq@_7Nq@_iXq@_8Zq@_Wcq@_wkq@_Xoq@_xwq@_Lzq@_Z6q@_z8q@_nHr@_BKr@_ORr@_2ar@_Qdr@_Qjr@_Rvr@_54r@_g8r@_6Es@_uJs@_7Qs@_VTs_lys_m4s@_M8s@_oKt@_bPt@_cVt_Wcu_9fu@_You@_+ru@_Z0u@_A4u@_z8u@_N/u_2Uv_QXv@_qfv@_Eiv@_eqv@_fwv@_54v@_g8v@_bDx@");
    
        // test
        writeMetaFile("map-2461679.meta", "__HOgE__v/YJ_Etest_Etest_Etest___B_________D______");
        writeChunkFile("chunk-2461679-1-0.chunk", "__+Q@__YU@__xX@");
    }

    void writeConfig(IStringType key, IStringType value) {
        // write to fs
        std::ofstream t("/config-" + key + ".kv");
        t << value;
        t.close();
    }

    ConfigData readConfig(IStringType key) {
        // read from fs
        // check if file exists
        std::ifstream t("/config-" + key + ".kv");
        if (!t.good()) {
            return ConfigData();
        }
        std::string str((std::istreambuf_iterator<char>(t)),
                        std::istreambuf_iterator<char>());
        t.close();
        return ConfigData(str);
    }
};
#endif

class ConfigInstance {
public:
    FS *fs;
    ConfigInstance(FS *fs) {
        this->fs = fs;
    }

    int getNoteSpeed() {
        ConfigData data = fs->readConfig("noteSpeed");
        if(data.hasData) return string2int(data.data);
        return 400;
    }
};