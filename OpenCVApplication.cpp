// OpenCVApplication.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "common.h"


void testOpenImage()
{
	char fname[MAX_PATH];
	while(openFileDlg(fname))
	{
		Mat src;
		src = imread(fname);
		imshow("opened image",src);
		waitKey();
	}
}

void testOpenImagesFld()
{
	char folderName[MAX_PATH];
	if (openFolderDlg(folderName)==0)
		return;
	char fname[MAX_PATH];
	FileGetter fg(folderName,"bmp");
	while(fg.getNextAbsFile(fname))
	{
		Mat src;
		src = imread(fname);
		imshow(fg.getFoundFileName(),src);
		if (waitKey()==27)
			break;
	}
}

void testColor2Gray()
{
	char fname[MAX_PATH];
	while(openFileDlg(fname))
	{
		Mat_<Vec3b> src = imread(fname, IMREAD_COLOR);

		int height = src.rows;
		int width = src.cols;

		Mat_<uchar> dst(height, width);

		for (int i=0; i<height; i++)
		{
			for (int j=0; j<width; j++)
			{
				Vec3b v3 = src(i,j);
				uchar b = v3[0];
				uchar g = v3[1];
				uchar r = v3[2];
				dst(i,j) = (r+g+b)/3;
			}
		}
		
		imshow("original image",src);
		imshow("gray image",dst);
		waitKey();
	}
}

Mat_ <uchar> getMiniPoza(Mat_<uchar> src, Point p, int latime, int lungime)
{
	Mat_ <uchar> dst(latime, lungime);
	int i2 = 0, j2 = 0;

	for (int i = p.x; i < p.x + latime; i++)
	{
		for (int j = p.y; j < p.y + lungime ; j++)
		{	
			
			
			dst(i2, j2) = src(i, j);
			j2++;
		}
		i2++;
		j2 = 0;
	}
	
	imshow("mini", dst);
	waitKey(0);

	return dst;
}

float RMSE(Mat_<uchar> poza1, Mat_<uchar> poza2, int pozitie)
{
	float sum = 0;
	float scor = 0;
	float bestScore = 999999;
	int pozitieDePotrivire = 0; 
	
	if (pozitie == 0)
	{
		for (int i = 0; i < poza1.cols; i++)
		{
			sum += (1.0f * (poza1(0, i) - poza2(poza2.rows - 1, i)) * (poza1(0, i) - poza2(poza2.rows - 1, i))) / poza1.cols;
		}
		scor = sqrt(sum);
		printf("\nscor potrivire (caz 0) ==> img2 peste img1 = %f", scor);
		bestScore = scor;
	}
		
	sum = 0;

	if (pozitie == 1)
	{
		for (int i = 0; i < poza1.rows; i++)
		{
			sum += (1.0f * (poza1(i, poza1.cols - 1) - poza2(i, 0)) * (poza1(i, poza1.cols - 1) - poza2(i, 0))) / poza1.rows;
		}
		scor = sqrt(sum);
		printf("\nscor potrivire (caz 1) ==> img2 in dreapta lui img1 = %f", scor);
		if (scor < bestScore)
		{
			bestScore = scor;
			pozitieDePotrivire = 1;
		}
	}
	sum = 0;

	if (pozitie == 2)
	{
		for (int i = 0; i < poza1.cols; i++)
		{
			sum += (1.0f * (poza1(poza1.rows - 1, i) - poza2(0, i)) * (poza1(poza1.rows - 1, i) - poza2(0, i))) / poza1.cols;

		}
		scor = sqrt(sum);
		printf("\nscor potrivire (caz 2) ==> img2 sub img1 = %f ", scor);
		if (scor < bestScore)
		{
			bestScore = scor;
			pozitieDePotrivire = 2;
		}
	}
	sum = 0;

	if (pozitie == 3)
	{
		for (int i = 0; i < poza1.rows; i++)
		{
			sum += (1.0f * (poza1(i, 0) - poza2(i, poza2.cols - 1)) * (poza1(i, 0) - poza2(i, poza2.cols - 1))) / poza1.rows;
		}
		scor = sqrt(sum);
		printf("\nscor potrivire (caz 3) ==> img2 in stanga lui img1 = %f", scor);
		if (scor < bestScore)
		{
			bestScore = scor;
			pozitieDePotrivire = 3;
		}

	}

	return bestScore;
}

void impartireImg()
{
	char fname[MAX_PATH];
	openFileDlg(fname);
	Mat_<uchar> src = imread(fname, IMREAD_GRAYSCALE);
	Mat_<uchar> dst(src.rows, src.cols);

	std::vector<Mat_<uchar>> miniPoze;

	int mijlocRanduri = src.rows / 2;
	int mijlocColoane = src.cols / 2;
	int latime = mijlocRanduri;
	int lungime = mijlocColoane;
	
	int X[] = { 0, 0			,mijlocRanduri, mijlocRanduri};
	int Y[] = { 0, mijlocColoane,0			  , mijlocColoane };

	int X1[] = { 0, 0			,mijlocRanduri, mijlocRanduri };
	int Y1[] = { 0, mijlocColoane,mijlocColoane,			0 };

	for (int i = 0; i < 4; i++)
	{
		Mat_<uchar> miniPoza = getMiniPoza(src, Point(X[i], Y[i]), latime, lungime);
		miniPoze.push_back(miniPoza);
	}

	
	Mat_ <uchar> pozaStart = miniPoze[0]; //consideram ca e piesa de inceput a puzzle-ului

	int flags[4] = { 0,0,0,0 }; //vector cu flag-uri pentru pozele pe care le verificam

	
	flags[0]=1; //poza aceasta nu va fi verificata, consideram ca o alegem pe prima
	
	float bestScore = 100000;
	float auxScore = 0.0;
	int indiceminiPoza = 0; 
	int tipPotrivire = 1;
	int indexPunctStart = 0;
	int iminiPoza = 0, jminiPoza = 0;

	
	//desenam prima poza
	for (int i = X1[indexPunctStart]; i < X1[indexPunctStart] + latime; i++)
	{
		jminiPoza = 0;
		for (int j = Y1[indexPunctStart]; j < Y1[indexPunctStart] +lungime; j++)
		{
			dst(i, j) = pozaStart(iminiPoza, jminiPoza); //pentru ca minipoza e mai mica in dimensiune
			jminiPoza++;
		}
		iminiPoza++;
	}

	printf("Am desenat prima poza");

	for (int j = 0; j < 3; j++)
	{
		for (int i = 0; i < miniPoze.size(); i++)
		{
			if (flags[i] == 0)
			{
				//verifica ultima <-> prima coloana
				auxScore = RMSE(pozaStart, miniPoze[i], tipPotrivire);

				if (auxScore < bestScore)
				{
					bestScore = auxScore;
					indiceminiPoza = i;
				}

			}
		}


		iminiPoza = 0;
		jminiPoza = 0;
		indexPunctStart++;
		pozaStart = miniPoze[indiceminiPoza];


		//desenam prima poza
		for (int i = X1[indexPunctStart]; i < X1[indexPunctStart] + latime; i++)
		{
			jminiPoza = 0;
			for (int j = Y1[indexPunctStart]; j < Y1[indexPunctStart] +lungime; j++)
			{
				dst(i, j) = pozaStart(iminiPoza, jminiPoza); //pentru ca minipoza e mai mica in dimensiune
				jminiPoza++;
			}
			iminiPoza++;
		}

		flags[indiceminiPoza] = 1; //am verificat poza asta
		tipPotrivire++;
		bestScore = 100000;
		auxScore = 0.0;

		printf("\nPotrivire la iteratia %d", j);
		imshow("Imagine",  miniPoze[indiceminiPoza]);
		waitKey();

	}

	imshow("Result", dst);
	waitKey();
	
}


int main()
{
	int op;
	do
	{
		system("cls");
		destroyAllWindows();
		printf("Menu:\n");
		printf(" 1 - Basic image opening..\n");
		printf(" 2 - Open BMP images from folder\n");
		printf(" 3 - Color to Gray\n");
		printf(" 4 - 2x2\n");
		printf(" 0 - Exit\n\n");
		printf("Option: ");
		scanf("%d",&op);
		switch (op)
		{
			case 1:
				testOpenImage();
				break;
			case 2:
				testOpenImagesFld();
				break;
			case 3:
				testColor2Gray();
				break;
			case 4:
				impartireImg();
				break;
		}
	}
	while (op!=0);
	return 0;
}
