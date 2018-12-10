void FEscan()
{
  int n{63}; 
  for(int i = 0; i<=n; ++i)
     {
  e->CacheMasks(0);
  e->ConfigureVariable(ST_STROBE_DELAY,i);
  e->ExecuteConfigs();
  ABCStarThreePointGain(1.0,666,256,1,-1);
  e->CacheMasks(1);
  e->CacheMasks(0);
  ABCStarThreePointGain(1.0,667,256,1,-1);
  e->CacheMasks(1); 

  }
}
