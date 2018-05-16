#!/bin/env python
import subprocess
import fitTagAndProbe_script
import plotEffSlices_script
import argparse

parser = argparse.ArgumentParser()

parser.add_argument(
    '--channel', default="m", required=True)
parser.add_argument(
    '--fit', action="store_true")
parser.add_argument(
    '--plot', action="store_true")
args = parser.parse_args()
if "sm" in args.channel:
    dir = "output/tag_and_probe_sm_v17_1/"
elif "dm" in args.channel:
    dir = "output/tag_and_probe_dm_v17_1/"
elif "e" in args.channel:
    dir = "output/tag_and_probe_e_v17_1/"
else:
    "Only Options are sm / dm / e !!"
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
        "TITLE": "Trg IsoMu27",
        "BKG": "Exponential",
        "SIG": "DoubleVCorr"
    },

    "Trg24_Iso_pt_bins_inc_eta": {
        "SMALL": "eff.Trg_Iso",
        "DIR": dir,
        "TITLE": "Trg IsoMu24",
        "BKG": "Exponential",
        "SIG": "DoubleVCorr"
    },
    "Trg24_Iso_pt_eta_bins": {
        "SMALL": "eff.Trg_Iso",
        "DIR": dir,
        "TITLE": "Trg IsoMu24",
        "BKG": "Exponential",
        "SIG": "DoubleVCorr"
    },
    "Trg_IsoMu27_or_IsoMu24_pt_eta_bins": {
        "SMALL": "eff.Trg_Iso",
        "DIR": dir,
        "TITLE": "Trg IsoMu24 or IsoMu27",
        "BKG": "Exponential",
        "SIG": "DoubleVCorr"
    },
    "Trg_IsoMu27_or_IsoMu24_pt_bins_inc_eta": {
        "SMALL": "eff.Trg_Iso",
        "DIR": dir,
        "TITLE": "Trg IsoMu24 or IsoMu24",
        "BKG": "Exponential",
        "SIG": "DoubleVCorr"
    }
}
el_parameters = {
    "ID_pt_eta_bins": {
        "SMALL": "eff.ID",
        "DIR": dir,
        "TITLE": "Non-trig MVA ID 17",
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
        "TITLE": "Trg Ele27eta2p1WPTight",
        "BKG": "Exponential",
        "SIG": "DoubleVCorr"
    },
    "Trg35_AIso1_pt_bins_inc_eta": {
        "SMALL": "eff.Trg35_AIso1inc",
        "DIR": dir,
        "TITLE": "Trg El35eta2p1WPTight | Iso_{rel} #in [0.10,0.20]",
        "BKG": "Exponential",
        "SIG": "DoubleVCorr"
    },
    "Trg35_AIso2_pt_bins_inc_eta": {
        "SMALL": "eff.Trg35_AIso2inc",
        "DIR": dir,
        "TITLE": "Trg El35eta2p1WPTight | Iso_{rel} #in [0.20,0.50]",
        "BKG": "Exponential",
        "SIG": "DoubleVCorr"
    },
    "Trg35_Iso_pt_eta_bins": {
        "SMALL": "eff.Trg35_Iso",
        "DIR": dir,
        "TITLE": "Trg El35eta2p1WPTight",
        "BKG": "Exponential",
        "SIG": "DoubleVCorr"
    },
    "Trg_AIso1_pt_bins_inc_eta": {
        "SMALL": "eff.Trg_AIso1inc",
        "DIR": dir,
        "TITLE": "Trg Ele27eta2p1WPTight | Iso_{rel} #in [0.10,0.20]",
        "BKG": "Exponential",
        "SIG": "DoubleVCorr"
    },
    "Trg_AIso2_pt_bins_inc_eta": {
        "SMALL": "eff.Trg_AIso2inc",
        "DIR": dir,
        "TITLE": "Trg Ele27eta2p1WPTight | Iso_{rel} #in [0.20,0.50]",
        "BKG": "Exponential",
        "SIG": "DoubleVCorr"
    },
        "Trg32_AIso1_pt_bins_inc_eta": {
        "SMALL": "eff.Trg32_AIso1inc",
        "DIR": dir,
        "TITLE": "Trg El32eta2p1WPTight | Iso_{rel} #in [0.10,0.20]",
        "BKG": "Exponential",
        "SIG": "DoubleVCorr"
    },
        "Trg32_AIso2_pt_bins_inc_eta": {
        "SMALL": "eff.Trg32_AIso2inc",
        "DIR": dir,
        "TITLE": "Trg El32eta2p1WPTight | Iso_{rel} #in [0.20,0.50]",
        "BKG": "Exponential",
        "SIG": "DoubleVCorr"
    },
    "Trg32_Iso_pt_eta_bins": {
        "SMALL": "eff.Trg32_Iso",
        "DIR": dir,
        "TITLE": "Trg El32eta2p1WPTight",
        "BKG": "Exponential",
        "SIG": "DoubleVCorr"
    },
    "Trg32_or_Trg35_Iso_pt_eta_bins": {
        "SMALL": "eff.Trg",
        "DIR": dir,
        "TITLE": "Trg El32eta2p1WPTight or Trg El35eta2p1WPTight",
        "BKG": "Exponential",
        "SIG": "DoubleVCorr"
    },
}
mu_sliceing = {
    "ID_pt_eta_bins": {
        "SMALL": "eff.ID",
        "DIR": dir,
        "TITLE": "Medium ID",
        "BKG": "CMSShape",
        "SIG": "DoubleVCorr",
        "y_range": [0.8,1.0],
        "ratio_y_range": [0.8,1.2]
    },
    "Iso_pt_eta_bins": {
        "SMALL": "eff.Iso",
        "DIR": dir,
        "TITLE": "Iso_{rel} < 0.15",
        "BKG": "Exponential",
        "SIG": "DoubleVCorr",
        "y_range": [0.6,1.0],
        "ratio_y_range": [0.8,1.2]
    },
    "AIso1_pt_eta_bins": {
        "SMALL": "eff.AIso1",
        "DIR": dir,
        "TITLE": "Iso_{rel} #in [0.15,0.25]",
        "BKG": "Exponential",
        "SIG": "DoubleVCorr",
        "y_range": [0.0,0.4],
        "ratio_y_range": [0.02,1.98]
    },
    "AIso2_pt_eta_bins": {
        "SMALL": "eff.AIso2",
        "DIR": dir,
        "TITLE": "Iso_{rel} #in [0.25,0.50]",
        "BKG": "Exponential",
        "SIG": "DoubleVCorr",
        "y_range": [0.0,0.4],
        "ratio_y_range": [0.02,1.98]
    },
    "Trg_Iso_pt_eta_bins": {
        "SMALL": "eff.Trg_Iso",
        "DIR": dir,
        "TITLE": "Trg IsoMu27",
        "BKG": "Exponential",
        "SIG": "DoubleVCorr",
        "y_range": [0.0,1.0],
        "ratio_y_range": [0.95,1.2]
    },
    "Trg_AIso1_pt_bins_inc_eta": {
        "SMALL": "eff.Trg_Iso",
        "DIR": dir,
        "TITLE": "Trg IsoMu27",
        "BKG": "Exponential",
        "SIG": "DoubleVCorr",
        "y_range": [0.0,1.0],
        "ratio_y_range": [0.95,1.2]
    },
    "Trg_AIso2_pt_bins_inc_eta": {
        "SMALL": "eff.Trg_Iso",
        "DIR": dir,
        "TITLE": "Trg IsoMu27",
        "BKG": "Exponential",
        "SIG": "DoubleVCorr",
        "y_range": [0.0,1.0],
        "ratio_y_range": [0.95,1.2]
    },
    "Trg24_Iso_pt_eta_bins": {
        "SMALL": "eff.Trg_Iso",
        "DIR": dir,
        "TITLE": "Trg IsoMu24",
        "BKG": "Exponential",
        "SIG": "DoubleVCorr",
        "y_range": [0.0,1.0],
        "ratio_y_range": [0.95,1.2]
    },
    "Trg_IsoMu27_or_IsoMu24_pt_eta_bins": {
        "SMALL": "eff.Trg_Iso",
        "DIR": dir,
        "TITLE": "Trg IsoMu24 or IsoMu27",
        "BKG": "Exponential",
        "SIG": "DoubleVCorr",
        "y_range": [0.0,1.0],
        "ratio_y_range": [0.95,1.2]
    }
}

el_sliceing = {
    "ID_pt_eta_bins": {
        "SMALL": "eff.ID",
        "DIR": dir,
        "TITLE": "MVA ID 17",
        "BKG": "CMSShape",
        "SIG": "DoubleVCorr",
        "y_range": [0.4,1.05],
        "ratio_y_range": [0.95,1.2]
    },
    "Iso_pt_eta_bins": {
        "SMALL": "eff.Iso",
        "DIR": dir,
        "TITLE": "Iso_{rel} < 0.10",
        "BKG": "Exponential",
        "SIG": "DoubleVCorr",
        "y_range": [0.6,1.05],
        "ratio_y_range": [0.95,1.2]
    },
    "AIso1_pt_eta_bins": {
        "SMALL": "eff.AIso1",
        "DIR": dir,
        "TITLE": "Iso_{rel} #in [0.10,0.20]",
        "BKG": "Exponential",
        "SIG": "DoubleVCorr",
        "y_range": [0.0,0.4],
        "ratio_y_range": [0.02,1.98]
    },
    "AIso2_pt_eta_bins": {
        "SMALL": "eff.AIso2",
        "DIR": dir,
        "TITLE": "Iso_{rel} #in [0.20,0.50]",
        "BKG": "Exponential",
        "SIG": "DoubleVCorr",
        "y_range": [0.0,0.4],
        "ratio_y_range": [0.02,1.98]
    },
    "Trg_Iso_pt_eta_bins": {
        "SMALL": "eff.Trg_Iso",
        "DIR": dir,
        "TITLE": "Trg Ele25",
        "BKG": "Exponential",
        "SIG": "DoubleVCorr",
        "y_range": [0.0,1.0],
        "ratio_y_range": [0.95,1.2]
    },
    "Trg_AIso1_pt_bins_inc_eta": {
        "SMALL": "eff.Trg_AIso1inc",
        "DIR": dir,
        "TITLE": "Trg Ele25 | Iso_{rel} #in [0.10,0.20]",
        "BKG": "Exponential",
        "SIG": "DoubleVCorr",
        "y_range": [0.0,1.0],
        "ratio_y_range": [0.95,1.2]
    },
    "Trg_AIso2_pt_bins_inc_eta": {
        "SMALL": "eff.Trg_Iso",
        "DIR": dir,
        "TITLE": "Trg Ele25 | Iso_{rel} #in [0.20,0.50]",
        "BKG": "Exponential",
        "SIG": "DoubleVCorr",
        "y_range": [0.0,1.0],
        "ratio_y_range": [0.95,1.2]
    },
    "Trg35_Iso_pt_eta_bins": {
        "SMALL": "eff.Trg_Iso",
        "DIR": dir,
        "TITLE": "Trg Ele35",
        "BKG": "Exponential",
        "SIG": "DoubleVCorr",
        "y_range": [0.0,1.0],
        "ratio_y_range": [0.95,1.2]
    },
    "Trg35_AIso1_pt_bins_inc_eta": {
        "SMALL": "eff.Trg35_AIso1inc",
        "DIR": dir,
        "TITLE": "Trg Ele35 | Iso_{rel} #in [0.10,0.20]",
        "BKG": "Exponential",
        "SIG": "DoubleVCorr",
        "y_range": [0.0,1.0],
        "ratio_y_range": [0.02,1.98]
    },
    "Trg35_AIso2_pt_bins_inc_eta": {
        "SMALL": "eff.Trg35_Iso",
        "DIR": dir,
        "TITLE": "Trg Ele35 | Iso_{rel} #in [0.20,0.50]",
        "BKG": "Exponential",
        "SIG": "DoubleVCorr",
        "y_range": [0.0,1.0],
        "ratio_y_range": [0.02,1.98]
    },
    "Trg32_Iso_pt_eta_bins": {
        "SMALL": "eff.Trg_Iso",
        "DIR": dir,
        "TITLE": "Trg Ele32",
        "BKG": "Exponential",
        "SIG": "DoubleVCorr",
        "y_range": [0.0,1.0],
        "ratio_y_range": [0.95,1.2]
    },
    "Trg32_AIso1_pt_bins_inc_eta": {
        "SMALL": "eff.Trg32_AIso1inc",
        "DIR": dir,
        "TITLE": "Trg Ele32 | Iso_{rel} #in [0.10,0.20]",
        "BKG": "Exponential",
        "SIG": "DoubleVCorr",
        "y_range": [0.0,1.0],
        "ratio_y_range": [0.02,1.98]
    },
    "Trg32_AIso2_pt_bins_inc_eta": {
        "SMALL": "eff.Trg32_Iso",
        "DIR": dir,
        "TITLE": "Trg Ele32 | Iso_{rel} #in [0.20,0.50]",
        "BKG": "Exponential",
        "SIG": "DoubleVCorr",
        "y_range": [0.0,1.0],
        "ratio_y_range": [0.02,1.98]
    },
    "Trg32_or_Trg35_Iso_pt_eta_bins": {
        "SMALL": "eff.Trg",
        "DIR": dir,
        "TITLE": "Trg Ele32 or Trg Ele35",
        "BKG": "Exponential",
        "SIG": "DoubleVCorr",
        "y_range": [0.0,1.0],
        "ratio_y_range": [0.1,1.9]
    },

}
if args.fit:
    if "m" in args.channel:
        for label in parameters.keys():
            pass
            if "sm" in args.channel:
                filename = ["output/ZmmTP_Embedding.root", "output/ZmmTP_Data_sM.root", "output/ZmmTP_DY.root"]
            else:
                filename = ["output/ZmmTP_Embedding.root", "output/ZmmTP_Data.root", "output/ZmmTP_DY.root"]
            dir_ext = ["/embedding", "/data", "/DY"]
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
                
        # electron plots
    elif "e" in args.channel:
        for label in el_parameters.keys():
            filename = ["output/ZeeTP_Embedding.root", "output/ZeeTP_Data.root", "output/ZeeTP_DYJetsToLL.root"]
            dir_ext = ["/embedding", "/data", "/DY"]
            for i, file in enumerate(filename):
                try:
                    fitTagAndProbe_script.main(
                        filename=file,
                        name=label,
                        sig_model=el_parameters[label]["SIG"],
                        bkg_model=el_parameters[label]["BKG"],
                        title=el_parameters[label]["TITLE"],
                        particle="e",
                        isMC=False,
                        postfix="",
                        plot_dir=dir + label + dir_ext[i],
                        bin_replace=None)
                except AttributeError:
                    pass
if args.plot:
    if "m" in args.channel:
        for label in mu_sliceing.keys():
            files = ["output/ZmmTP_Data_sM_Fits_" + label + ".root",
                    "output/ZmmTP_Embedding_Fits_" + label + ".root",
                    "output/ZmmTP_DY_Fits_" + label + ".root"
                    ]
            draw_options = [
                {
                    'Title':'Data'},
                {
                    'MarkerColor':4,
                    'LineColor':4,
                    'MarkerStyle':21,
                    'Title':"#mu#rightarrow#mu embedded"},
                {
                    'MarkerColor':2,
                    'LineColor':2,
                    'MarkerStyle':21,
                    'Title':"Z#rightarrow#mu#mu simulation"}
                    ]
            try:
                plotEffSlices_script.main(
                    files=files,
                    label=label,
                    draw_options=draw_options,
                    output="efficiency", 
                    title= mu_sliceing[label]["TITLE"] + "Efficiency" , 
                    y_range= mu_sliceing[label]["y_range"], 
                    ratio_y_range= mu_sliceing[label]["ratio_y_range"], 
                    binned_in= "#eta", 
                    x_title= "Muon p_{T} (GeV)", 
                    ratio_to= 0, 
                    plot_dir= dir + label, 
                    label_pos= 3)
            except ReferenceError:
                print label + " Fits do not exist"
                pass
    elif "e" in args.channel:
        for label in el_sliceing.keys():
            files = ["output/ZeeTP_Data_Fits_" + label + ".root",
                    "output/ZeeTP_Embedding_Fits_" + label + ".root",
                    "output/ZeeTP_DYJetsToLL_Fits_" + label + ".root"
                    ]
            draw_options = [
                {
                    'Title':'Data'},
                {
                    'MarkerColor':4,
                    'LineColor':4,
                    'MarkerStyle':21,
                    'Title':"#mu#rightarrow e embedded"},
                {
                    'MarkerColor':2,
                    'LineColor':2,
                    'MarkerStyle':21,
                    'Title':"Z#rightarrow ee simulation"}
                    ]
            try:
                plotEffSlices_script.main(
                    files=files,
                    label=label,
                    draw_options=draw_options,
                    output="efficiency", 
                    title= el_sliceing[label]["TITLE"] + "Efficiency" , 
                    y_range= el_sliceing[label]["y_range"], 
                    ratio_y_range= el_sliceing[label]["ratio_y_range"], 
                    binned_in= "#eta", 
                    x_title= "Electron p_{T} (GeV)", 
                    ratio_to= 0, 
                    plot_dir= dir + label, 
                    label_pos= 3)
            except ReferenceError:
                print label + " Fits do not exist"
                pass