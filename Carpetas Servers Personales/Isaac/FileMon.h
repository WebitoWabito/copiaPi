#ifndef FILEMON_H
#define FILEMON_H

#include <vector>
#include <string>

#define MAX_CLUSTERS 1024
#define CLUSTER_SIZE 64
#define END -1

struct FileEntry
{
  std::string name;
  int startCluster;
  int size;
};

extern int FAT[MAX_CLUSTERS];
extern bool bitmap[MAX_CLUSTERS];
extern char disk[MAX_CLUSTERS][CLUSTER_SIZE];
extern std::vector<FileEntry> directory;

void Init();

int getFreeCluster();
int writeFile(const char *data, int size);
void readFile(int startCluster, int fileSize);
void deleteFile(const std::string &name);

void createFile();
void listFiles();
void readFileFromDirectory();
void deleteFileFromDirectory();
void menu();

#endif