/*
 * Objectives : (a) Information embedding using 1-LSB substitution
 *              (b) Calculation of MSE and PSNR
 *              (c) Writing resultant 2D array in PGM file
 */

/*
 * Compilation command :    gcc -o embed1LSB embed1LSB.c
 * Execution sequence  :    ./embed1LSB
 */

// importing library files
#include<stdio.h>

// defining bit size
#define bit_size 8

// declaring global variables
float MSE,PSNR;

// declaring functions
void embedInfo(FILE* matrixFile,FILE* secretFile,FILE* newMatrixFile);
char* oneLSB(char* binary,int secretInfo);
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
        secretFile = fopen("SecretInfo128.txt","rb");    // contains secret information
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
				
		        //printing MSE and PSNR
		        printf("MSE for 1-LSB substitution  = %.4f\n",MSE);
		        printf("PSNR for 1-LSB substitution = %.4f\n",PSNR);
		}

        return 0;
}

// function for embedding information, writing in new file and calculating MSE and PSNR
void embedInfo(FILE* matrixFile,FILE* secretFile, FILE* newMatrixFile)
{
        fprintf(newMatrixFile,"P2\n");              // writing magic number to identify pgm file
        fprintf(newMatrixFile,"%d %d\n",256,256);   // writing matrix width and height to the file
        fprintf(newMatrixFile,"255\n");             // writing the maximum gray value to the file
        float squaredError=0.0f;
        int i;
        for(i=0; i<256; i++)
        {
                int j;
                for(j=0; j<256; j++)
                {
                        int secretInfo, matrixElement, newElement;
                        fscanf(secretFile,"%d",&secretInfo);
                        fscanf(matrixFile,"%d",&matrixElement);
                        newElement = binToDec(oneLSB(decToBin(matrixElement),secretInfo));
                        fprintf(newMatrixFile,"%d ",newElement);
                        squaredError+=power((matrixElement-newElement),2);
                }
        }
        // calculating MSE and PSNR
        int n=256*256;
        MSE=squaredError/n;
        int max=power(2,bit_size)-1;
        
        //Simplifying the expression PSNR=10*logten(power(max,2)/MSE)
        PSNR=20*logten(max)-10*logten(MSE);
}

// function for implementing 1-LSB substitution
char* oneLSB(char *binary, int secretInfo)
{
        binary[bit_size-1] = (char)(secretInfo+48);
        return binary;
}

// function for converting decimal number to binary form
char *decToBin(int decimal)
{
        int  i=0;
        float temp;
        //char* result = (char*)malloc((bit_size + 1) * sizeof(char));
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