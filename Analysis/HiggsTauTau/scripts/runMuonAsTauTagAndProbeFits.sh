#!/bin/sh

#LABEL="TrgTau_Iso_pt_eta_bins"; SMALL="eff.TrgTau_Iso"; DIR="output/tag_and_probe_mastau/${LABEL}"; TITLE="Tau120"; BKG="Exponential"; SIG="DoubleVCorr"; python scripts/fitTagAndProbe.py Tau_Trigger/ZmmTP_Data.root:${LABEL} --bkg-model ${BKG} --sig-model ${SIG} --title "${TITLE}" -p ${DIR}/data; python scripts/fitTagAndProbe.py Tau_Trigger/ZmmTP_DYJetsToLL.root:${LABEL} --bkg-model ${BKG} --sig-model ${SIG} --title "${TITLE}" -p ${DIR}/mc; python scripts/plotEffSlices.py Tau_Trigger/ZmmTP_Data_Fits_${LABEL}.root:${LABEL}:'Title="Data"' Tau_Trigger/ZmmTP_DYJetsToLL_Fits_${LABEL}.root:${LABEL}:'MarkerColor=2,LineColor=2,MarkerStyle=21,Title="MC"' --y-range 0.0,1.0 --ratio-to 1 --x-title "Muon p_{T} (GeV)" -o "${SMALL}" --title "${TITLE} Efficiency" --plot-dir ${DIR}

LABEL="TrgTau_Iso_pt_eta_bins"; SMALL="eff.TrgTau_Iso"; DIR="output/tag_and_probe_mastau/${LABEL}"; TITLE="Tau120"; BKG="Exponential"; SIG="DoubleVCorr"; python scripts/plotEffSlices.py Tau_Trigger/ZmmTP_Data_Fits_${LABEL}.root:${LABEL}:'Title="Data"' Tau_Trigger/ZmmTP_DYJetsToLL_Fits_${LABEL}.root:${LABEL}:'MarkerColor=2,LineColor=2,MarkerStyle=21,Title="MC"' --y-range 0.0,1.0 --ratio-to 1 --x-title "Muon p_{T} (GeV)" -o "${SMALL}" --title "${TITLE} Efficiency" --plot-dir ${DIR}


