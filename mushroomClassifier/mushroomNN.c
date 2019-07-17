
/****************************************************************************/
/*Program : mushroomNN.c                                                    */
/*Description: classify mushrooms using Back propogation on a neural network*/
/*Author: David Nachmanson                                                  */
/*Last Date modified: 11/13/2017                                            */
/****************************************************************************/


#include "stdio.h"
#include "stdlib.h"
#include "time.h"
#include "math.h"


const int numSamples = 8124;
const int numAttributes = 23;
int numHiddenNeurons;
int numOutputNeurons;
const double testDataFraction = .15;
const double trainingDataFraction = .85;
double learningSpeed;
const int maxEpochs = 100;

void enumerateMushroomData(FILE * stream, double mushroomTrainingData [(int) (numSamples * trainingDataFraction)][numAttributes],
double mushroomTestingData[(int)((numSamples * testDataFraction) + 1)][numAttributes]) {
	srand(time(NULL));
	int testDataStartPoint = rand( ) % (numSamples - (int)(numSamples * testDataFraction) + 1);
	int testDataEndPoint = testDataStartPoint + numSamples * testDataFraction;
	int trainingRow = 0;
	int testRow = 0;
	int dataRow = 0;
	int attributeColumn;
	int lineIndex;
	int inputtingToTestData = 0;
	char line[1024];




	fgets(line,1024,stream);//Throw away attribute names.
	while(fgets(line,1024,stream) )
	{
		lineIndex = 0;
		attributeColumn = 0;

		if(testDataStartPoint <= dataRow && dataRow <= testDataEndPoint)
		{
				inputtingToTestData = 1;
	  }
		while(line[lineIndex] != '\0')
		{

			if(line[lineIndex] != ',')
			{
					if(inputtingToTestData){
						mushroomTestingData[testRow][attributeColumn] = (double) line[lineIndex] / 100;
					} else {
						mushroomTrainingData[trainingRow][attributeColumn] = (double) line[lineIndex] / 100;
					}
					attributeColumn++;
			}
			lineIndex++;
	  }
		dataRow++;
		if(inputtingToTestData)
		{
			testRow++;
			inputtingToTestData = 0;
		} else
	  {
			trainingRow++;
		}
	}

}

 double sigmoidFunction(double input){
	return 1/(1 + exp(-1 *(input)));
}

int desiredOutput(double desiredOutput, double oOutputValues[]){


	double poison = (double) 'p' / 100;
	double edible = (double) 'e' / 100;

	for(int i = 0; i < numOutputNeurons;i++)
	{

		if(!((desiredOutput == poison && oOutputValues[i] < .5) || (desiredOutput == edible && oOutputValues[i] > .5)))
		{
			return 0;
		}
	}

	return 1;

}

void initializeNetworkWeights(double ihWeights[numAttributes - 1][numHiddenNeurons], double hoWeights[numHiddenNeurons][numOutputNeurons]){
	srand(time(NULL));
  for(int i = 0; i < numAttributes - 1; i++)
	{
			for(int j = 0; j < numHiddenNeurons; j++)
			{
				ihWeights[i][j] = (rand()/(double)(RAND_MAX))*abs(-2) -1;
			}
	}

	for(int i = 0; i < numHiddenNeurons; i++)
	{
			for(int j = 0; j < numOutputNeurons; j++)
			{
				hoWeights[i][j] = (rand()/(double)(RAND_MAX))*abs(-2) -1;
			}
	}
}

void calculateTrainingOutputValues(double hOutputValues[], double oOutputValues[],
	double ihWeights[numAttributes - 1][numHiddenNeurons], double hoWeights[numHiddenNeurons][numOutputNeurons],
	double mushroomTrainingData[(int) (numSamples * trainingDataFraction)][numAttributes],int mushroomRow){
		double sum = 0;

		for(int i = 0; i < numHiddenNeurons; i++)
		{
				for(int j = 0; j < numAttributes - 1; j++)
				{
					sum = sum + mushroomTrainingData[mushroomRow][j + 1] * ihWeights[j][i];
				}

				hOutputValues[i] = sigmoidFunction(sum);

				sum = 0;
	  }

		for(int i = 0; i < numOutputNeurons; i++)
		{
				for(int j = 0; j < numHiddenNeurons; j++){
					sum = sum + hoWeights[j][i] * hOutputValues[j];
				}
				oOutputValues[i] = sigmoidFunction(sum);
		}

}

void calculateErrorValues (double hoWeights[numHiddenNeurons][numOutputNeurons],double hErrorValues[numHiddenNeurons],double oErrorValues[numOutputNeurons],
double hOutputValues[numHiddenNeurons],double oOutputValues[numOutputNeurons],double desiredOutput){

	int target;
	double sum = 0;
	double poison = (double)'p'/ 100;
	if(desiredOutput == poison)
	{
		target = 0;
	} else
	{
		target = 1;
	}

	for(int i = 0; i < numOutputNeurons; i++)
	{
		oErrorValues[i] = (oOutputValues[i] * ((1  - oOutputValues[i]) * ( target - oOutputValues[i])));
	}

	for (int i = 0; i < numHiddenNeurons; i++)
	{
		for(int j = 0; j < numOutputNeurons; j++)
		{
			sum = sum + hoWeights[i][j] * oErrorValues[j];
		}
		hErrorValues[i] = (hOutputValues[i] * ( (1  - hOutputValues[i]) * sum));
		sum = 0;
	}
}

void updateWeights(double ihWeights[numAttributes - 1][numHiddenNeurons],double hoWeights[numHiddenNeurons][numOutputNeurons],double hErrorValues[],double oErrorValues[]
,double hOutputValues[],double mushroomTrainingData[(int) (numSamples * trainingDataFraction)][numAttributes], int mushroomRow){

	for(int i = 0; i < numAttributes - 1; i++)
	{
		for(int j = 0; j < numHiddenNeurons; j++)
		{
			ihWeights[i][j] += (learningSpeed * hErrorValues[j]) * mushroomTrainingData[mushroomRow][i + 1];
		}
	}

	for(int i = 0; i < numHiddenNeurons; i++)
	{
		for(int j = 0; j < numOutputNeurons; j++)
		{
			hoWeights[i][j] += (learningSpeed * oErrorValues[j]);
		}
	}
}

void calculateTestingOutputValues(double hOutputValues[], double oOutputValues[],
	double ihWeights[numAttributes - 1][numHiddenNeurons], double hoWeights[numHiddenNeurons][numOutputNeurons],
	double mushroomTestingData[ (int)(numSamples * testDataFraction) + 1][numAttributes],int mushroomRow){

		double sum = 0;

		for(int i = 0; i < numHiddenNeurons; i++)
		{
				for(int j = 0; j < numAttributes - 1; j++)
				{
					sum = sum + (int )mushroomTestingData[mushroomRow][j + 1] * ihWeights[j][i];
				}
				hOutputValues[i] = sigmoidFunction(sum);
	  }

		sum = 0;
		for(int i = 0; i < numOutputNeurons; i++)
		{
				for(int j = 0; j < numHiddenNeurons; j++)
				{
					sum = sum + hoWeights[j][i] * hOutputValues[j];
				}
				oOutputValues[i] = sigmoidFunction(sum);
		}

}

double testNeuralNet(double hOutputValues[], double oOutputValues[],double ihWeights[numAttributes - 1][numHiddenNeurons],
	 double hoWeights[numHiddenNeurons][numOutputNeurons],double mushroomTestingData[ (int)(numSamples * testDataFraction) + 1][numAttributes]){

	double accuracyCount = 0;
	for(int i = 0; i < (int)(numSamples * testDataFraction) + 1; i++)
	{
		calculateTestingOutputValues(hOutputValues,oOutputValues,
			ihWeights,hoWeights,mushroomTestingData,i);
			if(desiredOutput(mushroomTestingData[i][0], oOutputValues))
			{
				accuracyCount++;
			}
  }

	return accuracyCount/((int) (numSamples * testDataFraction) + 1);
}

void printNetworkWeights(double ihWeights[numAttributes - 1][numHiddenNeurons],double hoWeights[numHiddenNeurons][numOutputNeurons]){
	fprintf(stderr,"input to hidden layer weights, A[i][j] = weight from i'th input to j'th node in hidden layer\n\n");
	for(int i = 0; i < numAttributes - 1; i++)
  {
		for(int j = 0; j < numHiddenNeurons; j++)
		{
			fprintf(stderr, "%f ", ihWeights[i][j]);
		}
		fprintf(stderr,"\n");
	}
	fprintf(stderr,"\n");

	fprintf(stderr,"hidden to output layer weights, A[i][j] = weight from i'th hidden node to j'th node in hidden layer\n\n");

	for(int i = 0; i < numHiddenNeurons; i++)
  {
		for(int j = 0; j < numOutputNeurons; j++)
		{
			fprintf(stderr, "%f ", hoWeights[i][j]);
		}
		fprintf(stderr,"\n");
	}
	fprintf(stderr,"\n");
}


 void trainNeuralNet(double ihWeights[numAttributes - 1][numHiddenNeurons],double hoWeights[numHiddenNeurons][numOutputNeurons],
	double hErrorValues[numHiddenNeurons],double oErrorValues[numOutputNeurons],double hOutputValues[numHiddenNeurons],double oOutputValues[numOutputNeurons],
	double mushroomTrainingData[(int) (numSamples * trainingDataFraction)][numAttributes]){


		int mushroomRow = 0;
		int trainingAccuracy = 0;
		int previousTrainingAccuracy = -1;
		int calledBP = 0;
		int epochCount = 0;

		clock_t begin = clock();
		while(trainingAccuracy/ ((int) numSamples * trainingDataFraction) < .99 && epochCount < maxEpochs)
		{
			previousTrainingAccuracy = trainingAccuracy;
			trainingAccuracy = 0;
			while(mushroomRow < (int) (numSamples * trainingDataFraction))
			{

				calculateTrainingOutputValues(hOutputValues,oOutputValues,
					ihWeights,hoWeights,
					mushroomTrainingData,mushroomRow);
		      while(!(desiredOutput(mushroomTrainingData[mushroomRow][0], oOutputValues)))
					{
						calledBP = 1;
						calculateErrorValues(hoWeights,hErrorValues,oErrorValues,hOutputValues,oOutputValues,mushroomTrainingData[mushroomRow][0]);

						updateWeights(ihWeights,hoWeights,hErrorValues,oErrorValues,hOutputValues,mushroomTrainingData,mushroomRow);

						calculateTrainingOutputValues(hOutputValues,oOutputValues,
							ihWeights,hoWeights,mushroomTrainingData,mushroomRow);
		     }
				 if(!(calledBP))
				 {
					 	trainingAccuracy++;
				 }
				 calledBP = 0;
		     mushroomRow++;
		}
		epochCount++;
		fprintf(stderr, "epoch # : %d, accuracy : %f \n",epochCount,trainingAccuracy/ ((int) numSamples * trainingDataFraction));
		mushroomRow = 0;
	}
	clock_t end = clock();
	double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
	fprintf(stderr, "training accuracy : %f\n", trainingAccuracy/ ((int) numSamples * trainingDataFraction));
	fprintf(stderr,"CPU run time : %f seconds \n", time_spent);
	fprintf(stderr,"total number of epochs: %d \n\n",epochCount);
}



int main(int argc, char *argv[])
{
if(argc != 4)
{
	fprintf(stderr,"specify input - # hidden neurons, # output neurons, learning speed\n");
	exit(1);
}
numHiddenNeurons = atoi(argv[1]);
numOutputNeurons = atoi(argv[2]);
learningSpeed =  atof(argv[3]);
double mushroomTrainingData[(int) (numSamples * trainingDataFraction)][numAttributes];
double mushroomTestingData[ (int)(numSamples * testDataFraction) + 1][numAttributes];
double ihWeights[numAttributes - 1][numHiddenNeurons];
double hoWeights[numHiddenNeurons][numOutputNeurons];
double hErrorValues[numHiddenNeurons]; //how much network error varies with respect to input to hidden neurons.
double oErrorValues[numOutputNeurons];
double hOutputValues[numHiddenNeurons];
double oOutputValues[numOutputNeurons];
FILE* stream = fopen("mushrooms.csv", "r");
enumerateMushroomData(stream,mushroomTrainingData,mushroomTestingData);
fprintf(stderr,"number of inputs = %d\n",numAttributes - 1);
fprintf(stderr,"number of hidden layers = 1\n");
fprintf(stderr,"number of neurons in hidden layer = %d\n",numHiddenNeurons);
fprintf(stderr,"number of outputs = 1\n");
fprintf(stderr,"initial learning speed =  %f\n\n" ,learningSpeed);
fprintf(stderr,"initial network weights... \n\n");
initializeNetworkWeights(ihWeights,hoWeights);
printNetworkWeights(ihWeights,hoWeights);
trainNeuralNet(ihWeights,hoWeights,hErrorValues,oErrorValues,hOutputValues,oOutputValues,mushroomTrainingData);
fprintf(stderr,"trained network weights... \n\n");
printNetworkWeights(ihWeights,hoWeights);
fprintf(stderr,"neural net accuracy = %f\n", testNeuralNet(hOutputValues, oOutputValues,ihWeights,hoWeights, mushroomTestingData));
}
