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
        'name': 'TrgTau_Iso_pt_eta_bins',
        'var': 'm_ll(50,75,125)',
        'tag': 'muon_p && trg_t_IsoMu22 && id_p && iso_p < 0.15',
        'probe': 'trg_p_PFTau120',
        'binvar_x': 'pt_p',
        'bins_x': [20., 21., 22., 23., 24., 25., 30., 40., 50., 60., 80., 100., 200., 1000.],
        'binvar_y': 'abs(eta_p)',
        'bins_y': [0, 0.9, 1.2, 2.1, 2.4]
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
    'DYJetsToLL': analysis.TTreeEvaluator('inclusive/ZmmTP', 'output/HTT2016Studies_Jan12/ZmmTP/DYJetsToLLM50.root'),
    'Data': analysis.TTreeEvaluator('inclusive/ZmmTP', 'output/HTT2016Studies_Jan12/ZmmTP/SingleMuon.root') #,
    #'DataB': analysis.TTreeEvaluator('inclusive/ZmmTP', 'output/HTT2016Studies_Jan12/ZmmTP/SingleMuonB.root'),
    #'DataC': analysis.TTreeEvaluator('inclusive/ZmmTP', 'output/HTT2016Studies_Jan12/ZmmTP/SingleMuonC.root'),
    #'DataD': analysis.TTreeEvaluator('inclusive/ZmmTP', 'output/HTT2016Studies_Jan12/ZmmTP/SingleMuonD.root'),
    #'DataE': analysis.TTreeEvaluator('inclusive/ZmmTP', 'output/HTT2016Studies_Jan12/ZmmTP/SingleMuonE.root'),
    #'DataF': analysis.TTreeEvaluator('inclusive/ZmmTP', 'output/HTT2016Studies_Jan12/ZmmTP/SingleMuonF.root'),
    #'DataG': analysis.TTreeEvaluator('inclusive/ZmmTP', 'output/HTT2016Studies_Jan12/ZmmTP/SingleMuonG.root'),
    #'DataH': analysis.TTreeEvaluator('inclusive/ZmmTP', 'output/HTT2016Studies_Jan12/ZmmTP/SingleMuonH.root')
}


# sys.exit(0)

for sample in trees:
    outfile = ROOT.TFile('Tau_Trigger/ZmmTP_%s.root' % sample, 'RECREATE')
    hists = trees[sample].Draw(drawlist, compiled=True)

    i = 0
    for cfg in bin_cfgs:
        wsp = ROOT.RooWorkspace('wsp_'+cfg['name'], '')
        var = wsp.factory('m_ll[100,75,125]')

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
