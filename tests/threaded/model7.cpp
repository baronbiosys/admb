/*
 * $Id: model7.cpp 437 2012-04-26 23:43:13Z johnoel $
 *
 * Author: David Fournier
 * Copyright (c) 2008-2012 Regents of the University of California
 */
#if defined(USE_LAPLACE)
#  include <df1b2fun.h>
#else
#  include <admodel.h>
#endif
//#include <parallel.h>
#include <signal.h>

#undef ADMB_CONFIGURE
#ifdef ADMB_CONFIGURE
  //#include "../../admb_configure.h"
#endif

void vm_initialize(void);
int have_jvm=0;



void strip_full_path(const adstring& _s)
{
  adstring& s = (adstring&) _s;
  int n=s.size();
  int i=0;
  for (i=n-1;i>=1;i--)
  {
    if ( s(i)=='\\' || s(i) == '/' || s(i)==':') break;
  }
  s=s(i+1,n);
}

void set_signal_handlers(void)
{
  signal(SIGFPE,exit_handler);
  signal(SIGSEGV,exit_handler);
  signal(SIGILL,exit_handler);
  signal(SIGINT,exit_handler);
}



ad_comm::ad_comm(int _argc,char * _argv[])
{
  if (option_match(_argc,_argv,"-version") > -1
   || option_match(_argc,_argv,"--version") > -1)
  {
    void banner(const adstring& program_name);
    banner(_argv[0]);

    exit(0);
  }

  ad_comm::argc=_argc;
  ad_comm::argv=_argv;
  int pvm_flag=0;
  if (option_match(_argc,_argv,"-time")>-1)
  {
    time_flag=1;
  }
  else
  {
    time_flag=0;
  }
  if (time_flag)
  {
    if (!ptm)
    {
      ptm=new adtimer();
    }
    if (!ptm1)
    {
      ptm1=new adtimer();
    }
  }
  no_atlas_flag=0;
  if (option_match(_argc,_argv,"-noatlas")>-1) no_atlas_flag=1;
  if (option_match(_argc,_argv,"-slave")>-1)  pvm_flag=2;
  if (option_match(_argc,_argv,"-master")>-1) pvm_flag=1;

#if defined(USE_ADPVM)
  if (pvm_flag)
    pvm_manager = new adpvm_manager(pvm_flag);
  else
#endif
    pvm_manager = NULL;


#if defined(USE_ADPVM)
  if (pvm_manager)
  {
    if (pvm_manager->mode==2)  //slave
    {
      int on=0; int nopt=0;
      if ( (on=option_match(_argc,_argv,"-slave",nopt))>-1)
      {
        if (nopt ==1)
        {
          pvm_manager->slave_number=atoi(ad_comm::argv[on+1]);
        }
        else
        {
          cerr << "Wrong number of options to -slave -- must be 1"
                  " you have " << nopt << endl;
          ad_exit(1);
        }
      }
      if ( (on=option_match(_argc,_argv,"-slavedir",nopt))>-1)
      {
        if (nopt ==1)
        {
          ad_chdir(_argv[on+1]);
        }
        else
        {
          cerr << "Wrong number of options to -slavedir -- must be 1"
                  " you have " << nopt << endl;
        }
      }
    }
  }
#endif


  /*
    if (option_match(_argc,_argv,"-gui")>-1)
    {
      vm_initialize();
    }
  */
  set_signal_handlers();
  adprogram_name=_argv[0];
  int len=strlen(_argv[0]);
  for (int i=1;i<=len;i++) adprogram_name[i]=tolower(adprogram_name[i]);
#if !defined(__SPDLL__)
  strip_full_path(adprogram_name);
#endif
  adstring workdir;
  ad_getcd(workdir);
//#define STR(x) STR2(x)
//#define STR2(x) #x
  if (_argc>1)
  {
    if (option_match(_argc,_argv,"-?")>-1
     || option_match(_argc,_argv,"-help")>-1
     || option_match(_argc,_argv,"--help")>-1)
    {
      // remove path (if user runs -help)
      unsigned int i;
      for (i=adprogram_name.size();i>=1;i--)
      {
#ifdef _WIN32
        if (adprogram_name(i) == '\\')
#else
        if (adprogram_name(i) == '/')
#endif
        {
          adprogram_name=adprogram_name(i+1,adprogram_name.size());
          break;
        }
      }

#ifdef ADMB_CONFIGURE
    //(*ad_printf)(" %s", (char*)admb_banner);
#endif
      (*ad_printf)( "Usage: %s [options]\n\n",(char*)(adprogram_name));

      (*ad_printf)( "Options:\n");
      (*ad_printf)( " -ainp FILE      change default ascii input parameter filename to FILE\n");
      (*ad_printf)( " -binp FILE      change default binary input parameter filename to FILE\n");
      (*ad_printf)( " -est            only do the parameter estimation\n");
      (*ad_printf)( " -noest          do not do the parameter estimation (optimization) \n");
      (*ad_printf)( " -ind FILE       change default input data filename to FILE\n");
      (*ad_printf)( " -lmn N          use limited memory quasi newton -- keep N steps\n");
      (*ad_printf)( " -lmn2 N         use other limited memory quasi newton -- keep N steps\n");
      (*ad_printf)( " -dd N           check derivatives after N function evaluations\n");
      (*ad_printf)( " -lprof          perform profile likelihood calculations\n");
      (*ad_printf)( " -maxph N        increase the maximum phase number to N\n");
      (*ad_printf)( " -mcdiag         use diagonal covariance matrix for mcmc with diagonal values 1\n");
      (*ad_printf)( " -mcmc [N]       perform markov chain monte carlo with N simulations\n");
      (*ad_printf)( " -mcmult N       multiplier N for mcmc default\n");
      (*ad_printf)( " -mcr            resume previous mcmc\n");
      (*ad_printf)( " -mcrb  N        reduce amount of correlation in the covariance matrix 1<=N<=9\n");
      (*ad_printf)( " -mcnoscale      don't rescale step size for mcmc depending on acceptance rate\n");
      (*ad_printf)( " -nosdmcmc       turn off mcmc histogram calcs to make mcsave run faster\n");
      (*ad_printf)( " -mcgrope N      use probing strategy for mcmc with factor N\n");
      (*ad_printf)( " -mcseed N       seed for random number generator for markov chain monte carlo\n");
      (*ad_printf)( " -mcscale N      rescale step size for first N evaluations\n");
      (*ad_printf)( " -mcsave N       save the parameters for every Nth simulation\n");
      (*ad_printf)( " -mceval         go through the saved mcmc values from a previous mcsave\n");
    //(*ad_printf)( " -mcu            use uniformaly distributed steps for mcmc\n");
      (*ad_printf)( " -crit N1,N2,... set gradient magnitude convergence criterion to N\n");
      (*ad_printf)( " -iprint N       print out function minimizer report every N iterations\n");
      (*ad_printf)( " -maxfn N1,N2,.. set maximum number opf function eval's to N\n");
      (*ad_printf)( " -rs             if function minimizer can't make progress rescale and try again\n");
    //(*ad_printf)( " -sp             for DLL running from splus write to command window\n");
      (*ad_printf)( " -nox            suppress vector and gradient values in minimizer screen report\n");
      (*ad_printf)( " -phase N        start minimization in phase N\n");
      (*ad_printf)( " -simplex        use simplex for minimization -- deprecated, use -neldmead\n");
      (*ad_printf)( " -neldmead       use Nelder-Mead simplex algorithm for minimization\n");
      (*ad_printf)( " -nohess         don't do hessian or delta method for std dev\n");
      (*ad_printf)( " -eigvec         calculate eigenvectors of the Hessian\n");
      (*ad_printf)( " -sdonly         do delta method for std dev estimates without redoing hessian\n");
      (*ad_printf)( " -ams N          set arrmblsize to N (ARRAY_MEMBLOCK_SIZE)\n");
      (*ad_printf)( " -cbs N          set CMPDIF_BUFFER_SIZE to N (ARRAY_MEMBLOCK_SIZE)\n");
      (*ad_printf)( " -mno N          set the maximum number of independent variables to N\n");
      (*ad_printf)( " -mdl N          set the maximum number of dvariables to N\n");
      (*ad_printf)( " -master         run as PVM master program\n");
      (*ad_printf)( " -gbs N          set GRADSTACK_BUFFER_SIZE to N (ARRAY_MEMBLOCK_SIZE)\n");
      (*ad_printf)( " -master         run as PVM master program\n");
      (*ad_printf)( " -slave          run as PVM slave program\n");
      (*ad_printf)( " -pvmtime        record timing information for PVM performance analysis\n");
      (*ad_printf)( " -info           show how to cite ADMB, license, and acknowledgements\n");
      (*ad_printf)( " -version        show version information\n");
      (*ad_printf)( " -help           show this message\n\n");
    //if (function_minimizer::random_effects_flag)
    //{
      (*ad_printf)( "Random effects options if applicable\n");
      (*ad_printf)( " -nr N           maximum number of Newton-Raphson steps\n");
      (*ad_printf)( " -imaxfn N       maximum number of evals in quasi-Newton inner optimization\n");
      (*ad_printf)( " -is N           set importance sampling size to N\n");
      (*ad_printf)( " -isf N          set importance sampling size funnel blocks to N\n");
      (*ad_printf)( " -isdiag         print importance sampling diagnostics\n");
      (*ad_printf)( " -hybrid         do hybrid Monte Carlo version of MCMC\n");
      (*ad_printf)( " -hbf            set the hybrid bounded flag for bounded parameters\n");
      (*ad_printf)( " -hyeps          mean step size for hybrid Monte Carlo\n");
      (*ad_printf)( " -hynstep        number of steps for hybrid Monte Carlo\n");
      (*ad_printf)( " -noinit         do not initialize RE before inner optimization\n");
      (*ad_printf)( " -ndi N          set maximum number of separable calls\n");
      (*ad_printf)( " -ndb N          set number of blocks for RE derivatives (reduce temp file size)\n");
      (*ad_printf)( " -ddnr           use high precision Newton-Raphson, for banded Hessian case only\n");
      (*ad_printf)( " -nrdbg          verbose reporting for debugging newton-raphson\n");
#  if defined(__MINI_MAX__)
      (*ad_printf)( " -mm N           do minimax optimization\n");
#  endif
      (*ad_printf)( " -shess          use sparse Hessian structure inner optimzation\n\n");

      (*ad_printf)("Read online documentation at http://admb-project.org\n");
      (*ad_printf)("Contact <users@admb-project.org> for help.\n");
    //}
      ad_exit(0);
    }
    else if (option_match(_argc,_argv,"-info") > -1)
    {
      (*ad_printf)("ADMB Information\n");
      (*ad_printf)("================\n\n");

      (*ad_printf)("How to Cite ADMB\n");
      (*ad_printf)("----------------\n\n");

      (*ad_printf)("Fournier, D.A., H.J. Skaug, J. Ancheta, J. Ianelli, A. Magnusson, M.N. Maunder,\n");
      (*ad_printf)("A. Nielsen, and J. Sibert. 2012. AD Model Builder: using automatic\n");
      (*ad_printf)("differentiation for statistical inference of highly parameterized complex\n");
      (*ad_printf)("nonlinear models. Optim. Methods Softw. 27:233-249.\n\n");

#ifdef ADMB_CONFIGURE
      //(*ad_printf)(" %s", (char*)admb_banner);
#endif
      (*ad_printf)("License\n");
      (*ad_printf)("-------\n\n");

      (*ad_printf)("Copyright (c) 2008-2012\n");
      (*ad_printf)("Regents of the University of California and ADMB Foundation\n\n");

      (*ad_printf)("ADMB is free software and comes with ABSOLUTELY NO WARRANTY.\n");
      (*ad_printf)("You are welcome to redistribute it under certain conditions.\n\n");

      (*ad_printf)("AD Model Builder, or ADMB, was developed by David Fournier of Otter Research\n");
      (*ad_printf)("Ltd, Sidney, BC, Canada. In 2007, scientists from the University of Hawai'i at\n");
      (*ad_printf)("Manoa Pelagic Fisheries Research Program (John Sibert and Anders Nielsen) and\n");
      (*ad_printf)("the Inter-American Tropical Tuna Commission (Mark Maunder), in consultation with\n");
      (*ad_printf)("scientists from NOAA Fisheries (Richard Methot), created the non-profit ADMB\n");
      (*ad_printf)("Foundation (admb-foundation.org) with the goal of increasing the number of ADMB\n");
      (*ad_printf)("users by making the software free and open source. In partnership with NOAA\n");
      (*ad_printf)("Fisheries and the National Center for Ecological Analysis and Synthesis (NCEAS,\n");
      (*ad_printf)("www.nceas.ucsb.edu), the ADMB Foundation obtained funding from the Gordon and\n");
      (*ad_printf)("Betty Moore Foundation (www.moore.org) to acquire the copyright to the ADMB\n");
      (*ad_printf)("software suite, in order to make it broadly and freely available to the research\n");
      (*ad_printf)("community. In 2008 the copyright was transferred from Otter Research Ltd to the\n");
      (*ad_printf)("University of California. The binary files were released in November 2008 and\n");
      (*ad_printf)("the source code was released in December 2009. More information about the ADMB\n");
      (*ad_printf)("Project can be found at admb-project.org.\n\n");

      (*ad_printf)("ADMB was originally developed by David Fournier of Otter Research Ltd.\n");
      (*ad_printf)("It is now maintained by the ADMB Core Team, currently consisting of\n\n");

      (*ad_printf)("  Anders Nielsen (Denmark)\n");
      (*ad_printf)("  Arni Magnusson (Iceland)\n");
      (*ad_printf)("  Ben Bolker (Canada)\n");
      (*ad_printf)("  Casper Berg (Denmark)\n");
      (*ad_printf)("  Chris Grandin (Canada)\n");
      (*ad_printf)("  David Fournier (Canada)\n");
      (*ad_printf)("  Derek Seiple (USA)\n");
      (*ad_printf)("  Hans Skaug (Norway)\n");
      (*ad_printf)("  Ian Taylor (USA)\n");
      (*ad_printf)("  Jim Ianelli (USA)\n");
      (*ad_printf)("  John Sibert (USA)\n");
      (*ad_printf)("  Johnoel Ancheta (USA)\n");
      (*ad_printf)("  Mark Maunder (USA)\n");
      (*ad_printf)("  Matthew Supernaw (USA)\n");
      (*ad_printf)("  Mollie Brooks (USA)\n");
      (*ad_printf)("  Steve Martell (Canada)\n");
      (*ad_printf)("  Teresa A'mar (USA)\n");
      (*ad_printf)("  Weihai Liu (USA)\n");
      ad_exit(0);
    }
  }
  allocate();
}

ad_comm::ad_comm(void)
{
  allocate();
}

void ad_comm::allocate(void)
{
#if defined (_WIN32)
  directory_prefix='\\';
#else
  directory_prefix='/';
#endif
  adstring tmpstring;
  // remove path (if __SPDLL__ is not defined)
#if !defined(__SPDLL__)
  for (int i = adprogram_name.size(); i >= 1; i--)
  {
    if (adprogram_name(i)==directory_prefix)
    {
      adprogram_name=adprogram_name(i+1,adprogram_name.size());
      break;
    }
  }

#endif
  // strip off the .exe if it is there
  int n=adprogram_name.size();
  if(n>4)
  {
    if (adprogram_name(n - 3) == '.'
        && adprogram_name(n - 2) == 'e'
        && adprogram_name(n - 1) == 'x'
        && adprogram_name(n) == 'e')
    {
      n -= 4;
    }
  }
  adprogram_name=adprogram_name(1,n);

  // change the working directory name
  if (argc > 1)
  {
    int on=0;
    if ( (on=option_match(argc,argv,"-wd"))>-1)
    {
      if (on>argc-2 || argv[on+1][0] == '-')
      {
        cerr << "Invalid input data command line option"
                " -- ignored" << endl;
      }
      else
      {
        tmpstring = adstring(argv[on+1]);
	wd_flag=1;
      }
    }
  }
  if (length(tmpstring))
  {
    if (tmpstring(length(tmpstring)) == directory_prefix)
    {
      adprogram_name=tmpstring + adprogram_name;
      working_directory_path = tmpstring;
    }
    else
    {
      adprogram_name=tmpstring + directory_prefix + adprogram_name;
      working_directory_path = tmpstring + directory_prefix;
    }
  }

  tmpstring=adprogram_name + adstring(".dat");
  if (argc > 1)
  {
    int on=0;
    if ( (on=option_match(argc,argv,"-ind"))>-1)
    {
      if (on>argc-2 || argv[on+1][0] == '-')
      {
        cerr << "Invalid input data command line option"
                " -- ignored" << endl;
      }
      else
      {
        tmpstring = adstring(argv[on+1]);
      }
    }
  }
  global_datafile= new cifstream(tmpstring);
  if (!global_datafile)
  {
    cerr << "Error trying to open data input file "
         << tmpstring << endl;
  }
  else
  {
    if (!(*global_datafile))
    {
      cerr << "Error trying to open data input file "
           << tmpstring << endl;
      delete global_datafile;
      global_datafile=NULL;
    }
  }
  adstring ts=adprogram_name + adstring(".log");
  global_logfile= new ofstream( (char*)ts);

  int biopt=-1;
  int aiopt=-1;
  biopt=option_match(argc,argv,"-binp");
  aiopt=option_match(argc,argv,"-ainp");

  tmpstring=adprogram_name + adstring(".bin");
  if (!global_bparfile && aiopt == -1)
  {
    if (biopt>-1)
    {
      if (biopt>argc-2 || argv[biopt+1][0] == '-')
      {
        cerr << "Invalid input parameter file command line option"
                " -- ignored" << endl;
      }
      else
      {
        tmpstring = adstring(argv[biopt+1]);
      }
    }
    global_bparfile= new uistream(tmpstring);
    if (global_bparfile)
    {
      if (!(*global_bparfile))
      {
	if (biopt>-1)
	{
	  cerr << "Error trying to open binary inoput par file "
	       << tmpstring << endl;
	  exit(1);
	}
        delete global_bparfile;
        global_bparfile=NULL;
      }
    }
  }
  tmpstring=adprogram_name + adstring(".pin");
  if (!global_parfile)
  {
    if (aiopt>-1)
    {
      if (aiopt>argc-2 || argv[aiopt+1][0] == '-')
      {
        cerr << "Invalid input parameter file command line option"
                " -- ignored" << endl;
      }
      else
      {
        tmpstring = adstring(argv[aiopt+1]);
      }
    }
    global_parfile= new cifstream(tmpstring);
    if (global_parfile)
    {
      if (!(*global_parfile))
      {
	if (aiopt>-1)
	{
	  cerr << "Error trying to open ascii inoput par file "
	       << tmpstring << endl;
	  exit(1);
	}
        delete global_parfile;
        global_parfile=NULL;
      }
    }
  }
}

ad_comm::~ad_comm()
{
  if (ptm)
  {
    delete ptm;
    ptm=0;
  }
  if (ptm1)
  {
    delete ptm1;
    ptm1=0;
  }
  if (global_datafile)
  {
    delete global_datafile;
    global_datafile=NULL;
  }
  if (global_parfile)
  {
    delete global_parfile;
    global_parfile=NULL;
  }
  if (global_logfile)
  {
    delete global_logfile;
    global_logfile=NULL;
  }
}

void function_minimizer::pre_userfunction(void)
{
#if defined(USE_LAPLACE)
  if (lapprox)
  {
    if (lapprox->hesstype==2)
    {
      //lapprox->num_separable_calls=0;
      lapprox->separable_calls_counter=0;
    }
  }
#endif
  userfunction();
#if defined(USE_LAPLACE)
  if (lapprox)
  {
    if (lapprox->hesstype==2)
    {
      lapprox->num_separable_calls=lapprox->separable_calls_counter;

      double tmp=0.0;
      int inner_opt_value=inner_opt();
      if (lapprox->saddlepointflag==2)
      {
        if (inner_opt_value !=0 )
        {
          for (int i = 1; i <= lapprox->num_separable_calls; i++)
          {
            tmp-=(*lapprox->separable_function_difference)(i);
          }
          value(*objective_function_value::pobjfun)=tmp;
        }
      }
      else
      {
        for (int i = 1; i <= lapprox->num_separable_calls; i++)
        {
          tmp+=(*lapprox->separable_function_difference)(i);
        }
        value(*objective_function_value::pobjfun)=tmp;
      }
    }
  }
#endif
}
#if defined(USE_ADMPI)
void add_slave_suffix(const adstring& _tmpstring)
{
  ADUNCONST(adstring,tmpstring)
  if (ad_comm::mpi_manager)
  {
    if (ad_comm::mpi_manager->is_slave())
    {
      tmpstring += "_";
      tmpstring += str(ad_comm::mpi_manager->get_slave_number());
       cout << "In slave " << tmpstring << endl;
    }
    else
    {
      tmpstring += "_master";
       cout << "In master " << tmpstring << endl;
    }
  }
}

#else if defined(USE_PTHREADS)


/*
void add_slave_suffix(const adstring& _tmpstring)
{
  ADUNCONST(adstring,tmpstring)
  if (ad_comm::pthreads_manager)
  {
    if (ad_comm::pthreads_manager->is_slave())
    {
      tmpstring += "_";
      tmpstring += str(ad_comm::pthreads_manager->get_slave_number());
       cout << "In slave " << tmpstring << endl;
    }
    else
    {
      tmpstring += "_master";
       cout << "In master " << tmpstring << endl;
    }
  }
}
*/

#endif