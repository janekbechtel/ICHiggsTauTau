#!/bin/env python
import subprocess
import fitTagAndProbe_script
import plotEffSlices_script

dir = "output_test/tag_and_probe_m_17_1/"
parameters = {
    "Trg_AIso1_pt_bins_inc_eta": {
        "SMALL": "Trg_AIso1inc",
        "DIR": dir,
        "TITLE": "Trg_AIso1_pt_bins_inc_eta",
        "BKG": "Exponential",
        "SIG": "DoubleVCorr"
    },
    "Trg_AIso1_pt_eta_bins": {
        "SMALL": "eff.Trg_AIso1",
        "DIR": dir,
        "TITLE": "Trg_AIso1_pt_eta_bins",
        "BKG": "Exponential",
        "SIG": "DoubleVCorr"
    },
    "Trg_AIso2_pt_bins_inc_eta": {
        "SMALL": "eff.Trg_AIso2",
        "DIR": dir,
        "TITLE": "Trg_AIso2_pt_bins_inc_eta",
        "BKG": "Exponential",
        "SIG": "DoubleVCorr"
    },
    "Trg_AIso2_pt_eta_bins": {
        "SMALL": "eff.Trg_AIso2",
        "DIR": dir,
        "TITLE": "Trg_AIso2_pt_eta_bins",
        "BKG": "Exponential",
        "SIG": "DoubleVCorr"
    },
    "ID_pt_eta_bins": {
        "SMALL": "eff.ID",
        "DIR": dir,
        "TITLE": "CMSSHAPE",
        "BKG": "Exponential",
        "SIG": "DoubleVCorr"
    },
    "Iso_pt_eta_bins": {
        "SMALL": "eff.Iso",
        "DIR": dir,
        "TITLE": "Iso_{rel} < 0.15",
        "BKG": "Exponential",
        "SIG": "DoubleVCorr"
    },
    "LooseIso_pt_eta_bins": {
        "SMALL": "eff.Iso",
        "DIR": dir,
        "TITLE": "Iso_{rel} < 0.2",
        "BKG": "Exponential",
        "SIG": "DoubleVCorr"
    },
    "AIso1_pt_eta_bins": {
        "SMALL": "eff.AIso1",
        "DIR": dir,
        "TITLE": "AIso1_pt_eta_bins",
        "BKG": "Exponential",
        "SIG": "DoubleVCorr"
    },
    "AIso2_pt_eta_bins": {
        "SMALL": "eff.AIso2",
        "DIR": dir,
        "TITLE": "AIso2_pt_eta_bins",
        "BKG": "Exponential",
        "SIG": "DoubleVCorr"
    },
    "Trg_Iso_pt_eta_bins": {
        "SMALL": "eff.Trg_Iso",
        "DIR": dir,
        "TITLE": "Trg IsoMu22",
        "BKG": "Exponential",
        "SIG": "DoubleVCorr"
    },
    "Trg24_Iso_pt_eta_bins": {
        "SMALL": "eff.Trg24_Iso",
        "DIR": dir,
        "TITLE": "Trg IsoMu24",
        "BKG": "Exponential",
        "SIG": "DoubleVCorr"
    },
}

for label in parameters.keys():
    pass
    filename = ["output/ZmmTP_Embedding.root", "output/ZmmTP_Data.root"]
    dir_ext = ["/embedding", "/data"]
    for i, file in enumerate(filename):
        try:
            fitTagAndProbe_script.main(
                filename=file,
                name=label,
                sig_model=parameters[label]["SIG"],
                bkg_model=parameters[label]["BKG"],
                title=parameters[label]["TITLE"],
                particle="m",
                isMC=False,
                postfix="",
                plot_dir=dir + label + dir_ext[i],
                bin_replace=None)
        except AttributeError:
            pass


## for slice efficiencies
sliceing = {
    "Trg_AIso1_pt_bins_inc_eta": {
        "SMALL": "eff.Trg_Iso",
        "DIR": dir,
        "TITLE": "Trg IsoMu22",
        "BKG": "Exponential",
        "SIG": "DoubleVCorr"
    },
    "Trg_AIso2_pt_bins_inc_eta": {
        "SMALL": "eff.Trg_Iso",
        "DIR": dir,
        "TITLE": "Trg IsoMu22",
        "BKG": "Exponential",
        "SIG": "DoubleVCorr"
    },
    "Trg24_Iso_pt_eta_bins": {
        "SMALL": "eff.Trg24_Iso",
        "DIR": dir,
        "TITLE": "Trg IsoMu22",
        "BKG": "Exponential",
        "SIG": "DoubleVCorr"
    },
    "Trg24_AIso1_pt_bins_inc_eta": {
        "SMALL": "eff.Trg24_Iso",
        "DIR": dir,
        "TITLE": "Trg IsoMu22",
        "BKG": "Exponential",
        "SIG": "DoubleVCorr"
    },
    "Trg24_AIso2_pt_bins_inc_eta": {
        "SMALL": "eff.Trg24_Iso",
        "DIR": dir,
        "TITLE": "Trg IsoMu22",
        "BKG": "Exponential",
        "SIG": "DoubleVCorr"
    },
}

for label in sliceing.keys():
    files = ["output/ZmmTP_Data_Fits_" + label + ".root",
            "output/ZmmTP_Embedding_Fits_" + label + ".root"
            #"output/ZmmTP_DYJetsToLL_Fits_" + label + ".root"
            ]
    draw_options = [
        {
            'Title':'Data'},
        {
            'MarkerColor':4,
            'LineColor':4,
            'MarkerStyle':21,
            'Title':"#mu#rightarrow#mu embedded"},
        # {
        #     'MarkerColor':2,
        #     'LineColor':2,
        #     'MarkerStyle':21,
        #     'Title':"Z#rightarrow#mu#mu simulation"}
            ]
    try:
        plotEffSlices_script.main(
            files=files,
            label=label,
            draw_options=draw_options,
            output="efficiency", 
            title= sliceing[label]["TITLE"] , 
            y_range= [0.0,0.1], 
            ratio_y_range= [0.95,1.2], 
            binned_in= "#eta", 
            x_title= "Muon p_{T} (GeV)", 
            ratio_to= 0, 
            plot_dir= dir + label, 
            label_pos= 3)
    except ReferenceError:
        print label + " Fits do not exist"
        pass

# electron plots
el_parameters = {
    "ID_pt_eta_bins": {
        "SMALL": "eff.ID",
        "DIR": dir,
        "TITLE": "Non-trig MVA ID 16",
        "BKG": "CMSShape",
        "SIG": "DoubleVCorr"
    },
    "Iso_pt_eta_bins": {
        "SMALL": "eff.Iso",
        "DIR": dir,
        "TITLE": "Iso_{rel} < 0.10",
        "BKG": "Exponential",
        "SIG": "DoubleVCorr"
    },
    "LooseIso_pt_eta_bins": {
        "SMALL": "eff.Iso",
        "DIR": dir,
        "TITLE": "Iso_{rel} < 0.15",
        "BKG": "Exponential",
        "SIG": "DoubleVCorr"
    },
    "AIso1_pt_eta_bins": {
        "SMALL": "eff.Iso",
        "DIR": dir,
        "TITLE": "Iso_{rel} #in [0.10,0.20]",
        "BKG": "Exponential",
        "SIG": "DoubleVCorr"
    },
    "AIso2_pt_eta_bins": {
        "SMALL": "eff.Iso",
        "DIR": dir,
        "TITLE": "Iso_{rel} #in [0.20,0.50]",
        "BKG": "Exponential",
        "SIG": "DoubleVCorr"
    },
    "Trg_Iso_pt_eta_bins": {
        "SMALL": "eff.Trg_Iso",
        "DIR": dir,
        "TITLE": "Trg Ele25eta2p1WPTight",
        "BKG": "Exponential",
        "SIG": "DoubleVCorr"
    },
    "Trg35_AIso1_pt_bins_inc_eta": {
        "SMALL": "eff.Trg35_AIso1inc",
        "DIR": dir,
        "TITLE": "Trg Ele25eta2p1WPTight | Iso_{rel} #in [0.10,0.20]",
        "BKG": "Exponential",
        "SIG": "DoubleVCorr"
    },
    "Trg35_AIso2_pt_bins_inc_eta": {
        "SMALL": "eff.Trg35_AIso2inc",
        "DIR": dir,
        "TITLE": "Trg Ele25eta2p1WPTight | Iso_{rel} #in [0.20,0.50]",
        "BKG": "Exponential",
        "SIG": "DoubleVCorr"
    },
    "Trg35_Iso_pt_eta_bins": {
        "SMALL": "eff.Trg35_Iso",
        "DIR": dir,
        "TITLE": "Trg Ele25eta2p1WPTight",
        "BKG": "Exponential",
        "SIG": "DoubleVCorr"
    },
    "Trg_AIso1_pt_bins_inc_eta": {
        "SMALL": "eff.Trg_AIso1inc",
        "DIR": dir,
        "TITLE": "Trg Ele25eta2p1WPTight | Iso_{rel} #in [0.10,0.20]",
        "BKG": "Exponential",
        "SIG": "DoubleVCorr"
    },
    "Trg_AIso2_pt_bins_inc_eta": {
        "SMALL": "eff.Trg_AIso2inc",
        "DIR": dir,
        "TITLE": "Trg Ele25eta2p1WPTight | Iso_{rel} #in [0.20,0.50]",
        "BKG": "Exponential",
        "SIG": "DoubleVCorr"
    },
}
for label in el_parameters.keys():
    pass
    filename = ["output/ZmmTP_Embedding.root", "output/ZmmTP_Data.root"]
    dir_ext = ["/embedding", "/data"]
    for i, file in enumerate(filename):
        try:
            fitTagAndProbe_script.main(
                filename=file,
                name=label,
                sig_model=parameters[label]["SIG"],
                bkg_model=parameters[label]["BKG"],
                title=parameters[label]["TITLE"],
                particle="e",
                isMC=False,
                postfix="",
                plot_dir=dir + label + dir_ext[i],
                bin_replace=None)
        except AttributeError:
            pass