#include "lipa.h"
#include <iostream>
#include<fstream>


using namespace std;

//Declaring the main variables:
Image3CH oryg_img(600, 800);
Image3CH oryg_img_final(600, 800);
Image1CH gray_img(600, 800);
Image1CH bin_img(600, 800);
Image1CH testimage(600, 800);
Image1CH img_out(600, 800);
Image1CH img_in(600,800);

double pad[602][802];
double white_pad[602][802]{ 0 };
double black_pad[602][802]{ 0 };

double tabl_cz_seg[24];
double tabl_b_seg[4];

int licznik_karo = 0;



ofstream plik;




//LipaLib - Learning Image Processing Autonomic Library

// =========== RGB to GRAY FUNCTION EXAMPLE =================//
void rgbTogray(Image3CH& rgbImg, Image1CH& grayImg) // arguments with & because we want to save changes to images after performing funtion
{
	//Check if image sizes are equal
	if (rgbImg.width() == grayImg.width() && rgbImg.height() == grayImg.height())
	{
		for (int i = 0; i < rgbImg.width(); i++) //iteration on rows
			for (int j = 0; j < rgbImg.height(); j++) //iteration on the columns
			{
				grayImg(i, j).Intensity() = (rgbImg(i, j).Red() + rgbImg(i, j).Green() + rgbImg(i, j).Blue()) / 3; // I = (1/3)*(R+G+B) (i,j) - (number of row, number of column)
			}
	}
	else
	{
		std::cerr << "Image sizes are mismatched" << std::endl; //print an error
		return;
	}
}
// ==============================================//

//binarise function in use - Also used from the code specified above
void binarise() {


	for (int i = 0; i < (testimage.width() + 2); i++)
		for (int j = 0; j < (testimage.height() + 2); j++)
		{
			if ((pad[i][j]) > 0.835)
			{
				pad[i][j] = 1.0;
			}
			else
			{
				pad[i][j] = 0.0;
			}
		}
	for (int i = 0; i < (testimage.width()); i++) {
		for (int j = 0; j < (testimage.height()); j++) {
			testimage(i, j).Intensity() = pad[i][j];
		}
	}
}

//padding function in use 
void padding() {
	fill(pad[0], pad[0] + 602 * 802, 1); //padding with 1 and 0 will give different results at the edges but there will be no significant changes
	for (int i = 0; i < testimage.width(); i++) {
		for (int j = 0; j < testimage.height(); j++) {
			pad[i + 1][j + 1] = double(testimage(i, j).Intensity());
		}
	}
}

//Maximum Filter Function - this filter will find the maximum value of pixel in the sub matrix and will replacs the center element with that value, basically it will make the image look more sharper, i.e., high contrast
double maxfiltr(int i, int j) {
	double gret = pad[i][j];
	for (int a = 0; a < 3; a++) {
		for (int b = 0; b < 3; b++) {
			if ((pad[i + a][j + b]) > gret) {
				gret = pad[i + a][j + b];
			}
		}
	}
	return gret;
}

//Minimum Filter Function- this filter will find the minimum value of pixel in the sub matrix and will replace the center element with that value, basically it will make the image look smooth, i.e., less contrast
double minfiltr(int i, int j) {
	double les = pad[i][j];
	for (int a = 0; a < 3; a++) {
		for (int b = 0; b < 3; b++) {
			if ((pad[i + a][j + b]) < les) {
				les = pad[i + a][j + b];
			}
		}
	}
	return les;
}

//Median Filter Function - this filter will find the average value of all pixels in the sub matrix and will replace the center element with that value
double medianfiltr(int i, int j) {
	double sum = 0;
	for (int a = 0; a < 3; a++) {
		for (int b = 0; b < 3; b++) {
			sum += pad[i + a][j + b];
		}
	}
	sum = sum / 9;
	return sum;
}

//Convolution Function for  filters-the main function used; this function uses the predefined kernel to perform convolution operation on the padded image matrix
double convfiltr(int i, int j, double kernel[][3]) {
	double sum = 0;
	for (int a = 0; a < 3; a++) {
		for (int b = 0; b < 3; b++) {
			sum += ((pad[i + a][j + b]) * (kernel[a][b]));
		}
	}
	sum = sum / 9;
	return sum;
}

Image1CH dylatacja(Image1CH img_in, int wymiar)  { ////////////////////////////// DYLATACJA
	Image1CH img_out(600,800);
	for (int k = 0; k < 602; k++) {
		for (int l = 0; l < 802; l++)
			pad[k][l] = 0;
		
	}
	int n = (wymiar - 1) / 2;
	for (int a=0; a < (img_in.width() - wymiar); a++) {
		for (int b=0; b < (img_in.height() - wymiar); b++) {
			for (int x = 0; x < wymiar; x++) {
				for (int y = 0; y < wymiar; y++) {
					if (x == n && y == n)
						continue;
					if (img_in(a + x,b + y).Intensity() == 1) 
						pad[a + n][ b + n] = 1;
				}
			}
		}
	}
	for (int o = 0; o < 599; o++) {
		for (int p = 0; p < 799; p++) 
			img_out(o,p).Intensity() = pad[o][p];	
	}
	return img_out;
}

Image1CH erozja(Image1CH img_in, int wymiar) { //////////////////////////// EROZJA
	Image1CH img_out(600, 800);
	for (int k = 0; k < 602; k++) 
		for (int l = 0; l < 802; l++)
			pad[k][l] = 1;
	
	int n = (wymiar - 1) / 2;
	for (int a = 0; a < (img_in.width() - wymiar); a++) 
		for (int b = 0; b < (img_in.height() - wymiar); b++) 
			for (int x = 0; x < wymiar; x++) 
				for (int y = 0; y < wymiar; y++) {
					if (x == n && y == n)
						continue;
					if (img_in(a + x, b + y).Intensity() == 0)
						pad[a + n][b + n] = 0;
				}
				
	
	for (int o = 0; o < 599; o++) 
		for (int p = 0; p < 799; p++)
			img_out(o, p).Intensity() = pad[o][p];
	
	return img_out;
}

void segmentacja(Image1CH& img_in) { //////////////////////////// SEGMENTACJA
	double biale_licznik = 1;
	double czarne_licznik = 1;

	for (int k = 0; k < (img_in.width()- 1); k++) 
		for (int l = 0; l < (img_in.height() - 1); l++) 
			if (img_in(k, l).Intensity() == 1) {
				white_pad[k][l] = biale_licznik;
				black_pad[k][l] = 0;
				biale_licznik++;
			}
			else {
				white_pad[k][l] = 0;
				black_pad[k][l] = czarne_licznik;
				czarne_licznik++;
			}
		
	for (int a = 0; a < (img_in.width() - 1); a++) 
		for (int b = 0; b < (img_in.height() - 1); b++) 
			if (img_in(a, b).Intensity() != 0) {
				for (int x = -1; x < 2; x++)
					for (int y = -1; y < 2; y++)
						if (white_pad[a + x][b + y] > 0 && white_pad[a + x][b + y] < white_pad[a][b])
							white_pad[a][b] = white_pad[a + x][b + y];
			}
			else
				for (int x = -1; x < 2; x++)
					for (int y = -1; y < 2; y++)
						if (black_pad[a + x][b + y] > 0 && black_pad[a + x][b + y] < black_pad[a][b])
							black_pad[a][b] = black_pad[a + x][b + y];

	for (int a = (img_in.width() - 1); a > 1; a--)
		for (int b = (img_in.height() - 1); b > 1; b--)
			if (img_in(a, b).Intensity() != 0) {
				for (int x = -1; x < 2; x++)
					for (int y = -1; y < 2; y++)
						if (white_pad[a + x][b + y] > 0 && white_pad[a + x][b + y] < white_pad[a][b])
							white_pad[a][b] = white_pad[a + x][b + y];
			}
			else
				for (int x = -1; x < 2; x++)
					for (int y = -1; y < 2; y++)
						if (black_pad[a + x][b + y] > 0 && black_pad[a + x][b + y] < black_pad[a][b])
							black_pad[a][b] = black_pad[a + x][b + y];

	for (int a = 1; a < (img_in.width() - 1); a++)
		for (int b = img_in.height() - 1; b > 1; b--)
			if (img_in(a, b).Intensity() != 0) {
				for (int x = -2; x < 3; x++)
					for (int y = -2; y < 3; y++)
						if (white_pad[a + x][b + y] > 0 && white_pad[a + x][b + y] < white_pad[a][b])
							white_pad[a][b] = white_pad[a + x][b + y];
			}
			else
				for (int x = -2; x < 3; x++)
					for (int y = -2; y < 3; y++)
						if (black_pad[a + x][b + y] > 0 && black_pad[a + x][b + y] < black_pad[a][b])
							black_pad[a][b] = black_pad[a + x][b + y];

	

	

	
	for (int k = 0; k < (img_in.width() - 1); k++)
		for (int l = 0; l < (img_in.height() - 1); l++) {
			if (white_pad[k][l] == 1)
				white_pad[k][l] = 0;
			if (black_pad[k][l] == 1)
				black_pad[k][l] = 0;
		}
}

Image3CH polacz_srodki(Image3CH img_rgb, int karta1, int karta2) {  //////////////³aczenie srogków wskazanych kart
	
	
	/////////////////// wyznaczanie srodka karty

	int max_x = 0;
	int max_y =0;
	int min_x =0;
	int min_y =0;
	int x1 =0;
	int y1 = 0;
	int x2 = 0;
	int y2 = 0;
	int licznik_czarnych = 0;
	

	///////////////// wyznaczanie srodka karty
	for (int poz = 0; poz < 4; poz++) {

		for (int i = 0; i < 599; i++) { /////////////// wyznaczanie min_x1	
			for (int j = 0; j < 799; j++) {
				if (white_pad[i][j] == tabl_b_seg[poz]) {
					
					min_x = i;
					break;
				}
			}
			if (min_x > 0)
				break;
		}
		//cout << "tabl_b_seg[poz] " << tabl_b_seg[poz] << endl;
	
		for (int i = 599; i > 0; i--) {  /////////////// wyznaczanie max_x	1
			for (int j = 799; j > 0; j--) {
				if (white_pad[i][j] == tabl_b_seg[poz]) {
					max_x = i;
					break;
				}
			}
			if (max_x > 0)
				break;
		}

		for (int j = 0; j < 799; j++) { /////////////// wyznaczanie min_y	1
			for (int i = 0; i < 599; i++) {
				if (white_pad[i][j] == tabl_b_seg[poz]) {
					min_y = j;
					break;
				}
			}
			if (min_y > 0)
				break;

		}

		for (int j = 799; j > 0; j--) { /////////////// wyznaczanie max_y	1
			for (int i = 599; i > 0; i--) {
				if (white_pad[i][j] == tabl_b_seg[poz]) {
					max_y = j;
					break;
				}
			}
			if (max_y > 0)
				break;
		}

		//cout << max_x << " " << max_y << " " << min_y << " " << min_x << endl;

		


		///////////// liczenie czarnych elementów na wycinku ka¿dej karty
		int flaga = 0;
		int flaga_karo = 0;
		double temp_tabl_cz_seg[11];
		for (int k = 0; k < 11; k++)
			temp_tabl_cz_seg[k] = -1;
		for (int i = min_x; i < max_x; i++) {
			for (int j = min_y; j < max_y; j++) {
				if (black_pad[i][j] == 0)
					continue;
				for (int k = 0; k <= licznik_czarnych; k++)
					if (temp_tabl_cz_seg[k] != black_pad[i][j])
						flaga = 0;
					else {
						flaga = 1;
						break;
					}
				if (flaga) {
					flaga = 0;
					continue;
				}
				else {
					temp_tabl_cz_seg[licznik_czarnych] = black_pad[i][j];
					licznik_czarnych++;
				}

			}
		}
		//cout << "licznik czarnych po cyklu:" << licznik_czarnych << endl;
		if (licznik_czarnych == 3 || licznik_czarnych == 2) //////////////////////////////// Zliczanie karo
			licznik_karo += licznik_czarnych;
		
		if (licznik_czarnych == karta1) {
			x1 = (max_x + min_x) / 2;
			y1 = (max_y + min_y) / 2;

		}
		else if (licznik_czarnych == karta2) {
			x2 = (max_x + min_x) / 2;
			y2 = (max_y + min_y) / 2;
		}
		licznik_czarnych =0;
		max_x = 0;
		max_y = 0;
		min_x = 0;
		min_y = 0;
		
	}
		

		img_rgb.DrawLine(x1, y1, x2, y2, 1,0,0);
		img_rgb.DrawPoint(x1, y1, 0, 1, 0);
		img_rgb.DrawPoint(x2, y2, 0, 1, 0);
		
		return img_rgb;
	
	
}

Image3CH rysuj_linie(Image3CH img_rgb, int karta, float& g_a, float& g_b) {////////////// RYSOWANIE LINII przecinaj¹cej figury karty



	int max_x = 0;
	int max_y = 0;
	int min_x = 0;
	int min_y = 0;
	

	int x_el1 = 0;
	int y_el1 = 0;
	int x_el2 = 0;
	int y_el2 = 0;

	int max_x_el[2] = { 0 };
	int max_y_el[2] = { 0 };
	int min_x_el[2] = { 0 };
	int min_y_el[2] = { 0 };
	
	int licznik_czarnych = 0;
	double temp_tabl_cz_seg[11];


	///////////////// wyznaczanie srodka karty
	for (int poz = 0; poz < 4; poz++) {

		for (int i = 0; i < 599; i++) { /////////////// wyznaczanie min_x1	
			for (int j = 0; j < 799; j++) {
				if (white_pad[i][j] == tabl_b_seg[poz]) {

					min_x = i;
					break;
				}
			}
			if (min_x > 0)
				break;
		}
		//cout << "tabl_cz_seg[poz] " << tabl_b_seg[poz] << endl;

		for (int i = 599; i > 0; i--) {  /////////////// wyznaczanie max_x	1
			for (int j = 799; j > 0; j--) {
				if (white_pad[i][j] == tabl_b_seg[poz]) {
					max_x = i;
					break;
				}
			}
			if (max_x > 0)
				break;
		}

		for (int j = 0; j < 799; j++) { /////////////// wyznaczanie min_y	1
			for (int i = 0; i < 599; i++) {
				if (white_pad[i][j] == tabl_b_seg[poz]) {
					min_y = j;
					break;
				}
			}
			if (min_y > 0)
				break;

		}

		for (int j = 799; j > 0; j--) { /////////////// wyznaczanie max_y	1
			for (int i = 599; i > 0; i--) {
				if (white_pad[i][j] == tabl_b_seg[poz]) {
					max_y = j;
					break;
				}
			}
			if (max_y > 0)
				break;
		}

		//cout << max_x << " " << max_y << " " << min_y << " " << min_x << endl;




		///////////// liczenie czarnych elementów na wycinku ka¿dej karty
		int flaga = 0;
		
		for (int k = 0; k < 11; k++)
			temp_tabl_cz_seg[k] = -1;
		for (int i = min_x; i < max_x; i++) {
			for (int j = min_y; j < max_y; j++) {
				if (black_pad[i][j] == 0)
					continue;
				for (int k = 0; k <= licznik_czarnych; k++)
					if (temp_tabl_cz_seg[k] != black_pad[i][j])
						flaga = 0;
					else {
						flaga = 1;
						break;
					}
				if (flaga) {
					flaga = 0;
					continue;
				}
				else {
					temp_tabl_cz_seg[licznik_czarnych] = black_pad[i][j];
					licznik_czarnych++;
				}

			}

		}
		//cout << "licznik czarnych po cyklu:" << licznik_czarnych << endl;


		


		for (int k = 0; k < 10; k++)//sprawdzenie
			//cout <<"temp_tablczseg"<< temp_tabl_cz_seg[k]<<endl;


		if (licznik_czarnych == karta) { ////////////////////////////////////////// wyznaczanie srodka elemntow czarnych na karcie
			for (int p = 0; p < 2; p++) {
				for (int i = 0; i < 599; i++) { /////////////// wyznaczanie min_x1	
					for (int j = 0; j <799; j++) {
						if (black_pad[i][j] == temp_tabl_cz_seg[p]) {

							min_x_el[p] = i;
							break;
						}
					}
					if (min_x_el[p] > 0)
						break;
				}
				//cout << "_temp_tabl_cz_seg[p] " << temp_tabl_cz_seg[p] << endl;

				for (int i = 599; i > 0; i--) {  /////////////// wyznaczanie max_x	1
					for (int j =799; j > 0; j--) {
						if (black_pad[i][j] == temp_tabl_cz_seg[p]) {
							max_x_el[p] = i;
							break;
						}
					}
					if (max_x_el[p] > 0)
						break;
				}

				for (int j =0; j <799; j++) { /////////////// wyznaczanie min_y	1
					for (int i = 0; i <599; i++) {
						if (black_pad[i][j] == temp_tabl_cz_seg[p]) {
							min_y_el[p] = j;
							break;
						}
					}
					if (min_y_el[p] > 0)
						break;

				}

				for (int j = 799; j > 0; j--) { /////////////// wyznaczanie max_y	1
					for (int i = 599; i > 0; i--) {
						if (black_pad[i][j] == temp_tabl_cz_seg[p]) {
							max_y_el[p] = j;
							break;
						}
					}
					if (max_y_el[p] > 0)
						break;
					
				}
				//cout << max_x_el[p] << " " << max_y_el[p] << " " << min_y_el[p] << " " << min_x_el[p] << endl;
			}
			break;

		}
		
		licznik_czarnych = 0;
		max_x = 0;
		max_y = 0;
		min_x = 0;
		min_y = 0;
		

	}

		x_el1 = (max_x_el[0] + min_x_el[0]) / 2;
		y_el1 = (max_y_el[0] + min_y_el[0]) / 2;
		x_el2 = (max_x_el[1] + min_x_el[1]) / 2;
		y_el2 = (max_y_el[1] + min_y_el[1]) / 2;
		//cout <<"WSP CZARNYCH EL"<< x_el1 << endl << y_el1 << endl << x_el2 << endl << y_el2;
		

		///////////////////////////////////////////////////////////////wyznaczenie prostej przez ca³y obraz

		//////wyznaczanie wspolczynnika 'a'
		float a;
		float delta_y = 0;
		float delta_x = 0;

		delta_y = y_el2 - y_el1;
		delta_x = x_el2 - x_el1;
		
		a = delta_y / delta_x;
		
		//// wyznaczenie wspolczynnika 'b'

		float b;

		b = y_el1 - (a * x_el1);
		//cout << "delt_x" << delta_x << " delta_y" << delta_y << endl << "a" << a << endl<<"b"<<b<<endl;
	
		//// wyznacanie pkt skranych X1 Y1 X2 Y2
		int X1, Y1, X2, Y2;

		if ((-b/a) > 599) {
			X1 = 599;
			Y1 = a * X1 + b;
		}
		else {
			X1 = -b / a;
			Y1 = X1 * a + b;
		}

		if ( b > 799) {
			Y2 = 799;
			X2 = (Y2-b)/a;
		}
		else {
			Y2 = b;
			X2 = (Y2 - b) / a;
		}
		img_rgb.DrawLine(X1, Y1, X2, Y2, 0, 0, 1);


		g_a = a;
		g_b = b;

		return img_rgb;
}

void wyznacz_przeciecie(float g_a1, float g_b1, float g_a2, float g_b2, int& X, int& Y) {
	
	X = (g_b2 - g_b1) / (g_a1 - g_a2);
	Y = g_a1 * X + g_b1;
	//cout << X << "  " << Y << endl;

}

// main function or entry point of the program, from where the input will be taken from the user and respective output will be shown, the effect on the image will be visible 
int main()
	{
	int wybor = 1;
	
	while (wybor) {
		cout << "Prosze podac numer obrazu do przerobienia" << endl << "1 - obraz oryginalny," << endl << "2 - obraz blur," << endl << "3 - obraz salt and pepper." << endl;
		cin >> wybor;
		switch (wybor) {
		case 1:
			cout << endl << "Wybrano obraz oryginalny" << endl;
			oryg_img.LoadImage("img\\004.jpg");
			wybor = 0;
			break;
		case 2:
			cout << endl << "Wybrano obraz Blur" << endl;
			oryg_img.LoadImage("img\\004_blur.jpg");
			wybor = 0;

			break;
		case 3:
			cout << endl << "Wybrano obraz Salt&pepper" << endl;
			oryg_img.LoadImage("img\\004_salt_pepper.jpg");
			wybor = 0;

			break;
		default:
			cout << endl << "Sprobuj jeszcze raz...	" << endl<<endl;
			break;
		}
	}
	

	rgbTogray(oryg_img, gray_img);

	testimage = gray_img;
	padding();

	/////////////////// FILTR MEDIANOWY
	for (int k = 0; k < 3; k++) {
		for (int i = 0; i < (testimage.width()); i++) {
			for (int j = 0; j < (testimage.height()); j++) {
				testimage(i, j).Intensity() = medianfiltr(i, j);
			}
		}
	}
	testimage.ShowImage("GRAY");
	binarise();
	testimage.ShowImage("BIN");
	bin_img = testimage;

	for (int k = 0; k < 10; k++) {
		testimage = erozja(testimage, 3);
		testimage = dylatacja(testimage, 3);
	}

	testimage = dylatacja(testimage, 3);
	testimage = erozja(testimage, 3);
	testimage = dylatacja(testimage, 3);
	testimage = dylatacja(testimage, 3);
	testimage = dylatacja(testimage, 3);

	///////////////////// FILTR MEDIANOWY
	for (int k = 0; k < 3; k++) {
		for (int i = 0; i < (testimage.width()); i++) {
			for (int j = 0; j < (testimage.height()); j++) {
				testimage(i, j).Intensity() = medianfiltr(i, j);
			}
		}
	}
	testimage = erozja(testimage, 7);
	testimage = erozja(testimage, 5);
	testimage = dylatacja(testimage, 3);
	testimage = dylatacja(testimage, 3);
	testimage = dylatacja(testimage, 3);
	testimage = dylatacja(testimage, 3);
	testimage = dylatacja(testimage, 3);
	testimage = dylatacja(testimage, 3);
	testimage = erozja(testimage, 7);
	testimage = erozja(testimage, 3);

	testimage.ShowImage("DILATING & ERODING");

	
	//////////////////////////////////////////// SEGMENTACJA
	segmentacja(testimage);
	plik.open("white_pad2.txt");
	
	for (int i = 0; i < 600; i++) {
		for (int j = 0; j < 800; j++) {
			plik << i << "		" << j << "		" << white_pad[i][j]<<endl;
		}
	}
	plik.close();

	plik.open("black_pad2.txt");

	for (int i = 0; i < 600; i++) {
		for (int j = 0; j < 800; j++) {
			plik <<i<<"		"<<j<<"		"<< black_pad[i][j] << endl;
		}
	}
	plik.close();

	
	
	//////////////////////////////////////////// katalogowanie segmentow do ich tablic.

	int licznik_czarnych=0;
	int licznik_bialych = 0;
	int flaga = 0;
	double wartosc=0;
	
	for (int k = 0; k < 24; k++)
		tabl_cz_seg[k] = -1;
	for (int i = 0; i < 599; i++)
		for (int j = 0; j < 799; j++) {
			if (black_pad[i][j] == 0)
				continue;
			for (int k = 0; k <= licznik_czarnych; k++)
				if (tabl_cz_seg[k] != black_pad[i][j])
					flaga = 0;
				else {
					flaga = 1;
					break;
				}
			if (flaga) {
				flaga = 0;
				continue;
			}
			else {
				tabl_cz_seg[licznik_czarnych] = black_pad[i][j];
				licznik_czarnych++;
			}

		}
	for (int k = 0; k < 4; k++)
		tabl_b_seg[k] = -1;
	for (int i = 0; i < 599; i++)
		for (int j = 0; j < 799; j++) {
			if (white_pad[i][j] == 0)
				continue;
			for (int k = 0; k <= licznik_bialych; k++)
				if (tabl_b_seg[k] != white_pad[i][j])
					flaga = 0;
				else {
					flaga = 1;
					break;
				}
			if (flaga) {
				flaga = 0;
				continue;
			}
			else {
				tabl_b_seg[licznik_bialych] = white_pad[i][j];
				licznik_bialych++;
			}

		}
				
	 oryg_img_final= polacz_srodki(oryg_img, 2,9); ///////////////////////////// RYSOWANIE LINII MIDZY SRODKAMI KART NA ORYINALNYM
	 
	 float g_a1, g_a2, g_b1, g_b2; //// wspo³czynniki rownan prostych 

	 oryg_img_final = rysuj_linie(oryg_img_final, 3,g_a1,g_b1);
	 oryg_img_final = rysuj_linie(oryg_img_final, 2, g_a2, g_b2);
	 

	 //cout<<endl << g_a1 << " " << g_b1 << " " << g_a2 << " " << g_b2 << endl;

	 int X, Y;//////// wspolrzedne pkt pzeciecia

	 wyznacz_przeciecie(g_a1, g_b1, g_a2, g_b2, X, Y);
	 
	 oryg_img_final.DrawPoint(X,Y, 0, 1, 0);
	 oryg_img_final.ShowImage("FINAL");
	 //cout << X << "  " << Y << endl;
	
	 oryg_img_final.SaveImage("koniec.jpg");
	 cout << "Ilosc symboli karo  - " << licznik_karo << endl;
	
	system("pause");
	return 0;
} // The end.
