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
		imwrite("C:\\Users\\allex\\OneDrive\\Desktop\\School\\An3_recontractare\\PI\\OpenCVApplication-VS2019_OCV451_basic\\Images\\prof.bmp", dst);
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

	//imshow("mini", dst);
	//waitKey(0);

	return dst;
}

/// Directie 1 -> dreapta
/// Directie 2 -> jos
/// Directie 3 -> stanga
/// Directie 4 -> sus
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
	float pondere[3] = { 0.95, 0.03, 0.02 };

	if (poza1.cols != poza2.cols) {
		//printf("Poza rotita gresit, L != l");
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

	int divizor = 0;
	printf("In cate imagini vreti sa impartim?\n Divizor = ");
	scanf("%d", &divizor);


	int rotiri = 0;
	printf("Rotiri poze?\n rotire = ");
	scanf("%d", &rotiri);

	int latime = src.rows / divizor;
	int lungime = src.cols / divizor;

	


	std::vector<Point> cordonate;

	for (int i = 0; i < divizor; i++) {
		for (int j = 0; j < divizor; j++) {
			cordonate.push_back(Point(i * latime, j * lungime));
		}
	}

	std::vector<Point> cordonateDesen;
	for (int i = 0; i < divizor; i++) {
		if (i % 2 == 0) {
			for (int j = 0; j < divizor; j++) {
				cordonateDesen.push_back(Point(i * latime, j * lungime));
			}
		}

		if (i % 2 == 1) {
			for (int j = divizor - 1; j >= 0; j--) {
				cordonateDesen.push_back(Point(i * latime, j * lungime));
			}
		}
	}

	for (int i = 0; i < divizor * divizor; i++)
	{
		Mat_<uchar> miniPoza = getMiniPoza(src, cordonate[i], latime, lungime);
		miniPoze.push_back(miniPoza);
	}
	
	int* flags = (int*)calloc(divizor * divizor, sizeof(int));	//vector cu flag-uri pentru pozele pe care le verificam
	flags[0] = 1;												//poza aceasta nu va fi verificata, consideram ca o alegem pe prima

	float bestScore = 100000;
	float auxScore = 0.0;
	int indiceminiPoza = 0;
	int tipPotrivire = 1;
	int indexPunctStart = 0;
	int iminiPoza = 0, jminiPoza = 0;

	int pozeAjutor[100] = {};//indicii pozelor de deasupra pozei cautate
	pozeAjutor[0] = 0;


	int directie1 = 0;
	Mat_ <uchar> pozaCrt = miniPoze[0];

	//desenam prima poza
	for (int i = cordonate[0].x; i < cordonate[0].x + latime; i++)
	{
		jminiPoza = 0;
		for (int j = cordonate[0].y; j < cordonate[0].y + lungime; j++)
		{
			dst(i, j) = pozaCrt(iminiPoza, jminiPoza);
			jminiPoza++;
		}
		iminiPoza++;
	}

	int pozaIn = 0; //index inserare pentru vectorul de poze-sus 
	int pozaOut = 0;//index scoatere pentru vecorul de poze-sus

	
	//Pentru fiecare poza
	for (int i = 0; i < divizor; i++) {
		
		if (i % 2 == 0) directie1 = 1;//pe linii pare se merge spre dreapta
		
		if (i % 2 != 0) directie1 = 3;//pe linii impare se merge spre stanga

		for (int k = 0; k < divizor; k++) {

			if (i % 2 == 0)		//calcul index de unde se scoate poza de deasupra
			{
				if (k < divizor - 1)
				{		
					pozaOut = k + 1;
				}

				else pozaOut = divizor - 1;

			}

			else {
				if (k < divizor - 1)
				{
					pozaIn = divizor - k - 2;
					pozaOut = divizor - k - 2;
				}
				else pozaOut = 0;


			}
			

			if (k == divizor - 1) directie1 = 2;
			if (i == divizor - 1 && k == divizor - 1) continue; // ultima imagine, nu mai potrivim pe ea

			printf("Potrivire pe poza %d:\n", indiceminiPoza);
			Mat_ <uchar> poz1 = extractMat(pozaCrt, directie1);

			for (int j = 0; j < miniPoze.size(); j++)
			{
				if (flags[j] == 1) continue;
				
				auxScore = 999999;
				if (rotiri == 1)//se verifica toate laturile pozei 2
				{
					for (int direction = 0; direction < 4; direction++) {
						Mat_ <uchar> poz2 = extractMat(miniPoze[j], direction);
						Mat_ <uchar> poz22 = extractMat(miniPoze[j], 0);

						printf("Pentru poza %d, directia %d: ", j, direction);
						float aux = RMSE(poz1, poz2);


						if (i > 0)//prima linie nu are poze deasupra
						{
							Mat_<uchar> pozaDeAjutor = extractMat(miniPoze[pozeAjutor[pozaOut]], 2);
							//printf("ajutor de la  %d = %d\n", pozaOut, pozeAjutor[pozaOut]);
							//imshow("Ajutor", miniPoze[pozeAjutor[pozaOut]]);
							//waitKey(0);
							float scorDeAjutor = RMSE(poz22, pozaDeAjutor);
							aux += scorDeAjutor;
						}

						if (aux < auxScore) {
							auxScore = aux;
						}

						printf("\n");
					}
				}
				else //se verifica doar laturile adiacente
				{	
					int direction2; //din ce parte extragem culoril la poza 2
					if (i % 2 == 0 && k < divizor - 1) direction2 = 3;
					if (i % 2 == 0 && k == divizor - 1) direction2 = 0;
					if (i % 2 == 1 && k < divizor - 1) direction2 = 1;
					if (i % 2 == 1 && k == divizor - 1) direction2 = 0;


					
					Mat_ <uchar> poz2 = extractMat(miniPoze[j], direction2);
					Mat_ <uchar> pozaCandidat = extractMat(miniPoze[j], 0);//poza pentru care calculam scorul

					printf("Pentru poza %d, directia %d:   ", j, direction2);
					float aux = RMSE(poz1, poz2);


					if (i > 0) //prima linie nu are poze deasupra
					{
						Mat_<uchar> pozaDeAjutor = extractMat(miniPoze[pozeAjutor[pozaOut]], 2);
						//printf("ajutor de la  %d = %d\n", pozaOut, pozeAjutor[pozaOut]);
						//imshow("Ajutor", miniPoze[pozeAjutor[pozaOut]]);
						//waitKey(0);
						float scorDeAjutor = RMSE(pozaCandidat, pozaDeAjutor);
						aux += scorDeAjutor;
					}

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
			printf("\n\n");

			iminiPoza = 0;
			jminiPoza = 0;
			indexPunctStart++;
			pozaCrt = miniPoze[indiceminiPoza];


			//Desenam poza potrivita
			for (int l = cordonateDesen[indexPunctStart].x; l < cordonateDesen[indexPunctStart].x + latime; l++)
			{
				jminiPoza = 0;
				for (int m = cordonateDesen[indexPunctStart].y; m < cordonateDesen[indexPunctStart].y + lungime; m++)
				{
					dst(l, m) = pozaCrt(iminiPoza, jminiPoza);
					jminiPoza++;
				}
				iminiPoza++;
			}
			flags[indiceminiPoza] = 1; //am verificat poza asta
			bestScore = 100000;
			
			imshow("MiniPoza adaugata", dst);
			waitKey();

			if (i % 2 == 0 )//pe randuri pare de la stanga la dreapta
			{	
				if (k < divizor - 1)
				{
					pozaIn = k + 1;
					pozaOut = k + 1;
				}

				else pozaOut = divizor - 1;

			
			}

			else { //pe randuri impare de la dreapta la stanga
				if (k < divizor - 1)
				{
					pozaIn = divizor - k - 2;
					pozaOut = divizor - k - 2;
				}
				else pozaOut = 0;
			
				
			}
		

			pozeAjutor[pozaIn] = indiceminiPoza;


			for (int i2 = 0; i2 < 10; i2++)
			{


				//printf("%d ", pozeAjutor[i2]);


			}
			
			
		}
	}
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