#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string.h>

using namespace std;

int main(){
  DIR *dir;
  struct dirent *dp;
  string path = "/home/xiao/pelmani/txt_data/";

  dir = opendir(path.c_str());

  for(dp = readdir(dir) ; dp != NULL ; dp = readdir(dir)){
    if(strstr(dp->d_name,".txt") != NULL){ 
      string filename = string(dp->d_name);
      string complete_path = path + filename;
      ifstream fin(complete_path.c_str());

      if (!fin) return 1;

      stringstream stream;
      stream << fin.rdbuf();
      fin.close();

      string data(stream.str());

      cout << data;
    }
  }

  closedir(dir);
  return 0;
}
