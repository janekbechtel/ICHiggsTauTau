#!/bin/sh

JOBDIR=
INJOBDIR=0

while [ $# -gt 0 ] ; do

    case $1 in
        -h|--help|-help)
            echo "usage: "$0"  [-d <job directory>]"
            exit 1;
            ;;
        -d)
	JOBDIR=$2
	INJOBDIR=1
            shift 2;
            ;;
        *)
            echo " **** ERROR:  Unrecognised option: "
            $0 -h
            exit 1;
            ;;
    esac ;
done ;


for CHANNEL in nunu enu munu
  do
  for MET in 130 0 70
    do
    for DOQCD in 0 1 2
      do
      
      if (( "$INJOBDIR" == "0" )); then
	  JOBDIR=jobs/$CHANNEL/MET$MET/DOQCD$DOQCD/
      fi
      
      echo "Processing directory: "$JOBDIR

      RESULT=0
      
      for LOGFILE in `ls $JOBDIR/*.log`
	do
	grep -q "Processing Complete" $LOGFILE
	if (( "$?" == 1 )); then
	echo "File $LOGFILE failed !"
	let RESULT=1
	SHFILE=`echo $LOGFILE | sed "s/log/sh/"`
	grep -q "Error opening the file" $LOGFILE
	if (( "$?" == 0 )); then
	    echo "--> Error opening a file, resubmitting job $SHFILE!"
	    ./scripts/submit_ic_batch_job.sh hepshort.q $SHFILE
	else
	    tail -5 $LOGFILE
	    echo "** SUGGESTION: resubmit with longer queue"
	    echo "./scripts/submit_ic_batch_job.sh hepmedium.q $SHFILE"
	fi
	fi
      done

      if (( "$RESULT" == 0 )); then
	  echo "-- All Clear !"
      fi
      
      if (( "$INJOBDIR" == "1" )); then
	  exit;
      fi
      
    done
  done
done