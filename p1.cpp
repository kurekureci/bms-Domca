#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <vector>
#include <math.h>

#define FREKV 900
#define HIGH_MOB 1.2

using namespace std;

/*
 * trida BTS stanice
 */

class BTS{
  private:
    int cid;
    int rssi;
    double signal;
    double ant_h; // vyska anteny v metrech
    double power; // vykon BTS sloupce ve watech
    double LU; // path loss - utlum signalu
    double distance; // vzdalenost od stanice    
    
  public:
    BTS() //bezparametricky konstruktor
    {
      cid = 0;
      rssi = 0;
      signal = 0;
      ant_h = 0;
      power = 0;
      LU = 0;
      distance = 0;
    }
    
    BTS(int Cid, int Rssi, double Signal, double ant, double pow)   //konstruktor
    {
      cid = Cid;
      rssi = Rssi;
      signal = Signal;
      ant_h = ant;
      power = pow;
      LU = 0;
      distance = 0;
    }
    
    void setCid(int Cid) 
    {
      cid = Cid;
    }
    
    void setRssi(int Rssi) 
    {
      rssi = Rssi;
    }
    
    void setSignal(double Signal) 
    {
      signal = Signal;
    }
    
    void setAntH(double ant) //metoda - nastaveni vysky anteny
    {
      ant_h = ant;
    }
    
    void setPower(double pow) // metoda - nastaveni vykonu
    {
      power = pow;
    }
    
    void setLU(double lu)
    {
      LU = lu;
    }
    
    void setDistance(double dist)
    {
      distance = dist;
    }
    
    int getCid() 
    {
      //cout << "CID: " << cid << endl;
      return(cid);
    }
    
    int getRssi() 
    {
      //cout << "RSSI" << rssi << endl;
      return(rssi);
    }
    
    double getSignal() 
    {
      //cout << "Signal: " << signal << endl;
      return(signal);
    }
    
    double getAntH() //metoda - vypsani vysky anteny
    {
      //cout << "Vyska anteny: " << ant_h << " m" << endl;
      return(ant_h);
    }
    
    double getPower() // metoda - vypsani vykonu
    {
      //cout << "Vykon BTS sloupce: " << power << " W" << endl;
      return(power);
    }
    
    double getLU()
    {
      return LU;
    }
    
    double getDistance()
    {
      return distance;
    }
    
    /*void getAll() // metoda pro vypis vsech parametru instance
    {
      cout << endl << "Vypis parametru: " << endl;
      getCid();
      getRssi();
      getSignal();
      getAntH();
      getPower();
    }*/
};

vector <BTS*> allBTS; // vektor ukazatelů na instance tridy BTS

// funkce pro zisk parametru ze vstupu - parsovani vstupu --> udaje

int getParameters(char *fileName)
{
  string line;
  int CID;
  int RSSI;
  int Signal;
  int Ant_H;
  int Power;
  size_t pos;
  ifstream myfile (fileName); //otevreni souboru pro cteni
  
  if (myfile.is_open())
  {
    BTS *currentBTS;
    getline (myfile,line); // zisk 1. radku - popisny
    
    while (getline (myfile,line)) // pokud je dalsi radek v souboru -> parsovani radku -> zisk hodnot pro vypocet
    {
      pos = line.find(";"); //zjisti pozici prvniho znaku, ktery hledame
      
      if (pos != std::string::npos) // kdyz je znak nalezen
      {
        line.erase(0,pos+1); // smazat vse po prvni strednik
        //cout << line << endl;
      }
      
      pos = line.find(";");
      
      if (pos != std::string::npos)
      {
        CID = stoi(line.substr (0,pos)); // podretezec po dalsi strednik (substr(od, delka)) -> prevod na integer (stoi)
        //cout << "CID:" << CID << endl;
        line.erase(0,pos+1);
      }
      
      pos = line.find(";");
      
      if (pos != std::string::npos)
      {
        RSSI = stoi(line.substr (0,pos));
        //cout << "RSSI:" << RSSI << endl;
        line.erase(0,pos+1);
      }
      
      pos = line.find(";");
      
      if (pos != std::string::npos)
      {
        Signal = stod(line.substr (0,pos));
        //cout << "Signal:" << Signal << endl;
        line.erase(0,pos+1);
      }
      
      pos = line.find(";");
      
      if (pos != std::string::npos)
      {
        Ant_H = stod(line.substr (0,pos));
        //cout << "Ant_H:" << Ant_H << endl;
        line.erase(0,pos+1);
        Power = stod(line.substr (0));
        //cout << "Power:" << Power << endl << endl;
      }
      
      currentBTS = new BTS(CID, RSSI, Signal, Ant_H, Power); //dynamicke vytvareni instanci tridy BTS
      allBTS.push_back (currentBTS); // pridani ukazatele na instanci na konec vektoru
      
      //(*allBTS.back()).getAll(); // vypsani parametru instance ulozene ve vektoru :)
    }
    myfile.close(); // zavre soubor
  }
  else 
  {
    cout << "Unable to open file" << endl;
    return 3;
  }
  
  return 0;
}

void countLU() // vypocet LU
{
  int length = allBTS.size(); // delka vektoru instanci
  
  for(int i=0;i<length;i++)
  {
    double power = (*allBTS[i]).getPower();
    double sig = (*allBTS[i]).getSignal();
    double powerdB = 10*log10(1000*power); // vykon z wattu na dB
    double lu = powerdB-sig; // vpocet LU vykon - signal ? 
    (*allBTS[i]).setLU(lu); // ulozeni do instance 
  }
}

// vypocet vzdalenosti

void countDist()
{
  int length = allBTS.size(); // delka vektoru instanci
  
  for(int i=0;i<length;i++)
  {
    double hB = (*allBTS[i]).getAntH();
    double lu = (*allBTS[i]).getLU();
    double CH = 3.2* pow((log10(11.75*HIGH_MOB)),2.0)-4.97;
    double delenec = (lu-69.55-26.16*log10(FREKV)+13.82*log10(hB)+CH);
    double delitel = (44.9-6.55*log10(hB));
    double podil = (delenec/delitel);
    double dist = pow(10.0,podil);
    (*allBTS[i]).setDistance(dist); // ulozeni do instance 
    //cout << "Vzdalenost v km: " << dist << endl;
  }
}

// vypocet polohy


/*
 * HLAVNI PROGRAM
 * */

int main(int argc,char *argv[])
{
  //BTS bts(2,3); // vytvoreni instance tridy BTS s nazvem bts
  /*BTS *bts;
  bts = new BTS(2,3,5,4,8);*/
  
  // osetreni vstupu
  if(argc < 2) // malo vstupnich parametru
  {
    cerr << "Too few input parameters." << endl;
    return 1;
  }
  else if (argc > 2) // moc vstupnich parametru
  {
    cerr << "Too many input parameters." << endl;
    return 2;
  }
  else // ok - vstupni parametry
  {
    char *fileName = argv[1]; // jméno souboru s informacemi = parameter programu 
    getParameters(fileName);
    countLU();
    countDist();
  }
  
  return EXIT_SUCCESS;
}

