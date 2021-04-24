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
		//modificare in cod
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
		if (waitKey()==27) //ESC pressed
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

int RMSE(Mat_<uchar> poza1, Mat_<uchar> poza2, int pozitieImg1, int pozitieImg2)
{
	float sum = 0;
	float scor = 0;
	float bestScore = 999999;
	int pozitieDePotrivire = 0; 
	
	//if (pozitieImg1 == 0 && pozitieImg2 == 2)
	//	return -1;

	//TEST SUS-JOS
	//if (!(pozitieImg1 == 0 && pozitieImg2 == 1) && !(pozitieImg1 == 0 && pozitieImg2 == 3))
	//{
		for (int i = 0; i < poza1.cols; i++)
		{
			sum += (1.0f * (poza1(0, i) - poza2(poza2.rows - 1, i)) * (poza1(0, i) - poza2(poza2.rows - 1, i))) / poza1.cols;
			//printf("\n culoare1(0,%d)=%d culoare2(%d,%d)=%d ", i, poza1(0, i), poza2.rows - 1,i, poza2(poza2.rows - 1, i));
		}
		scor = sqrt(sum);
		printf("\nscor potrivire (caz 0) ==> img2 peste img1 = %f", scor);
		bestScore = scor;
		
	//}
	sum = 0;

	//TEST DREAPTA STANGA
	//if ((pozitieImg1 == 0 && pozitieImg2 == 1) || !(pozitieImg1 == 0 && pozitieImg2 == 3))
	//{
		for (int i = 0; i < poza1.rows; i++)
		{
			sum += (1.0f * (poza1(i, poza1.cols - 1) - poza2(i, 0)) * (poza1(i, poza1.cols - 1) - poza2(i, 0))) / poza1.rows;
			//printf("\n culoare1(%d,%d)=%d culoare2(%d,0)=%d ",i, poza1.cols - 1, poza1(i, poza1.cols - 1),i, poza2(i, 0));
		}
		scor = sqrt(sum);
		printf("\nscor potrivire (caz 1) ==> img2 in dreapta lui img1 = %f", scor);
		if (scor < bestScore)
		{
			bestScore = scor;
			pozitieDePotrivire = 1;
		}
	//}
	sum = 0;
	//TEST JOS-SUS
	//if (!(pozitieImg1 == 0 && pozitieImg2 == 1) || (pozitieImg1 == 0 && pozitieImg2 == 3))
	//{
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
	//}
	sum = 0;

	//TEST STANGA-DREAPTA
	//if (!(pozitieImg1 == 0 && pozitieImg2 == 1) && !(pozitieImg1 == 0 && pozitieImg2 == 3))
	//{
		for (int i = 0; i < poza1.rows; i++)
		{
			sum += (1.0f * (poza1(i, 0) - poza2(i, poza2.cols - 1)) * (poza1(i, 0) - poza2(i, poza2.cols - 1))) / poza1.rows;
			//printf("\n culoare1(%d,%d)=%d culoare2(%d,0)=%d ", i, poza1.cols - 1, poza1(i, poza1.cols - 1), i, poza2(i, 0));
		}
		scor = sqrt(sum);
		printf("\nscor potrivire (caz 3) ==> img2 in stanga lui img1 = %f", scor);
		if (scor < bestScore)
		{
			bestScore = scor;
			pozitieDePotrivire = 3;
		}
	//}


	return pozitieDePotrivire;
	// 0 => p1-jos p2-sus
	// 1 => p1-stanga p2-dreapta
	// 2 => p1-sus p2-jos
	// 3 => p1-dreapta p2-stanga
}

void impartireImg()
{
	char fname[MAX_PATH];
	openFileDlg(fname);
	Mat_<uchar> src = imread(fname, IMREAD_GRAYSCALE);

	std::vector<Mat_<uchar>> miniPoze;

	int mijlocRanduri = src.rows / 2;
	int mijlocColoane = src.cols / 2;
	int latime = mijlocRanduri;
	int lungime = mijlocColoane;
	
	int X[] = { 0, 0			,mijlocRanduri, mijlocRanduri};
	int Y[] = { 0, mijlocColoane,0			  , mijlocColoane };

	//printf("mijloc randuri = %d mijloc col = %d latime = %d lungime = %d", mijlocRanduri, mijlocColoane, latime, lungime);

	//impartire 2x2
	for (int i = 0; i < 4; i++)
	{
		Mat_<uchar> miniPoza = getMiniPoza(src, Point(X[i], Y[i]), latime, lungime);
		miniPoze.push_back(miniPoza);
	}

	//afisare mini poze
	for (int i = 0; i < miniPoze.size(); i++)
	{
		//imshow("miniPoza", miniPoze[i]);
		//waitKey(0);
	}
	Mat_ <uchar> miniPoza0 = miniPoze[0];
	Mat_ <uchar> miniPoza1 = miniPoze[1];
	Mat_ <uchar> miniPoza2 = miniPoze[2];
	Mat_ <uchar> miniPoza3 = miniPoze[3];


	//teste
	int pozitie;

	pozitie = RMSE(miniPoza0, miniPoza1, 10, 20);
	printf("\n poze(0,1) => pozitie = %d\n", pozitie);

	pozitie = RMSE(miniPoza0, miniPoza2, 10, 20);
	printf("\n poze(0,2) => pozitie = %d\n", pozitie);

	pozitie = RMSE(miniPoza1, miniPoza0, 10, 20);
	printf("\n poze(1,0) => pozitie = %d\n", pozitie);

	pozitie = RMSE(miniPoza1, miniPoza3, 10, 20);
	printf("\n poze(1,3) => pozitie = %d\n", pozitie);

	pozitie = RMSE(miniPoza2, miniPoza0, 10, 20);
	printf("\n poze(2,0) => pozitie = %d\n", pozitie);

	pozitie = RMSE(miniPoza2, miniPoza3, 10, 20);
	printf("\n poze(2,3) => pozitie = %d\n", pozitie);

	pozitie = RMSE(miniPoza3, miniPoza1, 10, 20);
	printf("\n poze(3,1) => pozitie = %d\n", pozitie);

	pozitie = RMSE(miniPoza3, miniPoza2, 10, 20);
	printf("\n poze(3,2) => pozitie = %d\n", pozitie);


	imshow("src", src);
	waitKey(0);

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
