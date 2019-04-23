#include <iostream>
#include <ctime>
#include <cstdlib>
#include <cmath>
#include <vector>
#include <algorithm>
#include <fstream>
#include <string>
#include <sstream>
#include "InstanceGenerator.h"

using namespace std;



struct Object{
    int nr; // Number of job. Strat from 0, -1 means it's maintenance
    int t; // time, necessery to complete first operation
    int s = -1; // moment of starting  first operation
    int type; // 0 - maintenance, 1 - job
    int distance; // Number of operations between last maintenance and this operation. Equal to -1 for maintenance
};
struct Human{
    int parent; // index of parent in starting simulation
    vector<Object> m1; //operations on machine 1
    vector<Object> m2; //operations on machine 2
    vector<int> orderM1; // order of operations on machine 1
    vector<int> orderM2; // order of operations on machine 2
    int fitValue;
};

void fitLowerBest(vector<Human> &generation);

/*
subfunction for function sort
*/
bool acompare(Object a, Object b) {
    return a.s < b.s;
}
/*
subfunction for function sort
*/
bool acompare2(Object a, Object b) {
    return a.nr < b.nr;
}
bool acompare3(Human a, Human b) {
    return a.fitValue < b.fitValue;
}

/*
showing order of inserting operations on given machine
oper - machine
*/
void showOrder(Human h){
    cout<<"kolejnosc operacji na maszynie nr 1"<<endl;
    for(int i = 0; i < h.orderM1.size() ; i++){
        cout<<h.orderM1[i]<<", ";
    }
    cout<<endl;
    cout<<"kolejnosc operacji na maszynie nr 2"<<endl;
    for(int i = 0; i < h.orderM2.size() ; i++){
        cout<<h.orderM2[i]<<", ";
    }
    cout<<endl;
}
void fitFunction(vector<Human> &generation);
void showFitValue(vector<Human> generation){
    for(int i = 0; i< generation.size(); i++){
        cout<<" i = "<<i<< " "<<generation[i].fitValue<<endl;
        if(i%200 == 0 && i != 0)system("pause");
    }

}
/*
showing maintenances and sum parameter
*/
void showMnt(int sum, vector<Object> mnt){
    cout<<"sum = "<<sum<<endl;
    for(int i = 0; i < mnt.size() ; i++)
        cout<<"i="<<i<<": " <<mnt[i].s<<"-"<<mnt[i].s + mnt[i].t - 1<<" duration = "<<mnt[i].t<<endl;
}
/*
calculate real time needed to end job(including bonus
for doing jobs in sequence before next maintenance)
*/
int realTm(int tm, int dist){
    int reduction;
    if(dist < 5)
        reduction =  ceilf(((float)(tm * 5 * dist) / 100));
    else
        reduction = ceilf(((float)(tm * 5 * 5) / 100));
    return tm - reduction;
}
/*
showing Operations on both machines
*/
void showOper(vector<Object> oper1, vector<Object> oper2){
    for(int i = 0; i < oper1.size() ; i++){
        cout<<"i="<<i<< " typ: "<<oper1[i].type;
        cout<<"Nr op: "<<oper1[i].nr<<" : " <<oper1[i].s<<"-"<<oper1[i].s + realTm(oper1[i].t,oper1[i].distance) - 1<<" duration = "<<
        oper1[i].t<<" real Time ="<< realTm(oper1[i].t,oper1[i].distance) <<" "<<oper1[i].distance<<" \t";
        if(i < oper2.size() ){
            cout<<"Nr operacji: "<<oper2[i].nr<< " ";
            cout<<oper2[i].s<<"-"<<oper2[i].s + oper2[i].t - 1<<" duration = "<<oper2[i].t<<endl;
        }
        else cout<<endl;
    }
}

void load(int &n, int &sum, Human &inst, vector<Object> &mnt, int nr_Instance){
    ifstream f;
    string fileName, extention = ".txt";
    stringstream ss;
    ss << nr_Instance;
    fileName = ss.str() + extention;
    inst.m1.clear();
    inst.m2.clear();
    mnt.clear();
    int s;
    char s2;
    Object hndl, hndl3;
    Object hndl2;
    sum = 0;

    f.open(fileName.c_str());
    if (!f.is_open()){
        cout<<"Nie otworzono pliku do odczytu"<<endl;
    }
    f>>n; //ignoring instance number
    f>>n;
    inst.m1.reserve(n);
    inst.m2.reserve(n); // alocating n places in memory for now, to not be forced, to rewrite whole vector later to another place
    for(int i = 0; i < n; i++){
        f>>hndl.t;
        f>> s2;
        sum += hndl.t;
        hndl.type = 1;
        hndl.nr = i;
        inst.m1.push_back(hndl);
        hndl3.type = 1;
        hndl3.nr = i;
        f>>hndl3.t;
        f>> s2;
        inst.m2.push_back(hndl3);
        f>>s>>s2>>s>>s2;
    }
    sum = (sum *3 )/4;
    while(!f.eof()){
        f>>s>>s2>>s>>s2;
        f>>hndl2.t;
        sum += hndl2.t;
        f>> s2;
        f>>hndl2.s;
        f>> s2;
        hndl2.type = 0;
        hndl2.nr = -1;
        hndl2.distance = -1;
        mnt.push_back(hndl2);
    }
    f.close();
    mnt.pop_back();
    inst.m1.reserve(n + mnt.size());
    sort(mnt.begin(),  mnt.end(), acompare);
    sort(inst.m1.begin(),  inst.m1.end(), acompare2);
    sort(inst.m2.begin(),  inst.m2.end(), acompare2);
}

void save(Human best, Human parent, int nr_Instance, int mini, int maxi, ofstream &f2){//maxi, mini i f2 to parametry, ktore dodawalem i usuwalem w zaleznosci od nazwy pliku, ktora chcialem uzyskac
    //a takze od danych, ktore chcialem dac do podsumowania serii. mozne je usunac, wraz z odwolaniami do nich - wszystko bedzie dzialac
    ofstream f;
    int idleNr = 1, mntNr = 1, sumMaintM1 = 0, idleStart, sumIdleM1 = 0, sumIdleM2 = 0;
    string fileName, extention = ".txt";
    stringstream ss, ss2, ss3;
    ss << nr_Instance;
    fileName = "odp" + ss.str();
    ss2 << mini;
    ss3 << maxi;
    fileName += "- range=" + ss2.str() + "-" + ss3.str() + extention;
    /*if(selectionMetod > 90){
        fileName = "C:\\Users\\kamil_2\\Desktop\\AlgorytmGenetyczny\\ranking\\" + fileName;
    }
    else{
        if(selectionMetod > 50){
            fileName = "C:\\Users\\kamil_2\\Desktop\\AlgorytmGenetyczny\\turniej\\" + fileName;
        }
        else{
            if(selectionMetod > 0){
                fileName = "C:\\Users\\kamil_2\\Desktop\\AlgorytmGenetyczny\\ruletka\\" + fileName;
            }
            else{
                fileName = "C:\\Users\\kamil_2\\Desktop\\AlgorytmGenetyczny\\mieszane\\" + fileName;
            }
        }
    }*/
    f.open(fileName.c_str());
    if (!f.is_open()){
        cout<<"Nie otworzono pliku do zapisu"<<endl;
    }
    f<<nr_Instance<<endl;
    f<<best.fitValue<<", "<<parent.fitValue<<endl;
    f<<"M1: "<<"op1_"<<best.m1[0].nr<<", "<<best.m1[0].s<<", "<<best.m1[0].t<<", "<<realTm(best.m1[0].t, best.m1[0].distance)<<"; ";
    for(int i = 1; i< best.m1.size(); i++){
        if(best.m1[i - 1].s + realTm(best.m1[i - 1].t, best.m1[i - 1].distance) != best.m1[i].s ){//if idle
            idleStart = best.m1[i - 1].s + realTm(best.m1[i - 1].t, best.m1[i - 1].distance);
            f<<"idle"<<idleNr++<<"_M1, "<<idleStart<<", "<<best.m1[i].s - idleStart<<"; ";
            sumIdleM1 += best.m1[i].s - idleStart;
        }
        if(best.m1[i].nr == -1){//if maintenance
            f<<"maint"<<mntNr++<<"_M1, "<<best.m1[i].s<<", "<<best.m1[i].t<<"; ";
            sumMaintM1 +=best.m1[i].t;
        }
        else{ //if job
            f<<"op1_"<<best.m1[i].nr<<", "<<best.m1[i].s<<", "<<best.m1[i].t<<", "<<realTm(best.m1[i].t, best.m1[i].distance)<<"; ";
        }

    }
    idleNr = 1;
    f<<endl<<"M2: idle"<<idleNr++<<"_M2, 0, "<<best.m2[0].s<<"; ";
    sumIdleM2 += best.m2[0].s;
    f<<"op2_"<<best.m2[0].nr<<", "<<best.m2[0].s<<", "<<best.m2[0].t<<", "<<best.m2[0].t<<"; ";
    for(int i = 1; i< best.m2.size(); i++){
        if(best.m2[i - 1].s + best.m2[i - 1].t != best.m2[i].s ){//if idle
            idleStart = best.m2[i - 1].s + best.m2[i - 1].t;
            f<<"idle"<<idleNr++<<"_M2, "<<idleStart<<", "<<best.m2[i].s - idleStart<<"; ";
            sumIdleM2 += best.m2[i].s - idleStart;
        }
        if(best.m2[i].nr == -1){//if maintenance
            f<<"maint"<<mntNr++<<"_M2, "<<best.m2[i].s<<", "<<best.m2[i].t<<"; ";//in case i made some mistake - it should not never execute
        }
        else{
            f<<"op2_"<<best.m2[i].nr<<", "<<best.m2[i].s<<", "<<best.m2[i].t<<", "<<best.m2[i].t<<"; ";
        }

    }
    f<<endl<<sumMaintM1<<endl<<0<<endl<<sumIdleM1<<endl<<sumIdleM2;
    f2<<";"<<sumIdleM1<<";"<<sumIdleM2;
    f.close();
}



void addSecondOper(vector<Object> &machine, Object &oper, int minStartTm){
    int j = 0;
    while( j < machine.size() &&
          (machine[j].s + machine[j].t) < minStartTm ){
        j++;
    }
    j--;
    if(j + 1 == machine.size() ){//operation need to be inserted behind all operations
        oper.s = minStartTm;
        machine.push_back(oper);
    }
    else{
        if(j == -1 && (machine[0].s >= minStartTm + oper.t)){ // operation can be inserted before all operations
            oper.s = minStartTm;
            machine.insert(machine.begin(), oper);
        }
        else{
            if(j == -1)j = 0;
            while( j + 1 < machine.size() &&
                    (machine[j + 1].s - max(minStartTm,(machine[j].s + machine[j].t))) < oper.t)
                j++;
            oper.s = max(minStartTm,(machine[j].s + machine[j].t));

            machine.insert(machine.begin() + j + 1, oper);
        }
    }
}
void addFirstOper(vector<Object> &machine, Object &oper){
    int j = 0;
    if(machine.size() == 0)cout<<"nie ma maintenancow!"<<endl;
    if(machine[0].s < oper.t){
        while( j + 1 < machine.size() &&
          (machine[j + 1].s - (machine[j].s + realTm(machine[j].t, machine[j].distance))) < realTm(oper.t, machine[j].distance + 1))
          j++;
        oper.s = machine[j].s + realTm(machine[j].t, machine[j].distance);
        oper.distance = machine[j].distance + 1;
        if(machine.size() > j + 1 && machine[j + 1].distance != -1)
            machine[j + 1].distance++;
        machine.insert(machine.begin() + j + 1, oper);
    }
    else{
        oper.s = 0;
        oper.distance = 0;
        machine.insert(machine.begin(), oper);
    }
}

/*
mix order of operations on machines
*/
void mixing(Human &mixedInst, Human inst){
    for(int i = 0; i <  inst.m1.size(); i++){
        inst.m1[i].s = rand();
        inst.m2[i].s = rand();
    }
    sort(inst.m1.begin(),  inst.m1.end(), acompare);
    sort(inst.m2.begin(),  inst.m2.end(), acompare);// sort with loop above mix order of "inserting operations"(raising flags, that operation is there)
    for(int i = 0; i <  inst.m1.size(); i++){
        mixedInst.orderM1.push_back(inst.m1[i].nr);
    }
    for(int i = 0; i <  inst.m2.size(); i++){
        mixedInst.orderM2.push_back(inst.m2[i].nr);
    }
}

/*
create Human to population
inst - basic instance without maintenances
mnt - maintenances
mixed - if false, then create new order of adding operations to machine
*/
Human createHuman(Human inst, Human mixedInst, vector<Object> mnt, bool mixed = true){
    bool touching;
    mixedInst.m1.clear();
    mixedInst.m2.clear();
    int select1 = 0, select2 = 0, to = 0;
    int j = 0;
    if(!mixed)
        mixing(mixedInst, inst);
    for(int i = 0; i < mnt.size(); i++){ // inserting maintenances in right place
        mixedInst.m1.push_back(mnt[i]);
    }
    for(int i = 0; i < inst.m1.size() ; i++){
        addFirstOper(mixedInst.m1, inst.m1[mixedInst.orderM1[i]]);
    }
    for(int i = 0; i < inst.m2.size() ; i++){
        j = 0;
        while(inst.m2[mixedInst.orderM2[i]].nr != inst.m1[j].nr)j++;
        addSecondOper(mixedInst.m2, inst.m2[mixedInst.orderM2[i]], inst.m1[j].s + realTm(inst.m1[j].t, inst.m1[j].distance));
    }
    return mixedInst;
}
/*
create our population after loading
*/
void createPopulation(int popNmb, int n, vector<Human> &generation, Human inst, vector<Object> mnt){
    Human mixedInst;
    mixedInst.m1.reserve(n);
    mixedInst.m2.reserve(n);
    for(int i =0; i < popNmb; i++){
        generation.push_back(createHuman(inst, mixedInst, mnt, false));
        generation[i].parent = i;
    }
}
/*
calculate  our population
*/
void recreatePopulation(int n, vector<Human> &generation, Human inst, vector<Object> mnt){
    int parent;
    for(int i =0; i < generation.size(); i++){
        parent = generation[i].parent;
        generation[i] = createHuman(inst, generation[i], mnt);
        generation[i].parent = parent;
    }
}

/*
First part of metaheuristic - random mixing places of our jobs
k - how many changes in order of inserting in each machine (in percent)
*/
void mutation(int k, int n, Human &human, Human inst, vector<Object> mnt){
    int mutNmb = ceil((float)(n*k)/100);
    int jobNmb1, jobNmb2, jobIndex1, jobIndex2;
    for(int i =0; i < mutNmb / 2; i++){
        jobIndex1 = rand() % n;
        jobIndex2 = rand() % n;
        jobNmb1 = human.orderM1[jobIndex1];
        human.orderM1[jobIndex1] = human.orderM1[jobIndex2];
        human.orderM1[jobIndex2] = jobNmb1;

        jobIndex1 = rand() % n;
        jobIndex2 = rand() % n;
        jobNmb1 = human.orderM2[jobIndex1];
        human.orderM2[jobIndex1] = human.orderM2[jobIndex2];
        human.orderM2[jobIndex2] = jobNmb1;
    }
}
/*
Mutating whole generation
k - how many changes in order of inserting in each machine (in percent)
*/
void mutateAll(int k, int n, vector<Human> &generation, Human inst, vector<Object> mnt){
    fitLowerBest(generation);
    int localOptimum = 0;
    for(int i = 0; i < generation.size(); i++){
        if(generation[i].fitValue != 0)
            mutation(k, n, generation[i], inst, mnt);
        else{
            if(localOptimum > 2){// system against local optima
                if(localOptimum > generation.size()/5)
                    mutation(k + 70, n, generation[i], inst, mnt);
                else
                    mutation(k + 30, n, generation[i], inst, mnt);
            }
            localOptimum++;
        }
    }
}

/*
find highest fitValue
*/
int maX(vector<Human> &generation){
    int tmpMax = generation[0].fitValue;
    for(int i = 1; i < generation.size(); i++){
        if(tmpMax < generation[i].fitValue)
            tmpMax = generation[i].fitValue;
    }
    return tmpMax;
}
/*
fins lowest fitValue
*/
int miN(vector<Human> &generation){
    int tmpMin = generation[0].fitValue;
    for(int i = 1; i < generation.size(); i++){
        if(tmpMin > generation[i].fitValue)
            tmpMin = generation[i].fitValue;
    }
    return tmpMin;
}
int miNindex(vector<Human> &generation){
    int tmpMin = generation[0].fitValue, index = 0;
    for(int i = 1; i < generation.size(); i++){
        if(tmpMin > generation[i].fitValue){
            tmpMin = generation[i].fitValue;
            index = i;
        }
    }
    return index;
}

/*
calculate amount of time for each Human  to do all his jobs
*/
void fitFunction(vector<Human> &generation){
    Object last;
    int mini, range;
    for(int i = 0; i < generation.size(); i++){
        last = generation[i].m2[generation[i].m2.size() - 1];
        generation[i].fitValue = last.s + last.t;// jobs need this amount of time (counting from 1, not 0)
    }
    range = maX(generation) - miN(generation);
}
/*
fit Function. Higher value - better human
*/
void fitHigherBest(vector<Human> &generation){
    int range, mini;
    fitFunction(generation);
    mini = miN(generation);
    range = maX(generation) - mini;
    for(int i = 0; i < generation.size(); i++){
        generation[i].fitValue -= mini;
        generation[i].fitValue = range - generation[i].fitValue;
    }
}
/*
fit Function. Lower value - better human
*/
void fitLowerBest(vector<Human> &generation){
    int mini;
    fitFunction(generation);
    mini = miN(generation);
    for(int i = 0; i < generation.size(); i++)
        generation[i].fitValue -= mini;
}

/*
way of selection - low selection pressure
*/
int roulette(vector<Human> generation, int popNmb){
    int sum = 0, selected = 0;
    for( int i = 0; i < popNmb; i++)
        sum += generation[i].fitValue;
    sum = sum * rand()/RAND_MAX + 1;
    while(sum > 0){
        selected = (selected + 1)% popNmb;
        sum -= generation[selected].fitValue;
    }
    return selected;
}
/*
way of selection - high selection pressure
chosen[] - table with index of elements, who contest each other
*/
int contest(vector<Human> generation, vector<int> choosen){
    int best = choosen.back(), tmp;
    choosen.pop_back();
    while(!choosen.empty()){
        tmp = choosen.back();
        choosen.pop_back();
        if(generation[tmp].fitValue < generation[best].fitValue)
            best = tmp;
    }
    return best;
}
/*
way of selection - very high selection pressure
popNmb - expected population size after selection
*/
void ranking(vector<Human> &generation, int popNmb){
    sort(generation.begin(), generation.end(), acompare3);
    for(int i = generation.size() - 1; i >= popNmb; i--){
        generation.erase(generation.begin() + i);
    }
}

/*
create new human by crossing two humans
p1,p2 - parents of new human
*/
void breed(Human p1, Human p2, vector<Human> &generation){
    Human chd;
    chd.m1.reserve(p1.m1.size());
    chd.m2.reserve(p1.m2.size());
    int third = p1.orderM1.size()/3, j = p1.orderM1.size() - 1;
    vector<int> oldMid1, newMid1, oldMid2, newMid2;
    chd.orderM1.insert(chd.orderM1.begin(), p1.orderM1.begin(), p1.orderM1.begin() + third);
    chd.orderM2.insert(chd.orderM2.begin(), p1.orderM2.begin(), p1.orderM2.begin() + third);
    oldMid1.assign(p1.orderM1.begin() + third, p1.orderM1.begin() + 2 * third);
    oldMid2.assign(p1.orderM2.begin() + third, p1.orderM2.begin() + 2 * third);
    while(newMid1.size() != oldMid1.size()){
        if(find(oldMid1.begin(), oldMid1.end(), p2.orderM1[j--]) != oldMid1.end()){
            newMid1.push_back(p2.orderM1[j + 1]);
        }
    }
    j = p1.orderM1.size() - 1;
    while(newMid2.size() != oldMid2.size()){
        if(find(oldMid2.begin(), oldMid2.end(), p2.orderM1[j--]) != oldMid2.end()){
            newMid2.push_back(p2.orderM1[j + 1]);
        }
    }
    chd.orderM1.insert(chd.orderM1.end(), newMid1.begin(), newMid1.end());
    chd.orderM2.insert(chd.orderM2.end(), newMid2.begin(), newMid2.end());
    chd.orderM1.insert(chd.orderM1.end(), p1.orderM1.begin() + 2 * third, p1.orderM1.end());
    chd.orderM2.insert(chd.orderM2.end(), p1.orderM2.begin() + 2 * third, p1.orderM2.end());
    chd.parent = p1.parent;
    generation.push_back(chd);
}
/*
Second part of metaheuristic - exchanging jobs between two parents
popNumb - amount of poeple in population at beginning
expansion - how many times should population growth(in percent). F.e. popNmb = 100 and expansion = 400,
then after crossover our generation have minimum 400 humans
*/
void crossover(int expansion, int n, int popNmb, Human inst, vector<Object> mnt, vector<Human> &generation){
    int p1, p2, newPopNmb = (expansion * popNmb) / 100;
    recreatePopulation(n, generation, inst, mnt);
    fitHigherBest(generation);
    while(generation.size() < newPopNmb){
        p1 = roulette(generation, popNmb);
        p2 = roulette(generation, popNmb);
        breed(generation[p1], generation[p2], generation);
        breed(generation[p2], generation[p1], generation);
    }
}

/*
Third part of metaheuristic - selecting humans to remove
popNmb - expected population size after selection
inst - instance of our problem(from file)
n - number of jobs
progress - time, to end of all calculations (in percent)
*/
void selection(int popNmb, int n, Human inst, vector<Object> mnt, vector<Human> &generation, double progress){
    recreatePopulation(n, generation, inst, mnt);
    fitLowerBest(generation);
    vector<int> choosen;
    int best;
    if(progress < 50.0){
        while(generation.size() > popNmb){
            generation.erase(generation.begin() + roulette(generation, generation.size()));
        }
    }
    else{
        if(progress < 90.0){
            while(generation.size() > popNmb){
                for(int i = 0; i < 4; i++){
                    choosen.push_back(rand()%generation.size());
                }
                best = contest(generation, choosen);
                sort(choosen.begin(), choosen.end());
                for(int i = 0; i < 4; i++){
                    if(best != choosen.back() && generation.size() > popNmb){
                        generation.erase(generation.begin() + choosen.back());
                    }
                    choosen.pop_back();
                }
            }
        }
        else{
            ranking(generation, popNmb);
        }
    }
}

/*
metaheuristic, which work for a given time
*/
void metaheuristicTm(int popNmb, int expansion, int nr_Instance, double workTm, ofstream &f, int mini, int maxi){//maxi, mini i f to parametry, ktore dodawalem i usuwalem w zaleznosci od nazwy pliku, ktora chcialem uzyskac
    //a takze od danych, ktore chcialem dac do podsumowania serii. mozne je usunac, wraz z odwolaniami do nich - wszystko bedzie dzialac(jesli usuniemy je takze z funkcji save)
    int n, sum, startMin, endMin;
    Human inst, best;
    vector<Object> mnt;
    vector<Human> generation, startGeneration;
    clock_t start;//to measure time
    double tm = 0, progress = 0, measuerTm = 0;//to measure time(in seconds)
    //if(selectionMetod > 0 )progress = selectionMetod;
    cout<<"wczytuje "<<nr_Instance<<endl;
    load(n, sum, inst, mnt, nr_Instance);
    cout<<"wczytalem "<<nr_Instance<<endl;
    f<<endl;
    srand(time(0));
    createPopulation(popNmb, n, generation, inst, mnt);
    fitFunction(generation);
    startGeneration = generation;
    //cout<<"sum = "<< sum<<endl;
    startMin = miN(generation);
    //cout<<"Najlepsza wartosc "<<startMin<<" na miejscu "<<miNindex(generation)<<endl;
    int i = 1;
    start = clock();
    while(workTm > tm){
        progress = tm / workTm * 100;
        //cout<<"petla nr "<<i++<<"obliczanie: "<<progress<<"%"<<endl;
        mutateAll(20-(20*i/99), n, generation, inst, mnt);
        crossover(expansion, n, popNmb, inst, mnt, generation);
        selection(popNmb, n, inst, mnt, generation, progress);
        //cout<<"rozmiar"<<generation.size()<<endl;
        fitFunction(generation);
        //cout<<"Najlepsza wartosc "<<miN(generation)<<" na miejscu "<<miNindex(generation)<<endl<<endl;
        tm = static_cast < double >(clock()-start)/CLOCKS_PER_SEC;
        /*if(tm > measuerTm){
            if(i % 2 == 0)
                f<<(int)measuerTm<<",5;"<<miN(generation)<<endl;
            else
                f<<measuerTm<<";"<<miN(generation)<<endl;
            i++;
            measuerTm += 0.5;
        }*/

    }
    //cout<<"obliczanie: "<<progress<<"%"<<endl;
    endMin = miN(generation);
    f<<mini<<";"<<maxi<<";"<<endMin;
    //cout<<"Najlepsza wartosc poczatkowa "<<startMin<<endl;
    //cout<<"Najlepsza wartosc koncowa "<<endMin<<" na miejscu "<<miNindex(generation)<<endl;
    //cout<<"Wartosc rodzica "<<startGeneration[generation[miNindex(generation)].parent].fitValue<<endl;
    //cout<<"koncowy rozmiar"<<generation.size()<<endl;
    best = generation[miNindex(generation)];
    save(best, startGeneration[best.parent], nr_Instance,mini, maxi, f);
    //showOper(generation[miNindex(generation)].m1, generation[miNindex(generation)].m2);
}
void metaheuristicIteration(int popNmb, int expansion, int nr_Instance, int iterations, ofstream &f, vector<int> &opt){//opt i f to parametry, ktore dodawalem i usuwalem w zaleznosci od nazwy pliku, ktora chcialem uzyskac
    //a takze od danych, ktore chcialem dac do podsumowania serii. mozne je usunac, wraz z odwolaniami do nich - wszystko bedzie dzialac(jesli usuniemy je takze z funkcji save)
    int n, sum, startMin, endMin;
    Human inst, best;
    vector<Object> mnt;
    vector<Human> generation, startGeneration;
    clock_t start;//to measure time
    double tm = 0, progress, measuerTm = 0;//to measure time(in seconds)
    cout<<"wczytuje "<<nr_Instance<<endl;
    load(n, sum, inst, mnt, nr_Instance);
    cout<<"wczytalem "<<nr_Instance<<endl;
    f<<endl;
    opt.push_back(sum);
    srand(time(0));
    createPopulation(popNmb, n, generation, inst, mnt);
    fitFunction(generation);
    startGeneration = generation;
    startMin = miN(generation);
    //cout<<"Najlepsza wartosc "<<startMin<<" na miejscu "<<miNindex(generation)<<endl;
    int i = 1;
    start = clock();
    for(int i = 1; i <= iterations; i++){
        progress = (double)(i / iterations) * 100;
        //cout<<"petla nr "<<i++<<"obliczanie: "<<progress<<"%"<<endl;
        mutateAll(20-(20*i/99), n, generation, inst, mnt);
        crossover(expansion, n, popNmb, inst, mnt, generation);
        selection(popNmb, n, inst, mnt, generation, progress);
        //cout<<"rozmiar"<<generation.size()<<endl;
        fitFunction(generation);
        //cout<<"Najlepsza wartosc "<<miN(generation)<<" na miejscu "<<miNindex(generation)<<endl<<endl;
        tm = static_cast < double >(clock()-start)/CLOCKS_PER_SEC;
        /*if(tm > measuerTm){
            if(i % 2 == 0)
                f<<(int)measuerTm<<",5;"<<miN(generation)<<endl;
            else
                f<<measuerTm<<";"<<miN(generation)<<endl;
            i++;
            measuerTm += 0.5;
        }*/

    }
    //cout<<"obliczanie: "<<progress<<"%"<<endl;
    endMin = miN(generation);
    f<<expansion<<";"<<endMin;
    //cout<<"Najlepsza wartosc poczatkowa "<<startMin<<endl;
    //cout<<"Najlepsza wartosc koncowa "<<endMin<<" na miejscu "<<miNindex(generation)<<endl;
    //cout<<"Wartosc rodzica "<<startGeneration[generation[miNindex(generation)].parent].fitValue<<endl;
    //cout<<"koncowy rozmiar"<<generation.size()<<endl;
    best = generation[miNindex(generation)];
    save(best, startGeneration[best.parent], nr_Instance, expansion, expansion, f);
    //showOper(generation[miNindex(generation)].m1, generation[miNindex(generation)].m2);
}
int main(){
    //pozostawiam czesc zakomentowanego kodu, by bylo widac jak przeprowadzalem przynajmniej niektore testy
    int popNmb = 50;
    int sum = 0, expansion = 250;
    int from = 1, to = 1;
    double  workTm = 30.0;
    int n = 50, k = 20, mini = 5, maxi = 20;
    ofstream f;
    f.open("Podsumowanie - range=15+5x - dane-30tm, 150exp,100pop.txt");
    InstanceGenerator generator;
    generator.start(n, k, from, to, mini, maxi);
    metaheuristicTm(popNmb, expansion, 1, workTm, f, mini , maxi );
    //vector<int> opt;
    //metaheuristicIteration(500, expansion, 1, 30, f, opt);
    //metaheuristic(popNmb, expansion, 1, workTm, f, opt);
    //cout<<"Przewidywany czas pracy " << 4 * workTm * 40<<"sekund"<<endl;
    /*for(int i = 1; i <= 10; i++){
        generator.start(n, k, from + (i-1)*to,to * i, mini + 5 * i, maxi + 5 * i);
        for(int j = from + (i-1)*to; j <= to * i; j++){
            metaheuristicTm(popNmb, expansion, j, workTm, f, mini + 5 * i, maxi + 5 * i);
        }
        f<<endl;
    }*/
    f.close();
    /*
    for(int i = from; i <= to - from + 1; i++){
        metaheuristicTm(popNmb, expansion, i, workTm, f, 20);//ruletka
    }
    f.close();
    f.open("Podsumowanie - turniej - dane-20tm, 150exp,100pop.txt");
    for(int i = 1; i <= to - from + 1; i++){
        metaheuristicTm(popNmb, expansion, i, workTm, f, 60);//turniejowa
    }
    f.close();
    f.open("Podsumowanie - ranking - dane-20tm, 150exp,100pop.txt");
    for(int i = 1; i <= to - from + 1; i++){
        metaheuristicTm(popNmb, expansion, i, workTm, f, 95);//rankingowa
    }
    f.close();
    f.open("Podsumowanie - mieszana - dane-20tm, 150exp,100pop.txt");
    for(int i = 1; i <= to - from + 1; i++){
        metaheuristicTm(popNmb, expansion, i, workTm, f, -1);//mieszana
    }*/
    /*for(int i = 1; i <= 30; i++){
        for(int j = 1; j <= 10; j++){
            metaheuristicTm(popNmb, 100 + expansion*j, i, workTm, f, opt);
        }
        f<<endl;
    }
    f.close();
    f.open("Podsumowanie - dane-30iter,50x exp,100pop.txt");
    for(int i = 1; i <= 30; i++){
        for(int j = 1; j <= 10; j++){
            metaheuristicIteration(popNmb, 100 + expansion*j, i, 30, f, opt);
        }
        f<<endl;
    }*/

    /*while(!opt.empty()){
        f<<opt.back()<<endl;
        opt.pop_back();
    }*/

    return 0;
}
