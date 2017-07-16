#include <fstream>
#include <iostream>
#include <iostream>

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

#include "TFile.h"
#include "TH2Poly.h"
#include "TH2I.h"
#include "TH1I.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TKey.h"

#include "TCanvas.h"
#include "TStyle.h"


TCanvas *c1;

// Plot any histogram to pdf
template <typename TH>
void plot_hist_to_img(TH *hist, TCanvas *c, std::string pathname, bool is_cmssw=false)
{

  if (is_cmssw)
    gStyle->SetPalette(54);
  else
    gStyle->SetPalette(56);

  gStyle->SetOptStat(0);
  gStyle->SetNumberContours(50);

  TString drawOpt("");
  if (hist->InheritsFrom("TH2"))
    drawOpt="COLZ2";
  if (hist->InheritsFrom("TH2Poly"))
    drawOpt="COLZ2 TEXT";

  hist->Draw(drawOpt);
  c->Update();
  c->SaveAs((pathname + "/" + hist->GetName() + ".pdf").c_str());
  c->SaveAs((pathname + "/" + hist->GetName() + ".png").c_str());


  // for some specific plots, we save them also in the main directory:

  TString histName = hist->GetName();
  if (histName.Contains("hexagons_occ_HA_bit") ||
      histName.Contains("p_waveform_HG")) {
    // These plots are to be displayed at the top level
    std::vector<std::string> strs;
    boost::split(strs, pathname, boost::is_any_of("/"));
    //for (Int_t i = 0; i < 5; ++i)
    //std::cout<<strs[i]<<std::endl;
    c->SaveAs(("/"+strs[0]+"/"+strs[1]+"/"+strs[2]+"/"+strs[3]+"/"+strs[4]+"/"+ hist->GetName() + ".png").c_str());
    c->SaveAs(("/"+strs[0]+"/"+strs[1]+"/"+strs[2]+"/"+strs[3]+"/"+strs[4]+"/"+ hist->GetName() + ".pdf").c_str());
  }

  if (histName.Contains("h_hitmap_Calice") ||
      histName.Contains("XYmap_WireChamber") ){
    // These are the main plots of the secondary level
    std::vector<std::string> strs;
    boost::split(strs, pathname, boost::is_any_of("/"));
    c->SaveAs(("/"+strs[0]+"/"+strs[1]+"/"+strs[2]+"/"+strs[3]+"/"+strs[4]+"/"+ strs[5]+"/"+ hist->GetName() + ".png").c_str());
    c->SaveAs(("/"+strs[0]+"/"+strs[1]+"/"+strs[2]+"/"+strs[3]+"/"+strs[4]+"/"+ strs[5]+"/"+ hist->GetName() + ".pdf").c_str());
  }
}

// Iterate over hexaboard/layer/wire chamber
void iter_over_detector_part(TDirectoryFile *tdf_detector_part, std::string pathname, bool is_cmssw)
{
  TIter next(tdf_detector_part->GetListOfKeys());

  TKey *key1, *key2, *key3;

  for (Int_t i = 0; (key1 = (TKey*)next()); ++i)
    {
      std::cout<<" Doing CMSSW DQM plots"<<std::endl;
      
      // Create directory for each hexaboard
      std::string sub_pathname(pathname + key1->GetName());
      boost::filesystem::create_directories(sub_pathname);

      TDirectoryFile *tdf = (TDirectoryFile*)key1->ReadObj();

      if (is_cmssw) {
	// These are plots from CMSSW DQM of Arnaud
	//They are barried under more sub-directories:
	TIter next_2(tdf->GetListOfKeys());
	for (Int_t j = 0; (key2 = (TKey*)next_2()); ++j)
	  {
	    // Create that sub directory	    
	    std::string sub_pathname_2(pathname + key1->GetName()+"/"+key2->GetName());
	    boost::filesystem::create_directories(sub_pathname_2);

	    TDirectoryFile *tdf_2 = (TDirectoryFile*)key2->ReadObj();

	    TIter next_cmssw_hist(tdf_2->GetListOfKeys());

	    for (Int_t k = 0; (key3 = (TKey*)next_cmssw_hist()); ++k)
	      {
		plot_hist_to_img(key3->ReadObj(), c1, sub_pathname_2, is_cmssw);
	      }
	  }
      }
      else {
	std::cout<<" Doing Online MOnitor plots"<<std::endl;
	// These are plots from Online Monitor
	// loop over histograms
	TIter next_hexagon_hist(tdf->GetListOfKeys());
	for (Int_t j = 0; (key2 = (TKey*)next_hexagon_hist()); ++j)
	  {
	    plot_hist_to_img(key2->ReadObj(), c1, sub_pathname);
	  }
      }
      
    }
}

int main( int argc, char *argv[] )
{

  std::string file_name, out_dirname;

  if (argc < 2) {
    std::cout << "This script plots all histograms found in a rootfile from EUDAD." << std::endl;
    std::cout << std::endl;
    std::cout << "Usage:" << std::endl;
    std::cout << "    make_dqm_plots *RUNNUMBER*  [cmssw <board_id>]" << std::endl;
    exit(0);
  }

  char runNum[4];
  sprintf(runNum, "%04d", atoi(argv[1]));
  std::cout<<"Will produce plots for Run "<<runNum<<std::endl;


  bool cmssw = 0;
  std::string board_id = "0";
  if (argc == 4 && std::string(argv[2])=="cmssw") {
    board_id = std::string((argv[3]));
    file_name = "/home/daq/hexaDataAnalysis/rootData/julyBeamTest/HexaOutput_"+std::string(argv[1])+"_RDOUT" + std::string(board_id) + ".root";
    out_dirname = "/home/daq/web_dqm/RUN_" + std::string(runNum) + "_cmssw_DQM";
    cmssw = 1;
  }

  else {
    file_name = "/home/daq/eudaq-shift/data_root/run" + std::string(argv[1]) + ".root";
    out_dirname = "/home/daq/web_dqm/RUN_" + std::string(runNum) + "_OnlineMon";
  }
  // Read in ROOT File
  TFile *tfile = new TFile(file_name.c_str(), "READ");

  std::cout<<"file name: "<<file_name<<std::endl;
  std::cout<<"output directory: "<<out_dirname<<std::endl;

  // Canvas to draw all the plots on
  c1 = new TCanvas("c1", "c1", 500, 500);
  c1->cd();

  TIter next(tfile->GetListOfKeys());
  TKey *key;

  while ((key = (TKey*)next()))
    {
      std::string subdet_name = key->GetName();
      if (cmssw)
	subdet_name = key->GetName()+std::string("-RDB-")+std::string(board_id);
      // Create Directory structure
      boost::filesystem::create_directories(out_dirname + "/" + subdet_name);
      TDirectoryFile *tdf_subdet = (TDirectoryFile*)key->ReadObj();
      iter_over_detector_part(tdf_subdet, out_dirname + "/" + subdet_name + "/", cmssw);

    }

  delete c1;
}
