void legend(TLegend *leg, TGraph *g1, TGraph *g2, TGraph *g3)
{
  leg->AddEntry(g1,"Bit 5");
  leg->AddEntry(g2,"Bit 123");
  leg->AddEntry(g3,"Average Unbonded");
  leg->Draw("same");
}

void settings(TGraph *g, int style, int color)
{
  g->SetMarkerStyle(style);
  g->SetMarkerColor(color);
}

struct vectors
{
  std::vector<double> noisevec = {};
  std::vector<double> gainvec = {};
};
  
vectors readfile(int run, int scan)
{
  std::string filePath = "results/abc_star_RC_" + std::to_string(run) + "_" + std::to_string(scan) + ".txt";
  
  std::cout << filePath << "\n";
  ifstream inFile;
  inFile.open(filePath);
  vectors v;
  double noisesum{0};
  double gainsum{0};
  int i{0};
  int linecount{0};
  if (inFile.good())
    { 
      std::string line;
      getline(inFile,line);
      std::cout << line << "\n";
      while(!inFile.eof())
	{
	  getline(inFile,line);
	  stringstream ss(line);
	  int chan, code;
	  float gain, offset, innse;
	  std::string comment;
	  if(linecount<127){	  
	    ss >> chan >> code >> gain >> offset >> innse >> comment; 

	    if(comment == "high"){
	      v.noisevec.push_back(innse); //bonded
	      v.gainvec.push_back(gain);
	    }
	    else if (comment == "unbonded" && (chan == 5 || chan == 123))
	      {
		v.noisevec.push_back(innse);
		v.gainvec.push_back(gain); 
	      }
	    else if(comment == "unbonded" && (chan != 5 && chan != 123)){
	      noisesum += innse; 
	      gainsum += gain;
	      ++i;
	    }
	  }

	  linecount++;
	  std::cout << chan << "\t" << code << "\t" << gain << "\t" << offset <<"\t" << innse << "\t"  << comment <<  "\n";
	}
      double noiseaverage = noisesum/(i*1.0);
      v.noisevec.push_back(noiseaverage);
      double gainaverage = gainsum/(i*1.0);
      v.gainvec.push_back(gainaverage);
    }
  else std::cout << "File not found" << "\n";
  
  return v;
  inFile.close();
}

void FEplots()
{
  int run{51};
  int scan = {1};
  int N{64};
  int n{0};
  std::vector<int> stdelay;

  std::cout << "Enter run number..." << "\n";
  std::cin >> run;
  if(!cin.good())
    {
      std::cout << "Invalid Input, exiting..." << "\n";
      return;
    }
  
  ofstream high5stdelaynoise, high123stdelaynoise, averagestdelaynoise, high5stdelaygain, high123stdelaygain, averagestdelaygain;
  high5stdelaynoise.open("high5stdelaynoise.txt");
  high123stdelaynoise.open("high123stdelaynoise.txt");
  averagestdelaynoise.open("averagestdelaynoise.txt");
  high5stdelaygain.open("high5stdelaygain.txt");
  high123stdelaygain.open("high123stdelaygain.txt");
  averagestdelaygain.open("averagestdelaygain.txt");

  for(int i1{0}; i1<N; ++i1)
    {
      vectors v1 = readfile(run,scan + n);
      n+=3;
      vectors v2 = readfile(run,scan + n);
      n+=3;
      high5stdelaygain << i1 << "\t" << v1.gainvec[0] << std::endl;
      high123stdelaygain << i1 << "\t" << v2.gainvec[0] << std::endl;
      averagestdelaygain << i1 << "\t" << ((v1.gainvec[1] + v2.gainvec[1])/2.) << std::endl;
      high5stdelaynoise << i1 << "\t" << v1.noisevec[0] << std::endl;
      high123stdelaynoise << i1 << "\t" << v2.noisevec[0] << std::endl;
      averagestdelaynoise << i1 << "\t" << ((v1.noisevec[1] + v2.noisevec[1])/2.) << std::endl;
}
  
  high5stdelaygain.close(); high123stdelaygain.close(); averagestdelaygain.close();
  high5stdelaynoise.close(); high123stdelaynoise.close(); averagestdelaynoise.close();

  TGraph *high5_g = new TGraph("high5stdelaygain.txt");
  TGraph *high123_g = new TGraph("high123stdelaygain.txt");
  TGraph *average_g = new TGraph("averagestdelaygain.txt");

  TGraph *high5_n = new TGraph("high5stdelaynoise.txt");
  TGraph *high123_n = new TGraph("high123stdelaynoise.txt");
  TGraph *average_n = new TGraph("averagestdelaynoise.txt");

  settings(high5_g, 20, 1);
  settings(high123_g, 20, 2);
  settings(average_g, 20, 4);
  settings(high5_n, 20, 1);
  settings(high123_n, 20, 2);
  settings(average_n, 20, 4);

  TMultiGraph *mg = new TMultiGraph();
  mg->Add(high5_g,"p");
  mg->Add(high123_g,"p");
  mg->Add(average_g,"p");
  mg->GetYaxis()->SetTitle("Gain (mV)");
  mg->GetXaxis()->SetTitle("Strobe Delay");

  TMultiGraph *mn = new TMultiGraph();
  mn->Add(high5_n,"p");
  mn->Add(high123_n,"p");
  mn->Add(average_n,"p");
  mn->GetYaxis()->SetTitle("Noise (mV)");
  mn->GetXaxis()->SetTitle("Strobe Delay");
 
  TCanvas *can1 = new TCanvas("Gain","Gain",600,600);
  std::cout << "LHS Gain" << std::endl;
  high5_g->Print("all");
  std::cout << "========================================" << std::endl;
  std::cout << "RHS Gain" << std::endl;
  high123_g->Print("all");
  std::cout << "========================================" << std::endl;
  std::cout << "Average Gain" << std::endl;
  average_g->Print("all");
  std::cout << "========================================" << std::endl;
  std::cout << "========================================" << std::endl;
  mg->Draw("AP");
  TLegend *lg = new TLegend(0.6,0.1,0.9,0.2);
  legend(lg,high5_g,high123_g,average_g);

  TCanvas *can2 = new TCanvas("Noise","Noise",600,600);
  std::cout << "LHS Noise" << std::endl;
  high5_n->Print("all");
  std::cout << "========================================" << std::endl;
  std::cout << "RHS Noise" << std::endl;
  high123_n->Print("all");
  std::cout << "========================================" << std::endl;
  std::cout << "Average Noise" << std::endl;
  average_n->Print("all");
  std::cout << "========================================" << std::endl;
  mn->Draw("AP");  
  TLegend *ln = new TLegend(0.6,0.1,0.9,0.2);
  legend(ln,high5_n,high123_n,average_n);
  
  can1->SaveAs("Gain_vs_StrobeDelay.png");
  can2->SaveAs("Noise_vs_StrobeDelay.png");

}

