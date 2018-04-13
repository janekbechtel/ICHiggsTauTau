#!/bin/sh

LABEL="muon_Selection_ID"; SMALL="eff.ID"; DIR="output/tag_and_probe_m_v16_6"; TITLE="Medium ID"; BKG="CMSShape"; SIG="DoubleVCorr"; 
python scripts/plotEffSlices.py output/ZmmTP_Data_Fits_${LABEL}.root:${LABEL}:'Title="Data"' --y-range 0.7,1.0 --ratio-y-range 0.8,1.2 --label-pos 3 --x-title "#eta" -o "${SMALL}" --title "${TITLE} Efficiency" --plot-dir ${DIR}
LABEL="muon_Selection_EmbeddedID"; SMALL="eff.emb_ID"; DIR="output/tag_and_probe_m_v16_6"; TITLE="(Global & Loose) ID"; BKG="CMSShape"; SIG="DoubleVCorr"; 
python scripts/plotEffSlices.py output/ZmmTP_Data_Fits_${LABEL}.root:${LABEL}:'Title="Data"' --y-range 0.7,1.0 --ratio-y-range 0.8,1.2 --label-pos 3 --x-title "#eta" -o "${SMALL}" --title "${TITLE} Efficiency" --plot-dir ${DIR}
LABEL="muon_Selection_VVLIso"; SMALL="eff.VVLIso"; DIR="output/tag_and_probe_m_v16_6"; TITLE="VVL Iso"; BKG="CMSShape"; SIG="DoubleVCorr"; 
python scripts/plotEffSlices.py output/ZmmTP_Data_Fits_${LABEL}.root:${LABEL}:'Title="Data"' --y-range 0.9,1.0 --ratio-y-range 0.8,1.2 --label-pos 3 --x-title "#eta" -o "${SMALL}" --title "${TITLE} Efficiency" --plot-dir ${DIR}

LABEL="muon_Selection_Iso"; SMALL="eff.Iso"; DIR="output/tag_and_probe_m_v16_6"; TITLE="Iso_{rel} < 0.15"; BKG="Exponential"; SIG="DoubleVCorr"; 
python scripts/plotEffSlices.py output/ZmmTP_Data_Fits_${LABEL}.root:${LABEL}:'Title="Data"' --y-range 0.7,1.0 --ratio-y-range 0.8,1.2 --label-pos 3 --x-title "#eta" -o "${SMALL}" --title "${TITLE} Efficiency" --plot-dir ${DIR}

LABEL="muon_Selection_Trg"; SMALL="eff.Trg_Iso"; DIR="output/tag_and_probe_m_v16_6"; TITLE="Trg IsoMu22"; BKG="Exponential"; SIG="DoubleVCorr"; 
python scripts/plotEffSlices.py output/ZmmTP_Data_Fits_${LABEL}.root:${LABEL}:'Title="Data"'  --y-range 0.5,1.0 --label-pos 3 --ratio-y-range 0.95,1.2 --x-title "#eta" -o "${SMALL}" --title "${TITLE} Efficiency" --plot-dir ${DIR}
