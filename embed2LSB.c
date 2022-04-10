/*
 * Objectives : (a) Information embedding using 2-LSB substitution with OPAP
 *              (b) Calculation of MSE and PSNR (with and without OPAP)
 *              (c) Writing resultant 2D array in PGM file (with OPAP only)
 */

/*
 * Compilation command :    gcc -o embed2LSB embed2LSB.c
 * Execution sequence  :    ./embed2LSB
 */

// importing library files
#include<stdio.h>

// defining bit size
#define bit_size 8

// declaring global variables
float MSE,PSNR,MSEwithOPAP,PSNRwithOPAP;

// declaring functions
void embedInfo(FILE* matrixFile,FILE* secretFile,FILE* newMatrixFile);
char* twoLSB(char* binary, int secretInfoBit1, int secretInfoBit2);
char* decToBin(int decimal);
int binToDec(char binary[]);
int extractNum(char c);
float power(float base,float expo);
float logten(float x);
void reverse(char* str);

// main function
int main()
{
        // declaring file pointers for embedding secret information
        FILE *secretFile,*matrixFile,*newMatrixFile;

        // opening files
        secretFile = fopen("SecretInfo256.txt","rb");    // contains secret information
        matrixFile = fopen("CoverInfo1.txt","rb");  // contains the original matrix
        newMatrixFile = fopen("NewImage.pgm","wb");   // contains the modified matrix

		// checking if all files exist or not
		if(secretFile==NULL || matrixFile==NULL)
				printf("File(s) not found.");
		else
		{
		        // embedding information and calculating MSE and PSNR
		        MSE=0.0f;
		        PSNR=0.0f;
		        embedInfo(matrixFile,secretFile,newMatrixFile);
		
		        // closing files
		        fclose(newMatrixFile);
		        fclose(secretFile);
		        fclose(matrixFile);
				
		        //printing MSE and PSNR (with and without OPAP)
		        printf("MSE for 2-LSB substitution (with OPAP)     = %.4f\n",MSEwithOPAP);
		        printf("PSNR for 2-LSB substitution (with OPAP)    = %.4f\n",PSNRwithOPAP);
		        printf("MSE for 2-LSB substitution (without OPAP)  = %.4f\n",MSE);
		        printf("PSNR for 2-LSB substitution (without OPAP) = %.4f\n",PSNR);
		}

        return 0;
}

// function for embedding information, writing in new file and calculating MSE and PSNR
void embedInfo(FILE* matrixFile,FILE* secretFile, FILE* newMatrixFile)
{
        fprintf(newMatrixFile,"P2\n");              // writing magic number to identify pgm file
        fprintf(newMatrixFile,"%d %d\n",256,256);   // writing matrix width and height to the file
        fprintf(newMatrixFile,"255\n");             // writing the maximum gray value to the file
        float squaredError=0.0f,squaredErrorWithOPAP=0.0f;
        int i;
        for(i=0; i<256; i++)
        {
                int j;
                for(j=0; j<256; j++)
                {
                        int secretInfoBit1, secretInfoBit2, matrixElement, newElement, delta, newElementWithOPAP;
                        fscanf(secretFile,"%d",&secretInfoBit1);
                        fscanf(secretFile,"%d",&secretInfoBit2);
                        fscanf(matrixFile,"%d",&matrixElement);
                        newElement = binToDec(twoLSB(decToBin(matrixElement),secretInfoBit1,secretInfoBit2));
                        squaredError+=power((matrixElement-newElement),2);
                        
                        // OPAP calculation
						delta=newElement-matrixElement;                        
                        int r=2;
                        if(power(2,r-1)<delta && delta<power(2,r) && newElement>=power(2,r))
                        		newElementWithOPAP=newElement-power(2,r);
                        else if((0.0f-power(2,r))<=delta && delta<=(0.0f-power(2,r-1)) && newElement<(power(2,bit_size)-power(2,r)))
                        		newElementWithOPAP=newElement+power(2,r);
                        else
                        		newElementWithOPAP=newElement;
						fprintf(newMatrixFile,"%d ",newElementWithOPAP);
                        squaredErrorWithOPAP+=power((matrixElement-newElementWithOPAP),2);
                }
        }
        // calculating MSE and PSNR
        int n=256*256;
        MSE=squaredError/n;
        MSEwithOPAP=squaredErrorWithOPAP/n;
        int max=power(2,bit_size)-1;
        
        //Simplifying the expression PSNR=10*logten(power(max,2)/MSE)
        PSNR=20*logten(max)-10*logten(MSE);
        PSNRwithOPAP=20*logten(max)-10*logten(MSEwithOPAP);
}

// function for implementing 1-LSB substitution
char* twoLSB(char *binary, int secretInfoBit1, int secretInfoBit2)
{
        binary[bit_size-2] = (char)(secretInfoBit1+48);
        binary[bit_size-1] = (char)(secretInfoBit2+48);
        return binary;
}

// function for converting decimal number to binary form
char *decToBin(int decimal)
{
        int  i=0;
        float temp;
        static char result[bit_size+1];
        while(decimal != 0 )
        {
                result[i] = (char)((decimal % 2)+48);
                decimal /= 2;
                i++;
        }
        result[bit_size] ='\0';
        if(i<bit_size)
        {
                int k;
                for(k=i; k<bit_size;k++)
                result[k] = '0';
        }
        reverse(result);
        return result;
}

// function for converting binary number to decimal form
int binToDec(char binary[])
{
        int decimal=0;
        int i;
        for(i=0;i<bit_size;i++)
                decimal += (extractNum(binary[i]) * power(2,bit_size-i-1));
        return(decimal);
}

// function for extracting integer value from a character
int extractNum(char c)
{
        int num;
        if(c < 58 && c > 47)
                num = c - '0';
        else
                num = c - 'A' + 10;
        return(num);
}

// function to calculate power
float power(float base,float expo)
{
        int i;
        float result=1.0f;
        for(i=0;i<expo;i++)
                result*=base;
        return result;
}

// function to calculate logarithm to the base 10 of x
float logten(float x)
{
        float count=1.0f, curr=0.0f;
		int i;
		for(i=0;i<1000;i++)
		{
				curr+=power((x-1)/(x+1),count)/count;
				count+=2.0f;
		}		
        curr*=2.0f;
        curr/=2.302585f;
        return curr;
}

// function to reverse a character array
void reverse(char *str)
{
		int j;
        char ch='0';
        for(j=0;j<bit_size/2;j++)
        {
                ch=str[j];
                str[j]=str[bit_size-1-j];
                str[bit_size-1-j]=ch;
        }
}