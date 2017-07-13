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
void plot_hist_to_pdf(TH *hist, TCanvas *c, std::string pathname)
{

  gStyle->SetPalette(56);
  gStyle->SetOptStat(0);
  gStyle->SetNumberContours(50);

  TString drawOpt("");
  if (hist->InheritsFrom("TH2"))
    drawOpt="COLZ2";
  if (hist->InheritsFrom("TPoly"))
    drawOpt="COLZ2 TEXT";
  
  hist->Draw(drawOpt);
  c->Update();
  c->SaveAs((pathname + "/" + hist->GetName() + ".pdf").c_str());
  c->SaveAs((pathname + "/" + hist->GetName() + ".png").c_str());


  // for some specific plots, we save them also in the main directory:

  TString histName = hist->GetName();
  if (histName.Contains("hexagons_occ_HA_bit") ||
      histName.Contains("p_waveform_HG")) {
    std::vector<std::string> strs;
    boost::split(strs, pathname, boost::is_any_of("/"));
    //for (Int_t i = 0; i < 5; ++i)
    //std::cout<<strs[i]<<std::endl;
    c->SaveAs(("/"+strs[0]+"/"+strs[1]+"/"+strs[2]+"/"+strs[3]+"/"+strs[4]+"/"+ + hist->GetName() + ".png").c_str());
  }
}

// Iterate over hexaboard/layer/wire chamber
void iter_over_detector_part(TDirectoryFile *tdf_detector_part, std::string pathname)
{
    TIter next(tdf_detector_part->GetListOfKeys());

    TKey *key1, *key2;

    for (Int_t i = 0; (key1 = (TKey*)next()); ++i)
    {
        // Create directory for each hexaboard
        std::string sub_pathname(pathname + key1->GetName());
        boost::filesystem::create_directories(sub_pathname);

        TDirectoryFile *tdf = (TDirectoryFile*)key1->ReadObj();

        // loop over histograms
        TIter next_hexagon_hist(tdf->GetListOfKeys());
        for (Int_t j = 0; (key2 = (TKey*)next_hexagon_hist()); ++j)
        {
            plot_hist_to_pdf(key2->ReadObj(), c1, sub_pathname);
        }
    }
}

int main( int argc, char *argv[] )
{
    if (argc < 1 + 1)
    {
        std::cout << "This script plots all histograms found in a rootfile from EUDAC." << std::endl;
        std::cout << std::endl;
        std::cout << "Usage:" << std::endl;
        std::cout << "    make_dqm_plots *RUNNUMBER*" << std::endl;
        exit(0);
    }


    // Read in ROOT File
    std::string file_dirname = "/home/daq/eudaq-shift/data_root/run" + std::string(argv[1]) + ".root";
    TFile *tfile = new TFile(file_dirname.c_str(), "READ");

    // Determine the name of the output directory
    //std::vector<std::string> strs;
    //boost::split(strs, argv[1], boost::is_any_of("/"));
    //for (Int_t i = 0; i < 5; ++i) strs[strs.size() - 1].pop_back();
    std::string out_dirname = "/home/daq/web_dqm/RUN_" + std::string(argv[1]) + "_OnlineMon";


    // Canvas to draw all the plots on
    c1 = new TCanvas("c1", "c1", 500, 500);
    c1->cd();

    TIter next(tfile->GetListOfKeys());
    TKey *key;

    while ((key = (TKey*)next()))
    {
       std::string subdet_name = key->GetName();
       // Create Directory structure
       boost::filesystem::create_directories(out_dirname + "/" + subdet_name);
       TDirectoryFile *tdf_subdet = (TDirectoryFile*)key->ReadObj();
       iter_over_detector_part(tdf_subdet, out_dirname + "/" + subdet_name + "/");

    }

    delete c1;
}
