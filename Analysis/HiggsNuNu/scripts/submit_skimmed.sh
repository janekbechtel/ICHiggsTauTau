#!/bin/sh

## Try and take the JOBWRAPPER and JOBSUBMIT commands
## from the environment if set, otherwise use these defaults
: ${JOBWRAPPER:="./scripts/generate_job.sh"}
: ${JOBSUBMIT:="eval"}

#JOBSCRIPT="./scripts/submit_ic_batch_job.sh" 

OTHEROPTIONS="--doTopCR=false"
DOCERN=0

if (( "$DOCERN" == "1" )); then
    JOBSCRIPT="./scripts/submit_cern_batch_job.sh" 
    JOBQUEUE="1nh"
    PREFIX=/afs/cern.ch/work/a/amagnan/SkimFiles/Apr04/nunu/
else 
    JOBSCRIPT="./scripts/submit_ic_batch_job.sh" 
    JOBQUEUE="hepshort.q"
    PREFIX=/vols/ssd00/cms/amagnan/trigskims/nunu/MET130/
fi

export JOBSUBMIT=$JOBSCRIPT" "$JOBQUEUE


echo "Using job-wrapper: " $JOBWRAPPER
echo "Using job-submission: " $JOBSUBMIT

CONFIG=scripts/DefaultConfigMC.cfg
PRODUCTION=Apr04

for METCUT in 130 #0 130
  do
  for CHANNEL in nunu enu munu taunu 
    do
    for SYST in central #JESUP JESDOWN JERBETTER JERWORSE #NOTE TO RUN JER DOSMEAR MUST BE SET TO TRUE IN THE CONFIG
      do
      SYSTOPTIONS="--dojessyst=false --dojersyst=false"
      JOBDIRPREFIX=jobs/
      JOBDIR=$JOBDIRPREFIX/$CHANNEL/MET$METCUT/
      OUTPUTPREFIX=lepeffoffoutput/ #oldanalysisruns/220713_taunominaltightwithsysts/output/
      OUTPUTDIR=$OUTPUTPREFIX/$CHANNEL/MET$METCUT/
      
      if [ "$SYST" = "JESUP" ]
	  then
	  SYSTOPTIONS="--dojessyst=true --jesupordown=true --dojersyst=false"
	  JOBDIR=$JOBDIRPREFIX/$CHANNEL/MET$METCUT/JESUP/
	  OUTPUTDIR=$OUTPUTPREFIX/$CHANNEL/MET$METCUT/JESUP/
      fi
      
      if [ "$SYST" = "JESDOWN" ]
	  then
	  SYSTOPTIONS="--dojessyst=true --jesupordown=false --dojersyst=false"
	  JOBDIR=$JOBDIRPREFIX/$CHANNEL/MET$METCUT/JESDOWN/
	  OUTPUTDIR=$OUTPUTPREFIX/$CHANNEL/MET$METCUT/JESDOWN/
      fi

      if [ "$SYST" = "JERBETTER" ]
	  then
	  SYSTOPTIONS="--dojessyst=false --dojersyst=true --jerbetterorworse=true"
	  JOBDIR=$JOBDIRPREFIX/$CHANNEL/MET$METCUT/JERBETTER/
	  OUTPUTDIR=$OUTPUTPREFIX/$CHANNEL/MET$METCUT/JERBETTER/
      fi

      if [ "$SYST" = "JERWORSE" ]
	  then
	  SYSTOPTIONS="--dojessyst=false --dojersyst=true --jerbetterorworse=false"
	  JOBDIR=$JOBDIRPREFIX/$CHANNEL/MET$METCUT/JERWORSE/
	  OUTPUTDIR=$OUTPUTPREFIX/$CHANNEL/MET$METCUT/JERWORSE/
      fi

      echo "Config file: $CONFIG"
      
      mkdir -p $JOBDIR
      mkdir -p $OUTPUTDIR
      
      for FILELIST in `ls filelists/skim/$PRODUCTION/nunu/*`
	do
	echo "Processing files in "$FILELIST
	
	echo $FILELIST
	echo $FILELIST > tmp.txt
	sed "s/filelists\/skim\/${PRODUCTION}\/nunu\///" tmp.txt > tmp2.txt
	

	grep "MET-2012" tmp.txt
	if (( "$?" == 0 )); then
	    #continue
	    JOB=Data_`sed "s/\.dat//" tmp2.txt`
	    CONFIG=scripts/DefaultConfig.cfg
	else 
	    #continue
	    JOB=MC_`sed "s/\.dat//" tmp2.txt`
	    CONFIG=scripts/DefaultConfigMC.cfg
	fi
    #JOB=`sed "s/\.dat//" tmp2.txt`
	
	echo "JOB name = $JOB"

	DOWSTREAM=0
	
	grep "EWK-W2j" tmp.txt
	if (( "$?" == 0 )); then
	    let DOWSTREAM=1
	fi
	grep  "JetsToLNu" tmp.txt
	if (( "$?" == 0 )); then
	    let DOWSTREAM=1
	fi
	
	if (( "$DOWSTREAM" == "1" )); then
	    for FLAVOUR in enu munu taunu
	      do
	      WJOB=$JOB"_"$FLAVOUR
	      

	      if (( "$DOCERN" == "1" )); then
		  $JOBWRAPPER "./bin/HiggsNuNu --cfg=$CONFIG --filelist="$FILELIST" --input_prefix=$PREFIX --output_name=$WJOB.root --output_folder=$OUTPUTDIR --met_cut=$METCUT $SYSTOPTIONS --channel=$CHANNEL --wstream=$FLAVOUR $OTHEROPTIONS &> $JOBDIR/$WJOB.log" $JOBDIR/$WJOB.sh 0
		  $JOBSUBMIT $JOBDIR/$WJOB.sh $WJOB  
	      else 
		  $JOBWRAPPER "./bin/HiggsNuNu --cfg=$CONFIG --filelist="$FILELIST" --input_prefix=$PREFIX --output_name=$WJOB.root --output_folder=$OUTPUTDIR --met_cut=$METCUT $SYSTOPTIONS --channel=$CHANNEL --wstream=$FLAVOUR $OTHEROPTIONS &> $JOBDIR/$WJOB.log" $JOBDIR/$WJOB.sh
		  $JOBSUBMIT $JOBDIR/$WJOB.sh
	      fi                                                                                 
	    done
	else  
	    if (( "$DOCERN" == "1" )); then
	    $JOBWRAPPER "./bin/HiggsNuNu --cfg=$CONFIG --filelist="$FILELIST" --input_prefix=$PREFIX --output_name=$JOB.root --output_folder=$OUTPUTDIR --met_cut=$METCUT $SYSTOPTIONS --channel=$CHANNEL $OTHEROPTIONS &> $JOBDIR/$JOB.log" $JOBDIR/$JOB.sh 0
	    $JOBSUBMIT $JOBDIR/$JOB.sh $JOB
	    else
		$JOBWRAPPER "./bin/HiggsNuNu --cfg=$CONFIG --filelist="$FILELIST" --input_prefix=$PREFIX --output_name=$JOB.root --output_folder=$OUTPUTDIR --met_cut=$METCUT $SYSTOPTIONS --channel=$CHANNEL $OTHEROPTIONS &> $JOBDIR/$JOB.log" $JOBDIR/$JOB.sh
		$JOBSUBMIT $JOBDIR/$JOB.sh
	    fi
	fi
	
	rm tmp.txt tmp2.txt
	
      done

    done
    
  done
  
done

#if (( "$#" != "2" ))
#then
#echo ""
#    exit
#fi
