//#include "../src/spinapi.h"
#include "../src/fid.h"
#include "time.h"
#include <stdio.h>
#include <string.h>
static int
pb_write_felix (const char *fnameout, const char *title_string, int num_points, float SW, float SF, 
                const int *real_data, const int *imag_data)
{
  int NP = num_points;
  float mySW = SW / 1000.0f;
  float spectrometer_freq = SF;

  // FELIX will crash if the spectrometer frequency is DC
  // we shift it ever so slightly to avoid this.
  if (spectrometer_freq == 0.0f)
     spectrometer_freq = 0.0000000001f;
  
  //Get time to put in header
  char buff_time[20];
  time_t rawtime;
  struct tm * timeinfo;
  time(&rawtime);
  timeinfo = localtime(&rawtime);
  strftime(buff_time,20,"%x %X",timeinfo);
  
  //Get SpinAPI version to put in header
  const char *version = "SPINAPI TESTER";//pb_get_version();
  
  //try to open fnameout for writing
  FILE *pFile = fopen (fnameout, "wb");

  //make sure fopen worked
  if (pFile == NULL)
    {
      //debug (DEBUG_ERROR, "Error opening file %s.", fnameout);
      fprintf (stderr, "Error opening file %s.", fnameout);
      return -1;
    }

  //the following code was mostly cut and paste from spinwrap.dll source

  long paramsize = 32;
  long tmp_long;
  float tmp_fl;
  short int tmp_unit;
  bool tmp_bool;
  short int tmp_ft;
  short int tmp_dm;

	/*******************************************
	 * Marker
	 * First 32 bytes of file
	 *******************************************/
  char felixForWindows[32] = "Felix for Windows             \0";
  int index = 0;
  unsigned int UUINT = 0x01000000;

  for (index = 0; index < 32 - sizeof (unsigned int); index++)
    {
      if (felixForWindows[index] == 0x20 && index > 9)
	fputc (0x00, pFile);
      else
	fputc (felixForWindows[index], pFile);
    }
  //UINT 0x0100
  fwrite (&UUINT, sizeof (unsigned int), 1, pFile);
      fflush(pFile);


	/*********************************************
	 * parameters: 1024 bytes - Marker (32 bytes)
	 *********************************************/

  //Acqusition Parameters
  tmp_ft = FID_TYPE;		// Type of FID data
  fwrite (&tmp_ft, sizeof (short), 1, pFile);
      fflush(pFile);
  paramsize += sizeof (short);

  tmp_long = NP;		//Number of Points
  fwrite (&tmp_long, sizeof (long), 1, pFile);
      fflush(pFile);
  paramsize += sizeof (long);

  tmp_fl = mySW * (float) 1000;	// Spectral Width
  fwrite (&tmp_fl, sizeof (float), 1, pFile);
      fflush(pFile);
  paramsize += sizeof (float);

  tmp_fl = (float) (1 / (mySW * 1000)) * NP;	// Acquisition Time
  fwrite (&tmp_fl, sizeof (float), 1, pFile);
      fflush(pFile);
  paramsize += sizeof (float);

  tmp_fl = (float) spectrometer_freq;	// Spectrometer Frequency
  fwrite (&tmp_fl, sizeof (float), 1, pFile);
      fflush(pFile);
  paramsize += sizeof (float);

  //Processing Parameters
  tmp_long = DC;		// Drift Correction Points
  fwrite (&tmp_long, sizeof (long), 1, pFile);
      fflush(pFile);
  paramsize += sizeof (long);
  tmp_long = FOURIER_NUMBER;	// Fourier Number
  fwrite (&tmp_long, sizeof (long), 1, pFile);
      fflush(pFile);
  paramsize += sizeof (long);
  tmp_fl = LINE_BROAD;		// Line Broadening
  fwrite (&tmp_fl, sizeof (float), 1, pFile);
      fflush(pFile);
  paramsize += sizeof (float);
  tmp_fl = GAUSS_BROAD;		// Gaussian Broadening
  fwrite (&tmp_fl, sizeof (float), 1, pFile);
      fflush(pFile);
  paramsize += sizeof (float);

  //Display Parameters
  tmp_dm = DISP_MODE;		// Display Mode
  fwrite (&tmp_dm, sizeof (short), 1, pFile);
      fflush(pFile);
  paramsize += sizeof (short);
  tmp_fl = PH0;			// Zero order phase
  fwrite (&tmp_fl, sizeof (float), 1, pFile);
      fflush(pFile);
  paramsize += sizeof (float);
  tmp_fl = PH1;			// first order phase
  fwrite (&tmp_fl, sizeof (float), 1, pFile);
      fflush(pFile);
  paramsize += sizeof (float);
  tmp_fl = REF_LINE_FREQ;	//reference line frequency
  fwrite (&tmp_fl, sizeof (float), 1, pFile);
      fflush(pFile);
  paramsize += sizeof (float);
  // Set the reference point for the zero frequency line. If this is
  // set to half of the number of points in the FFT, the zero frequency
  // will be in the center
  tmp_long = num_points / 2;
  fwrite (&tmp_long, sizeof (long), 1, pFile);
      fflush(pFile);
  paramsize += sizeof (long);

  tmp_unit = FID_UNITS;		// FID units
  fwrite (&tmp_unit, sizeof (short), 1, pFile);
      fflush(pFile);
  paramsize += sizeof (short);
  tmp_unit = SPEC_UNITS;	// Spectrum Units
  fwrite (&tmp_unit, sizeof (short), 1, pFile);
      fflush(pFile);
  paramsize += sizeof (short);
  tmp_bool = AXIS;		// Axis on/off
  fwrite (&tmp_bool, sizeof (bool), 1, pFile);
      fflush(pFile);
  paramsize += sizeof (bool);

  putc (0x00, pFile);		// Extra blank character ?
  paramsize += 1;

  tmp_fl = SCALE_OFFSET;	// Scale offset
  fwrite (&tmp_fl, sizeof (float), 1, pFile);
      fflush(pFile);
  paramsize += sizeof (float);
  tmp_long = START_PLOT;	// Start of plot
  fwrite (&tmp_long, sizeof (long), 1, pFile);
      fflush(pFile);
  paramsize += sizeof (long);
  tmp_long = WIDTH_PLOT;	// Width of plot
  fwrite (&tmp_long, sizeof (long), 1, pFile);
      fflush(pFile);
  paramsize += sizeof (long);
  tmp_fl = VERT_OFFSET;		// Vertical offset
  fwrite (&tmp_fl, sizeof (float), 1, pFile);
      fflush(pFile);
  paramsize += sizeof (float);
  tmp_fl = VERT_SCALE;		// vertical scale
  fwrite (&tmp_fl, sizeof (float), 1, pFile);
      fflush(pFile);
  paramsize += sizeof (float);
  tmp_long = FID_START;		// FID start of plot
  fwrite (&tmp_long, sizeof (long), 1, pFile);
      fflush(pFile);
  paramsize += sizeof (long);
  tmp_long = NP;		// FID width of plot
  fwrite (&tmp_long, sizeof (long), 1, pFile);
      fflush(pFile);
  paramsize += sizeof (long);
  tmp_fl = FID_VERT_OFFSET;	// FID vertical offset
  fwrite (&tmp_fl, sizeof (float), 1, pFile);
      fflush(pFile);
  paramsize += sizeof (float);
  tmp_fl = FID_VERT_SCALE;	// FID vertical scale
  fwrite (&tmp_fl, sizeof (float), 1, pFile);
      fflush(pFile);
  paramsize += sizeof (float);

  tmp_long = 0x00000000;
  for (index = 0; index < 1024 - paramsize; index++)
    {
      fwrite (&tmp_long, 1, 1, pFile);
      fflush(pFile);
    }

	/**************************
	 * Title Block: 512 bytes *
	 **************************/
  /*
     int t = 00;
     int aa = 0;
     for (aa = 0; aa < 128; aa++)
     fwrite(&t, sizeof(t), 1, pFile);
   */
                            
  char buffer[512] = {0};
  sprintf (buffer,
	   "RadioProcessor\r\nSpinCore Technologies, Inc.\r\nwww.spincore.com\r\n\r\nTime = %s\r\nSpinAPI = %s\r\n%s",
	   buff_time,version,title_string);
  fwrite (buffer, sizeof (char), 512, pFile);
      fflush(pFile);

  
  int k;
  long temp;
  for (k = 0; k < NP; k++)
    {
      //get real value from ram file
      temp = real_data[k];
      //fscanf(pRam, "%ld", &temp);

      //write to felix file
      //if (k != 0)
      fwrite (&temp, sizeof (long), 1, pFile);
      fflush(pFile);


      //get imag value from ram file
      //fscanf(pRam, "%ld", &temp);
      temp = imag_data[k];

      //write to felix file
      //if (k != 0)
      fwrite (&temp, sizeof (long), 1, pFile);
      fflush(pFile);
    }

  fwrite (&temp, sizeof (long), 1, pFile);
      fflush(pFile);
  fwrite (&temp, sizeof (long), 1, pFile);
      fflush(pFile);

  //fwrite(&temp,sizeof(long),1,pFile);
  //fwrite(&temp,sizeof(long),1,pFile);


  //close the files
  if (pFile != NULL)
    fclose (pFile);


  return 0;
}

#define N 64
int main(void) {
    int real[N], imag[N];
    memset(real, 0, sizeof(real));
    memset(imag, 0, sizeof(imag));
    return pb_write_felix("out", "TITLE", N, 0.5, 0.3, real, imag);
}
