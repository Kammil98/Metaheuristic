
#ifndef INSTANCEGENERATOR_H
#define INSTANCEGENERATOR_H
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

struct Object1{
    int t; // time, necessery to complete first operation
    int s; // moment of starting  first operation
    int type; // 0 - maintenance, 1 - job, 2 - break
};
class InstanceGenerator
{
    public:
        InstanceGenerator();
        static bool compare( Object1 a,  Object1 b){
            return a.s < b.s;
        }
        void showMnt1(int sum, std::vector<Object1> mnt);
        void showOper1(std::vector<Object1> oper1, std::vector< Object1> oper2);
        void createMnt(int mntNumb, int minObjTm, int maxObjTm, int sum, std::vector< Object1> &mnt, std::vector<bool> &selected);
        void saveInstance(std::vector<Object1> oper1, std::vector<Object1> oper2, std::vector<Object1> mnt, int nr_Instance);
        void generateInst(int n, int k, int nr_Instance, int minObjTm, int maxObjTm);
        void start(int n, int k, int from, int to, int minObjTm = 5, int maxObjTm = 20);

    protected:
    private:

};
#endif // INSTANCEGENERATOR_H
