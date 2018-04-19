#!/bin/sh

LABEL="muon_Selection_Iso"; SMALL="eff.Iso"; DIR="output/tag_and_probe_m_v16_3/${LABEL}"; TITLE="Iso_{rel} < 0.15"; BKG="Exponential"; SIG="DoubleVCorr"; python scripts/fitTagAndProbe.py output/ZmmTP_Data.root:${LABEL} --bkg-model ${BKG} --sig-model ${SIG} --title "${TITLE}" -p ${DIR}/data

LABEL="muon_Selection_VVLIso"; SMALL="eff.Iso"; DIR="output/tag_and_probe_m_v16_3/${LABEL}"; TITLE="Iso_{rel} < 0.4"; BKG="Exponential"; SIG="DoubleVCorr"; python scripts/fitTagAndProbe.py output/ZmmTP_Data.root:${LABEL} --bkg-model ${BKG} --sig-model ${SIG} --title "${TITLE}" -p ${DIR}/data

LABEL="muon_Selection_EmbeddedID"; SMALL="eff.ID"; DIR="output/tag_and_probe_m_v16_3/${LABEL}"; TITLE="Medium (HIP-safe for B to F) ID"; BKG="CMSShape"; SIG="DoubleVCorr"; python scripts/fitTagAndProbe.py output/ZmmTP_Data.root:${LABEL} --bkg-model ${BKG} --sig-model ${SIG} --title "${TITLE}" -p ${DIR}/data

LABEL="muon_Selection_Iso"; SMALL="eff.Iso"; DIR="output/tag_and_probe_m_v16_3/${LABEL}"; TITLE="Iso_{rel} < 0.15"; BKG="Exponential"; SIG="DoubleVCorr"; python scripts/fitTagAndProbe.py output/ZmmTP_Data.root:${LABEL} --bkg-model ${BKG} --sig-model ${SIG} --title "${TITLE}" -p ${DIR}/data

LABEL="muon_Selection_Trg"; SMALL="eff.Trg_Iso"; DIR="output/tag_and_probe_m_v16_3/${LABEL}"; TITLE="Trg IsoMu24"; BKG="Exponential"; SIG="DoubleVCorr"; python scripts/fitTagAndProbe.py output/ZmmTP_Data.root:${LABEL} --bkg-model ${BKG} --sig-model ${SIG} --title "${TITLE}" -p ${DIR}/data
