#include <stdio.h>
#include <stdlib.h>
#include "gng1106plplot.h"
#include <math.h>
#include <setjmp.h>
#define TRUE 1
#define FALSE 0
#define MAX_T
#define MAX_PC
#define MAX_VALS 5
#define N 50
#define MINVAL 1e-08
/*
    Names: Elias Tarabey (300289536) and Mathew Aoun (300275840)
*/
//-------------------------------------------------------------//
// Structure type definition
typedef struct{ // Structure type definition for the circuit.
    double C;
    double R;
    double td;
    double minpc, maxpc;
    double arrvalsresults[N][N];
    double valsarr[MAX_VALS];
    }ELEC_CIRCUIT;
//-------------------------------------------------------------//
// Function prototypes
void checkinp(ELEC_CIRCUIT *);
int CalculateLmin(ELEC_CIRCUIT *);
void Calculate(ELEC_CIRCUIT *);
double fx(ELEC_CIRCUIT *, double, double);
double eq9(ELEC_CIRCUIT *, double);
void trycatch(jmp_buf);
void WriteFile(ELEC_CIRCUIT *);
int ReadFile(ELEC_CIRCUIT *);
void PlotFunc(ELEC_CIRCUIT *);
void plot(ELEC_CIRCUIT *, double *, double *);
// --------------------------------------------------------------//
/*----------------------------------------------------------
void main():
-	Parameters: None
-	Return value: None
-	Logic/Algorithm:
o	The main function will be used to call the functions that will be described in this document, but it will also communicate with the user in order that they might enter the appropriate values when prompted.
o	The function shall also contain do while loops in order to make the user to enter a valid input. If not, the do while loop continues until the user enters a valid input.
o	The main function shall also ask the user if they want to save their inputs in a file, and if they do, the program will call the writefile function.
o	The main function shall also call all of the functions defined in this document.
---------------------------------------------------------------*/

void main(){
ELEC_CIRCUIT FindL; // Structure variable definition.
int boolcheck;
boolcheck = 0; // Bool validation variable for do/while loops.
char val;
boolcheck = ReadFile(&FindL);
while(boolcheck == 0){
checkinp(&FindL);
boolcheck = CalculateLmin(&FindL);}
Calculate(&FindL);
PlotFunc(&FindL);
if(boolcheck == 1){
printf("Do you want to save your data in a file?\n");
scanf(" %s",&val);
if(val == 'y'){
    WriteFile(&FindL);
}
}
}
/*----------------------------------------------------------
int checkinp(structvariabletype *circuitptr)
-	Parameters :
o	circuitptr : Pointer that references the structure variable where the values of R, C, td, minpc and maxpc are stored.
-	Return value: 1 (TRUE) or 0 (FALSE)
-	Logic/Algorithm:
o	This function implements logic statements that verify that the values of R, C, td, minpc and maxpc, and diffpc using a for loop structure to verify if the inputs are of type string or are negative.
o	If any of the values are negative, or are strings, the function returns the symbolic constant FALSE.
o	If the values are valid (they are positive and are not strings), the function returns the symbolic constant TRUE (1), which breaks out of the do while loop of the main function.
---------------------------------------------------------------*/

void checkinp(ELEC_CIRCUIT *elecptr){ // Function to get user inputs and to validate them.
    int i;
    i = 0;
    double diffpc,testval;
    testval = 0;
    diffpc = 0;
    while(i<MAX_VALS+1){
    switch(i){ // switch statement used to get user inputs.
    case 0:
        printf("Enter the value of C: \n");
        scanf("%lf",&testval);
       elecptr->C = testval;
        break;
    case 1:
        printf("Enter the value for R\n");
        scanf("%lf",&testval);
        elecptr->R = testval;
        break;
    case 2:
        printf("Enter the value for td\n");
        scanf("%lf",&testval);
        elecptr->td = testval;
        break;
    case 3:
        printf("Enter the value for minpc\n");
        scanf("%lf",&testval);
        elecptr->minpc = testval;
        break;
    case 4:
        printf("Enter the value for maxpc\n");
        scanf("%lf",&testval);
        elecptr->maxpc = testval;
        break;
    case 5: // Goes back to case 3 if minpc>=maxpc.
        diffpc = elecptr->maxpc - elecptr->minpc;
        if(diffpc <= 0||elecptr->minpc>=1||elecptr->maxpc>=1){
            printf("Invalid values for minpc and maxpc. Please try again.\n");
            i = 2;
        }
        break;
    }
    if(testval <= 0){
        printf("Invalid value entered. Please try again.\n");
    }
    else{
        elecptr->valsarr[i] = testval;
        i += 1;
    }
    }
}
/*----------------------------------------------------------
-	int CalculateLmin(structvariabletype * circuitptr)
o	Parameters:
	circuitptr : Pointer that references the structure variable where the values of R, C, td, minpc and maxpc are stored.
o	Return value: 1 (TRUE) or 0 (FALSE)
o	Logic/Algorithm: This function will be used to calculate the L by using equation 7 and if we find that the value of L is bigger than the Lmin then we will tell the user to try again (do/while loop).
---------------------------------------------------------------*/

int CalculateLmin(ELEC_CIRCUIT *circuitptr){ // Function that verifies if L in Calc() is more or less than Lmin. Returns True values are valid and False if not.
    double Lmin, L;
    Lmin = 0;
    L = 0;
    Lmin = (circuitptr->C*pow(circuitptr->R,2))/4;
    L = eq9(circuitptr,circuitptr->minpc);
    if (L<Lmin){
        printf("The values inputed give a smaller L value than expected. Please try again.\n");
        return FALSE;
    }
    else{
        return TRUE;
    }
}
/*----------------------------------------------------------
-	Double Calculate(structvariabletype * circuitptr)
o	Parameters:
	circuitptr : Pointer that references the structure variable where the values of R, C, td, minpc and maxpc are stored.
-	Return Value: The value calculated
-	Logic/Algorithm: This function will be used to calculate the 9th and 10th equations and after the calculation, it will verify the value and the roots to make sure they are both the same if not then we will ask the user to enter new values.
---------------------------------------------------------------*/

void Calculate(ELEC_CIRCUIT *circuitptr){ // Function that uses the bisectional root method to find the values of L given a value of pc in a while, and saves them in a 2D array in the structure variable.
double xl, xu, xr,fxl,fxr, increment, pc;
int i;
i = 0;
pc = circuitptr->minpc;
xl = 0.0;
xu = eq9(circuitptr,circuitptr->maxpc);
xr = 0.0;
increment = (circuitptr->maxpc-circuitptr->minpc)/N;
while(i<N){
    xr = (xl+xu)/2;
    fxl = fx(circuitptr,xl,pc);
    fxr = fx(circuitptr,xr,pc);
    if(fxl*fxr<0){
        xu = xr;
    }
    else{
        xl = xr;
    }
    if(fabs(fxl*fxr)<MINVAL){
        circuitptr->arrvalsresults[i][0] = pc;
        circuitptr->arrvalsresults[i][1] = xr;
        xl = 0.0;
        xu = eq9(circuitptr,circuitptr->maxpc);
        pc += increment;
        i += 1;
    }
}
}
double fx(ELEC_CIRCUIT *circuitptr,double x, double pcval){ // Equation 10 calculation
    double eq;
    eq = exp(((-1)*(circuitptr->R)*(circuitptr->td))/(2*x))-pcval;
    return eq;
}

double eq9(ELEC_CIRCUIT *circuitptr,double pc){ // Equation 9 function.
    double result;
    result = ((-1)*circuitptr->R*circuitptr->td)/(2*log(pc));
    return result;
}
/*----------------------------------------------------------
void plot(structvariabletype * circuitptr, double *xPtr, double *yPtr)
-	Parameters:
o	n: number of points in the arrays
o	*xPtr: pointer to x values (horizontal axis).
o	*yPtr: pointer to y values (vertical axis).
-	Return value: None
-	Description: Initialises the plot.  The following values in the referenced structure are used to set up the plot: x[0], x[n-1] also assume that x values are sequential Miny, Maxy - vertical axis range. Also, sets up a white background and black foreground colours. Then plots the curve accessed using *xPtr and *yPtr.

---------------------------------------------------------------*/

void plot(ELEC_CIRCUIT *eleccircuit, double *xPtr, double *yPtr) // Function that plots the function based off the values in the 2D array.
{
    double miny, maxy;
    double range;  // range of vertical axix
        // Setup plot configuration
    plsdev("wingcc");  // Sets device to wingcc - CodeBlocks compiler
    // Initialise the plot
    plinit();
    // Configure the axis and labels
    plwidth(3);          // select the width of the pen
    // Find range for axis
    miny = eleccircuit ->arrvalsresults[0][1];
    maxy = eleccircuit -> arrvalsresults[N-1][1];
    plenv(eleccircuit->arrvalsresults[0][0],eleccircuit->arrvalsresults[N-1][0],miny,maxy,0,1);
    plcol0(BLUE);
    pllab("pc","L","Plot of the values of L for a given value of pc");
    plcol0(BLUE);
    plline(N,xPtr,yPtr);

    plend();
}
/*----------------------------------------------------------
Function plotFunc
-	void plotFunc(structvariabletype * circuitptr)
o	Parameters:
	Begin: begin on the intervals (x values) to plot
	End: end of interval (x values) to plot
	Description: Plot the function on the interval between begin and the end.  Plots an x at the roots
---------------------------------------------------------------*/

void PlotFunc(ELEC_CIRCUIT *eleccircuit){ // Function that puts the values of the 2D into two seperate arryas for plotting and calls plot().
 double x[N];
    double y[N];
    //double inc; // increment for incrementing x
    double yRoots[N];
    double nroots;
    int ix;
    // Calculate function points
    x[0] = eleccircuit -> arrvalsresults[0][0];
    y[0] = eleccircuit -> arrvalsresults[0][1];
    for(ix=1;ix<N;ix++)
    {
        x[ix] = eleccircuit -> arrvalsresults[ix][0];
        y[ix] = eleccircuit -> arrvalsresults[ix][1];
    }
    // Calculate y points at the root
    // Plot
    plot(eleccircuit, x, y);
}
/*----------------------------------------------------------
Function WriteFile
-	double WriteFile (structvariabletype * circuitptr)
o	Parameters :
	* structvariabletype * circuitptr
o	Return Value: None
o	Description: This function is used to write in the file
o	Logic/Algorithm: This function is used at the end of the code when you are done getting all your data the code is gonna ask you to if you wanna save it in a file and if yes they will ask you to write the name of the code so later on if you would like to return those resulats they will be under a file name.
---------------------------------------------------------------*/
void WriteFile(ELEC_CIRCUIT *eleccircuit){ // Function that writes the user input into a binary file.
    int num,ix;
    double a;
   FILE *fptr;
   char filename[1000];
   printf("Name the file: ");
   scanf(" %s",filename);
   fptr=fopen(filename,"wb");
    if (fptr==NULL){
        printf("Can't open");
        }
    else{
        fwrite(eleccircuit,sizeof(ELEC_CIRCUIT),1,fptr);}
   fclose(fptr);
}
/*----------------------------------------------------------
double ReadFile (structvariabletype * circuitptr)
-	Parameters:
o	* structvariabletype * circuitptr
-	Return Value: True or False
-	Description: This function is used to read a file
-	Logic/Algorithm: The functions start by asking the user if they want to read from a file. If the users say yes, then they will have to enter the file name. If not, then the programming code will asking them to put the input needed.
---------------------------------------------------------------*/

int ReadFile(ELEC_CIRCUIT *eleccircuit){ // Function that reads the data in a specific binary file given by the user and puts tgose values in their respective variables in the structure variable.
   char ch,file_name[1000000];
   ELEC_CIRCUIT a;
   FILE *fp;
   printf("Do you want to read from a file? ");
   scanf(" %s",&ch);
   if(ch == 'y'){
    printf("Enter name of a file you wish to see\n");
    scanf("%s",file_name);
    fp = fopen(file_name,"rb");
    if (fp==NULL){
        printf("Can't open\n");
        printf("We will enter new values now.\n");
        return FALSE;
        }
    else{
        fread(&a,sizeof(ELEC_CIRCUIT),1,fp);
        *eleccircuit = a;
    }
   fclose(fp);
   return TRUE+1;}
   else{
    return FALSE;
   }
   }


