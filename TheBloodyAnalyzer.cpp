#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

#ifdef _WIN32
#include <conio.h>
//for now only for 1 chain(!)
//CUDA
#endif
bool debug = true;
int ContactMatrix[5000][5000];
double DistanceMatrix[5000][5000];

double DistanceMatrixNormer = 0;
double CONTACT_CUT = 2.5;
void InitStats(int Len, int StpLen, int minlen, int maxlen,int);
void CalculateStats();
void outputStats(FILE *f);
double rgs[5000]; //starts with 2
double rs[5000];
double conts[5000];
double rgwithout2s[5000];

const int NContBins = 100;
double NContRadius[NContBins];
double NContStep = 0.1;

int rgstep;
int rgmax;
int rglen;
int rgmin;
int rgcounter;
int rgstart = 0;
int N;


struct Chain;
struct Mon;
struct Vector;

struct rgb{
    double r;       // percent
    double g;       // percent
    double b;       // percent
} ;
 struct hsv{
    double h;       // angle in degrees
    double s;       // percent
    double v;       // percent
} ;

hsv   rgb2hsv(rgb in);
rgb   hsv2rgb(hsv in);

hsv rgb2hsv(rgb in)
{
    hsv         out;
    double      min, max, delta;

    min = in.r < in.g ? in.r : in.g;
    min = min  < in.b ? min  : in.b;

    max = in.r > in.g ? in.r : in.g;
    max = max  > in.b ? max  : in.b;

    out.v = max;                                // v
    delta = max - min;
    if (delta < 0.00001)
    {
        out.s = 0;
        out.h = 0; // undefined, maybe nan?
        return out;
    }
    if( max > 0.0 ) { // NOTE: if Max is == 0, this divide would cause a crash
        out.s = (delta / max);                  // s
    } else {
        // if max is 0, then r = g = b = 0              
            // s = 0, v is undefined
        out.s = 0.0;
        out.h = 0;                            // its now undefined
        return out;
    }
    if( in.r >= max )                           // > is bogus, just keeps compilor happy
        out.h = ( in.g - in.b ) / delta;        // between yellow & magenta
    else
    if( in.g >= max )
        out.h = 2.0 + ( in.b - in.r ) / delta;  // between cyan & yellow
    else
        out.h = 4.0 + ( in.r - in.g ) / delta;  // between magenta & cyan

    out.h *= 60.0;                              // degrees

    if( out.h < 0.0 )
        out.h += 360.0;

    return out;
}


rgb hsv2rgb(hsv in)
{
    double      hh, p, q, t, ff;
    long        i;
    rgb         out;

    if(in.s <= 0.0) {       // < is bogus, just shuts up warnings
        out.r = in.v;
        out.g = in.v;
        out.b = in.v;
        return out;
    }
    hh = in.h;
    if(hh >= 360.0) hh = 0.0;
    hh /= 60.0;
    i = (long)hh;
    ff = hh - i;
    p = in.v * (1.0 - in.s);
    q = in.v * (1.0 - (in.s * ff));
    t = in.v * (1.0 - (in.s * (1.0 - ff)));

    switch(i) {
    case 0:
        out.r = in.v;
        out.g = t;
        out.b = p;
        break;
    case 1:
        out.r = q;
        out.g = in.v;
        out.b = p;
        break;
    case 2:
        out.r = p;
        out.g = in.v;
        out.b = t;
        break;

    case 3:
        out.r = p;
        out.g = q;
        out.b = in.v;
        break;
    case 4:
        out.r = t;
        out.g = p;
        out.b = in.v;
        break;
    case 5:
    default:
        out.r = in.v;
        out.g = p;
        out.b = q;
        break;
    }
    return out;     
}






struct Vector
{
public: double x[3];
       double L;
       Vector(double x1,double y1,double z1)
       {
                  x[0] = x1;
                  x[1] = y1;
                  x[2] = z1;

       //           L = sqrt(x1*x1 + y1*y1 + z1*z1);

       }
       Vector()
       {
       }


       Vector& operator = (const Vector &n)
       {
              x[0] = n.x[0];
              x[1] = n.x[1];
              x[2] = n.x[2];

 //                 L = sqrt(x[0]*x[0] + x[1]*x[1] + x[2]*x[2]);
       return *this;
       }
       bool operator == (const Vector &n)
       {
       if(x[0] == n.x[0] && x[1] == n.x[1] && x[2] == n.x[2])
       {
       return true;
       }
       else
       {
       return false;
       }
       }
       double len()
       {
       return sqrt(x[0]*x[0] + x[1]*x[1] + x[2]*x[2]);
       }
       double sqlen()
       {
           return x[0]*x[0] + x[1]*x[1] + x[2]*x[2];
       }

      /*  Vector getReal()
        {
            Vector tmp;
            for(int i = 0; i < 3; i++)
            {

            tmp.x[i]=x[i]-SIZE[i]*floor(x[i]/SIZE[i]);


            }

        return tmp;
        }
*/



};

Vector operator+(const Vector &a,const Vector &b)
       {

          Vector c(a.x[0]+b.x[0],a.x[1]+b.x[1],a.x[2]+b.x[2]);
          return c;
       }
Vector operator-(const Vector &a,const Vector &b)
       {
          Vector c(a.x[0]-b.x[0],a.x[1]-b.x[1],a.x[2]-b.x[2]);
          return c;
       }


/*
Vector getReal(const Vector &un)
        {
        Vector tmp;
        for(int i = 0; i < 3; i++)
        {

        tmp.x[i]=un.x[i]-SIZE[i]*floor(un.x[i]/SIZE[i]);


        }

        return tmp;
        }
*/
double operator*(const Vector &a,const Vector &b)
{
     double c = a.x[0]*b.x[0] +a.x[1]*b.x[1]+a.x[2]*b.x[2];
          return c;

}

Vector operator*(const double a,const Vector &b)
{
 Vector tmp;
 tmp = b;
 for(int i = 0; i < 3; i++)
        {

        tmp.x[i]=a*tmp.x[i];


        }
return tmp;
}


Vector crd[2000];

double GyrationRadius(int Start, int End)
{
    double Rg = 0;
    Vector CMass,Vg;
    for(int j = 0; j < 3; j++)
    {
        CMass.x[j] = 0;//υσιυσιυσιυσιυσιυσιυσιυσιυσιυσιυσιυσιυσιυσιυσιυσιυσιυσιυσιυσιυσιυσιυσιυσιυσιυσιυσιυσιυσιυσιυσιυσισυισυισυισυισυισυισυισυυσιυσυυσιυσιυσιυσιυσιυσιυσιυσιυσιυσιυσιυσιυσιυσιυσιυσιυσιυσιυσιυσιυσιυσιυσιυσιυσιυσιυσιυσιυσιυσιυσιυσιυσιυσιυσιυσιυσιυσιυσιυσιυσιυσιυσιυσιυσιυσιυσιυσιυσιυσιυσιυσιυσιυσιυσυισυυσιυσιυσιυσι
    }
    for(int i = Start; i <= End; i ++)
    {
        for(int j = 0; j < 3; j++)
        {
        CMass.x[j] += crd[i].x[j];
        }
    }
    for(int j = 0; j < 3; j++)
    {
        CMass.x[j] = CMass.x[j]/(double)(End-Start+1);
    }


    for(int i = Start; i <= End; i ++)
    {
        Rg = Rg + (CMass - crd[i]).sqlen();
    }

    return Rg/(double)(End-Start+1);

}


void InitStats(int Len, int StpLen, int minlen, int maxlen,int StartMon = 0)
{
////chain stats
rglen = Len;
rgcounter = 0;
rgmin = minlen;
rgmax = maxlen;
rgstep = StpLen;
rgstart = StartMon;
for(int i = 0; i < rglen; i ++)
{
    rgs[i] = 0;
    rs[i] = 0;
    conts[i] = 0;
	rgwithout2s[i] = 0;
	}




}


void CalculateStats()
{

    for(int i = rgmin; i < rgmax; i+=rgstep)
    {
        double meanrs = 0;
        double meanrg = 0;
        int cnt = 0;
		double meanrg2w = 0;
        int lag = 1;
        int cnts = 0;
        for(int j = rgstart; j < rglen - i; j+=lag)
        {
          //  printf("alive at step %i %i\n",i,j);
			meanrs += (crd[j]-crd[j+i]).len();
            if((crd[j]-crd[j+i]).len() < CONTACT_CUT)
            {
                cnts++;
            }
            meanrg +=GyrationRadius(j,i+j);
            meanrg2w +=sqrt(GyrationRadius(j,i+j));
			cnt++;
        }
        rs[i] += meanrs/(double)cnt;
        rgs[i] += meanrg/(double)cnt;
        conts[i] += cnts/(double)cnt;
		rgwithout2s[i] += meanrg2w/(double)cnt;


    }

    rgcounter++;
}


void outputStats(FILE *ft)
{
    for(int i = 0; i < rgmax; i++)
    {
        if(rs[i]!=0)
        {
            fprintf(ft,"%i %f %f %f %f %f %f %f\n ", i,rs[i]/rgcounter,rgs[i]/rgcounter,conts[i]/rgcounter,log((double)i),log(rgs[i]/rgcounter),log((double)conts[i]/rgcounter),rgwithout2s[i]/rgcounter);


        }
    }
    fprintf(ft,"\n");


}

/*
void PrintDistMatrix(FILE* f)
{
//for homopolymer chains

fprintf(f,"** ");
for(int j = 0; j < C[ChNum].N; j++)
{
fprintf(f,"%i ", j);
}
fprintf(f,"\n");
for(int i = 0; i < C[ChNum].N; i++)
{
	fprintf(f,"%i ",i);
	for(int j = 0; j < C[ChNum].N; j++)
	{
		fprintf(f,"%f ", nearestImageR(C[ChNum].M[j], C[ChNum].M[i]));


	}
	fprintf(f,"\n");
}




}

*/

void UpdateContMatrix()
{
//for homopolymer chains

for(int i = 0; i < N; i++)
{

	for(int j = 0; j < N; j++)
	{
		DistanceMatrix[i][j]+=(crd[i] - crd[j]).len();
			
		if( (crd[i] - crd[j]).len() < CONTACT_CUT && abs(i-j)>1)
		{
			
			ContactMatrix[i][j]++;
		//	if(debug) printf("FUUCCCCKK\n");
			
		}
		for(int k = 0; k < NContBins; k++)
		{
			if((crd[i] - crd[j]).len() < k*NContStep && abs(i-j)>1)
			{
				NContRadius[k] ++;
				
			}
			
		}

	}

}

DistanceMatrixNormer+=1;


}



void outputGradVrml(char* filename)
{
    #ifndef _WIN32
char str[1000] = "cp vrml_header.txt ";
strcat(str,filename);
system(str);
#endif

#ifdef _WIN32
char str[1000] = "copy vrml_header.txt ";
strcat(str,filename);
system(str);
#endif

//system("y");
FILE *F = fopen(filename,"a");
//printf("%lld",mAcc);
//getch();

for(int j = 0; j < N; j ++)
{


/*

if(j == FIXED_POINT)
{
	 fprintf(F,"Transform { \n translation %f %f %f  children [ Shape {geometry Sphere {radius 2.0} \n appearance Appearance {material Material {diffuseColor %lf %lf %lf}}}]}\n",C[i].M[j].XReal.x[0],C[i].M[j].XReal.x[1],C[i].M[j].XReal.x[2], 1, 1, 1 );

	
}
else if(C[i].M[j].Typ == 1)
{

    fprintf(F,"Transform { \n translation %f %f %f  children [ USE Ball1]}\n",C[i].M[j].XReal.x[0],C[i].M[j].XReal.x[1],C[i].M[j].XReal.x[2]);
}
else
{
 */
 hsv color;
 color.h = ((double)j/(double)N)*360.0;
 color.s = 0.9;
 color.v = 0.8;
 rgb clrrgb = hsv2rgb(color);
 
 
 fprintf(F,"Transform { \n translation %f %f %f  children [ Shape {geometry Sphere {radius 0.5} \n appearance Appearance {material Material {diffuseColor %lf %lf %lf}}}]}\n",crd[j].x[0],crd[j].x[1],crd[j].x[2], clrrgb.r , clrrgb.g, clrrgb.b );
}



//}

/*
for(int y = 0; y < Nchains; y++)
{
fprintf(F,"Shape {appearance Appearance {material Material {emissiveColor 0 0 0}}geometry IndexedLineSet {coord Coordinate {point [");
for(int a = 0; a < C[y].N;a++)
{
   /*if(C[y].M[a].rigid == true)
    {
    printf("1\n");
    }
    else
    {
    printf("0\n");
    }
    getch();
    fprintf(F,"%f %f %f , \n",C[y].M[a].XReal.x[0],C[y].M[a].XReal.x[1],C[y].M[a].XReal.x[2]);
}
fprintf(F,"]}coordIndex [");
for(int a = 0; a< C[y].N; a++)
{
for(int yy = 0; yy < C[y].M[a].NumBonds ; yy++)
{


fprintf(F,"%i,%i,-1\n",a,C[y].M[a].Bonds[yy]);
}
}
fprintf(F,"]}}");

*/



/*
fprintf(F,"Shape {appearance Appearance {material Material {emissiveColor 0 0 0}}geometry IndexedLineSet {coord Coordinate {point [");
fprintf(F,"0 0 %f\n",SIZE[2]);
fprintf(F,"0 %f %f \n",SIZE[1],SIZE[2]);
fprintf(F,"0 %f 0\n",SIZE[1]);
fprintf(F,"0 0 0\n");
fprintf(F,"%f 0 0\n",SIZE[0]);
fprintf(F,"%f 0 %f\n",SIZE[0],SIZE[2]);
fprintf(F,"%f %f %f\n",SIZE[0],SIZE[1],SIZE[2]);
fprintf(F,"%f %f 0\n",SIZE[0],SIZE[1]);
fprintf(F,"]}coordIndex [");


fprintf(F,"0,1,-1\n");
fprintf(F,"1,2,-1\n");
fprintf(F,"2,3,-1\n");
fprintf(F,"0,3,-1\n");

fprintf(F,"3,4,-1\n");
fprintf(F,"1,6,-1\n");
fprintf(F,"2,7,-1\n");
fprintf(F,"0,5,-1\n");

fprintf(F,"4,5,-1\n");
fprintf(F,"5,6,-1\n");
fprintf(F,"6,7,-1\n");
fprintf(F,"7,4,-1\n");



fprintf(F,"]}}");
*/


fclose(F);





}


void PrintDistanceMatrix()
{
	
		FILE* f = fopen("DistanceMatrix.txt","w");
	
	for(int i = 0; i < N; i++)
	{
		
		for(int j =0 ; j < N; j++)
		{
			
			fprintf(f,"%lf ", DistanceMatrix[i][j]/DistanceMatrixNormer);
			
			
		}
		fprintf(f,"\n");
	}
	fclose(f);

	
	
}

void PrintDistanceMatrixAxis()
{
	
		FILE* f = fopen("DistanceMatrixAxis.txt","w");
	fprintf(f,"** ");
	for(int j = 0; j < N; j++)
	{
	fprintf(f,"%i ", j);
	}
fprintf(f,"\n");
	for(int i = 0; i < N; i++)
	{
		fprintf(f,"%i ",i);
		for(int j =0 ; j < N; j++)
		{
			
			fprintf(f,"%lf ", DistanceMatrix[i][j]/DistanceMatrixNormer);
			
			
		}
		fprintf(f,"\n");
	}
	fclose(f);

	
	
}



void PrintNCont()
{
	FILE* f = fopen("NContRadius.txt","w");
	

	for(int i = 0; i < NContBins; i++)
	{
		fprintf(f,"%lf %lf\n",NContStep*i, NContRadius[i]);
	}
	fclose(f);
	
	
}

void PrintContactMatrixAxis()
{
	FILE* f = fopen("ContactMatrixAxis.txt","w");
	fprintf(f,"** ");
	for(int j = 0; j < N; j++)
	{
	fprintf(f,"%i ", j);
	}
fprintf(f,"\n");
	for(int i = 0; i < N; i++)
	{
		fprintf(f,"%i ",i);
		for(int j =0 ; j < N; j++)
		{
			
			fprintf(f,"%i ", ContactMatrix[i][j]);
			
			
		}
		fprintf(f,"\n");
	}
	fclose(f);

}


void PrintContactMatrix()
{
	FILE* f = fopen("ContactMatrix.txt","w");

	for(int i = 0; i < N; i++)
	{
		
		for(int j =0 ; j < N; j++)
		{
			
			fprintf(f,"%i ", ContactMatrix[i][j]);
			
			
		}
		fprintf(f,"\n");
	}
	fclose(f);

}


int main(int argc, char *argv[])
{
	Vector pbcmin,pbcmax;
	if(argc < 4)
	{
		printf("chain length not specified, allow me to pop a jaunty little bonnet on your purview and ram up the shitter with lubricated horse cock!");
		
		
	}
	N = atoi(argv[1]);
	CONTACT_CUT = atof(argv[2]);
	NContStep = atof(argv[3]);
	printf("%i N\n",N);
	FILE *filenames = fopen("filenames.txt","r");
	int Nfiles;
	fscanf(filenames,"%i",&Nfiles);
	InitStats(N,1,0,N,0);
	for(int ff = 0; ff < N; ff++)
	{
		for(int gg = 0; gg < N; gg++)
		{
			ContactMatrix[ff][gg] = 0;
			DistanceMatrix[ff][gg] = 0;
		}
		
	}
	
	for(int gg = 0; gg < NContBins; gg++)
		{
			NContRadius[gg] = 0;
		}
	
	FILE *out = fopen("S_Rend2_Rg2_Pcont_lnS_lnRg2_lnPcont_Rg.txt","w");
	
	for(int k = 0; k < Nfiles; k++)
	{
		char fname[100];
		fscanf(filenames,"%s",&fname);
		FILE *crdFile = fopen(fname,"r");
		printf("Openging file %s\n",fname);	
		int a,b,c,d;
		for(int i = 0; i < N; i++)
		{
			if(fscanf(crdFile,"%lf %lf %lf %i %i %i %i",&crd[i].x[0],&crd[i].x[1],&crd[i].x[2],&a,&b,&c,&d) == EOF)
			{
				printf("Shit happened, wrong N or fucked up position files\n");
				return 0;
				
			}	
			printf("reading %lf %lf %lf\n",crd[i].x[0],crd[i].x[1],crd[i].x[2]);
			if(i > 0)
			{
			/*	for(mm = 0; mm <3 ; mm++)
				{
					double diff = crd[i].x[mm] - crd[i-1].x[mm] ;
					if( fabs(crd[i].x[mm] - crd[i-1].x[mm]) > 5)
					{
						if()
						
						
					}
					
				}
				
				*/
				if( (crd[i]-crd[i-1]).len() > 5)
				{
					printf("Length of bond > 5, you're probably using PBC\n Fuckity bye.");
					return 0;
					
				}
				
			}
			
		}
		printf("calculating bloody stats\n");
		CalculateStats();
		printf("updating contact matrix\n");
		UpdateContMatrix();
		
		
		fclose(crdFile);
	}
	outputStats(out);
	fclose(out);
	fclose(filenames);
	PrintContactMatrix();
	PrintContactMatrixAxis();
	PrintDistanceMatrix();
	PrintDistanceMatrixAxis();
	PrintNCont();
	if(Nfiles == 1)
	{
		char fuckfuck[50];
		    sprintf(fuckfuck,"gradpoly.vrml");
			outputGradVrml(fuckfuck);

		
	}
	
	
	
	
	
	
	
	
	
	
	
	return 0;
}


