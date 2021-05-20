// OpenCVApplication.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "common.h"

void testOpenImage()
{
	char fname[MAX_PATH];
	while (openFileDlg(fname))
	{
		Mat src;
		src = imread(fname);
		imshow("opened image", src);
		waitKey();
	}
}

void testOpenImagesFld()
{
	char folderName[MAX_PATH];
	if (openFolderDlg(folderName) == 0)
		return;
	char fname[MAX_PATH];
	FileGetter fg(folderName, "bmp");
	while (fg.getNextAbsFile(fname))
	{
		Mat src;
		src = imread(fname);
		imshow(fg.getFoundFileName(), src);
		if (waitKey() == 27)
			break;
	}
}

void testColor2Gray()
{
	char fname[MAX_PATH];
	while (openFileDlg(fname))
	{
		Mat_<Vec3b> src = imread(fname, IMREAD_COLOR);

		int height = src.rows;
		int width = src.cols;

		Mat_<uchar> dst(height, width);

		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				Vec3b v3 = src(i, j);
				uchar b = v3[0];
				uchar g = v3[1];
				uchar r = v3[2];
				dst(i, j) = (r + g + b) / 3;
			}
		}

		imshow("original image", src);
		imshow("gray image", dst);
		waitKey();
	}
}

Mat_ <uchar> getMiniPoza(Mat_<uchar> src, Point p, int latime, int lungime)
{
	Mat_ <uchar> dst(latime, lungime);
	int i2 = 0, j2 = 0;

	for (int i = p.x; i < p.x + latime; i++)
	{
		for (int j = p.y; j < p.y + lungime; j++)
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

Mat_<uchar> extractMat(Mat_<uchar> poza, int pozitie) {

	Mat_<uchar> extras;

	if (pozitie == 1 || pozitie == 3)
	{
		extras = Mat_<uchar>(3, poza.rows);
	}

	if (pozitie == 0 || pozitie == 2)
	{
		extras = Mat_<uchar>(3, poza.cols);
	}

	if (pozitie == 0)
	{
		for (int i = 0; i < 3; i++)
			for (int j = 0; j < poza.cols; j++)
				extras(i, j) = poza(i, j);
	}

	if (pozitie == 1)
	{
		int i2 = 0, j2 = 0;

		for (int i = poza.cols - 1; i >= poza.cols - 3; i--) {
			for (int j = 0; j < poza.rows; j++)
			{
				extras(i2, j2) = poza(j, i);
				j2++;
			}
			i2++;
			j2 = 0;
		}
	}

	if (pozitie == 2)
	{
		for (int i = 0; i < 3; i++)
			for (int j = 0; j < poza.cols; j++)
				extras(i, j) = poza(poza.rows - i - 1, j);
	}

	if (pozitie == 3)
	{
		int i2 = 0, j2 = 0;

		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < poza.rows; j++)
			{
				extras(i2, j2) = poza(j, i);
				j2++;
			}
			i2++;
			j2 = 0;
		}
	}

	return extras;
}

float RMSE(Mat_<uchar> poza1, Mat_<uchar> poza2)
{
	float sum = 0;
	float scor = 0;
	float pondere[3] = { 0.8, 0.15, 0.05 };

	if (poza1.cols != poza2.cols) {
		printf("Poza rotita gresit, L != l");
		return 999999;
	}

	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < poza1.cols; j++)
		{
			sum += (1.0f * (poza1(i, j) - poza2(i, j)) * (poza1(i, j) - poza2(i, j))) / poza1.cols * pondere[i];
		}
	}

	scor = sqrt(sum);
	printf("scor = %f;   ", scor);

	return scor;
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

	int X[] = { 0, 0			,mijlocRanduri, mijlocRanduri };
	int Y[] = { 0, mijlocColoane,0			  , mijlocColoane };

	int X1[] = { 0, 0			,mijlocRanduri, mijlocRanduri };
	int Y1[] = { 0, mijlocColoane,mijlocColoane,			0 };

	for (int i = 0; i < 4; i++)
	{
		Mat_<uchar> miniPoza = getMiniPoza(src, Point(X[i], Y[i]), latime, lungime);
		miniPoze.push_back(miniPoza);
	}



	int flags[4] = { 0,0,0,0 }; //vector cu flag-uri pentru pozele pe care le verificam

	flags[0] = 1; //poza aceasta nu va fi verificata, consideram ca o alegem pe prima


	float bestScore = 100000;
	float auxScore = 0.0;
	int indiceminiPoza = 0;
	int tipPotrivire = 1;
	int indexPunctStart = 0;
	int iminiPoza = 0, jminiPoza = 0;

	int directie1 = 1;
	Mat_ <uchar> pozaCrt = miniPoze[0];

	int directie2[3] = { 3, 0, 1 };
	int index = 0;

	//desenam prima poza
	for (int i = X1[indexPunctStart]; i < X1[indexPunctStart] + latime; i++)
	{
		jminiPoza = 0;
		for (int j = Y1[indexPunctStart]; j < Y1[indexPunctStart] + lungime; j++)
		{
			dst(i, j) = pozaCrt(iminiPoza, jminiPoza); //pentru ca minipoza e mai mica in dimensiune
			jminiPoza++;
		}
		iminiPoza++;
	}

	for (int i = 0; i < 3; i++) {
		printf("Potrivire pe poza %d:\n", i);
		Mat_ <uchar> poz1 = extractMat(pozaCrt, directie1);

		for (int j = 0; j < miniPoze.size(); j++)
		{
			if (flags[j] == 0)
			{
				auxScore = 999999;
				for (int direction = 0; direction < 4; direction++) {
					Mat_ <uchar> poz2 = extractMat(miniPoze[j], direction);

					printf("Pentru poza %d, directia %d: ", j, direction);
					//TODO 
					float aux = RMSE(poz1, poz2);

					if (aux < auxScore) {
						auxScore = aux;
					}

					printf("\n");
				}

				if (auxScore < bestScore)
				{
					bestScore = auxScore;
					printf("Best score: %f\n", bestScore);
					indiceminiPoza = j;
				}
			}
		}
		printf("\n\n\n");

		directie1++;
		index++;
		iminiPoza = 0;
		jminiPoza = 0;
		indexPunctStart++;
		pozaCrt = miniPoze[indiceminiPoza];


		//desenam prima poza
		for (int i = X1[indexPunctStart]; i < X1[indexPunctStart] + latime; i++)
		{
			jminiPoza = 0;
			for (int j = Y1[indexPunctStart]; j < Y1[indexPunctStart] + lungime; j++)
			{
				dst(i, j) = pozaCrt(iminiPoza, jminiPoza); //pentru ca minipoza e mai mica in dimensiune
				jminiPoza++;
			}
			iminiPoza++;
		}
		imshow("Poza pusa", dst);

		flags[indiceminiPoza] = 1; //am verificat poza asta
		bestScore = 100000;
		auxScore = 0.0;

		imshow("Imagine", miniPoze[indiceminiPoza]);
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
		scanf("%d", &op);
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
	} while (op != 0);
	return 0;
}// OpenCVApplication.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "common.h"

void testOpenImage()
{
	char fname[MAX_PATH];
	while (openFileDlg(fname))
	{
		Mat src;
		src = imread(fname);
		imshow("opened image", src);
		waitKey();
	}
}

void testOpenImagesFld()
{
	char folderName[MAX_PATH];
	if (openFolderDlg(folderName) == 0)
		return;
	char fname[MAX_PATH];
	FileGetter fg(folderName, "bmp");
	while (fg.getNextAbsFile(fname))
	{
		Mat src;
		src = imread(fname);
		imshow(fg.getFoundFileName(), src);
		if (waitKey() == 27)
			break;
	}
}

void testColor2Gray()
{
	char fname[MAX_PATH];
	while (openFileDlg(fname))
	{
		Mat_<Vec3b> src = imread(fname, IMREAD_COLOR);

		int height = src.rows;
		int width = src.cols;

		Mat_<uchar> dst(height, width);

		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				Vec3b v3 = src(i, j);
				uchar b = v3[0];
				uchar g = v3[1];
				uchar r = v3[2];
				dst(i, j) = (r + g + b) / 3;
			}
		}

		imshow("original image", src);
		imshow("gray image", dst);
		waitKey();
	}
}

Mat_ <uchar> getMiniPoza(Mat_<uchar> src, Point p, int latime, int lungime)
{
	Mat_ <uchar> dst(latime, lungime);
	int i2 = 0, j2 = 0;

	for (int i = p.x; i < p.x + latime; i++)
	{
		for (int j = p.y; j < p.y + lungime; j++)
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

Mat_<uchar> extractMat(Mat_<uchar> poza, int pozitie) {

	Mat_<uchar> extras;

	if (pozitie == 1 || pozitie == 3)
	{
		extras = Mat_<uchar>(3, poza.rows);
	}

	if (pozitie == 0 || pozitie == 2)
	{
		extras = Mat_<uchar>(3, poza.cols);
	}

	if (pozitie == 0)
	{
		for (int i = 0; i < 3; i++)
			for (int j = 0; j < poza.cols; j++)
				extras(i, j) = poza(i, j);
	}

	if (pozitie == 1)
	{
		int i2 = 0, j2 = 0;

		for (int i = poza.cols - 1; i >= poza.cols - 3; i--) {
			for (int j = 0; j < poza.rows; j++)
			{
				extras(i2, j2) = poza(j, i);
				j2++;
			}
			i2++;
			j2 = 0;
		}
	}

	if (pozitie == 2)
	{
		for (int i = 0; i < 3; i++)
			for (int j = 0; j < poza.cols; j++)
				extras(i, j) = poza(poza.rows - i - 1, j);
	}

	if (pozitie == 3)
	{
		int i2 = 0, j2 = 0;

		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < poza.rows; j++)
			{
				extras(i2, j2) = poza(j, i);
				j2++;
			}
			i2++;
			j2 = 0;
		}
	}

	return extras;
}

float RMSE(Mat_<uchar> poza1, Mat_<uchar> poza2)
{
	float sum = 0;
	float scor = 0;
	float pondere[3] = { 0.8, 0.15, 0.05 };

	if (poza1.cols != poza2.cols) {
		printf("Poza rotita gresit, L != l");
		return 999999;
	}

	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < poza1.cols; j++)
		{
			sum += (1.0f * (poza1(i, j) - poza2(i, j)) * (poza1(i, j) - poza2(i, j))) / poza1.cols * pondere[i];
		}
	}

	scor = sqrt(sum);
	printf("scor = %f;   ", scor);

	return scor;
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

	int X[] = { 0, 0			,mijlocRanduri, mijlocRanduri };
	int Y[] = { 0, mijlocColoane,0			  , mijlocColoane };

	int X1[] = { 0, 0			,mijlocRanduri, mijlocRanduri };
	int Y1[] = { 0, mijlocColoane,mijlocColoane,			0 };

	for (int i = 0; i < 4; i++)
	{
		Mat_<uchar> miniPoza = getMiniPoza(src, Point(X[i], Y[i]), latime, lungime);
		miniPoze.push_back(miniPoza);
	}



	int flags[4] = { 0,0,0,0 }; //vector cu flag-uri pentru pozele pe care le verificam

	flags[0] = 1; //poza aceasta nu va fi verificata, consideram ca o alegem pe prima


	float bestScore = 100000;
	float auxScore = 0.0;
	int indiceminiPoza = 0;
	int tipPotrivire = 1;
	int indexPunctStart = 0;
	int iminiPoza = 0, jminiPoza = 0;

	int directie1 = 1;
	Mat_ <uchar> pozaCrt = miniPoze[0];

	int directie2[3] = { 3, 0, 1 };
	int index = 0;

	//desenam prima poza
	for (int i = X1[indexPunctStart]; i < X1[indexPunctStart] + latime; i++)
	{
		jminiPoza = 0;
		for (int j = Y1[indexPunctStart]; j < Y1[indexPunctStart] + lungime; j++)
		{
			dst(i, j) = pozaCrt(iminiPoza, jminiPoza); //pentru ca minipoza e mai mica in dimensiune
			jminiPoza++;
		}
		iminiPoza++;
	}

	for (int i = 0; i < 3; i++) {
		printf("Potrivire pe poza %d:\n", i);
		Mat_ <uchar> poz1 = extractMat(pozaCrt, directie1);

		for (int j = 0; j < miniPoze.size(); j++)
		{
			if (flags[j] == 0)
			{
				auxScore = 999999;
				for (int direction = 0; direction < 4; direction++) {
					Mat_ <uchar> poz2 = extractMat(miniPoze[j], direction);

					printf("Pentru poza %d, directia %d: ", j, direction);
					//TODO 
					float aux = RMSE(poz1, poz2);

					if (aux < auxScore) {
						auxScore = aux;
					}

					printf("\n");
				}

				if (auxScore < bestScore)
				{
					bestScore = auxScore;
					printf("Best score: %f\n", bestScore);
					indiceminiPoza = j;
				}
			}
		}
		printf("\n\n\n");

		directie1++;
		index++;
		iminiPoza = 0;
		jminiPoza = 0;
		indexPunctStart++;
		pozaCrt = miniPoze[indiceminiPoza];


		//desenam prima poza
		for (int i = X1[indexPunctStart]; i < X1[indexPunctStart] + latime; i++)
		{
			jminiPoza = 0;
			for (int j = Y1[indexPunctStart]; j < Y1[indexPunctStart] + lungime; j++)
			{
				dst(i, j) = pozaCrt(iminiPoza, jminiPoza); //pentru ca minipoza e mai mica in dimensiune
				jminiPoza++;
			}
			iminiPoza++;
		}
		imshow("Poza pusa", dst);

		flags[indiceminiPoza] = 1; //am verificat poza asta
		bestScore = 100000;
		auxScore = 0.0;

		imshow("Imagine", miniPoze[indiceminiPoza]);
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
		scanf("%d", &op);
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
	} while (op != 0);
	return 0;
}