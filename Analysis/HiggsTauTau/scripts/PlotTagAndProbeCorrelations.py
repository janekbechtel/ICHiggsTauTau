#!/usr/bin/env python
# -*- coding: utf-8 -*-
import sys
import os
import argparse
import glob
import ROOT
import array
import matplotlib.pyplot as plt
ROOT.PyConfig.IgnoreCommandLineOptions = True


parser = argparse.ArgumentParser()

parser.add_argument(
    'input', nargs='+', help="""Input files""")
parser.add_argument(
    '--output', '-o', default='correlation_plots', help="""Name of the output
    plot without file extension""")
args = parser.parse_args()

f = ROOT.TFile.Open(args.input[0])
tree = f.Get("inclusive/ZeeTP")

pt_t = []
pt_p = []
trg_p = []
pt_t_trg_p = []
pt_t_notrg_p = []
pt_p_trg_p = []
pt_p_notrg_p = []
eff_ptt_ptp = []
nevents_ptt_ptp = []
emptylist = []
cum_eff = []
cum_evt = []
trg50 = [0,0,0,0,0,0,0,0,0,0,0,0,0,0]
all50 = [0,0,0,0,0,0,0,0,0,0,0,0,0,0]
trg30 = [0,0,0,0,0,0,0,0,0,0,0,0,0,0]
all30 = [0,0,0,0,0,0,0,0,0,0,0,0,0,0]
for i in range(16):
	emptylist.append(0.)
for i in range(14):
	eff_ptt_ptp.append(emptylist[:])
	nevents_ptt_ptp.append(emptylist[:])
	cum_eff.append(emptylist[:])
	cum_evt.append(emptylist[:])
count = 0
count2 = 0
for event in tree:
	if (event.m_ll > 80 and event.m_ll < 100):
		if (event.id_p and event.iso_p < 0.10):
			pt_t.append(event.pt_t)
			pt_p.append(event.pt_p)
			trg_p.append(event.trg_p_Ele25eta2p1WPTight)
			if event.trg_p_Ele25eta2p1WPTight:
				pt_t_trg_p.append(event.pt_t)
				pt_p_trg_p.append(event.pt_p)
			else:
				pt_t_notrg_p.append(event.pt_t)
				pt_p_notrg_p.append(event.pt_p)
			if (event.pt_p < 100 and event.pt_t < 100 and event.pt_p >= 20 and event.pt_t >= 30):
				nevents_ptt_ptp[int(event.pt_t/5)-6][int(event.pt_p/5)-4]+=1.
				if event.trg_p_Ele25eta2p1WPTight:
					eff_ptt_ptp[int(event.pt_t/5)-6][int(event.pt_p/5)-4]+=1.
			if (event.pt_p < 100 and event.pt_t < 100 and event.pt_p >= 50 and event.pt_t >= 30):
				all50[int(event.pt_t/5)-6]+=1
				if event.trg_p_Ele25eta2p1WPTight:
					trg50[int(event.pt_t/5)-6]+=1
			if (event.pt_p < 40 and event.pt_t < 100 and event.pt_p >= 30 and event.pt_t >= 30):
				all30[int(event.pt_t/5)-6]+=1
				if event.trg_p_Ele25eta2p1WPTight:
					trg30[int(event.pt_t/5)-6]+=1
	count += 1
	if count == 100000:
		count2 += 1
		print (str(count2)+" * 100000")
		count = 0
		
	#if count2 == 10:
	#	break
f.Close()

#test cum method
#for i in range(14):
#	eff_ptt_ptp[i][5] = 0.
#	nevents_ptt_ptp[i][5] = 10.
#eff_ptt_ptp[13][5]=10.

cum_eff[13]=eff_ptt_ptp[13][:]
cum_evt[13]=nevents_ptt_ptp[13][:]
for i in range(14-1):
	cum_eff[12-i]=[x+y for x, y in zip(cum_eff[13-i], eff_ptt_ptp[12-i])]
	cum_evt[12-i]=[x+y for x, y in zip(cum_evt[13-i], nevents_ptt_ptp[12-i])]
	all50[12-i]+=all50[13-i]
	trg50[12-i]+=trg50[13-i]
	all30[12-i]+=all30[13-i]
	trg30[12-i]+=trg30[13-i]




for i in range(16):
	for j in range(14):
		if (nevents_ptt_ptp[j][i] > 0):
			eff_ptt_ptp[j][i]/=nevents_ptt_ptp[j][i]
		else:
			eff_ptt_ptp[j][i]=0.
		#cum
		if (cum_evt[j][i] > 0):
			cum_eff[j][i]/=cum_evt[j][i]
		else:
			cum_eff[j][i]=0.
			
for j in range(14):
	if (all50[j] > 0):
		trg50[j]/=float(all50[j])
	else:
		trg50[j]=0.
	if (all30[j] > 0):
		trg30[j]/=float(all30[j])
	else:
		trg30[j]=0.
		
plt.hist2d(pt_t,pt_p, bins=80, range = [[20,100],[20,100]])
plt.xlabel('tag pt (GeV)')
plt.ylabel('probe pt (GeV)')
plt.title('pt correlation between tag and probe')
#plt.set_cmap('Blues')
plt.colorbar()
plt.savefig(args.output+"_pt_pt")

plt.clf()

plt.hist2d(pt_t,trg_p, bins=[80,2], range = [[20,100],[0,1]])
plt.xlabel('tag pt (GeV)')
plt.ylabel('probe trigger fired')
plt.title('correlation between tag pt and probe trigger')
plt.savefig(args.output+"_pt_trg")

plt.clf()

plt.hist(pt_t_trg_p, bins=80, range = [20,100])
plt.xlabel('tag pt (GeV)')
plt.ylabel('number of events')
plt.title('trigger fired')
plt.savefig(args.output+"_trg")

plt.clf()

plt.hist(pt_t_notrg_p, bins=80, range = [20,100])
plt.xlabel('tag pt (GeV)')
plt.ylabel('number of events')
plt.title('trigger not fired')
plt.savefig(args.output+"_notrg")

plt.clf()

plt.hist(pt_p_trg_p, bins=80, range = [20,100])
plt.xlabel('probe pt (GeV)')
plt.ylabel('number of events')
plt.title('trigger fired')
plt.savefig(args.output+"_p_trg")

plt.clf()

plt.hist(pt_p_notrg_p, bins=80, range = [20,100])
plt.xlabel('probe pt (GeV)')
plt.ylabel('number of events')
plt.title('trigger not fired')
plt.savefig(args.output+"_p_notrg")

plt.clf()

xlist = []
ylist = []
vallist = []

for i in range(14):
	for j in range(16):
		xlist.append(5*i+30)
		ylist.append(5*j+20)
		vallist.append(eff_ptt_ptp[i][j])

plt.hist2d(xlist,ylist, bins=[14,16], range = [[30,100],[20,100]], weights = vallist)
plt.xlabel('tag pt (GeV)')
plt.ylabel('probe pt (GeV)')
plt.title('efficiency over pt')
plt.set_cmap('Blues')
plt.colorbar()
plt.savefig(args.output+"_eff_over_pt_pt")

plt.clf()

mean = []
for i in range(16):
	mean.append(0.0)
	for j in range(14):
		mean[-1] += eff_ptt_ptp[j][i]
	mean[-1] /= 14.
vallist = []
for i in range(14):
	for j in range(16):
		vallist.append(eff_ptt_ptp[i][j]-mean[j])

plt.hist2d(xlist,ylist, bins=[14,16], range = [[30,100],[20,100]], weights = vallist)
plt.xlabel('tag pt (GeV)')
plt.ylabel('probe pt (GeV)')
plt.title('efficiency over pt')
plt.set_cmap('coolwarm')
plt.colorbar()
plt.savefig(args.output+"_releff_over_pt_pt")

plt.clf()

#cum
mean = []
for i in range(16):
	mean.append(0.0)
	for j in range(14):
		mean[-1] += cum_eff[j][i]
	mean[-1] /= 14.
vallist = []
for i in range(14):
	for j in range(16):
		vallist.append(cum_eff[i][j]-mean[j])

plt.hist2d(xlist,ylist, bins=[14,16], range = [[30,100],[20,100]], weights = vallist)
plt.xlabel('tag pt (GeV)')
plt.ylabel('probe pt (GeV)')
plt.title('efficiency over pt')
plt.set_cmap('coolwarm')
plt.colorbar()
plt.savefig(args.output+"_rel_cum_eff_over_pt_pt")

plt.clf()

xlist=[30,35,40,45,50,55,60,65,70,75,80,85,90,95]
plt.hist(xlist, weights=trg50, bins=14, range = [30,100])
plt.gca().set_ylim([0.8, 1.0])
plt.xlabel('tag pt (GeV)')
plt.ylabel('number of events')
plt.title('cum. efficiciency | probe pt from 50 to 100 GeV')
plt.savefig(args.output+"_50to100")

plt.clf()

xlist=[30,35,40,45,50,55,60,65,70,75,80,85,90,95]
plt.hist(xlist, weights=trg30, bins=14, range = [30,100])
plt.gca().set_ylim([0.6, 0.8])
plt.xlabel('tag pt (GeV)')
plt.ylabel('number of events')
plt.title('cum. efficiciency | probe pt from 30 to 40 GeV')
plt.savefig(args.output+"_30to40")
