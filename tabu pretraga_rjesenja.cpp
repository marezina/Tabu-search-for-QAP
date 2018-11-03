#include <iostream>
#include <cstdlib>
#include <fstream>
#include <ctime>

#define velicinaTabuListe 50
#define MAX 256

using namespace std; 

struct rjesenje{
	int permutacija[MAX];
	int iznos;
};

int d[MAX][MAX], f[MAX][MAX];
int red=0, stupac=0;
int velicinaProblema;
int indikator=0;

int brojIteracija;

rjesenje trenutnoRjesenje; 
rjesenje najboljeRjesenje;
rjesenje tabu[velicinaTabuListe];

int ind;
long najmanji;

void ispuniMatricu(char polje[], int prviRed){
	int brojInteger;
	string broj;
	string str(polje);
	for(int i=0; i<str.length(); i++){
		if(str[i]!=' ' && str[i]!='\0'){
			broj += str[i];
		}
		if((str[i+1]==' ' && broj!="") || (str[i+1]=='\0' && broj!="")){
			brojInteger=atoi(broj.c_str());
			if(prviRed==1)velicinaProblema=brojInteger;
			if(red==velicinaProblema){
				indikator=1;
				red=0;
				stupac=0;
			}
			if(prviRed==0 && indikator==0){
				d[red][stupac++]=brojInteger;
				if(stupac==velicinaProblema){
					red++;
					stupac=0;
				}
			}
			if(prviRed==0 && indikator==1){
				f[red][stupac++]=brojInteger;
				if(stupac==velicinaProblema){
					red++;
					stupac=0;
				}
			}
			broj="";
		}
	}
}

void ispisiMatricu(int matrica[MAX][MAX]){
	for(int i=0; i<velicinaProblema; i++){
		for(int j=0; j<velicinaProblema; j++){
			cout << matrica[i][j] << " ";
		}
		cout << endl;
	}
}

void ispisRjesenja(rjesenje polje){
	cout << "(";
	for(int i=0; i<velicinaProblema+1; i++){
		if(i<velicinaProblema)cout << polje.permutacija[i];
		if(i<velicinaProblema-1)cout << ", ";
		if(i>velicinaProblema-1)cout << ")" << endl << "Sum: " << polje.iznos << endl;
	}
}

void citajDatoteku(char nazivDatoteke[]){
	char red[200];
	fstream data;
	data.open(nazivDatoteke, ios::in);
	if(!data){
		cout << "Datoteka ne postoji!";
		return;	
	}
	data.getline(red, sizeof(red), '\n');
	ispuniMatricu(red, 1);
	while(1){
		data.getline(red, sizeof(red), '\n');
		ispuniMatricu(red, 0);
		if(data.eof())break;
	}
	data.close();
}

bool brojJeUPocetnomRjesenju(int broj){
	for(int i=0; i<velicinaProblema; i++)
		if(trenutnoRjesenje.permutacija[i]==broj)
			return true;
	return false;
}

void generirajPocetnoRjesenje(){
	int broj;
	for(int i=0; i<velicinaProblema; i++){
		bool izlazak=false;
		while(1){
			broj=rand()%velicinaProblema+1;
			if(!brojJeUPocetnomRjesenju(broj)){
				trenutnoRjesenje.permutacija[i]=broj;
				izlazak=true;
			}
			if(izlazak)break;
		}
	}
}

void inicirajTabuListu(){
	for(int i=0; i<velicinaTabuListe; i++){
		tabu[i].iznos=-1;
		for(int j=0; j<velicinaProblema; j++)
			tabu[i].permutacija[j]=-1;
	}
}

bool isTabu(rjesenje x){
	bool provjera = false;
	for(int i=0; i<velicinaTabuListe; i++){
		provjera = true;
		for(int j=0; j<velicinaProblema; j++){
			if(tabu[i].permutacija[j]!=x.permutacija[j]){
				provjera = false;
				break;
			}
		}
		if(provjera==true)return true;
	}
	return provjera;
}

void ubaciUTabuListu(rjesenje a){
	for(int i=velicinaTabuListe-1; i>=1; i--){
		memcpy(&tabu[i], &tabu[i-1], sizeof(tabu[i-1]));
	}
	memcpy(&tabu[0], &a, sizeof(a));
}

int funkcijaCilja(int rjesenje[]){
	int sum=0;
	for(int i=0; i<velicinaProblema; i++){
		int a, b;
		a=rjesenje[i]-1;
		for(int j=0; j<velicinaProblema; j++){
			b=rjesenje[j]-1;
			if(f[a][b]>0){
				sum += d[i][j]*f[a][b];
			}
		}
	}
	return sum;
}

int generirajSusjedstvo(){
	rjesenje susjedstvo[velicinaProblema];
	najmanji=999999999;
	int indeks=0;
	int pivot = rand()%velicinaProblema;
	for(int i=0; i<velicinaProblema; i++){
		//generiranje susjedstva
		/*
		//Generiranje susjedstva zamjenom dva susjedna elementa iz trenutnog rjesenja
		for(int j=0; j<velicinaProblema; j++){
			if(indeks==j && j<velicinaProblema-1 && i!=0){
				susjedstvo[i].permutacija[j]=trenutnoRjesenje.permutacija[j+1];
				susjedstvo[i].permutacija[j+1]=trenutnoRjesenje.permutacija[j];
				j++;
			}
			else{
				susjedstvo[i].permutacija[j]=trenutnoRjesenje.permutacija[j];
			}
		}
		if(i!=0)indeks++;
		*/
		
		//Generiranje susjedstva slucajnim odabirom pivota iz trenutnog rjesenja
		susjedstvo[i].permutacija[i]=trenutnoRjesenje.permutacija[pivot];
		susjedstvo[i].permutacija[pivot]=trenutnoRjesenje.permutacija[i];
		for(int j=0; j<velicinaProblema; j++){
			if(j==pivot || j==i)continue;
			else susjedstvo[i].permutacija[j]=trenutnoRjesenje.permutacija[j];
		}
		
		//izracun funkcije cilja za svakog clana susjedstva
		susjedstvo[i].iznos=funkcijaCilja(susjedstvo[i].permutacija);
		
		//pronalazak najboljeg rjesenja u susjedstvu
		if(susjedstvo[i].iznos<najmanji && !isTabu(susjedstvo[i])){
			najmanji=susjedstvo[i].iznos;
			ind=i;
		}
	}

	//azuriraj tabu listu
	ubaciUTabuListu(susjedstvo[ind]);

	//azuriraj trenutno rjesenje
	memcpy(&trenutnoRjesenje, &susjedstvo[ind], sizeof(susjedstvo[ind]));
}

int tabuPretraga(){
	generirajPocetnoRjesenje();
	
	inicirajTabuListu();
	trenutnoRjesenje.iznos = funkcijaCilja(trenutnoRjesenje.permutacija);
	
	cout << "\n\nPocetno generirano rjesenje: \n";
	ispisRjesenja(trenutnoRjesenje);
	
	//pocetno generirano rjesenje postaje najbolje
	memcpy(&najboljeRjesenje, &trenutnoRjesenje, sizeof(trenutnoRjesenje));
	
	for(int i=0; i<brojIteracija; i++){
		generirajSusjedstvo();
		if(trenutnoRjesenje.iznos<najboljeRjesenje.iznos){
			memcpy(&najboljeRjesenje, &trenutnoRjesenje, sizeof(trenutnoRjesenje));
			cout << "Iteracija " << i << ". Novo najbolje rjesenje " << najboljeRjesenje.iznos << " " << endl;	
		}
	}
	
	cout << "\n\nNajbolje pronadjeno rjesenje: \n";
	ispisRjesenja(najboljeRjesenje);
}

int main(){
	srand(time(0));

	char nazivDatoteke[20];
	cout << "Unesite naziv datoteke: ";
	cin.getline(nazivDatoteke, 20);
	
	citajDatoteku(nazivDatoteke);
	
	if(velicinaProblema>MAX){
		cout << "Velicina problema veca od dozvoljenog!" << endl;
		return 0;
	}
	
	cout << "\nVelicina problema: " << velicinaProblema << "\n\nDistances-d: \n";
	ispisiMatricu(d);
	cout << "\nFlows-f: \n";
	ispisiMatricu(f);
	
	cout << "\nBroj iteracija: ";
	cin >> brojIteracija;

	tabuPretraga();
	
	return 0;
}

