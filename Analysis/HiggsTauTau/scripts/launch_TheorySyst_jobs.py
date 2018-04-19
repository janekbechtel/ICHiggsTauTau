from UserCode.ICHiggsTauTau.jobs import Jobs
import argparse
import os

job_mgr = Jobs()
parser = argparse.ArgumentParser()
job_mgr.attach_job_args(parser)
args = parser.parse_args()
job_mgr.set_args(args)

basedir = '%s/src/UserCode/ICHiggsTauTau/Analysis/HiggsTauTau' % os.environ[
    'CMSSW_BASE']

MAX_EVTS = 100000
FILES_PER_JOB = 40
PROD='Oct06_MC_80X_'

DATA_SAMPLES = {
}

MC_SAMPLES = {
    'GluGluHToTauTau_M-125-herwigpp':           ['GluGluHToTauTau_M-125-herwigpp'],
    'GluGluHToTauTau_M-125':           ['GluGluHToTauTau_M-125'],
}

SAMPLES = {}
SAMPLES.update(DATA_SAMPLES)
SAMPLES.update(MC_SAMPLES)
SEQUENCES = ['Theory']
#SEQUENCES = ['Zmm', 'ZmmTP', 'Zee', 'ZeeTP', 'ZmtTP', 'ZmtTP/scale_t_hi', 'ZmtTP/scale_t_lo', 'EffectiveEvents']
#SEQUENCES = ['HashMap']

if 'HashMap' in SEQUENCES:
  FILES_PER_JOB = 1E6

WHITELIST = {
    'Theory': list(MC_SAMPLES.keys())
}

OUTPUT = 'output/HTT2016Studies_'+job_mgr.task_name
os.system('mkdir -p %s' % OUTPUT)
for seq in SEQUENCES:
    os.system('mkdir -p %s/%s' % (OUTPUT, seq))

task = job_mgr.task_name

for sa in SAMPLES:
    seqs = []
    doSample = False
    for seq in SEQUENCES:
        if seq in WHITELIST:
            if sa in WHITELIST[seq]:
                seqs.append(seq)
                doSample = True
        else:
            seqs.append(seq)
            doSample = True
    if not doSample:
        continue
    filelists = ['%s%s.dat' % (PROD, X) for X in SAMPLES[sa]]
    cfg = {
        # General settings
        'output_dir': '%s' % (OUTPUT),
        'output_name': '%s.root' % (sa),
        'filelists': filelists,
        'max_events': MAX_EVTS,
        'is_data': sa in DATA_SAMPLES.keys(),
        'sequences': seqs,
        # Lumi settings
        'lumi_mask': 'input/json/Cert_271036-276811_13TeV_PromptReco_Collisions16_JSON.txt',
        'lumi_out': '%s/lumi_%s' % (OUTPUT, sa),
        # Pileup weights
        'data_pu': 'input/pileup/Data_Pileup_2016_69p2mb_Cert_271036-276811.root:pileup',
        'data_pu_hi': 'input/pileup/Data_Pileup_2016_80mb_Cert_271036-276811.root:pileup',
        'mc_pu': 'input/pileup/MC_Spring16_PU25ns_V1.root:pileup',
        # Hash map settings
        'hash_map_mode': 0,
        'hash_map_input': 'input/string_hash/hash_map.json',
        'hash_map_output': 'hash_map_%s.json' % sa,
        # Trigger info settings
        'trigger_info_output': '%s/trigger_info_%s.json' % (OUTPUT, sa),
        # Scale factors workspace
        'sf_wsp': 'input/scale_factors/htt_scalefactors_v3.root',
        # ZmtTP decay mode selection
        'ZmtTP_tauDM': 'decayModeFinding'
        #'ZmtTP_tauDM': 'decayModeFindingNewDMs'
    }
    job_mgr.add_filelist_split_jobs(
        prog=basedir+'/bin/HTT2016Studies',
        cfg=cfg,
        files_per_job=FILES_PER_JOB,
        output_cfgs=['output_name', 'lumi_out', 'trigger_info_output'])
    job_mgr.task_name = task + '-' + sa
    job_mgr.flush_queue()
