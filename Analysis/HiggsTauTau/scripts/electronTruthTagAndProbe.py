import ROOT
import glob
import sys
# import json
from array import array
import UserCode.ICHiggsTauTau.analysis as analysis

ROOT.RooWorkspace.imp = getattr(ROOT.RooWorkspace, 'import')
ROOT.TH1.AddDirectory(0)


bin_cfgs = [
    {
        'name': 'ID_pt_bins_inc_eta',
        'var': 'pt_p(99,10,1000)',
        'tag': 'gen_p && wt < 1.0',
        'probe': 'id_p',
        'binvar_x': 'pt_p',
        'bins_x': [10., 20., 25., 30., 40., 50., 100., 200., 1000.],
        'binvar_y': 'abs(sc_eta_p)',
        'bins_y': [0, 2.5]
    },
    {
        'name': 'ID_pt_eta_bins',
        'var': 'pt_p(99,10,1000)',
        'tag': 'gen_p && wt < 1.0',
        'probe': 'id_p',
        'binvar_x': 'pt_p',
        'bins_x': [10., 20., 25., 30., 40., 50., 100., 200., 1000.],
        'binvar_y': 'abs(sc_eta_p)',
        'bins_y': [0, 1.0, 1.4442, 1.56, 2.1, 2.5]
    },
    {
        'name': 'Iso_pt_bins_inc_eta',
        'var': 'pt_p(99,10,1000)',
        'tag': 'gen_p && wt < 1.0 && id_p',
        'probe': 'iso_p < 0.10',
        'binvar_x': 'pt_p',
        'bins_x': [10., 20., 25., 30., 40., 50., 100., 200., 1000.],
        'binvar_y': 'abs(sc_eta_p)',
        'bins_y': [0, 2.5]
    },
    {
        'name': 'Iso_pt_eta_bins',
        'var': 'pt_p(99,10,1000)',
        'tag': 'gen_p && wt < 1.0 && id_p',
        'probe': 'iso_p < 0.10',
        'binvar_x': 'pt_p',
        'bins_x': [10., 20., 25., 30., 40., 50., 100., 200., 1000.],
        'binvar_y': 'abs(sc_eta_p)',
        'bins_y': [0, 1.0, 1.4442, 1.56, 2.1, 2.5]
    },
    #{
    #    'name': 'AIso1_pt_bins_inc_eta',
    #    'var': 'pt_p(99,10,1000)',
    #    'tag': 'gen_p && wt < 1.0 && id_p',
    #    'probe': 'iso_p >= 0.10 && iso_p < 0.20',
    #    'binvar_x': 'pt_p',
    #    'bins_x': [10., 20., 25., 30., 40., 50., 100., 1000.],
    #    'binvar_y': 'abs(sc_eta_p)',
    #    'bins_y': [0, 2.5]
    #},
    {
        'name': 'AIso1_pt_eta_bins',
        'var': 'pt_p(99,10,1000)',
        'tag': 'gen_p && wt < 1.0 && id_p',
        'probe': 'iso_p >= 0.10 && iso_p < 0.20',
        'binvar_x': 'pt_p',
        'bins_x': [10., 20., 25., 30., 40., 50., 100., 1000.],
        'binvar_y': 'abs(sc_eta_p)',
        'bins_y': [0, 1.0, 1.4442, 1.56, 2.1, 2.5]
    },
    #{
    #    'name': 'AIso2_pt_bins_inc_eta',
    #    'var': 'pt_p(99,10,1000)',
    #    'tag': 'gen_p && wt < 1.0 && id_p',
    #    'probe': 'iso_p >= 0.20 && iso_p < 0.50',
    #    'binvar_x': 'pt_p',
    #    'bins_x': [10., 20., 25., 30., 40., 50., 100., 1000.],
    #    'binvar_y': 'abs(sc_eta_p)',
    #    'bins_y': [0, 2.5]
    #},
    {
        'name': 'AIso2_pt_eta_bins',
        'var': 'pt_p(99,10,1000)',
        'tag': 'gen_p && wt < 1.0 && id_p',
        'probe': 'iso_p >= 0.20 && iso_p < 0.50',
        'binvar_x': 'pt_p',
        'bins_x': [10., 20., 25., 30., 40., 50., 100., 1000.],
        'binvar_y': 'abs(sc_eta_p)',
        'bins_y': [0, 1.0, 1.4442, 1.56, 2.1, 2.5]
    },
    #{
    #    'name': 'Trg_Iso_pt_bins_inc_eta',
    #    'var': 'pt_p(99,10,1000)',
    #    'tag': 'gen_p && wt < 1.0 && id_p && iso_p < 0.10',
    #    'probe': 'trg_p_Ele25eta2p1WPTight',
    #    'binvar_x': 'pt_p',
    #    'bins_x': [10., 20., 22., 24., 26., 28., 30., 40., 50., 100., 200., 1000.],
    #    'binvar_y': 'abs(sc_eta_p)',
    #    'bins_y': [0, 2.5]
    #},
    
    #25Tight
    {
        'name': 'Trg_Iso_pt_eta_bins',
        'var': 'pt_p(99,10,1000)',
        'tag': 'gen_p && wt < 1.0 && id_p && iso_p < 0.10',
        'probe': 'trg_p_Ele25eta2p1WPTight',
        'binvar_x': 'pt_p',
        'bins_x': [10., 20., 22., 24., 26., 28., 30., 40., 50., 100., 200., 1000.],
        'binvar_y': 'abs(sc_eta_p)',
        'bins_y': [0, 1.0, 1.4442, 1.56, 2.1, 2.5]
    },
    {
        'name': 'Trg_AIso1_pt_bins_inc_eta',
        'var': 'pt_p(99,10,1000)',
        'tag': 'gen_p && wt < 1.0 && id_p && iso_p >= 0.10 && iso_p < 0.20',
        'probe': 'trg_p_Ele25eta2p1WPTight',
        'binvar_x': 'pt_p',
        'bins_x': [10., 20., 22., 24., 26., 28., 30., 40., 50., 100., 1000.],
        'binvar_y': 'abs(sc_eta_p)',
        'bins_y': [0, 2.5]
    },
    {
        'name': 'Trg_AIso1_pt_eta_bins',
        'var': 'pt_p(99,10,1000)',
        'tag': 'gen_p && wt < 1.0 && id_p && iso_p >= 0.10 && iso_p < 0.20',
        'probe': 'trg_p_Ele25eta2p1WPTight',
        'binvar_x': 'pt_p',
        'bins_x': [10., 20., 22., 24., 26., 28., 30., 40., 50., 100., 1000.],
        'binvar_y': 'abs(sc_eta_p)',
        'bins_y': [0, 1.0, 1.4442, 1.56, 2.1, 2.5]
    },
    {
        'name': 'Trg_AIso2_pt_bins_inc_eta',
        'var': 'pt_p(99,10,1000)',
        'tag': 'gen_p && wt < 1.0 && id_p && iso_p >= 0.20 && iso_p < 0.50',
        'probe': 'trg_p_Ele25eta2p1WPTight',
        'binvar_x': 'pt_p',
        'bins_x': [10., 20., 22., 24., 26., 28., 30., 40., 50., 100., 1000.],
        'binvar_y': 'abs(sc_eta_p)',
        'bins_y': [0, 2.5]
    },
    {
        'name': 'Trg_AIso2_pt_eta_bins',
        'var': 'pt_p(99,10,1000)',
        'tag': 'gen_p && wt < 1.0 && id_p && iso_p >= 0.20 && iso_p < 0.50',
        'probe': 'trg_p_Ele25eta2p1WPTight',
        'binvar_x': 'pt_p',
        'bins_x': [10., 20., 22., 24., 26., 28., 30., 40., 50., 100., 1000.],
        'binvar_y': 'abs(sc_eta_p)',
        'bins_y': [0, 1.0, 1.4442, 1.56, 2.1, 2.5]
    },
    #27Tight
    #{
    #    'name': 'Trg27_Iso_pt_eta_bins',
    #    'var': 'pt_p(99,10,1000)',
    #    'tag': 'trg_t_Ele27eta2p1WPTight && id_p && iso_p < 0.10',
    #    'probe': 'trg_p_Ele27eta2p1WPTight',
    #    'binvar_x': 'pt_p',
    #    'bins_x': [10., 20., 22., 24., 26., 28., 30., 40., 50., 100., 200., 1000.],
    #    'binvar_y': 'abs(sc_eta_p)',
    #    'bins_y': [0, 1.0, 1.4442, 1.56, 2.1, 2.5]
    #},
    #{
    #    'name': 'Trg27_AIso1_pt_bins_inc_eta',
    #    'var': 'pt_p(99,10,1000)',
    #    'tag': 'trg_t_Ele27eta2p1WPTight && id_p && iso_p >= 0.10 && iso_p < 0.20',
    #    'probe': 'trg_p_Ele27eta2p1WPTight',
    #    'binvar_x': 'pt_p',
    #    'bins_x': [10., 20., 22., 24., 26., 28., 30., 40., 50., 100., 1000.],
    #    'binvar_y': 'abs(sc_eta_p)',
    #    'bins_y': [0, 2.5]
    #},
    #{
    #    'name': 'Trg27_AIso1_pt_eta_bins',
    #    'var': 'pt_p(99,10,1000)',
    #    'tag': 'trg_t_Ele27eta2p1WPTight && id_p && iso_p >= 0.10 && iso_p < 0.20',
    #    'probe': 'trg_p_Ele27eta2p1WPTight',
    #    'binvar_x': 'pt_p',
    #    'bins_x': [10., 20., 22., 24., 26., 28., 30., 40., 50., 100., 1000.],
    #    'binvar_y': 'abs(sc_eta_p)',
    #    'bins_y': [0, 1.0, 1.4442, 1.56, 2.1, 2.5]
    #},
    #{
    #    'name': 'Trg27_AIso2_pt_bins_inc_eta',
    #    'var': 'pt_p(99,10,1000)',
    #    'tag': 'trg_t_Ele27eta2p1WPTight && id_p && iso_p >= 0.20 && iso_p < 0.50',
    #    'probe': 'trg_p_Ele27eta2p1WPTight',
    #    'binvar_x': 'pt_p',
    #    'bins_x': [10., 20., 22., 24., 26., 28., 30., 40., 50., 100., 1000.],
    #    'binvar_y': 'abs(sc_eta_p)',
    #    'bins_y': [0, 2.5]
    #},
    #{
    #    'name': 'Trg27_AIso2_pt_eta_bins',
    #    'var': 'pt_p(99,10,1000)',
    #    'tag': 'trg_t_Ele27eta2p1WPTight && id_p && iso_p >= 0.20 && iso_p < 0.50',
    #    'probe': 'trg_p_Ele27eta2p1WPTight',
    #    'binvar_x': 'pt_p',
    #    'bins_x': [10., 20., 22., 24., 26., 28., 30., 40., 50., 100., 1000.],
    #    'binvar_y': 'abs(sc_eta_p)',
    #    'bins_y': [0, 1.0, 1.4442, 1.56, 2.1, 2.5]
    #},
    #27Loose
    #{
    #    'name': 'Trg27Loose_Iso_pt_eta_bins',
    #    'var': 'pt_p(99,10,1000)',
    #    'tag': 'trg_t_Ele27eta2p1WPLoose && id_p && iso_p < 0.10',
    #    'probe': 'trg_p_Ele27eta2p1WPLoose',
    #    'binvar_x': 'pt_p',
    #    'bins_x': [10., 20., 22., 24., 26., 28., 30., 40., 50., 100., 200., 1000.],
    #    'binvar_y': 'abs(sc_eta_p)',
    #    'bins_y': [0, 1.0, 1.4442, 1.56, 2.1, 2.5]
    #},
    #{
    #    'name': 'Trg27Loose_AIso1_pt_bins_inc_eta',
    #    'var': 'pt_p(99,10,1000)',
    #    'tag': 'trg_t_Ele27eta2p1WPLoose && id_p && iso_p >= 0.10 && iso_p < 0.20',
    #    'probe': 'trg_p_Ele27eta2p1WPLoose',
    #    'binvar_x': 'pt_p',
    #    'bins_x': [10., 20., 22., 24., 26., 28., 30., 40., 50., 100., 1000.],
    #    'binvar_y': 'abs(sc_eta_p)',
    #    'bins_y': [0, 2.5]
    #},
    #{
    #    'name': 'Trg27Loose_AIso1_pt_eta_bins',
    #    'var': 'pt_p(99,10,1000)',
    #    'tag': 'trg_t_Ele27eta2p1WPLoose && id_p && iso_p >= 0.10 && iso_p < 0.20',
    #    'probe': 'trg_p_Ele27eta2p1WPLoose',
    #    'binvar_x': 'pt_p',
    #    'bins_x': [10., 20., 22., 24., 26., 28., 30., 40., 50., 100., 1000.],
    #    'binvar_y': 'abs(sc_eta_p)',
    #    'bins_y': [0, 1.0, 1.4442, 1.56, 2.1, 2.5]
    #},
    #{
    #    'name': 'Trg27Loose_AIso2_pt_bins_inc_eta',
    #    'var': 'pt_p(99,10,1000)',
    #    'tag': 'trg_t_Ele27eta2p1WPLoose && id_p && iso_p >= 0.20 && iso_p < 0.50',
    #    'probe': 'trg_p_Ele27eta2p1WPLoose',
    #    'binvar_x': 'pt_p',
    #    'bins_x': [10., 20., 22., 24., 26., 28., 30., 40., 50., 100., 1000.],
    #    'binvar_y': 'abs(sc_eta_p)',
    #    'bins_y': [0, 2.5]
    #},
    #{
    #    'name': 'Trg27Loose_AIso2_pt_eta_bins',
    #    'var': 'pt_p(99,10,1000)',
    #    'tag': 'trg_t_Ele27eta2p1WPLoose && id_p && iso_p >= 0.20 && iso_p < 0.50',
    #    'probe': 'trg_p_Ele27eta2p1WPLoose',
    #    'binvar_x': 'pt_p',
    #    'bins_x': [10., 20., 22., 24., 26., 28., 30., 40., 50., 100., 1000.],
    #    'binvar_y': 'abs(sc_eta_p)',
    #    'bins_y': [0, 1.0, 1.4442, 1.56, 2.1, 2.5]
    #},
    
    #same again for PFTau
    #25Tight
    {
        'name': 'TrgOR_Iso_pt_eta_bins',
        'var': 'pt_p(99,10,1000)',
        'tag': 'gen_p && wt < 1.0 && id_p && iso_p < 0.10',
        'probe': 'trg_p_Ele25eta2p1WPTight || trg_p_PFTau120',
        'binvar_x': 'pt_p',
        'bins_x': [10., 20., 22., 24., 26., 28., 30., 40., 50., 100., 200., 1000.],
        'binvar_y': 'abs(sc_eta_p)',
        'bins_y': [0, 1.0, 1.4442, 1.56, 2.1, 2.5]
    },
    {
        'name': 'TrgOR_AIso1_pt_bins_inc_eta',
        'var': 'pt_p(99,10,1000)',
        'tag': 'gen_p && wt < 1.0 && id_p && iso_p >= 0.10 && iso_p < 0.20',
        'probe': 'trg_p_Ele25eta2p1WPTight || trg_p_PFTau120',
        'binvar_x': 'pt_p',
        'bins_x': [10., 20., 22., 24., 26., 28., 30., 40., 50., 100., 1000.],
        'binvar_y': 'abs(sc_eta_p)',
        'bins_y': [0, 2.5]
    },
    {
        'name': 'TrgOR_AIso1_pt_eta_bins',
        'var': 'pt_p(99,10,1000)',
        'tag': 'gen_p && wt < 1.0 && id_p && iso_p >= 0.10 && iso_p < 0.20',
        'probe': 'trg_p_Ele25eta2p1WPTight || trg_p_PFTau120',
        'binvar_x': 'pt_p',
        'bins_x': [10., 20., 22., 24., 26., 28., 30., 40., 50., 100., 1000.],
        'binvar_y': 'abs(sc_eta_p)',
        'bins_y': [0, 1.0, 1.4442, 1.56, 2.1, 2.5]
    },
    {
        'name': 'TrgOR_AIso2_pt_bins_inc_eta',
        'var': 'pt_p(99,10,1000)',
        'tag': 'gen_p && wt < 1.0 && id_p && iso_p >= 0.20 && iso_p < 0.50',
        'probe': 'trg_p_Ele25eta2p1WPTight || trg_p_PFTau120',
        'binvar_x': 'pt_p',
        'bins_x': [10., 20., 22., 24., 26., 28., 30., 40., 50., 100., 1000.],
        'binvar_y': 'abs(sc_eta_p)',
        'bins_y': [0, 2.5]
    },
    {
        'name': 'TrgOR_AIso2_pt_eta_bins',
        'var': 'pt_p(99,10,1000)',
        'tag': 'gen_p && wt < 1.0 && id_p && iso_p >= 0.20 && iso_p < 0.50',
        'probe': 'trg_p_Ele25eta2p1WPTight || trg_p_PFTau120',
        'binvar_x': 'pt_p',
        'bins_x': [10., 20., 22., 24., 26., 28., 30., 40., 50., 100., 1000.],
        'binvar_y': 'abs(sc_eta_p)',
        'bins_y': [0, 1.0, 1.4442, 1.56, 2.1, 2.5]
    },
    #27Tight
    #{
    #    'name': 'Trg27OR_Iso_pt_eta_bins',
    #    'var': 'pt_p(99,10,1000)',
    #    'tag': 'trg_t_Ele27eta2p1WPTight && id_p && iso_p < 0.10',
    #    'probe': 'trg_p_Ele27eta2p1WPTight || trg_p_PFTau120',
    #    'binvar_x': 'pt_p',
    #    'bins_x': [10., 20., 22., 24., 26., 28., 30., 40., 50., 100., 200., 1000.],
    #    'binvar_y': 'abs(sc_eta_p)',
    #    'bins_y': [0, 1.0, 1.4442, 1.56, 2.1, 2.5]
    #},
    #{
    #    'name': 'Trg27OR_AIso1_pt_bins_inc_eta',
    #    'var': 'pt_p(99,10,1000)',
    #    'tag': 'trg_t_Ele27eta2p1WPTight && id_p && iso_p >= 0.10 && iso_p < 0.20',
    #    'probe': 'trg_p_Ele27eta2p1WPTight || trg_p_PFTau120',
    #    'binvar_x': 'pt_p',
    #    'bins_x': [10., 20., 22., 24., 26., 28., 30., 40., 50., 100., 1000.],
    #    'binvar_y': 'abs(sc_eta_p)',
    #    'bins_y': [0, 2.5]
    #},
    #{
    #    'name': 'Trg27OR_AIso1_pt_eta_bins',
    #    'var': 'pt_p(99,10,1000)',
    #    'tag': 'trg_t_Ele27eta2p1WPTight && id_p && iso_p >= 0.10 && iso_p < 0.20',
    #    'probe': 'trg_p_Ele27eta2p1WPTight || trg_p_PFTau120',
    #    'binvar_x': 'pt_p',
    #    'bins_x': [10., 20., 22., 24., 26., 28., 30., 40., 50., 100., 1000.],
    #    'binvar_y': 'abs(sc_eta_p)',
    #    'bins_y': [0, 1.0, 1.4442, 1.56, 2.1, 2.5]
    #},
    #{
    #    'name': 'Trg27OR_AIso2_pt_bins_inc_eta',
    #    'var': 'pt_p(99,10,1000)',
    #    'tag': 'trg_t_Ele27eta2p1WPTight && id_p && iso_p >= 0.20 && iso_p < 0.50',
    #    'probe': 'trg_p_Ele27eta2p1WPTight || trg_p_PFTau120',
    #    'binvar_x': 'pt_p',
    #    'bins_x': [10., 20., 22., 24., 26., 28., 30., 40., 50., 100., 1000.],
    #    'binvar_y': 'abs(sc_eta_p)',
    #    'bins_y': [0, 2.5]
    #},
    #{
    #    'name': 'Trg27OR_AIso2_pt_eta_bins',
    #    'var': 'pt_p(99,10,1000)',
    #    'tag': 'trg_t_Ele27eta2p1WPTight && id_p && iso_p >= 0.20 && iso_p < 0.50',
    #    'probe': 'trg_p_Ele27eta2p1WPTight || trg_p_PFTau120',
    #    'binvar_x': 'pt_p',
    #    'bins_x': [10., 20., 22., 24., 26., 28., 30., 40., 50., 100., 1000.],
    #    'binvar_y': 'abs(sc_eta_p)',
    #    'bins_y': [0, 1.0, 1.4442, 1.56, 2.1, 2.5]
    #},
    #27Loose
    #{
    #    'name': 'Trg27LooseOR_Iso_pt_eta_bins',
    #    'var': 'pt_p(99,10,1000)',
    #    'tag': 'trg_t_Ele27eta2p1WPLoose && id_p && iso_p < 0.10',
    #    'probe': 'trg_p_Ele27eta2p1WPLoose || trg_p_PFTau120',
    #    'binvar_x': 'pt_p',
    #    'bins_x': [10., 20., 22., 24., 26., 28., 30., 40., 50., 100., 200., 1000.],
    #    'binvar_y': 'abs(sc_eta_p)',
    #    'bins_y': [0, 1.0, 1.4442, 1.56, 2.1, 2.5]
    #},
    #{
    #    'name': 'Trg27LooseOR_AIso1_pt_bins_inc_eta',
    #    'var': 'pt_p(99,10,1000)',
    #    'tag': 'trg_t_Ele27eta2p1WPLoose && id_p && iso_p >= 0.10 && iso_p < 0.20',
    #    'probe': 'trg_p_Ele27eta2p1WPLoose || trg_p_PFTau120',
    #    'binvar_x': 'pt_p',
    #    'bins_x': [10., 20., 22., 24., 26., 28., 30., 40., 50., 100., 1000.],
    #    'binvar_y': 'abs(sc_eta_p)',
    #    'bins_y': [0, 2.5]
    #},
    #{
    #    'name': 'Trg27LooseOR_AIso1_pt_eta_bins',
    #    'var': 'pt_p(99,10,1000)',
    #    'tag': 'trg_t_Ele27eta2p1WPLoose && id_p && iso_p >= 0.10 && iso_p < 0.20',
    #    'probe': 'trg_p_Ele27eta2p1WPLoose || trg_p_PFTau120',
    #    'binvar_x': 'pt_p',
    #    'bins_x': [10., 20., 22., 24., 26., 28., 30., 40., 50., 100., 1000.],
    #    'binvar_y': 'abs(sc_eta_p)',
    #    'bins_y': [0, 1.0, 1.4442, 1.56, 2.1, 2.5]
    #},
    #{
    #    'name': 'Trg27LooseOR_AIso2_pt_bins_inc_eta',
    #    'var': 'pt_p(99,10,1000)',
    #    'tag': 'trg_t_Ele27eta2p1WPLoose && id_p && iso_p >= 0.20 && iso_p < 0.50',
    #    'probe': 'trg_p_Ele27eta2p1WPLoose || trg_p_PFTau120',
    #    'binvar_x': 'pt_p',
    #    'bins_x': [10., 20., 22., 24., 26., 28., 30., 40., 50., 100., 1000.],
    #    'binvar_y': 'abs(sc_eta_p)',
    #    'bins_y': [0, 2.5]
    #},
    #{
    #    'name': 'Trg27LooseOR_AIso2_pt_eta_bins',
    #    'var': 'pt_p(99,10,1000)',
    #    'tag': 'trg_t_Ele27eta2p1WPLoose && id_p && iso_p >= 0.20 && iso_p < 0.50',
    #    'probe': 'trg_p_Ele27eta2p1WPLoose || trg_p_PFTau120',
    #    'binvar_x': 'pt_p',
    #    'bins_x': [10., 20., 22., 24., 26., 28., 30., 40., 50., 100., 1000.],
    #    'binvar_y': 'abs(sc_eta_p)',
    #    'bins_y': [0, 1.0, 1.4442, 1.56, 2.1, 2.5]
    #}
    
    #DESY like setup (using 27trigger and pt>28)
    {
        'name': 'DESYtag_ID_pt_eta_bins',
        'var': 'pt_p(99,10,1000)',
        'tag': 'gen_p && wt < 1.0',
        'probe': 'id_p',
        'binvar_x': 'pt_p',
        'bins_x': [10., 20., 25., 30., 40., 50., 100., 200., 1000.],
        'binvar_y': 'abs(sc_eta_p)',
        'bins_y': [0, 1.0, 1.4442, 1.56, 2.1, 2.5]
    },
    {
        'name': 'DESYtag_Iso_pt_eta_bins',
        'var': 'pt_p(99,10,1000)',
        'tag': 'gen_p && wt < 1.0 && id_p',
        'probe': 'iso_p < 0.10',
        'binvar_x': 'pt_p',
        'bins_x': [10., 20., 25., 30., 40., 50., 100., 200., 1000.],
        'binvar_y': 'abs(sc_eta_p)',
        'bins_y': [0, 1.0, 1.4442, 1.56, 2.1, 2.5]
    },
    {
        'name': 'DESYtag_Trg_Iso_pt_eta_bins',
        'var': 'pt_p(99,10,1000)',
        'tag': 'gen_p && wt < 1.0 && id_p && iso_p < 0.10',
        'probe': 'trg_p_Ele25eta2p1WPTight',
        'binvar_x': 'pt_p',
        'bins_x': [10., 20., 22., 24., 26., 28., 30., 40., 50., 100., 200., 1000.],
        'binvar_y': 'abs(sc_eta_p)',
        'bins_y': [0, 1.0, 1.4442, 1.56, 2.1, 2.5]
    },
    {
        'name': 'DESYtagNonSC_ID_pt_eta_bins',
        'var': 'pt_p(99,10,1000)',
        'tag': 'gen_p && wt < 1.0',
        'probe': 'id_p',
        'binvar_x': 'pt_p',
        'bins_x': [10., 20., 25., 30., 40., 50., 100., 200., 1000.],
        'binvar_y': 'abs(eta_p)',
        'bins_y': [0, 1.0, 1.4442, 1.56, 2.1, 2.5]
    },
    {
        'name': 'DESYtagNonSC_Iso_pt_eta_bins',
        'var': 'pt_p(99,10,1000)',
        'tag': 'gen_p && wt < 1.0 && id_p',
        'probe': 'iso_p < 0.10',
        'binvar_x': 'pt_p',
        'bins_x': [10., 20., 25., 30., 40., 50., 100., 200., 1000.],
        'binvar_y': 'abs(eta_p)',
        'bins_y': [0, 1.0, 1.4442, 1.56, 2.1, 2.5]
    },
    {
        'name': 'DESYtagNonSC_IDIso_pt_eta_bins',
        'var': 'pt_p(99,10,1000)',
        'tag': 'gen_p && wt < 1.0',
        'probe': 'id_p && iso_p < 0.10',
        'binvar_x': 'pt_p',
        'bins_x': [10., 20., 25., 30., 40., 50., 100., 200., 1000.],
        'binvar_y': 'abs(eta_p)',
        'bins_y': [0, 1.0, 1.4442, 1.56, 2.1, 2.5]
    },
    {
        'name': 'DESYtagNonSC_Trg_Iso_pt_eta_bins',
        'var': 'pt_p(99,10,1000)',
        'tag': 'gen_p && wt < 1.0 && id_p && iso_p < 0.10',
        'probe': 'trg_p_Ele25eta2p1WPTight',
        'binvar_x': 'pt_p',
        'bins_x': [10., 20., 22., 24., 26., 28., 30., 40., 50., 100., 200., 1000.],
        'binvar_y': 'abs(eta_p)',
        'bins_y': [0, 1.0, 1.4442, 1.56, 2.1, 2.5]
    }
]

drawlist = []
andable = set()

for cfg in bin_cfgs:
    cfg['hist'] = ROOT.TH2D(cfg['name'], cfg['name'],
                            len(cfg['bins_x'])-1, array('d', cfg['bins_x']),
                            len(cfg['bins_y'])-1, array('d', cfg['bins_y']))
    hist = cfg['hist']
    hist.GetXaxis().SetTitle(cfg['binvar_x'])
    hist.GetYaxis().SetTitle(cfg['binvar_y'])

    cfg['bins'] = []

    for i in xrange(1, hist.GetNbinsX()+1):
        for j in xrange(1, hist.GetNbinsY()+1):
            cfg['bins'].append('%s>=%g && %s<%g && %s>=%g && %s<%g' % (
                cfg['binvar_x'], hist.GetXaxis().GetBinLowEdge(i),
                cfg['binvar_x'], hist.GetXaxis().GetBinUpEdge(i),
                cfg['binvar_y'], hist.GetYaxis().GetBinLowEdge(j),
                cfg['binvar_y'], hist.GetYaxis().GetBinUpEdge(j),
                ))
            andable.add('%s>=%g' % (cfg['binvar_x'], hist.GetXaxis().GetBinLowEdge(i)))
            andable.add('%s<%g' % (cfg['binvar_x'], hist.GetXaxis().GetBinUpEdge(i)))
            andable.add('%s>=%g' % (cfg['binvar_y'], hist.GetYaxis().GetBinLowEdge(j)))
            andable.add('%s<%g' % (cfg['binvar_y'], hist.GetYaxis().GetBinUpEdge(j)))

    for b in cfg['bins']:
        drawlist.append((cfg['var'], '((%s) && !(%s) && (%s)) * wt' % (b, cfg['probe'], cfg['tag'])))
        drawlist.append((cfg['var'], '((%s) && (%s) && (%s)) * wt' % (b, cfg['probe'], cfg['tag'])))
        andable.add(cfg['probe'])
        andable.add(cfg['tag'])


trees = {
    'DYJetsToLL': analysis.TTreeEvaluator('inclusive/GenElectron', 'output/HTT2016Studies_Truth1/GenElectron/DYJetsToLLM50.root'),
    'EmbeddingMC': analysis.TTreeEvaluator('inclusive/GenElectron', 'output/HTT2016Studies_Truth1/GenElectron/EmbeddingMC.root'),
    'EmbeddingData': analysis.TTreeEvaluator('inclusive/GenElectron', 'output/HTT2016Studies_Truth1/GenElectron/EmbeddingData.root')
}


# sys.exit(0)

for sample in trees:
    outfile = ROOT.TFile('genElectron_truth/genElectron_%s.root' % sample, 'RECREATE')
    hists = trees[sample].Draw(drawlist, compiled=True)

    i = 0
    for cfg in bin_cfgs:
        wsp = ROOT.RooWorkspace('wsp_'+cfg['name'], '')
        var = wsp.factory('pt_p[99,10,1000]')

        outfile.cd()
        outfile.mkdir(cfg['name'])
        ROOT.gDirectory.cd(cfg['name'])

        for b in cfg['bins']:
            hists[2*i].SetName(b+':fail')
            hists[2*i+1].SetName(b+':pass')
            hists[2*i].Write()
            hists[2*i+1].Write()
            dat = wsp.imp(ROOT.RooDataHist(b, '', ROOT.RooArgList(var),
                          ROOT.RooFit.Index(wsp.factory('cat[fail,pass]')),
                          ROOT.RooFit.Import('fail', hists[2*i]),
                          ROOT.RooFit.Import('pass', hists[2*i+1]))
                          )
            i += 1
        outfile.cd()
        wsp.Write()
        cfg['hist'].Write()
        wsp.Delete()

    outfile.Close()
