#!/bin/sh

RUN=$1


echo "Will ran scripts for RUN: ${RUN}"


echo "First, let's run Arnaud's CMSSW analyzer"
sleep 2

cd ~/hexaDataAnalysis/CMSSW_8_0_1/src/HGCal/
./source_this.sh
eval `scramv1 runtime -sh`

cmsRun offlineMonitoring_cfg.py runNumber=${RUN} rdoutID=2;
cmsRun offlineMonitoring_cfg.py runNumber=${RUN} rdoutID=3;


echo "Done "

echo "Now, let's make those plots to appear on the web"

sleep 2 

/home/daq/web_dqm/scripts/make_dqm_plots/make_dqm_plots.exe ${RUN} cmssw 2
/home/daq/web_dqm/scripts/make_dqm_plots/make_dqm_plots.exe ${RUN} cmssw 3

echo "Done"
exho "The plots should appear at http://svhgcal01.cern.ch/RUN_${RUN}_CMSSW_DQM/ (in sub-sub-directories)"

echo "Now let's do the same for the plots of Online Monitor"


/home/daq/web_dqm/scripts/make_dqm_plots/make_dqm_plots.exe ${RUN}

sleep 2

echo "All Done."
echo "Enjoy at http://svhgcal01.cern.ch/"
