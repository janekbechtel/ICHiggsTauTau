#!/bin/sh




'''

LABEL="ID_pt_eta_bins"; SMALL="eff.ID"; DIR="output/tag_and_probe_m_v16_6/${LABEL}"; TITLE="Medium (HIP-safe for B to F) ID"; BKG="CMSShape"; SIG="DoubleVCorr"; python scripts/fitTagAndProbe.py output/ZmmTP_Embedding.root:${LABEL} --bkg-model ${BKG} --sig-model ${SIG} --title "${TITLE}" -p ${DIR}/embedding; python scripts/fitTagAndProbe.py output/ZmmTP_Data.root:${LABEL} --bkg-model ${BKG} --sig-model ${SIG} --title "${TITLE}" -p ${DIR}/data; 

LABEL="Iso_pt_eta_bins"; SMALL="eff.Iso"; DIR="output/tag_and_probe_m_v16_6/${LABEL}"; TITLE="Iso_{rel} < 0.15"; BKG="Exponential"; SIG="DoubleVCorr"; python scripts/fitTagAndProbe.py output/ZmmTP_Embedding.root:${LABEL} --bkg-model ${BKG} --sig-model ${SIG} --title "${TITLE}" -p ${DIR}/embedding; python scripts/fitTagAndProbe.py output/ZmmTP_Data.root:${LABEL} --bkg-model ${BKG} --sig-model ${SIG} --title "${TITLE}" -p ${DIR}/data;
LABEL="AIso1_pt_eta_bins"; SMALL="eff.AIso1"; DIR="output/tag_and_probe_m_v16_6/${LABEL}"; TITLE="Iso_{rel} #in [0.15,0.25]"; BKG="Exponential"; SIG="DoubleVCorr"; python scripts/fitTagAndProbe.py output/ZmmTP_Embedding.root:${LABEL} --bkg-model ${BKG} --sig-model ${SIG} --title "${TITLE}" -p ${DIR}/embedding; python scripts/fitTagAndProbe.py output/ZmmTP_Data.root:${LABEL} --bkg-model ${BKG} --sig-model ${SIG} --title "${TITLE}" -p ${DIR}/data;

LABEL="AIso2_pt_eta_bins"; SMALL="eff.AIso2"; DIR="output/tag_and_probe_m_v16_6/${LABEL}"; TITLE="Iso_{rel} #in [0.25,0.50]"; BKG="Exponential"; SIG="DoubleVCorr"; python scripts/fitTagAndProbe.py output/ZmmTP_Embedding.root:${LABEL} --bkg-model ${BKG} --sig-model ${SIG} --title "${TITLE}" -p ${DIR}/embedding; python scripts/fitTagAndProbe.py output/ZmmTP_Data.root:${LABEL} --bkg-model ${BKG} --sig-model ${SIG} --title "${TITLE}" -p ${DIR}/data; 

LABEL="Trg_Iso_pt_eta_bins"; SMALL="eff.Trg_Iso"; DIR="output/tag_and_probe_m_v16_6/${LABEL}"; TITLE="Trg IsoMu22"; BKG="Exponential"; SIG="DoubleVCorr"; python scripts/fitTagAndProbe.py output/ZmmTP_Embedding.root:${LABEL} --bkg-model ${BKG} --sig-model ${SIG} --title "${TITLE}" -p ${DIR}/embedding; python scripts/fitTagAndProbe.py output/ZmmTP_Data.root:${LABEL} --bkg-model ${BKG} --sig-model ${SIG} --title "${TITLE}" -p ${DIR}/data;

LABEL="Trg24_Iso_pt_eta_bins"; SMALL="eff.Trg24_Iso"; DIR="output/tag_and_probe_m_v16_6/${LABEL}"; TITLE="Trg IsoMu24"; BKG="Exponential"; SIG="DoubleVCorr"; python scripts/fitTagAndProbe.py output/ZmmTP_Embedding.root:${LABEL} --bkg-model ${BKG} --sig-model ${SIG} --title "${TITLE}" -p ${DIR}/embedding; python scripts/fitTagAndProbe.py output/ZmmTP_Data.root:${LABEL} --bkg-model ${BKG} --sig-model ${SIG} --title "${TITLE}" -p ${DIR}/data;



'''



LABEL="ID_pt_eta_bins"; SMALL="eff.ID"; DIR="output/tag_and_probe_e_v16_6/${LABEL}"; TITLE="Non-trig MVA ID 16"; BKG="CMSShape"; SIG="DoubleVCorr"; python scripts/fitTagAndProbe.py output/ZeeTP_Embedding.root:${LABEL} --bkg-model ${BKG} --sig-model ${SIG} --title "${TITLE}" -p ${DIR}/embedding --particle 'e'; python scripts/fitTagAndProbe.py output/ZeeTP_Data.root:${LABEL} --bkg-model ${BKG} --sig-model ${SIG} --title "${TITLE}" -p ${DIR}/data --particle 'e';

LABEL="Iso_pt_eta_bins"; SMALL="eff.Iso"; DIR="output/tag_and_probe_e_v16_6/${LABEL}"; TITLE="Iso_{rel} < 0.10"; BKG="Exponential"; SIG="DoubleVCorr"; python scripts/fitTagAndProbe.py output/ZeeTP_Embedding.root:${LABEL} --bkg-model ${BKG} --sig-model ${SIG} --title "${TITLE}" -p ${DIR}/embedding --particle 'e'; python scripts/fitTagAndProbe.py output/ZeeTP_Data.root:${LABEL} --bkg-model ${BKG} --sig-model ${SIG} --title "${TITLE}" -p ${DIR}/data --particle 'e'; 

LABEL="AIso1_pt_eta_bins"; SMALL="eff.Iso"; DIR="output/tag_and_probe_e_v16_6/${LABEL}"; TITLE="Iso_{rel} #in [0.10,0.20]"; BKG="Exponential"; SIG="DoubleVCorr"; python scripts/fitTagAndProbe.py output/ZeeTP_Embedding.root:${LABEL} --bkg-model ${BKG} --sig-model ${SIG} --title "${TITLE}" -p ${DIR}/embedding --particle 'e'; python scripts/fitTagAndProbe.py output/ZeeTP_Data.root:${LABEL} --bkg-model ${BKG} --sig-model ${SIG} --title "${TITLE}" -p ${DIR}/data --particle 'e'; 

LABEL="AIso2_pt_eta_bins"; SMALL="eff.Iso"; DIR="output/tag_and_probe_e_v16_6/${LABEL}"; TITLE="Iso_{rel} #in [0.20,0.50]"; BKG="Exponential"; SIG="DoubleVCorr"; python scripts/fitTagAndProbe.py output/ZeeTP_Embedding.root:${LABEL} --bkg-model ${BKG} --sig-model ${SIG} --title "${TITLE}" -p ${DIR}/embedding --particle 'e'; python scripts/fitTagAndProbe.py output/ZeeTP_Data.root:${LABEL} --bkg-model ${BKG} --sig-model ${SIG} --title "${TITLE}" -p ${DIR}/data --particle 'e';  


LABEL="Trg_Iso_pt_eta_bins"; SMALL="eff.Trg_Iso"; DIR="output/tag_and_probe_e_v16_6/${LABEL}"; TITLE="Trg Ele25eta2p1WPTight"; BKG="Exponential"; SIG="DoubleVCorr"; python scripts/fitTagAndProbe.py output/ZeeTP_Embedding.root:${LABEL} --bkg-model ${BKG} --sig-model ${SIG} --title "${TITLE}" -p ${DIR}/embedding --particle 'e'; python scripts/fitTagAndProbe.py output/ZeeTP_Data.root:${LABEL} --bkg-model ${BKG} --sig-model ${SIG} --title "${TITLE}" -p ${DIR}/data --particle 'e';  

LABEL="Trg_AIso1_pt_bins_inc_eta"; SMALL="eff.Trg_AIso1inc"; DIR="output/tag_and_probe_e_v16_6/${LABEL}"; TITLE="Trg Ele25eta2p1WPTight | Iso_{rel} #in [0.10,0.20]"; BKG="Exponential"; SIG="DoubleVCorr"; python scripts/fitTagAndProbe.py output/ZeeTP_Embedding.root:${LABEL} --bkg-model ${BKG} --sig-model ${SIG} --title "${TITLE}" -p ${DIR}/embedding --particle 'e'; python scripts/fitTagAndProbe.py output/ZeeTP_Data.root:${LABEL} --bkg-model ${BKG} --sig-model ${SIG} --title "${TITLE}" -p ${DIR}/data --particle 'e'; 

LABEL="Trg_AIso1_pt_eta_bins"; SMALL="eff.Trg_AIso1"; DIR="output/tag_and_probe_e_v16_6/${LABEL}"; TITLE="Trg Ele25eta2p1WPTight"; BKG="Exponential"; SIG="DoubleVCorr"; python scripts/fitTagAndProbe.py output/ZeeTP_Embedding.root:${LABEL} --bkg-model ${BKG} --sig-model ${SIG} --title "${TITLE}" -p ${DIR}/embedding --particle 'e'; python scripts/fitTagAndProbe.py output/ZeeTP_Data.root:${LABEL} --bkg-model ${BKG} --sig-model ${SIG} --title "${TITLE}" -p ${DIR}/data --particle 'e'; 

LABEL="Trg_AIso2_pt_bins_inc_eta"; SMALL="eff.Trg_AIso2inc"; DIR="output/tag_and_probe_e_v16_6/${LABEL}"; TITLE="Trg Ele25eta2p1WPTight | Iso_{rel} #in [0.20,0.50]"; BKG="Exponential"; SIG="DoubleVCorr"; python scripts/fitTagAndProbe.py output/ZeeTP_Embedding.root:${LABEL} --bkg-model ${BKG} --sig-model ${SIG} --title "${TITLE}" -p ${DIR}/embedding --particle 'e'; python scripts/fitTagAndProbe.py output/ZeeTP_Data.root:${LABEL} --bkg-model ${BKG} --sig-model ${SIG} --title "${TITLE}" -p ${DIR}/data --particle 'e';

LABEL="Trg_AIso2_pt_eta_bins"; SMALL="eff.Trg_AIso2"; DIR="output/tag_and_probe_e_v16_6/${LABEL}"; TITLE="Trg Ele25eta2p1WPTight"; BKG="Exponential"; SIG="DoubleVCorr"; python scripts/fitTagAndProbe.py output/ZeeTP_Embedding.root:${LABEL} --bkg-model ${BKG} --sig-model ${SIG} --title "${TITLE}" -p ${DIR}/embedding --particle 'e'; python scripts/fitTagAndProbe.py output/ZeeTP_Data.root:${LABEL} --bkg-model ${BKG} --sig-model ${SIG} --title "${TITLE}" -p ${DIR}/data --particle 'e'; 



