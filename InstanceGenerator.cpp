#include "InstanceGenerator.h"
#include <iostream>
#include <ctime>
#include <cstdlib>
#include <cmath>
#include <vector>
#include <algorithm>
#include <fstream>
#include <string>
#include <sstream>
#include <windows.h>
using namespace std;
InstanceGenerator::InstanceGenerator()
{
    //ctor
}

using namespace std;

void InstanceGenerator::showMnt1(int sum, vector< Object1> mnt){
    cout<<"sum = "<<sum<<endl;
    for(int i = 0; i < mnt.size() ; i++)
        cout<<"i="<<i<<": " <<mnt[i].s<<"-"<<mnt[i].s + mnt[i].t - 1<<" duration = "<<mnt[i].t<<endl;
}
void InstanceGenerator::showOper1(vector< Object1> oper1, vector< Object1> oper2){
    for(int i = 0; i < oper1.size() ; i++){
        cout<<"i="<<i<<": " <<oper1[i].s<<"-"<<oper1[i].s + oper1[i].t - 1<<" duration = "<<oper1[i].t<<"\t";
        cout<<oper2[i].s<<"-"<<oper2[i].s + oper2[i].t - 1<<" duration = "<<oper2[i].t<<endl;
    }
}

/*
creating maintenances
*/
void InstanceGenerator::createMnt(int mntNumb, int minObjTm, int maxObjTm, int sum, vector< Object1> &mnt, vector<bool> &selected){
    bool touching;
    int duration, select, range = maxObjTm - minObjTm;
    Object1 hndl;
    for(int i = 0; i < mntNumb  ; i++){
        do{
            touching = false;
            duration = (rand() % range) + minObjTm;
            select = rand() % sum;
            for( int j = select; j < select + duration; j++){
                if(selected[j]) touching = true;
            }

        }while(touching);
        for(int j = select; j < select + duration; j++){
            selected[j] = true;
        }
        hndl.t = duration;
        hndl.s = select;
        hndl.type = 0;
        mnt.push_back(hndl);
    }
}

/*
saving Instance to file
*/
void InstanceGenerator::saveInstance(vector<Object1> oper1, vector<Object1> oper2, vector<Object1> mnt, int nr_Instance){
    ofstream f;
    stringstream os;
    string extention = ".txt";
    os << nr_Instance;
    string fileName = os.str() + extention;
    f.open(fileName.c_str());
    f<<nr_Instance<<endl<<oper1.size();
    for(int i = 0; i< oper1.size(); i++){
        f<<endl<<oper1[i].t<<";"<<oper2[i].t<<";1;2;";
    }
    for(int i = 0; i< mnt.size(); i++){
        f<<endl<<i+1<<";1;"<<mnt[i].t<<";"<<mnt[i].s<<";";
    }
    f.close();
}

/*
n - number of jobs
k - minimal summary duration of maintenance in percent of n
(f.e. k = 20, n = 50 -> minimal summary duration of maintenance = 50*20/100 = 10)
population - table with instance to generate
*/
void InstanceGenerator::generateInst(int n, int k, int nr_Instance, int minObjTm, int maxObjTm){

    int minMntNmb = ceilf((float)(n * k) / 100), sum = 0, mntNmb;
    int select;
    vector<bool> selectedM1;
    vector<Object1> mnt;
    vector<Object1> oper1, oper2;
    Object1 hndl;
    int range = maxObjTm - minObjTm;
    for(int i = 0; i < n  ; i++){
        hndl.t = (rand() % range) + minObjTm;
        hndl.type = 1;
        oper1.push_back(hndl);
        sum += hndl.t;
        hndl.t = (rand() % range) + minObjTm;
        hndl.type = 1;
        oper2.push_back(hndl);
    }
    for(int i =0; i< sum; i++) selectedM1.push_back(false);
    createMnt(minMntNmb, minObjTm, maxObjTm, sum, mnt, selectedM1);
    sort(mnt.begin(),  mnt.end(),compare);
    saveInstance(oper1, oper2, mnt, nr_Instance);
    cout<<"Zapisalem do pliku "<<nr_Instance<<".txt"<<endl;
}


void InstanceGenerator::start(int n, int k, int from, int to, int minObjTm, int maxObjTm){
    for(int i =from; i<= to; i++){
        Sleep(200); // too create other seed for srand each time(clock())
        srand( time( NULL ) ^ clock() );
        generateInst(n, k, i,minObjTm, maxObjTm);
    }
}
