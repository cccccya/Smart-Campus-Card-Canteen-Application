#include "student.h"

Student::Student(){

}

//数字返回其格式化字符串 1->"01"   入口参数：需要格式化输出的字符串    出口参数：格式化字符串
inline QString Student::GetString(int n){
    return n>9?QString::number(n):getstring[n];
}

